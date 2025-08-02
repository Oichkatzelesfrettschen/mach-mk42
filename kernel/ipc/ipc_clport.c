/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	ipc_clport.c,v $
 * Revision 2.5  91/02/05  17:21:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:43:52  mrt]
 * 
 * Revision 2.4  90/12/14  10:59:57  jsb
 * 	Fixed all remaining memory leaks (except for clport structs).
 * 	Use separate clport_map instead of ipc_kernel_map.
 * 	[90/12/13  21:50:51  jsb]
 * 
 * 	Moved dcm interface stubs to dcm.c.
 * 	Moved packet, msg data structure definitions to ipc/ipc_clpacket.h.
 * 	Removed item field from msg structure. Removed debugging printfs.
 * 	Added garbage collection of pages, msgs, packets.
 * 	Cleaned up master_device_port exporting code.
 * 	[90/12/11  09:40:29  jsb]
 * 
 * 	First implementation for complex messages.
 * 	Changed inter-node port uids from 64 bits to 32.
 * 	Still to do:
 * 	    fix VM leaks
 * 	    handle moving receive rights
 * 	    handle out-of-line ports
 * 	    move new dcm interface to dcm module
 * 	    more efficient port lookup, reassembly, etc.
 * 	    eliminate data copies in copy object creation, etc.
 * 	    limit number of pages mapped in at once
 * 	    get rid of item vector in packet structure
 * 	    make use of pkt_msgid
 * 	[90/12/10  09:28:36  jsb]
 * 
 * Revision 2.3  90/12/04  14:50:39  jsb
 * 	Use machine-independent node_self routine instead of ipsc2_physnode.
 * 	[90/12/04  11:32:11  jsb]
 * 
 * Revision 2.2  90/09/28  16:54:34  jsb
 * 	First checkin.
 * 	[90/09/28  14:27:09  jsb]
 * 
 */
/*
 *	File:	ipc/ipc_cluster.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <mach_clport.h>

#if	MACH_CLPORT
#include <vm/vm_kern.h>
#include <vm/vm_page.h>		/* XXX */
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_marequest.h>
#include <ipc/ipc_clport.h>
#include <ipc/ipc_clpacket.h>

vm_map_t clport_map;

/*
 * A uid (unique id) = node + lid (local id)
 * We embed the node id in the uid because receive rights rarely move.
 *
 * 11 bits for node		-> max 2048 nodes
 * 21 bits for local id		-> max 2097151 exported ports per node
 */
#define	IP_CLPORT_NODE(uid)	((unsigned long)(uid) & 0x000007ff)
#define	IP_CLPORT_LID(uid)	(((unsigned long)(uid) >> 11) & 0x001fffff)
#define	IP_CLPORT(node, lid)	(((lid) << 11) | (node))
/* XXX we can check in ipc_init whether above macro will work,
   XXX ie whether there are too many nodes */

int ipc_mynode;
ipc_space_t ipc_space_remote;
extern void clport_receive_thread();
ipc_port_t ipc_clport_get_proxy(); /* forward */
int ipc_clport_export();

/* list of partially assembled incoming packets */
struct msg *msg_list = 0;

int k_packet = 0;
int k_msg = 0;
int k_clport = 0;

struct packet *
packet_allocate()
{
	k_packet++;
	return (struct packet *) kalloc(sizeof(struct packet));
}

packet_free(packet)
	struct packet *packet;
{
	k_packet--;
	kfree(packet, sizeof(struct packet));
}

struct msg *
msg_allocate()
{
	k_msg++;
	return (struct msg *) kalloc(sizeof(struct msg));
}

msg_free(msg)
	struct msg *msg;
{
	k_msg--;
	kfree(msg, sizeof(struct msg));
}

struct ipc_clport *
clport_allocate()
{
	k_clport++;
	return (struct ipc_clport *) kalloc(sizeof(struct ipc_clport));
}

clport_free(clport)
	struct ipc_clport *clport;
{
	k_clport--;
	kfree(clport, sizeof(struct ipc_clport));
}

/*
 * Called at not-interrupt level. Pages will be page-alligned.
 * Assemble packets into a message.
 * If a complete message has been assembled, call clport_receive.
 */
clport_receive_packet(packet)
	struct packet *packet;
{
	struct msg _msg, *msg, **msgp;

	/*
	 * XXX We should be replacing kernel vm with copy objects here.
	 * XXX Right now this is done in an ugly way by copy_extract.
	 */

	/*
	 * If this is the first and only packet, process it now.
	 */
	if (packet->pkt_last == 0) {
		msg = &_msg;
		msg->msg_first_pkt = msg->msg_last_pkt = packet;
		packet->pkt_next = 0;
		clport_receive(msg);
		return;
	}
	/*
	 * Otherwise, if this is the first but not last packet,
	 * create a new message and place the packet in it.
	 */
	if (packet->pkt_id == 0) {
		msg = msg_allocate();
		msg->msg_first_pkt = msg->msg_last_pkt = packet;
		msg->msg_next = msg;
		msg_list = msg;
		return;
	}
	/*
	 * Otherwise, this is not the first packet; thus there must
	 * already be a message. Find it and place the packet in it.
	 */
	for (msgp = &msg_list; ; msgp = &msg->msg_next) {
		if (! (msg = *msgp)) {
			panic("clport_receive_packet");
		}
		if (msg->msg_first_pkt->pkt_msgid == packet->pkt_msgid &&
		    msg->msg_first_pkt->pkt_remote == packet->pkt_remote) {
			break;
		}
	}
	msg->msg_last_pkt = msg->msg_last_pkt->pkt_next = packet;
	/*
	 * If the message is complete, take it off the list and process it.
	 */
	if (packet->pkt_id == packet->pkt_last) {
		*msgp = msg->msg_next;
		packet->pkt_next = 0;
		clport_receive(msg);
		msg_free(msg);
	}
}

/*
 * Create a copy object of specified size by concatenating
 * page-sized copy objects embedded in msg. Modify message
 * by skipping past all pkt_pages used to build requested
 * copy object.
 *
 * OPPORTUNITY TO WRITE AN UGLY HACK:
 *
 * Note the pessimality of unmapping memory and creating
 * copy objects in the case that the out-of-line data
 * is being sent to the kernel and is going to get mapped anyway.
 * Not that we can do anything about it, of course, since the
 * in-kernel receivers expect to see copy objects. Although
 * I guess I could hack memory_object_data_provided if I really
 * wanted to (i.e., implement a version that expected mapped-in
 * data instead of a copy object).
 */
vm_map_copy_t
copy_extract(msg, addr, length)
	struct msg *msg;
	vm_offset_t addr;
	vm_size_t length;
{
	vm_size_t size;
	vm_map_copy_t copy;
	vm_offset_t xxx_space = 0;	/* XXX */
	vm_offset_t xxx_start = 0;	/* XXX */
	vm_size_t xxx_size;	/* XXX */
	kern_return_t kr;

	xxx_size = size = round_page(addr + length) - trunc_page(addr);


	if (size % PAGE_SIZE) {
		panic("copy_extract: cannot happen\n");
	}
	if (size == 0) {
		return VM_MAP_COPY_NULL;
	}
#if 1
	if (size == PAGE_SIZE) {
		struct packet *packet = msg->msg_first_pkt;

		msg->msg_first_pkt = packet->pkt_next;
		kr = vm_map_copyin(clport_map, packet->pkt_page,
				   PAGE_SIZE, TRUE, FALSE, &copy);
		if (kr != KERN_SUCCESS) {
			panic("copy_extract: vm_map_copyin: %d\n", kr);
		}
		goto out;
	}
#endif
	/* XXX begin bogosity */
	if (vm_allocate(clport_map, &xxx_space, size, TRUE)
	    != KERN_SUCCESS) {
		panic("copy_extract: vm_allocate\n");
	}
	xxx_start = xxx_space;
	xxx_size = size;
	while (size > 0) {
		struct packet *packet = msg->msg_first_pkt;

		if (! packet || ! packet->pkt_page) {
			panic("copy_extract: no more data\n");
		}
		bcopy(packet->pkt_page, xxx_space, PAGE_SIZE);
		if (vm_deallocate(clport_map, packet->pkt_page, PAGE_SIZE)
		    != KERN_SUCCESS) {
			panic("copy_extract: vm_deallocate\n");
		}
		xxx_space += PAGE_SIZE;
		size -= PAGE_SIZE;
		msg->msg_first_pkt = packet->pkt_next;
	}
	kr = vm_map_copyin(clport_map, xxx_start, xxx_size, TRUE, FALSE,
			   &copy);
	if (kr != KERN_SUCCESS) {
		panic("copy_extract: vm_map_copyin.2: %d\n", kr);
	}

out:
#if 0
	copy->min_offset += (addr - trunc_page(addr));
	copy->max_offset -= (round_page(addr + length) - (addr + length));
#endif
	return copy;
}

vm_map_copy_t
xxx_copy_extract(msg, size)
	struct msg *msg;
	vm_size_t size;
{
	vm_map_copy_t copy;
	vm_offset_t xxx_space = 0;	/* XXX */
	vm_offset_t xxx_start = 0;	/* XXX */
	vm_size_t xxx_size = size;	/* XXX */
	boolean_t big = FALSE;
	kern_return_t kr;

	if (size % PAGE_SIZE) {
		panic("copy_extract: cannot happen\n");
	}
	if (size == 0) {
		return VM_MAP_COPY_NULL;
	}

	if (! msg->msg_first_pkt) {
		panic("__%d__\n", __LINE__);
	}

	xxx_start = xxx_space = (vm_offset_t) msg->msg_first_pkt->pkt_page;
	xxx_size = size;

	while (size > 0) {
		struct packet *packet = msg->msg_first_pkt;

		if (! packet || ! packet->pkt_page) {
			panic("copy_extract: no more data\n");
		}
		if (packet->pkt_page != (char *) xxx_space) {
			panic("found a hole... 0x%x != 0x%x\n",
			      packet->pkt_page, xxx_space);
		}
		xxx_space += PAGE_SIZE;
		size -= PAGE_SIZE;
		msg->msg_first_pkt = packet->pkt_next;
	}
	if (xxx_size != PAGE_SIZE) {
#if 0
		printf("  free active inac wire msg clport packet [%d][%x]\n",
		       xxx_size,
		       xxx_start);
		printf("%6d %6d %4d %4d %3d %6d %6d\n",
		       vm_page_free_count,
		       vm_page_active_count,
		       vm_page_inactive_count,
		       vm_page_wire_count,
		       k_msg,
		       k_clport,
		       k_packet);
		xk_foo();
#endif
	}
	if (vm_map_copyin(clport_map, xxx_start, xxx_size, TRUE, FALSE,
			  &copy) != KERN_SUCCESS) {
		panic("copy_extract: vm_map_copyin\n");
	}
	return copy;
}

#if 0
vm_map_copy_t
new_copy_extract(msg, size)
	struct msg *msg;
	vm_size_t size;
{
	vm_map_copy_t copy;
	vm_map_copy_t first_copy = VM_MAP_COPY_NULL;
	vm_map_copy_t last_copy;
	kern_return_t kr;

	if (size % PAGE_SIZE) {
		panic("copy_extract: cannot happen\n");
	}
	while (size > 0) {
		struct packet *packet = msg->msg_first_pkt;

		if (! packet || ! packet->pkt_page) {
			panic("copy_extract: no more data\n");
		}
		kr = vm_map_copyin(clport_map, packet->pkt_page, PAGE_SIZE,
				   TRUE, FALSE, &copy);
		if (kr != KERN_SUCCESS) {
			panic("copy_extract: vm_map_copyin\n");
		}
		if (first_copy) {
			link_it_in;
		} else {
			first_copy = last_copy = copy;
		}
		size -= PAGE_SIZE;
		msg->msg_first_pkt = packet->pkt_next;
	}
	return first_copy;
}
#endif

/*
 * Process a received message.
 * This routine is responsible for kfreeing all items XXX
 * and vm_deallocating all pages in message.
 * Caller (clport_receive_packet) will free msg and its packets.
 */
clport_receive(msg)
	struct msg *msg;
{
	unsigned long node;
	unsigned long size;
	mach_msg_return_t mr;
	struct packet *packet = msg->msg_first_pkt;
	ipc_kmsg_t kmsg;
	mach_msg_header_t *msgh;

	node = packet->pkt_remote;
	size = packet->pkt_data_len;
	kmsg = (ipc_kmsg_t) packet->pkt_data;
	msgh = (mach_msg_header_t *) &kmsg->ikm_header;

	msgh->msgh_local_port = (mach_port_t)
	    ipc_clport_get_proxy(msgh->msgh_local_port, TRUE);

	msgh->msgh_remote_port = (mach_port_t)
	    ipc_clport_get_proxy(msgh->msgh_remote_port, FALSE);

	if (! msgh->msgh_remote_port) {
		printf("(R%d) bogus remote port\n", ipc_mynode);
		/* XXX free stuff? or is this a panic? */
		return;
	}
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		clport_receive_complex(msg, msgh);
	}
	
	/*
	 * Free all packets
	 * XXX actually, it might be better if first packet
	 * were somebody else's responsibility, so that it could be
	 * on somebody's stack instead of having to call kalloc
	 */
	while (packet) {
		struct packet *next_packet = packet->pkt_next;
		packet_free(packet);
		packet = next_packet;
	}

	/*
	 * Hand it off to the send routine.
	 */
	kmsg->ikm_size = size;
	mr = ipc_mqueue_send(kmsg, 0, 0);
}

clport_receive_complex(msg, msgh)
	struct msg *msg;
	mach_msg_header_t *msgh;
{
	vm_offset_t saddr = (vm_offset_t) (msgh + 1);
	vm_offset_t eaddr = (vm_offset_t) msgh + msgh->msgh_size;

	while (saddr < eaddr) {
		mach_msg_type_long_t *type;
		mach_msg_type_size_t size;
		mach_msg_type_number_t number;
		boolean_t is_inline, longform, is_port;
		vm_size_t length;
		vm_offset_t addr;

		type = (mach_msg_type_long_t *) saddr;
		is_inline = type->msgtl_header.msgt_inline;
		longform = type->msgtl_header.msgt_longform;
		if (longform) {
			is_port = MACH_MSG_TYPE_PORT_ANY(type->msgtl_name);
			size = type->msgtl_size;
			number = type->msgtl_number;
			saddr += sizeof(mach_msg_type_long_t);
		} else {
			is_port = MACH_MSG_TYPE_PORT_ANY(type->msgtl_header.
							 msgt_name);
			size = type->msgtl_header.msgt_size;
			number = type->msgtl_header.msgt_number;
			saddr += sizeof(mach_msg_type_t);
		}

		/* calculate length of data in bytes, rounding up */
		length = ((number * size) + 7) >> 3;

		if (! is_inline && is_port) {
			panic("clport_receive_complex: out-of-line ports\n");
		}

		if (is_port) {
			ipc_port_t *ports = (ipc_port_t *) saddr;
			mach_msg_type_number_t i;

			for (i = 0; i < number; i++) {
				ports[i] = (ipc_port_t)
				    ipc_clport_get_proxy(ports[i],TRUE);
			}
		} else if (! is_inline) {
#if 0
			vm_offset_t addr = * (vm_offset_t *) saddr;
			vm_size_t size;

			size = round_page(addr + length) - trunc_page(addr);
/*if (size==8192)printf("8192 via msgh_id=%d\n", msgh->msgh_id);*/
			* (vm_map_copy_t *) saddr = copy_extract(msg, size);
#else
			vm_offset_t addr = * (vm_offset_t *) saddr;

			* (vm_map_copy_t *) saddr = copy_extract(msg, addr,
								 length);
#endif
		}
		if (is_inline) {
			saddr += (length + 3) &~ 3;
		} else {
			saddr += sizeof(vm_offset_t);
		}
	}
}

mach_msg_return_t
ipc_clport_send(kmsg)
	ipc_kmsg_t kmsg;
{
	mach_msg_header_t *msgh = (mach_msg_header_t *) &kmsg->ikm_header;
	ipc_port_t remote_port = (ipc_port_t) msgh->msgh_remote_port;
	ipc_port_t local_port = (ipc_port_t) msgh->msgh_local_port;
	struct packet *packet;

	packet = packet_allocate();

	/*
	 * Get the receiver's uid. There had better be one!
	 */
	if (! remote_port->ip_clport) {
		panic("ipc_clport_send\n");
	}
	msgh->msgh_remote_port =
	    (mach_port_t) remote_port->ip_clport->clport_uid;

	/*
	 * If there is a local port, get its uid (creating one if necessary).
	 */
	if (local_port) {
		if (! local_port->ip_clport) {
			ipc_clport_export(local_port);
		}
		msgh->msgh_local_port =
		    (mach_port_t) local_port->ip_clport->clport_uid;
	}

	/*
	 * Fill in the packet fields for the header.
	 * XXX we need some mechanism so that someone has a chance
	 * XXX to free this info!
	 */
	packet->pkt_remote = IP_CLPORT_NODE(msgh->msgh_remote_port);
	packet->pkt_msgid = 0; /* XXX */
	packet->pkt_id = 0;
	packet->pkt_page = (char *) 0;
	packet->pkt_data = (char *) kmsg;
	packet->pkt_data_len = msgh->msgh_size + IKM_OVERHEAD;

	/*
	 * If this is a complex message, we have more work to do.
	 */
	if (msgh->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		return ipc_clport_send_complex(packet, msgh);
	}
	/*
	 * This is a simple message, and we've done everything we need to,
	 * so send it off.
	 */
	packet->pkt_last = 0;
	clport_send(packet);
	packet_free(packet);
	return KERN_SUCCESS;
}

mach_msg_return_t
ipc_clport_send_complex(packet, msgh)
	struct packet *packet;
	mach_msg_header_t *msgh;
{
	vm_offset_t saddr = (vm_offset_t) (msgh + 1);
	vm_offset_t eaddr = (vm_offset_t) msgh + msgh->msgh_size;
	int xxx_port_count = 0;
	struct packet *last_packet = packet;
	int last_packet_id = 0;

	packet->pkt_next = 0;
	while (saddr < eaddr) {
		mach_msg_type_long_t *type;
		mach_msg_type_size_t size;
		mach_msg_type_number_t number;
		boolean_t is_inline, longform, is_port;
		vm_size_t length;
		vm_offset_t addr;

		type = (mach_msg_type_long_t *) saddr;
		is_inline = type->msgtl_header.msgt_inline;
		longform = type->msgtl_header.msgt_longform;
		if (longform) {
			is_port = MACH_MSG_TYPE_PORT_ANY(type->msgtl_name);
			size = type->msgtl_size;
			number = type->msgtl_number;
			saddr += sizeof(mach_msg_type_long_t);
		} else {
			is_port = MACH_MSG_TYPE_PORT_ANY(type->msgtl_header.
							 msgt_name);
			size = type->msgtl_header.msgt_size;
			number = type->msgtl_header.msgt_number;
			saddr += sizeof(mach_msg_type_t);
		}

		/* calculate length of data in bytes, rounding up */
		length = ((number * size) + 7) >> 3;

		/*
		 * XXX We need to notice here when receive rights are
		 * XXX being transfered, so that we can do the right thing.
		 */

		if (length == 0) {
			if (! is_inline) {
				* (vm_offset_t *) saddr = 0;
				saddr += sizeof(vm_offset_t);
			}
			continue;
		}
		if (is_port) {
			ipc_port_t *ports;
			mach_msg_type_number_t i;

			if (is_inline) {
				ports = (ipc_port_t *) saddr;
			} else {
				panic("clport out-of-line punt\n");
				ports = (ipc_port_t *) * (vm_offset_t *) saddr;
			}

			for (i = 0; i < number; i++) {
				if (! IP_VALID(ports[i])) {
					ports[i] = 0;
					continue;
				}
				if (! ports[i]->ip_clport) {
					ipc_clport_export(ports[i]);
				}
				ports[i] = (ipc_port_t)
				    ports[i]->ip_clport->clport_uid;
			}
			xxx_port_count += number;
		} else if (! is_inline) {
			vm_map_copy_t copy = * (vm_map_copy_t *) saddr;
			vm_size_t copy_len;
			vm_offset_t addr, off;
			kern_return_t kr;
			int wire;
			
			if (packet->pkt_page) {
				panic("clpbody: already one page!\n");
			}
			/*
			 * Copyout memory carried in the message.
			 * There may be several pages here.
			 * XXX should limit number of pages mapped in
			 * XXX at once to some reasonable number (like 8).
			 *
			 * Actually, limiting the total number of pages
			 * that can be mapped at once will change the whole
			 * structure of this loop. We need to be able
			 * to send pages out as they are generated.
			 * This dictates a optional second pass once we've
			 * reached a certain number of pages that we have
			 * mapped in.
			 */
			wire = vm_page_wire_count;
			addr = 0;
			kr = vm_map_copyout(clport_map, &addr, copy);


			if ((unsigned long)addr % PAGE_SIZE) {
				printf("== 0x%x\n", addr);
/*				addr = trunc_page(addr);*/
			}



#if 0
			if (node_self() == 0)
			printf("wire %d: %d -> %d\n",
			       __LINE__, wire, vm_page_wire_count);
#endif
			if (kr != KERN_SUCCESS) {
				copy_len = round_page(copy->max_offset)
				    - trunc_page(copy->min_offset);
				printf("clpbody: msgh_id=%d\n",
				       msgh->msgh_id);
				printf("clpbody: msgh_size=%d\n",
				       msgh->msgh_size);
				printf("clpbody: len=0x%x\n", copy_len);
				printf("clpbody: min=0x%x max=0x%x delta=%d\n",
				       copy->min_offset,
				       copy->max_offset,
				       copy->max_offset - copy->min_offset);
				panic("clpbody: can't copyout data: %d", kr);
			}
			
			copy_len = round_page(copy->max_offset)
			    - trunc_page(copy->min_offset);
			* (vm_offset_t *) saddr = copy->min_offset;
			
#if 0
			/*
			 * XXX hopefully VM_PROT_READ is okay;
			 * XXX why does MODP need VM_PROT_WRITE?
			 *
			 * XXX This problem below will probably go away
			 * XXX if we simply wire a page at a time.
			 */
			kr = vm_map_pageable(clport_map, addr,
					     addr + copy_len, VM_PROT_READ);
			if (kr != KERN_SUCCESS) {
				vm_offset_t addr2;

				copy_len = round_page(copy->max_offset)
				    - trunc_page(copy->min_offset);
				printf("clpbody: msgh_id=%d\n",
				       msgh->msgh_id);
				printf("clpbody: msgh_size=%d\n",
				       msgh->msgh_size);
				printf("clpbody: len=0x%x\n", copy_len);
				printf("clpbody: min=0x%x max=0x%x delta=%d\n",
				       copy->min_offset,
				       copy->max_offset,
				       copy->max_offset - copy->min_offset);
				printf("clpbody: can't wire data: %d", kr);


				addr2 = 0;
				kr = vm_allocate(clport_map, &addr2,
						 copy_len, TRUE);
				if (kr) {
					panic("clp %d %d\n", kr, __LINE__);
				}

				bcopy(addr, addr2, copy_len);
				kr = vm_deallocate(clport_map, addr,
						   copy_len);
				if (kr) {
					panic("clp %d %d\n", kr, __LINE__);
				}
				kr = vm_map_pageable(clport_map, addr2,
						     addr2 + copy_len,
						     VM_PROT_READ);
				if (kr != KERN_SUCCESS) {
					panic("clp %d %d\n", kr, __LINE__);
				}
				addr = addr2;
			}
#endif
#if 0
			/* XXX hopefully mapped in and will stay that way */
			kr = vm_map_pageable(clport_map, addr,
					     addr + copy_len, VM_PROT_NONE);
#endif
			for (off = 0; off < copy_len; off += PAGE_SIZE) {
				if (last_packet->pkt_page) {
					last_packet->pkt_next =
					    packet_allocate();
					last_packet = last_packet->pkt_next;
					last_packet->pkt_remote =
					    packet->pkt_remote;
					last_packet->pkt_msgid =
					    packet->pkt_msgid;
					last_packet->pkt_id = ++last_packet_id;
					last_packet->pkt_data_len = 0;
				}
				last_packet->pkt_page = (char *) (addr + off);
			}
		}
		if (is_inline) {
			saddr += (length + 3) &~ 3;
		} else {
			saddr += sizeof(vm_offset_t);
		}
	}


#if	xXx
	if (page_bail) {
		/* we've already counted remaining pages
		we should save some addresses, only do mapping on send
		for all but first page (maybe two pages) and send packets
		down here.
		*/
	}
#endif	xXx



	if (packet == last_packet) {
		packet->pkt_last = 0;
		clport_send(packet);
		packet_free(packet);
	} else {
		struct packet *first_packet = packet;
		struct packet *next_packet;

		last_packet->pkt_next = 0;
		for (packet = first_packet; packet; packet = packet->pkt_next){
			packet->pkt_last = last_packet_id;
		}
		for (packet = first_packet; packet; packet = next_packet) {
			clport_send(packet);
			next_packet = packet->pkt_next;
			packet_free(packet);
		}
	}
	return KERN_SUCCESS;
}

/*
 ******************************************************************************
 *
 *
 *	This should all be moved into ipc_proxy.c (except for init)
 *
 *
 ******************************************************************************
 */

unsigned long x_counter = 1;	/* 0 reserved for null port */
ipc_clport_t clport_list = (ipc_clport_t) 0;
vm_size_t clport_map_size = 1024 * 1024;

ipc_clport_init()
{
	extern ipc_port_t master_device_port;
	vm_offset_t min, max;

	clport_map = kmem_suballoc(kernel_map, &min, &max, clport_map_size,
				   FALSE); /* XXX pageable: should be FALSE */
	printf("ipc_clport_init: clport_map=0x%x\n", clport_map);


	ipc_mynode = node_self();
	master_device_port = ipc_port_alloc_kernel();
	if (master_device_port == IP_NULL)
	    panic("can't allocate master device port");
	ipc_clport_export(master_device_port);	/* allocates lid 1 */
}


ipc_port_t
master_device_port_at_node(node)
	int node;
{
	printf("%d: getting master device port for node %d\n",
	       node_self(), node);
	return ipc_clport_get_proxy(IP_CLPORT(node, 1), TRUE);
}

/*
 * create clport for local (real) port.
 */
ipc_clport_export(port)
	ipc_port_t port;
{
	ipc_clport_t clport;

	clport = port->ip_clport = clport_allocate();
	clport->clport_port = port;
	clport->clport_uid = IP_CLPORT(ipc_mynode, x_counter++);
/*printf("%d: export 0x%x\n", ipc_mynode, clport->clport_uid);*/

	clport->clport_next = clport_list;
	clport_list = clport;
}

/*
 * Find clport for given uid. If no such clport exists,
 * create one. Called with clport hash lock locked.
 */
ipc_port_t
ipc_clport_get_proxy(uid, create_proxy)
	unsigned long uid;
	boolean_t create_proxy;
{
	ipc_clport_t clport;
	ipc_port_t port;
	mach_port_t name;

	if (! uid) {
		return 0;
	}

	/*
	 * Sure would be nice to have some locking here,
	 * not to mention a more efficient data structure.
	 */
	for (clport = clport_list; clport; clport = clport->clport_next) {
		if (clport->clport_uid == uid) {
			return clport->clport_port;;
		}
	}
	if (! create_proxy) {
		printf("%d: get_proxy failed: 0x%x\n", node_self(), uid);
		return IP_NULL;
	}

	port = (ipc_port_t) io_alloc(IOT_PORT);
	if (port == IP_NULL)
		return IP_NULL;

	io_lock_init(&port->ip_object);
	port->ip_references = 1;
	port->ip_object.io_bits = io_makebits(TRUE, IOT_PORT, 0);

	/*
	 *	The actual values of ip_receiver_name aren't important,
	 *	as long as they are valid (not null/dead).
	 */

	ipc_port_init(port, ipc_space_remote, 1);

/*printf("%d: proxy  0x%x\n", ipc_mynode, uid);*/
	clport = port->ip_clport = clport_allocate();
	clport->clport_port = port;
	clport->clport_uid = uid;

	clport->clport_next = clport_list;
	clport_list = clport;

	return port;
}
#endif	MACH_CLPORT
