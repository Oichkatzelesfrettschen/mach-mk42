/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log:	io_req.h,v $
 * Revision 2.8  91/02/05  17:09:49  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:29:53  mrt]
 * 
 * Revision 2.7  90/06/02  14:48:08  rpd
 * 	Converted to new IPC.  Removed io_dev_port.
 * 	[90/03/26  21:56:39  rpd]
 * 
 * Revision 2.6  90/05/03  15:19:09  dbg
 * 	Add spare flags definition for device-dependent uses.
 * 	[90/03/14            dbg]
 * 
 * Revision 2.5  90/01/11  11:42:13  dbg
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.4  89/09/08  11:24:30  dbg
 * 	Allocate io_req elements from kalloc pool, to allow a
 * 	variable-length vm_page_t array at the end of the io_req_t.
 * 	[89/08/14            dbg]
 * 
 * 	Changed 'dev_hdr_t' to 'device_t'.
 * 	[89/08/01            dbg]
 * 
 * Revision 2.3  89/08/31  16:18:51  rwd
 * 	Added io_inband_zone
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:50  rwd
 * 	Added IO_INBAND.
 * 	[89/08/04            rwd]
 * 
 * 10-Oct-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	10/88
 */

#ifndef	_IO_REQ_
#define	_IO_REQ_

#include <mach/boolean.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/vm_param.h>
#include <kern/kalloc.h>
#include <vm/vm_page.h>
#include <device/device_types.h>
#include <device/dev_hdr.h>

#include <kern/macro_help.h>

/*
 * IO request element, queued on device for delayed replies.
 */
struct io_req {
	struct io_req *	io_next;	/* next, ... */
	struct io_req *	io_prev;	/* prev pointers: link in done,
					   defered, or in-progress list */
	device_t	io_device;	/* pointer to open-device structure */
	char *		io_dev_ptr;	/* pointer to driver structure -
					   filled in by driver if necessary */
	int		io_unit;	/* unit number ('minor') of device */
	int		io_op;		/* IO operation */
	dev_mode_t	io_mode;	/* operation mode (wait, truncate) */
	recnum_t	io_recnum;	/* starting record number for
					   random-access devices */

	union io_un {
	    io_buf_ptr_t    data;	/* data, for IO requests */
	} io_un;
#define	io_data		io_un.data

	long		io_count;	/* amount requested */
	long		io_alloc_size;	/* amount allocated, for read */
	long		io_residual;	/* amount NOT done */
	io_return_t	io_error;	/* error code */
	boolean_t	(*io_done)();	/* call when done - returns TRUE
					   if IO really finished */
	struct ipc_port	*io_reply_port;	/* reply port, for asynchronous
					   messages */
	mach_msg_type_name_t io_reply_port_type;
					/* send or send-once right? */
	struct io_req *	io_link;	/* forward link (for driver header) */
	struct io_req *	io_rlink;	/* reverse link (for driver header) */
	vm_size_t	io_ior_size;	/* size of IOR itself */
};
typedef struct io_req *	io_req_t;


/*
 * Flags and operations
 */

#define	IO_WRITE	0x00000000	/* operation is read */
#define	IO_READ		0x00000001	/* operation is write */
#define	IO_OPEN		0x00000002	/* operation is open */
#define	IO_DONE		0x00000100	/* operation complete */
#define	IO_ERROR	0x00000200	/* error on operation */
#define	IO_BUSY		0x00000400	/* operation in progress */
#define	IO_WANTED	0x00000800	/* wakeup when no longer BUSY */
#define	IO_BAD		0x00001000	/* bad disk block */
#define	IO_CALL		0x00002000	/* call io_done_thread when done */
#define IO_INBAND	0x00004000	/* mig call was inband */

#define	IO_SPARE_START	0x00010000	/* start of spare flags */

/*
 * Standard completion routine for io_requests.
 */
void	iodone(/* io_req_t */);

/*
 * The io request element may be followed by a variable-length array
 * of vm_page structure pointers, holding pointers to the physical memory
 * involved in the IO.
 */
#define	io_vm_page_list(ior)	((vm_page_t *)((ior)+1))
			    /*  (&(vm_page_t [])[0])
#define	io_vm_page_count(ior)	(atop(round_page((ior)->io_alloc_size)))

/*
 * Macros to allocate and free IORs.
 */
#define	io_req_alloc(ior,size)					\
	MACRO_BEGIN						\
	register vm_size_t	ior_size;			\
	ior_size = sizeof(struct io_req) +			\
		sizeof(vm_page_t) * atop(round_page(size));	\
	(ior) = (io_req_t)kalloc(ior_size);			\
	(ior)->io_ior_size = ior_size;				\
	MACRO_END

#define	io_req_free(ior)					\
	(kfree((char *)(ior), (ior)->io_ior_size))


zone_t	io_inband_zone; /* for inband reads */

#endif	_IO_REQ_
