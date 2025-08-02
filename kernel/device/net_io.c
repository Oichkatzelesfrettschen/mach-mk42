/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log:	net_io.c,v $
 * Revision 2.14  91/02/14  14:37:07  mrt
 * 	Added garbage collection of dead filters.
 * 	[91/02/12  12:11:10  af]
 * 
 * Revision 2.13  91/02/05  17:09:54  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:30:04  mrt]
 * 
 * Revision 2.12  91/01/08  15:09:48  rpd
 * 	Replaced NET_KMSG_GET, NET_KMSG_FREE
 * 	with net_kmsg_get, net_kmsg_put, net_kmsg_collect.
 * 	Increased net_kmsg_ilist_min to 4.
 * 	[91/01/05            rpd]
 * 	Fixed net_rcv_msg_thread to round message sizes up to an int multiple.
 * 	[90/12/07            rpd]
 * 
 * 	Fixed net_rcv_msg_thread to not set vm_privilege.
 * 	[90/11/29            rpd]
 * 
 * Revision 2.11  90/09/09  23:20:00  rpd
 * 	Zero the mapped_size stats for non mappable interfaces.
 * 	[90/08/30  17:41:00  af]
 * 
 * Revision 2.10  90/08/27  21:55:18  dbg
 * 	If multiple filters receive a packet, copy the header as well as
 * 	the body.  Fix from Dan Julin.
 * 	[90/08/27            dbg]
 * 
 * 	Fix filter check to account for literal word.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.9  90/08/06  15:06:57  rwd
 * 	Fixed a bug in parse_net_filter(), that was reading the
 * 	litteral from NETF_PUSHLIT as an instruction.
 * 	[90/07/18  21:56:20  dpj]
 * 
 * Revision 2.8  90/06/02  14:48:14  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:57:43  rpd]
 * 
 * Revision 2.7  90/02/22  20:02:21  dbg
 * 	Track changes to kmsg structure.
 * 	[90/01/31            dbg]
 * 
 * Revision 2.6  90/01/11  11:42:20  dbg
 * 	Make run in parallel.
 * 	[89/12/15            dbg]
 * 
 * Revision 2.5  89/12/08  19:52:22  rwd
 * 	Picked up changes from rfr to minimize wired down memory
 * 	[89/11/21            rwd]
 * 
 * Revision 2.4  89/09/08  11:24:35  dbg
 * 	Convert to run in kernel task.  Removed some lint.
 * 	[89/07/26            dbg]
 * 
 * Revision 2.3  89/08/11  17:55:18  rwd
 * 	Picked up change from rfr which made zone collectable and
 * 	decreased min net_kmesg to 2.
 * 	[89/08/10            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:58  rwd
 * 	Changed device_map to device_task_map
 * 	[89/08/04            rwd]
 * 
 * 13-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.  
 *
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/98
 *
 *	Packet filter code taken from vaxif/enet.c written		 
 *		CMU and Stanford. 
 */

#include <device/net_status.h>
#include <device/net_io.h>
#include <device/if_hdr.h>
#include <device/io_req.h>
#include <device/ds_routines.h>

#include <mach/boolean.h>
#include <mach/vm_param.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_mqueue.h>

#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>

/*
 * Network IO.
 */


/*
 * Each interface has a write port and a set of read ports.
 * Each read port has one or more filters to determine what packets
 * should go to that port.
 */

/*
 * Receive port for net, with packet filter.
 */
struct net_rcv_port {
	queue_chain_t	chain;		/* list of open_descriptors */
	ipc_port_t	rcv_port;	/* port to send packet to */
	int		rcv_count;	/* number of packets received */
	int		priority;	/* priority for filter */
	filter_t	*filter_end;	/* pointer to end of filter */
	filter_t	filter[NET_MAX_FILTER];
					/* filter operations */
};
typedef struct net_rcv_port *net_rcv_port_t;

/*
 * List of net kmsgs queued to be sent to users.
 */
decl_simple_lock_data(,net_rcv_msg_lock)
struct ipc_kmsg_queue	net_rcv_msg_queue;

zone_t		net_rcv_zone;

/*
 * List of net kmsgs that can be touched at interrupt level.
 */
decl_simple_lock_data(,net_kmsg_ilist_lock)
struct ipc_kmsg_queue	net_kmsg_ilist;
int		net_kmsg_ilist_free = 0;

/*
 * This value is critical to network performance.
 * At least this many buffers should be sitting in net_kmsg_ilist.
 * If this is set too small, we will drop network packets.
 * Even a low drop rate (<1%) can cause severe network throughput problems.
 */
int		net_kmsg_ilist_min = 4;

int		net_kmsg_ilist_hits = 0;
int		net_kmsg_ilist_misses = 0;

int		net_kmsg_send_hits = 0;
int		net_kmsg_send_misses = 0;

vm_size_t	net_kmsg_size;

ipc_kmsg_t net_kmsg_get()
{
	register ipc_kmsg_t kmsg;
	int s;

	s = splimp();
	simple_lock(&net_kmsg_ilist_lock);
	kmsg = ipc_kmsg_queue_first(&net_kmsg_ilist);
	if (kmsg != IKM_NULL) {
		ipc_kmsg_rmqueue_first_macro(&net_kmsg_ilist, kmsg);
		net_kmsg_ilist_free--;
	}
	simple_unlock(&net_kmsg_ilist_lock);
	(void) splx(s);

	if (kmsg != IKM_NULL) {
		net_kmsg_ilist_hits++;
	} else {
		net_kmsg_ilist_misses++;
		thread_wakeup((int)&net_rcv_msg_queue);
	}
	return kmsg;
}

void net_kmsg_put(kmsg)
	register ipc_kmsg_t kmsg;
{
	int s;

	s = splimp();
	simple_lock(&net_kmsg_ilist_lock);
	ipc_kmsg_enqueue_macro(&net_kmsg_ilist, kmsg);
	net_kmsg_ilist_free++;
	simple_unlock(&net_kmsg_ilist_lock);
	(void) splx(s);
}

void net_kmsg_collect()
{
	register ipc_kmsg_t kmsg;
	int s;

	s = splimp();
	simple_lock(&net_kmsg_ilist_lock);
	while (net_kmsg_ilist_free > net_kmsg_ilist_min) {
		kmsg = ipc_kmsg_dequeue(&net_kmsg_ilist);
		net_kmsg_ilist_free--;
		simple_unlock(&net_kmsg_ilist_lock);
		(void) splx(s);

		net_kmsg_free(kmsg);

		s = splimp();
		simple_lock(&net_kmsg_ilist_lock);
	}
	simple_unlock(&net_kmsg_ilist_lock);
	(void) splx(s);
}

mach_msg_type_t header_type = {
	MACH_MSG_TYPE_BYTE,
	8,
	NET_HDW_HDR_MAX,
	TRUE,
	FALSE,
	FALSE,
	0
};

mach_msg_type_t packet_type = {
	MACH_MSG_TYPE_BYTE,	/* name */
	8,			/* size */
	0,			/* number */
	TRUE,			/* inline */
	FALSE,			/* longform */
	FALSE			/* deallocate */
};

void
net_rcv_msg_thread()
{
	register ipc_kmsg_t	kmsg;
	register int		count;
	ipc_port_t		dest;
	struct ifnet		*ifp;
	int			s;

	/*
	 * Set thread privileges and highest priority.
	 */
	thread_swappable(current_thread(), FALSE);
	thread_set_own_priority(0);

	/*
	 * This loop is structured so that if the network
	 * isn't in use, or in use through a mapped interface,
	 * then we don't allocate network message buffers.
	 */
	for(;;) {
	    /*
	     * Pick up any pending network messages and deliver them.
	     */
	    s = splimp();
	    simple_lock(&net_rcv_msg_lock);
	    while ((kmsg = ipc_kmsg_dequeue(&net_rcv_msg_queue)) != IKM_NULL) {
		simple_unlock(&net_rcv_msg_lock);
		splx(s);

		/*
		 * Fill in the rest of the kmsg.
		 */
		dest = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		count = net_kmsg(kmsg)->net_rcv_msg_packet_count;
		ifp = (struct ifnet *) kmsg->ikm_header.msgh_local_port;

		ikm_init_special(kmsg, IKM_SIZE_NETWORK);

		kmsg->ikm_header.msgh_bits =
			MACH_MSGH_BITS(MACH_MSG_TYPE_PORT_SEND, 0);
		/* remember message sizes must be rounded up */
		kmsg->ikm_header.msgh_size =
			((mach_msg_size_t) (sizeof(struct net_rcv_msg)
					    - NET_RCV_MAX + count))+3 &~ 3;
		/* interrupt routine couldn't create new send right */
		kmsg->ikm_header.msgh_remote_port = (mach_port_t)
			ipc_port_copy_send(dest);

		kmsg->ikm_header.msgh_local_port = MACH_PORT_NULL;
		kmsg->ikm_header.msgh_kind = MACH_MSGH_KIND_NORMAL;
		kmsg->ikm_header.msgh_id = NET_RCV_MSG_ID;

		net_kmsg(kmsg)->header_type = header_type;
		net_kmsg(kmsg)->packet_type = packet_type;
		net_kmsg(kmsg)->net_rcv_msg_packet_count = count;

		/*
		 * Send the packet to the destination port.  Drop it
		 * if the destination port is over its backlog.
		 */

		if (!MACH_PORT_VALID(kmsg->ikm_header.msgh_remote_port)) {
			(void) net_remove_filter(ifp, dest);
			ipc_kmsg_destroy(kmsg);
		} else
		if (ipc_mqueue_send(kmsg, MACH_SEND_TIMEOUT, 0) !=
		     					MACH_MSG_SUCCESS) {
			net_kmsg_send_misses++;
			ipc_kmsg_destroy(kmsg);
		} else
			net_kmsg_send_hits++;

		s = splimp();
		simple_lock(&net_rcv_msg_lock);

	    }
	    thread_sleep((int)&net_rcv_msg_queue,
			 simple_lock_addr(net_rcv_msg_lock),
			 FALSE);
	    splx(s);

	    /*
	     * Replenish net kmsg pool if low.  We don't have
	     * the locks necessary to look at net_kmsg_ilist_free,
	     * but that's OK because misread values aren't critical.
	     */
	    while (net_kmsg_ilist_free < net_kmsg_ilist_min) {
		kmsg = net_kmsg_alloc();
		net_kmsg_put(kmsg);
	    }
	}
}

void
reorder_queue(first, last)
	register queue_t	first, last;
{
	register queue_entry_t	prev, next;

	prev = first->prev;
	next = last->next;

	prev->next = last;
	next->prev = first;

	last->prev = prev;
	last->next = first;

	first->next = next;
	first->prev = last;
}

/*
 * Incoming packet.  Header has already been moved to proper place.
 */
void
net_filter(ifp, kmsg, count)
	register struct ifnet	*ifp;
	register ipc_kmsg_t	kmsg;
	unsigned int		count;
{
	register net_rcv_port_t	infp, prevfp;
	int		rcount;
	struct ipc_kmsg_queue send_list;
	register ipc_kmsg_t	new_kmsg;
	int		spl;

	ipc_kmsg_queue_init(&send_list);

	spl = splimp();
	simple_lock(&ifp->if_rcv_port_list_lock);

	queue_iterate(&ifp->if_rcv_port_list, infp, net_rcv_port_t, chain) {
	    if (net_do_filter(infp, net_kmsg(kmsg)->packet, count)) {
		/*
		 * Deliver copy of packet to this channel.
		 */
		if (ipc_kmsg_queue_empty(&send_list)) {
		    /*
		     * Only receiver, so far
		     */
		    new_kmsg = kmsg;
		}
		else {
		    /*
		     * Other receivers - must allocate net message,
		     * and copy
		     */
		    new_kmsg = net_kmsg_get();
		    if (new_kmsg == IKM_NULL)
			/* no more buffers...*/
			break;
		    bcopy(
			net_kmsg(kmsg)->packet,
			net_kmsg(new_kmsg)->packet,
			count);
		    bcopy(
			net_kmsg(kmsg)->header,
			net_kmsg(new_kmsg)->header,
			NET_HDW_HDR_MAX);
		}

		/*
		 * Set up the message to be sent to the reply port.
		 * We cannot manipulate port references at interrupt
		 * level, so the reference in the message is NOT
		 * counted.  When we add code to destroy filters,
		 * we must remove all pending messages from the
		 * net_rcv queue BEFORE deallocating the port.
		 */
		new_kmsg->ikm_header.msgh_remote_port =
					(mach_port_t) infp->rcv_port;
		net_kmsg(new_kmsg)->net_rcv_msg_packet_count = count;

		/*
		 * For garbage collection purposes stick a ref
		 * to the interface in the message itself: if we
		 * later find the rcv_port is dead we can quickly
		 * remove the corresponding filter(s) without
		 * sweeping all interfaces.
		 */
		new_kmsg->ikm_header.msgh_local_port = (mach_port_t) ifp;

		/* Add to list - give to sending thread at end */
		ipc_kmsg_enqueue(&send_list, new_kmsg);

		rcount = ++infp->rcv_count;

		/*
		 * See if ordering of filters is wrong
		 */
		if (infp->priority >= NET_HI_PRI) {
		    prevfp = (net_rcv_port_t) queue_prev(&infp->chain);
		    /*
		     * If infp is not the first element on the queue,
		     * and the previous element is at equal priority
		     * but has a lower count, then promote infp to
		     * be in front of prevfp.
		     */
		    if ((queue_t)prevfp != &ifp->if_rcv_port_list &&
			infp->priority == prevfp->priority) {
			/*
			 * Threshold difference to prevent thrashing
			 */
			if (100 + prevfp->rcv_count < rcount)
			    reorder_queue(&prevfp->chain, &infp->chain);
		    }
		    /*
		     * High-priority filter -> no more deliveries
		     */
		    break;
		}
	    }
	}
	simple_unlock(&ifp->if_rcv_port_list_lock);

	if (!ipc_kmsg_queue_empty(&send_list)) {
	    /* Push all messages at sending thread */

	    simple_lock(&net_rcv_msg_lock);
	    while ((kmsg = ipc_kmsg_dequeue(&send_list)) != IKM_NULL)
		    ipc_kmsg_enqueue(&net_rcv_msg_queue, kmsg);
	    simple_unlock(&net_rcv_msg_lock);

	    thread_wakeup((int)&net_rcv_msg_queue);
	}
	else {
	    /* Not sent - recycle */
	    net_kmsg_put(kmsg);
	}

	splx(spl);
}

boolean_t
net_do_filter(infp, data, count)
	net_rcv_port_t		infp;
	register char *		data;
	unsigned int		count;
{
	int		stack[NET_MAX_FILTER+1];
	register int	*sp;
	register filter_t	*fp, *fpe;
	register unsigned int	op, arg;


	sp = &stack[NET_MAX_FILTER];
	fp = &infp->filter[0];
	fpe = infp->filter_end;

	*sp = TRUE;

	while (fp < fpe) {
	    arg = *fp++;
	    op = NETF_OP(arg);
	    arg = NETF_ARG(arg);

	    switch (arg) {
		default:
		    arg -= NETF_PUSHWORD;
		    if (/*arg >= 0 &&*/ arg < count/sizeof(short))
			arg = *(((unsigned short *)data)+arg);
		    else
			return (FALSE);
		    break;
		case NETF_PUSHLIT:
		    arg = *fp++;
		    break;
		case NETF_PUSHZERO:
		    arg = 0;
		    break;
		case NETF_NOPUSH:
		    arg = *sp++;
		    break;
	    }
	    switch (op) {
		case NETF_OP(NETF_NOP):
		    *--sp = arg;
		    break;
		case NETF_OP(NETF_AND):
		    *sp &= arg;
		    break;
		case NETF_OP(NETF_OR):
		    *sp |= arg;
		    break;
		case NETF_OP(NETF_XOR):
		    *sp ^= arg;
		    break;
		case NETF_OP(NETF_EQ):
		    *sp = (*sp == arg);
		    break;
		case NETF_OP(NETF_NEQ):
		    *sp = (*sp != arg);
		    break;
		case NETF_OP(NETF_LT):
		    *sp = (*sp < arg);
		    break;
		case NETF_OP(NETF_LE):
		    *sp = (*sp <= arg);
		    break;
		case NETF_OP(NETF_GT):
		    *sp = (*sp > arg);
		    break;
		case NETF_OP(NETF_GE):
		    *sp = (*sp >= arg);
		    break;
		case NETF_OP(NETF_COR):
		    if (*sp++ == arg)
			return (TRUE);
		    break;
		case NETF_OP(NETF_CAND):
		    if (*sp++ != arg)
			return (FALSE);
		    break;
		case NETF_OP(NETF_CNOR):
		    if (*sp++ == arg)
			return (FALSE);
		    break;
		case NETF_OP(NETF_CNAND):
		    if (*sp++ != arg)
			return (TRUE);
		    break;
		case NETF_OP(NETF_LSH):
		    *sp <<= arg;
		    break;
		case NETF_OP(NETF_RSH):
		    *sp >>= arg;
		    break;
		case NETF_OP(NETF_ADD):
		    *sp += arg;
		    break;
		case NETF_OP(NETF_SUB):
		    *sp -= arg;
		    break;
	    }
	}
	return ((*sp) ? TRUE : FALSE);
}

/*
 * Check filter for invalid operations or stack over/under-flow.
 */
boolean_t
parse_net_filter(filter, count)
	register filter_t	*filter;
	unsigned int		count;
{
	register int	sp;
	register filter_t	*fpe = &filter[count];
	register filter_t	op, arg;

	sp = NET_MAX_FILTER;

	for (; filter < fpe; filter++) {
	    op = NETF_OP(*filter);
	    arg = NETF_ARG(*filter);

	    switch (arg) {
		case NETF_NOPUSH:
		    break;
		case NETF_PUSHLIT:
		    filter++;
		    if (filter >= fpe)
			return (FALSE);	/* literal value not in filter */
		    sp--;
		    break;
		case NETF_PUSHZERO:
		    sp--;
		    break;
		default:
		    if (arg < NETF_PUSHWORD)
			return (FALSE);
		    sp--;
		    /*
		     * Cannot check for packet bounds without packet!
		     */
		    break;
	    }
	    if (sp < 2) {
		return (FALSE);	/* stack overflow */
	    }
	    if (op == NETF_OP(NETF_NOP))
		continue;

	    /*
	     * all non-NOP operators are binary.
	     */
	    if (sp > NET_MAX_FILTER-2)
		return (FALSE);

	    sp++;
	    switch (op) {
		case NETF_OP(NETF_AND):
		case NETF_OP(NETF_OR):
		case NETF_OP(NETF_XOR):
		case NETF_OP(NETF_EQ):
		case NETF_OP(NETF_NEQ):
		case NETF_OP(NETF_LT):
		case NETF_OP(NETF_LE):
		case NETF_OP(NETF_GT):
		case NETF_OP(NETF_GE):
		case NETF_OP(NETF_COR):
		case NETF_OP(NETF_CAND):
		case NETF_OP(NETF_CNOR):
		case NETF_OP(NETF_CNAND):
		case NETF_OP(NETF_LSH):
		case NETF_OP(NETF_RSH):
		case NETF_OP(NETF_ADD):
		case NETF_OP(NETF_SUB):
		    break;
		default:
		    return (FALSE);
	    }
	}
	return (TRUE);
}

/*
 * Set a filter for a network interface.
 *
 * We are given a naked send right for the rcv_port.
 * If we are successful, we must consume that right.
 */
io_return_t
net_set_filter(ifp, rcv_port, priority, filter, filter_count)
	struct ifnet	*ifp;
	ipc_port_t	rcv_port;
	int		priority;
	filter_t	*filter;
	unsigned int	filter_count;
{
	boolean_t	found;
	register net_rcv_port_t	infp, new_infp;
	int		s;

	/*
	 * Check the filter syntax.
	 */
	if (!parse_net_filter(filter, filter_count))
	    return (D_INVALID_OPERATION);

	/*
	 * Look for an existing filter on the same reply port.
	 * If there is one, replace it.
	 */
	new_infp = 0;
	s = splimp();
	simple_lock(&ifp->if_rcv_port_list_lock);

	while (TRUE) {
	    found = FALSE;
	    queue_iterate(&ifp->if_rcv_port_list, infp, net_rcv_port_t, chain)
	    {
		if (infp->rcv_port == rcv_port) {
		    /*
		     * Remove the old filter from list, and re-use its
		     * data structure.
		     */
		    remqueue(&ifp->if_rcv_port_list, (queue_entry_t)infp);
		    found = TRUE;

		    /*
		     * We keep the existing reference for the receive
		     * port.
		     */
		    break;
		}
	    }
	    if (found)
		break;

	    if (new_infp) {
		/*
		 * No existing filter - use the new structure.
		 */
		infp = new_infp;
		new_infp = 0;
		break;
	    }

	    /*
	     * Must unlock to allocate a new filter.  If someone else
	     * has added a filter on the same port after we have
	     * allocated, release the new structure and indicate
	     * an error.
	     */
	    simple_unlock(&ifp->if_rcv_port_list_lock);
	    splx(s);

	    new_infp = (net_rcv_port_t)zalloc(net_rcv_zone);
	    new_infp->rcv_port = rcv_port;
	    new_infp->rcv_count = 0;

	    /*
	     * Re-lock and try the lookup again.
	     */
	    s = splimp();
	    simple_lock(&ifp->if_rcv_port_list_lock);
	}

	/*
	 * Change filter.
	 */
	{
	    register int i;

	    infp->priority = priority;
	    for (i = 0; i < filter_count; i++)
		infp->filter[i] = filter[i];
	    infp->filter_end = &infp->filter[i];
	}

	/*
	 * Insert according to priority.
	 */
	{
	    register net_rcv_port_t	next;

	    queue_iterate(&ifp->if_rcv_port_list, next, net_rcv_port_t, chain)
		if (infp->priority > next->priority)
		    break;
	    enqueue_tail((queue_t)&next->chain, (queue_entry_t)infp);
	}

	simple_unlock(&ifp->if_rcv_port_list_lock);
	splx(s);

	if (found) {
	    /*
	     * We found the receive port, so we already have a reference.
	     * Deallocate the extra reference.
	     */
	    ipc_port_release_send(rcv_port);
	}

	if (new_infp) {
	    /*
	     * Allocated one filter structure too many.
	     */
	    zfree(net_rcv_zone, (vm_offset_t) new_infp);
	}

	return (D_SUCCESS);
}

/*
 * Remove a filter when it destination port is found dead
 */
io_return_t
net_remove_filter(ifp, rcv_port)
	struct ifnet	*ifp;
	ipc_port_t	rcv_port;
{
	boolean_t	found;
	register net_rcv_port_t	infp, old_infp;
	int		s;

	/*
	 * Look for all filters with this same reply port.
	 * If there is one, release it.
	 */
	old_infp = 0;
	found = FALSE;
	s = splimp();
	simple_lock(&ifp->if_rcv_port_list_lock);

	queue_iterate(&ifp->if_rcv_port_list, infp, net_rcv_port_t, chain)
	{
		/*
		 * Release previous filter now
		 */
		if (old_infp) {
		    zfree(net_rcv_zone, (vm_offset_t) old_infp);
		    old_infp = 0;
		}

		if (infp->rcv_port == rcv_port) {
		    /*
		     * Remove the filter from list, releasing of its
		     * data structure will be done next loop around
		     * (after we moved on to the next list element)
		     */
		    remqueue(&ifp->if_rcv_port_list, (queue_entry_t)infp);
		    old_infp = infp;
		    found = TRUE;
		}
	}

	simple_unlock(&ifp->if_rcv_port_list_lock);
	splx(s);

	if (old_infp)
	    zfree(net_rcv_zone, (vm_offset_t) old_infp);

	return (found ? D_SUCCESS : D_NO_SUCH_DEVICE);
}

/*
 * Other network operations
 */
io_return_t
net_getstat(ifp, flavor, status, count)
	struct ifnet	*ifp;
	int		flavor;
	dev_status_t	status;		/* pointer to OUT array */
	unsigned int	*count;		/* OUT */
{
	switch (flavor) {
	    case NET_STATUS:
	    {
		register struct net_status *ns = (struct net_status *)status;

		ns->min_packet_size = ifp->if_header_size;
		ns->max_packet_size = ifp->if_header_size + ifp->if_mtu;
		ns->header_format   = ifp->if_header_format;
		ns->header_size	    = ifp->if_header_size;
		ns->address_size    = ifp->if_address_size;
		ns->flags	    = ifp->if_flags;
		ns->mapped_size	    = 0;

		*count = NET_STATUS_COUNT;
		break;
	    }
	    case NET_ADDRESS:
	    {
		register int	addr_byte_count;
		register int	addr_long_count;
		register int	i;

		addr_byte_count = ifp->if_address_size;
		addr_long_count = (addr_byte_count + (sizeof(long)-1))
					 / sizeof(long);
		bcopy((char *)ifp->if_address,
		      (char *)status,
		      (unsigned) addr_byte_count);
		if (addr_byte_count < addr_long_count * sizeof(long))
		    bzero((char *)status + addr_byte_count,
			  (unsigned) (addr_long_count * sizeof(long)
				      - addr_byte_count));

		for (i = 0; i < addr_long_count; i++) {
		    register long word;

		    word = status[i];
		    status[i] = htonl(word);
		}
		*count = addr_long_count;
		break;
	    }
	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

io_return_t
net_write(ifp, start, ior)
	register struct ifnet *ifp;
	int		(*start)();
	io_req_t	ior;
{
	int	s;
	kern_return_t	rc;

	/*
	 * Reject the write if the interface is down.
	 */
	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
	    return (D_DEVICE_DOWN);

	/*
	 * Reject the write if the packet is too large or too small.
	 */
	if (ior->io_count < ifp->if_header_size ||
	    ior->io_count > ifp->if_header_size + ifp->if_mtu)
	    return (D_INVALID_SIZE);

	/*
	 * Wire down the memory.
	 */

	rc = device_write_get(ior);
	if (rc != KERN_SUCCESS)
	    return (rc);

	/*
	 * Queue the packet on the output queue, and
	 * start the device.
	 */
	s = splimp();
	IF_ENQUEUE(&ifp->if_snd, ior);
	(*start)(ifp->if_unit);
	splx(s);

	return (D_IO_QUEUED);
}

/*
 * Initialize the whole package.
 */
void
net_io_init()
{
	register vm_size_t	size;

	size = sizeof(struct net_rcv_port);
	net_rcv_zone = zinit(size,
			     size * 1000,
			     PAGE_SIZE,
			     FALSE,
			     "net_rcv_port");

	size = ikm_plus_overhead(sizeof(struct net_rcv_msg));
	net_kmsg_size = round_page(size);

	simple_lock_init(&net_kmsg_ilist_lock);
	ipc_kmsg_queue_init(&net_kmsg_ilist);

	simple_lock_init(&net_rcv_msg_lock);
	ipc_kmsg_queue_init(&net_rcv_msg_queue);
}
