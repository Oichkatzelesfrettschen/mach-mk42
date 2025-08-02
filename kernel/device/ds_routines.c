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
 * $Log:	ds_routines.c,v $
 * Revision 2.14  91/02/05  17:09:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:28:57  mrt]
 * 
 * Revision 2.13  91/01/08  15:09:38  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.12  90/12/14  10:59:39  jsb
 * 	Moved mechanism for mapping global to local device names
 * 	to the machine-dependent function dev_forward_name.
 * 	[90/12/14  09:37:18  jsb]
 * 
 * 	Added device request forwarding to support inter-node device access.
 * 	[90/12/14  08:30:53  jsb]
 * 
 * Revision 2.11  90/10/25  14:44:32  rwd
 * 	Let ds_device_write proceed w/o a valid reply port.  This is used
 * 	by the unix server ether_output routine.
 * 	[90/10/22            rwd]
 * 	Fixed ds_write_done to use ds_device_write_inband_reply
 * 	when appropriate.
 * 	[90/10/18            rpd]
 * 	Check for invalid reply ports.
 * 	[90/10/17            rwd]
 * 
 * Revision 2.10  90/09/09  14:31:27  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.9  90/06/02  14:48:00  rpd
 * 	Cleaned up check for default pager in ds_read_done.
 * 	[90/04/29            rpd]
 * 
 * 	Fixed ds_read_done to leave memory wired if the read reply
 * 	is being sent to the default pager.
 * 	[90/04/05            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  21:55:28  rpd]
 * 
 * Revision 2.8  90/02/22  20:02:12  dbg
 * 	Use vm_map_copy routines.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.7  90/01/11  11:42:01  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.6  89/11/29  14:08:54  af
 * 	iodone() should set the IO_DONE flag.
 * 	[89/11/03  16:58:16  af]
 * 
 * Revision 2.5  89/11/14  10:28:19  dbg
 * 	Make read and write handle zero-length transfers correctly (used
 * 	to implement select).
 * 	[89/10/27            dbg]
 * 
 * Revision 2.4  89/09/08  11:24:17  dbg
 * 	Converted to run in kernel context.
 * 	Add list of wired pages to tail of IOR allocated for write.
 * 	Reorganized file: moved open/close to beginning, map to end.
 * 	[89/08/23            dbg]
 * 
 * Revision 2.3  89/08/31  16:18:46  rwd
 * 	Added ds_read_inband and support
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:39  rwd
 * 	Added ds_write_inband for use by tty and ds_device_map_device.
 * 	[89/07/17            rwd]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added device_reply_terminate.
 *
 *  3-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/89
 */

#include <mach_clboot.h>

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mig_errors.h>
#include <mach/port.h>
#include <mach/vm_param.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

#include <kern/queue.h>
#include <kern/zalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/sched_prim.h>

#include <vm/vm_map.h>
#include <vm/vm_kern.h>

#include <device/device_types.h>
#include <device/dev_hdr.h>
#include <device/conf.h>
#include <device/io_req.h>
#include <device/ds_routines.h>
#include <device/net_status.h>
#include <device/device_port.h>
#include <device/device_reply.h>

#include <machine/machparam.h>

io_return_t
ds_device_open(open_port, reply_port, reply_port_type,
	       mode, name, device_p)
	ipc_port_t	open_port;
	ipc_port_t	reply_port;
	mach_msg_type_name_t reply_port_type;
	dev_mode_t	mode;
	char *		name;
	device_t	*device_p;	/* out */
{
	register device_t	device;
	register kern_return_t	result;
	register io_req_t	ior;
	char			namebuf[64];

	/*
	 * Open must be called on the master device port.
	 */
	if (open_port != master_device_port)
	    return (D_INVALID_OPERATION);

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

#if	MACH_CLBOOT
	/*
	 * Map global device name to <node> + local device name.
	 */
	if (name[0] != '<') {
		extern char *dev_forward_name();

		name = dev_forward_name(name, namebuf, sizeof(namebuf));
	}
	/*
	 * Look for explicit node specifier, e.g., <2>sd0a.
	 * If found, then forward request to correct device server.
	 * If not found, then remove '<n>' and process locally.
	 *
	 * XXX should handle send-right reply_port as well as send-once XXX
	 */
	if (name[0] == '<') {
		char *n;
		int node = 0;

		for (n = &name[1]; *n != '>'; n++) {
			if (*n >= '0' && *n <= '9') {
				node = 10 * node + (*n - '0');
			} else {
				return (D_NO_SUCH_DEVICE);
			}
		}
		if (node == node_self()) {
			name = &n[1];	/* skip trailing '>' */
		} else {
			open_port = (ipc_port_t)
			    master_device_port_at_node(node);
			forward_device_open_send(open_port, reply_port,
						 mode, name);
			return (MIG_NO_REPLY);
		}
	}
#endif	MACH_CLBOOT

	/*
	 * Find the device.
	 */
	device = device_lookup(name);
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	/*
	 * If the device is being opened or closed,
	 * wait for that operation to finish.
	 */
	device_lock(device);
	while (device->state == DEV_STATE_OPENING ||
		device->state == DEV_STATE_CLOSING) {
	    device->io_wait = TRUE;
	    thread_sleep((int)device, simple_lock_addr(device->lock), TRUE);
	    device_lock(device);
	}

	/*
	 * If the device is already open, increment the open count
	 * and return.
	 */
	if (device->state == DEV_STATE_OPEN) {

	    if (device->flag & D_EXCL_OPEN) {
		/*
		 * Cannot open a second time.
		 */
		device_unlock(device);
		device_deallocate(device);
		return (D_ALREADY_OPEN);
	    }

	    device->open_count++;
	    device_unlock(device);
	    *device_p = device;
	    return (D_SUCCESS);
	    /*
	     * Return deallocates device reference while acquiring
	     * port.
	     */
	}

	/*
	 * Allocate the device port and register the device before
	 * opening it.
	 */
	device->state = DEV_STATE_OPENING;
	device_unlock(device);

	/*
	 * Allocate port, keeping a reference for it.
	 */
	device->port = ipc_port_alloc_kernel();
	if (device->port == IP_NULL) {
	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    device->port = IP_NULL;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);
	    device_deallocate(device);
	    return (KERN_RESOURCE_SHORTAGE);
	}

	dev_port_enter(device);

	/*
	 * Open the device.
	 */
	io_req_alloc(ior, 0);

	ior->io_device	= device;
	ior->io_unit	= device->dev_number;
	ior->io_op	= IO_OPEN | IO_CALL;
	ior->io_mode	= mode;
	ior->io_error	= 0;
	ior->io_done	= ds_open_done;
	ior->io_reply_port = reply_port;
	ior->io_reply_port_type = reply_port_type;

	result = (*device->dev_ops->d_open)(device->dev_number, (int)mode, ior);
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result via ds_open_done.
	 */
	ior->io_error = result;
	(void) ds_open_done(ior);

	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply already sent */
}

boolean_t
ds_open_done(ior)
	register io_req_t	ior;
{
	kern_return_t		result;
	register device_t	device;

	device = ior->io_device;
	result = ior->io_error;

	if (result != D_SUCCESS) {
	    /*
	     * Open failed.  Deallocate port and device.
	     */
	    dev_port_remove(device);
	    ipc_port_dealloc_kernel(device->port);
	    device->port = IP_NULL;

	    device_lock(device);
	    device->state = DEV_STATE_INIT;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);

	    device_deallocate(device);
	    device = DEVICE_NULL;
	}
	else {
	    /*
	     * Open succeeded.
	     */
	    device_lock(device);
	    device->state = DEV_STATE_OPEN;
	    device->open_count = 1;
	    if (device->io_wait) {
		device->io_wait = FALSE;
		thread_wakeup((int)device);
	    }
	    device_unlock(device);

	    /* donate device reference to get port */
	}
	/*
	 * Must explicitly convert device to port, since
	 * device_reply interface is built as 'user' side
	 * (thus cannot get translation).
	 */
	if (IP_VALID(ior->io_reply_port)) {
		(void) ds_device_open_reply(ior->io_reply_port,
					    ior->io_reply_port_type,
					    result,
					    convert_device_to_port(device));
	} else
		device_deallocate(device);

	return (TRUE);
}

io_return_t
ds_device_close(device)
	register device_t	device;
{
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	device_lock(device);

	/*
	 * If device will remain open, do nothing.
	 */
	if (--device->open_count > 0) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * If device is being closed, do nothing.
	 */
	if (device->state == DEV_STATE_CLOSING) {
	    device_unlock(device);
	    return (D_SUCCESS);
	}

	/*
	 * Mark device as closing, to prevent new IO.
	 * Outstanding IO will still be in progress.
	 */
	device->state = DEV_STATE_CLOSING;
	device_unlock(device);

	/*
	 * ? wait for IO to end ?
	 *   only if device wants to
	 */

	/*
	 * Remove the device-port association.
	 */
	dev_port_remove(device);
	ipc_port_dealloc_kernel(device->port);

	/*
	 * Close the device
	 */
	(*device->dev_ops->d_close)(device->dev_number);

	/*
	 * Finally mark it closed.  If someone else is trying
	 * to open it, the open can now proceed.
	 */
	device_lock(device);
	device->state = DEV_STATE_INIT;
	if (device->io_wait) {
	    device->io_wait = FALSE;
	    thread_wakeup((int)device);
	}
	device_unlock(device);

	return (D_SUCCESS);
}

/*
 * Write to a device.
 */
io_return_t
ds_device_write(device, reply_port, reply_port_type, mode, recnum,
		data, data_count, bytes_written)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_t		data;
	unsigned int		data_count;
	int			*bytes_written;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver
	 */
	io_req_alloc(ior, data_count);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_WRITE | IO_CALL;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	if (data == 0)
	    panic("ds_device_write: no data");

	/*
	 * And do the write.
	 */
	result = (*device->dev_ops->d_write)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return the number of bytes actually written.
	 * Discard the local copy of the data.
	 * ASSUMES that data has been moved to local map
	 * and wired down.
	 */
	device_write_dealloc(ior);

	*bytes_written = ior->io_count - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Write to a device, but memory is in message.
 */
io_return_t
ds_device_write_inband(device, reply_port, reply_port_type, mode, recnum,
		       data, data_count, bytes_written)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_inband_t	data;
	unsigned int		data_count;
	int			*bytes_written; /* out */
{
	register io_req_t	ior;
	register io_return_t	result;

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Package the write request for the device driver.
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_WRITE | IO_CALL | IO_INBAND;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= data;
	ior->io_count		= data_count;
	ior->io_alloc_size	= 0;
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_write_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type = reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	if (data == 0)
	    panic("ds_device_write: no data");

	/*
	 * And do the write.
	 */
	result = (*device->dev_ops->d_write)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return the number of bytes actually written.
	 */
	*bytes_written = ior->io_count - ior->io_residual;

	/*
	 * Remove the extra reference.
	 */
	device_deallocate(device);

	io_req_free(ior);
	return (result);
}

/*
 * Wire down incoming memory to give to device.
 */
kern_return_t
device_write_get(ior)
	register io_req_t	ior;
{
	vm_map_copy_t		io_copy;
	vm_offset_t		new_addr;
	register kern_return_t	result;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	/*
	 * Cannot do if data in-band.
	 */
	if (ior->io_op & IO_INBAND)
	    return (D_INVALID_OPERATION);

	/*
	 * Move data to device IO map.
	 */
	io_copy = (vm_map_copy_t) ior->io_data;
	result = vm_map_copyout(
			device_io_map,
			&new_addr,
			io_copy);
	if (result != KERN_SUCCESS)
	    return (result);

	ior->io_data = (char *)new_addr;
	ior->io_alloc_size = ior->io_count;	/* amount to deallocate */

	/*
	 * And wire it down.
	 */
	result = io_wire(device_io_map,
			trunc_page(ior->io_data),
			round_page(ior->io_data + ior->io_count),
			VM_PROT_READ,
			io_vm_page_list(ior));
	if (result != KERN_SUCCESS)
	    return (result);

	return (KERN_SUCCESS);
}

/*
 * Unwire and deallocate memory wired down for IO.
 */
void
device_write_dealloc(ior)
	register io_req_t	ior;
{
	if (ior->io_alloc_size == 0)
	    return;

	(void) io_unwire(device_io_map,
			trunc_page(ior->io_data),
			round_page(ior->io_data + ior->io_alloc_size),
			io_vm_page_list(ior));
	(void) vm_deallocate(device_io_map,
			(vm_offset_t) ior->io_data,
			(vm_size_t) ior->io_alloc_size);
}

/*
 * Send write completion message to client, and discard the data.
 */
boolean_t
ds_write_done(ior)
	register io_req_t	ior;
{
	device_write_dealloc(ior);
	if (IP_VALID(ior->io_reply_port)) {
	    (void) (*((ior->io_op & IO_INBAND) ?
		      ds_device_write_reply_inband :
		      ds_device_write_reply))(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (int) (ior->io_count -
						     ior->io_residual));
	}
	device_deallocate(ior->io_device);

	return (TRUE);
}

/*
 * Read from a device.
 */
io_return_t
ds_device_read(device, reply_port, reply_port_type, mode, recnum,
	       bytes_wanted, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	io_buf_ptr_t		*data;		/* out */
	unsigned int		*data_count;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif lint

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read request for the device driver
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_READ | IO_CALL;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= bytes_wanted;
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * And do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the IO was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result via ds_read_done.
	 */
	ior->io_error = result;
	(void) ds_read_done(ior);
	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply has already been sent. */
}

/*
 * Read from a device, but return the data 'inband.'
 */
io_return_t
ds_device_read_inband(device, reply_port, reply_port_type, mode, recnum,
		      bytes_wanted, data, data_count)
	register device_t	device;
	ipc_port_t		reply_port;
	mach_msg_type_name_t	reply_port_type;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	char			*data;		/* pointer to OUT array */
	unsigned int		*data_count;	/* out */
{
	register io_req_t	ior;
	register io_return_t	result;

#ifdef lint
	*data = *data;
	*data_count = *data_count;
#endif lint

	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * There must be a reply port.
	 */
	if (!IP_VALID(reply_port)) {
	    printf("ds_* invalid reply port\n");
	    Debugger("ds_* reply_port");
	    return (MIG_NO_REPLY);	/* no sense in doing anything */
	}

	/*
	 * Package the read for the device driver
	 */
	io_req_alloc(ior, 0);

	ior->io_device		= device;
	ior->io_unit		= device->dev_number;
	ior->io_op		= IO_READ | IO_CALL | IO_INBAND;
	ior->io_mode		= mode;
	ior->io_recnum		= recnum;
	ior->io_data		= 0;		/* driver must allocate data */
	ior->io_count		= 
	    ((bytes_wanted < sizeof(io_buf_ptr_inband_t)) ?
		bytes_wanted : sizeof(io_buf_ptr_inband_t));
	ior->io_alloc_size	= 0;		/* no data allocated yet */
	ior->io_residual	= 0;
	ior->io_error		= 0;
	ior->io_done		= ds_read_done;
	ior->io_reply_port	= reply_port;
	ior->io_reply_port_type	= reply_port_type;

	/*
	 * The ior keeps an extra reference for the device.
	 */
	device_reference(device);

	/*
	 * Do the read.
	 */
	result = (*device->dev_ops->d_read)(device->dev_number, ior);

	/*
	 * If the io was queued, delay reply until it is finished.
	 */
	if (result == D_IO_QUEUED)
	    return (MIG_NO_REPLY);

	/*
	 * Return result, via ds_read_done.
	 */
	ior->io_error = result;
	(void) ds_read_done(ior);
	io_req_free(ior);

	return (MIG_NO_REPLY);	/* reply has already been sent. */
}


/*
 * Allocate wired-down memory for device read.
 */
kern_return_t device_read_alloc(ior, size)
	register io_req_t	ior;
	register vm_size_t	size;
{
	register vm_offset_t	addr;

	/*
	 * Nothing to do if no data.
	 */
	if (ior->io_count == 0)
	    return (KERN_SUCCESS);

	if (ior->io_op & IO_INBAND) {
	    ior->io_data = (io_buf_ptr_t) zalloc(io_inband_zone);
	    ior->io_alloc_size = sizeof(io_buf_ptr_inband_t);
	} else {
	    size = round_page(size);
	    addr = kmem_alloc_wired(kernel_map, size);
		/* not zero-filled */
	    if (addr == 0)
		return (KERN_NO_SPACE);

	    ior->io_data = (io_buf_ptr_t) addr;
	    ior->io_alloc_size = size;
	}

	return (KERN_SUCCESS);
}

boolean_t ds_read_done(ior)
	io_req_t	ior;
{
	vm_offset_t		start_data, end_data;
	vm_offset_t		start_sent, end_sent;
	register vm_size_t	size_read;

	if (ior->io_error)
	    size_read = 0;
	else
	    size_read = ior->io_count - ior->io_residual;

	start_data  = (vm_offset_t)ior->io_data;
	end_data    = start_data + size_read;

	start_sent  = (ior->io_op & IO_INBAND) ? start_data :
						trunc_page(start_data);
	end_sent    = (ior->io_op & IO_INBAND) ? 
		start_data + ior->io_alloc_size : round_page(end_data);

	/*
	 * Zero memory that the device did not fill.
	 */
	if (start_sent < start_data)
	    bzero((char *)start_sent, start_data - start_sent);
	if (end_sent > end_data)
	    bzero((char *)end_data, end_sent - end_data);


	/*
	 * Touch the data being returned, to mark it dirty.
	 * If the pages were filled by DMA, the pmap module
	 * may think that they are clean.
	 */
	{
	    register vm_offset_t	touch;
	    register int		c;

	    for (touch = start_sent; touch < end_sent; touch += PAGE_SIZE) {
		c = *(char *)touch;
		*(char *)touch = c;
	    }
	}

	/*
	 * Send the data to the reply port - this
	 * unwires and deallocates it.
	 */
	if (ior->io_op & IO_INBAND) {
	    (void)ds_device_read_reply_inband(ior->io_reply_port,
					      ior->io_reply_port_type,
					      ior->io_error,
					      (char *) start_data,
					      size_read);
	} else {
	    extern boolean_t memory_manager_default_port();
	    vm_map_copy_t copy;
	    kern_return_t kr;

	    /*
	     *	memory_manager default_port tells us if the memory
	     *  is going to the default pager.  In that case,
	     *	we don't unwire the data.  This prevents deadlocks.
	     */

	    kr = vm_map_copyin(kernel_map, start_data, size_read, TRUE,
			       memory_manager_default_port(ior->io_reply_port),
			       &copy);
	    if (kr != KERN_SUCCESS)
		panic("read_done: vm_map_copyin");

	    (void)ds_device_read_reply(ior->io_reply_port,
				       ior->io_reply_port_type,
				       ior->io_error,
				       (char *) copy,
				       size_read);
	}

	/*
	 * Free any memory that was allocated but not sent.
	 */
	if (ior->io_count != 0) {
	    if (ior->io_op & IO_INBAND) {
		zfree(io_inband_zone, (vm_offset_t)ior->io_data);
	    } else {
		register vm_offset_t	end_alloc;

		end_alloc = start_sent + round_page(ior->io_alloc_size);
		if (end_alloc > end_sent)
		    (void) vm_deallocate(kernel_map,
					 end_sent,
					 end_alloc - end_sent);
	    }
	}

	device_deallocate(ior->io_device);

	return (TRUE);
}

io_return_t
ds_device_set_status(device, flavor, status, status_count)
	register device_t	device;
	int			flavor;
	dev_status_t		status;
	unsigned int		status_count;
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_setstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_get_status(device, flavor, status, status_count)
	register device_t	device;
	int			flavor;
	dev_status_t		status;		/* pointer to OUT array */
	unsigned int		*status_count;	/* out */
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return ((*device->dev_ops->d_getstat)(device->dev_number,
					      flavor,
					      status,
					      status_count));
}

io_return_t
ds_device_set_filter(device, receive_port, priority, filter, filter_count)
	register device_t	device;
	ipc_port_t		receive_port;
	int			priority;
	filter_t		filter[];	/* pointer to IN array */
	unsigned int		filter_count;
{
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	/*
	 * Request is absurd if no receive port is specified.
	 */
	if (!IP_VALID(receive_port))
	    return (D_INVALID_OPERATION);

	return ((*device->dev_ops->d_async_in)(device->dev_number,
					       receive_port,
					       priority,
					       filter,
					       filter_count));
}

io_return_t
ds_device_map(device, protection, offset, size, pager, unmap)
	register device_t	device;
	vm_prot_t		protection;
	vm_offset_t		offset;
	vm_size_t		size;
	mach_port_t		*pager;	/* out */
	boolean_t		unmap;	/* ? */
{
#ifdef	lint
	unmap = unmap;
#endif	lint
	/*
	 * Refuse if device is dead or not completely open.
	 */
	if (device == DEVICE_NULL)
	    return (D_NO_SUCH_DEVICE);

	if (device->state != DEV_STATE_OPEN)
	    return (D_NO_SUCH_DEVICE);

	/* XXX note that a CLOSE may proceed at any point */

	return (device_pager_setup(device, protection, offset, size,
				    pager));
}

queue_head_t		io_done_list;
decl_simple_lock_data(,	io_done_list_lock)

#define	splio	splsched	/* XXX must block ALL io devices */

void iodone(ior)
	register io_req_t	ior;
{
	register int	s;

	/*
	 * If this IO request should not be queued on completion,
	 * don't.
	 */
	s = splio();
	ior->io_op |= IO_DONE;
	if ((ior->io_op & IO_CALL) == 0) {
	    ior->io_op &= ~IO_WANTED;
	    thread_wakeup((int)ior);
	} else {
	    simple_lock(&io_done_list_lock);
	    enqueue_tail(&io_done_list, (queue_entry_t)ior);
	    thread_wakeup((int)&io_done_list);
	    simple_unlock(&io_done_list_lock);
	}
	splx(s);
}


void io_done_thread()
{
	register int	s;
	register io_req_t	ior;

	/*
	 * Set thread privileges and highest priority.
	 */
	thread_swappable(current_thread(), FALSE);
	current_thread()->vm_privilege = TRUE;
	thread_set_own_priority(0);

	while (TRUE) {
	    s = splio();
	    simple_lock(&io_done_list_lock);
	    ior = (io_req_t)dequeue_head(&io_done_list);
	    if (ior != 0) {
		simple_unlock(&io_done_list_lock);
		splx(s);

		if ((*ior->io_done)(ior)) {
		    /*
		     * IO done - free io_req_elt
		     */
		    io_req_free(ior);
		}
		/* else routine has re-queued it somewhere */
	    }
	    else {
		thread_sleep((int)&io_done_list,
			     simple_lock_addr(io_done_list_lock),
			     FALSE);
		splx(s);
	    }
	}
}

#define	DEVICE_IO_MAP_SIZE	(64 * PAGE_SIZE)

void ds_init()
{
	vm_offset_t	device_io_min, device_io_max;

	queue_init(&io_done_list);
	simple_lock_init(&io_done_list_lock);

	device_io_map = kmem_suballoc(kernel_map,
				      &device_io_min,
				      &device_io_max,
				      DEVICE_IO_MAP_SIZE,
				      FALSE);

	io_inband_zone = zinit(sizeof(io_buf_ptr_inband_t),
			    1000 * sizeof(io_buf_ptr_inband_t),
			    10 * sizeof(io_buf_ptr_inband_t),
			    FALSE,
			    "io inband read buffers");
}


/*
 * Clean up any io_req items that will return to a dead port.
 */
boolean_t
device_reply_search(dev, dead_port)
	register device_t	dev;
	mach_port_t		dead_port;
{
	return ((*dev->dev_ops->d_port_death)(dev->dev_number, dead_port));
}

boolean_t
device_reply_terminate(dead_port)
	mach_port_t	dead_port;
{
	return (dev_map(device_reply_search, dead_port));
}


iowait(bp)
io_req_t bp;
{
    int s;

    s = splbio();
    while ((bp->io_op&IO_DONE)==0) {
	assert_wait((int)bp, FALSE);
	thread_block((void (*)()) 0);
    }
    splx(s);
}
