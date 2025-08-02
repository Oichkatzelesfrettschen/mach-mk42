/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * $Log:	server_loop.c,v $
 * Revision 2.3  91/02/05  17:29:13  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:17:32  mrt]
 * 
 * Revision 2.2  90/06/02  14:56:08  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:18:09  rpd]
 * 
 * Revision 2.1  89/08/03  15:52:01  rwd
 * Created.
 * 
 * Revision 2.4  89/01/10  23:31:54  rpd
 * 	Changed to require use of LOCAL_PORT to specify a port set.
 * 	Changed xxx_port_enable to port_set_add.
 * 	[89/01/10  13:33:38  rpd]
 * 
 * Revision 2.3  88/10/18  03:36:35  mwyoung
 * 	Allow the local port (on which a message is to be received) to
 * 	be redefined by this module's client.
 * 	[88/10/01            mwyoung]
 * 
 * Revision 2.2  88/07/23  01:21:04  rpd
 * Changed port_enable to xxx_port_enable.
 * 
 * 11-Jan-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Corrected error in timeout handling.
 *
 * 15-Dec-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Created.
 */

/*
 *	File:	kern/server_loop.c
 *
 *	A common server loop for builtin tasks.
 */

/*
 *	Must define symbols for:
 *		SERVER_NAME		String name of this module
 *		SERVER_LOOP		Routine name for the loop
 *		SERVER_DISPATCH		MiG function(s) to handle message
 *
 *	Must redefine symbols for pager_server functions.
 */

#include <mach/port.h>
#include <mach/message.h>
#include <vm/vm_kern.h>		/* for kernel_map */

void		SERVER_LOOP(rcv_set)
{
	mach_msg_size_t		max_size = 8192;
	mach_msg_return_t	r;
	mach_port_t		my_self;
	vm_offset_t		messages;
	register
	mach_msg_header_t	*in_msg;
	mach_msg_header_t	*out_msg;
	mach_msg_header_t	*tmp_msg;

	/*
	 *	Find out who we are.
	 */

	my_self = mach_task_self();

	/*
	 *	Allocate our message buffers.
	 *	[The buffers must reside in kernel space, since other
	 *	message buffers (in the mach_user_external module) are.]
	 */

	messages = kmem_alloc(kernel_map, 2 * max_size);
	if (messages == 0) {
		printf("%s: can't allocate message buffers", SERVER_NAME);
		panic(SERVER_NAME);
	}
	in_msg = (mach_msg_header_t *) messages;
	out_msg = (mach_msg_header_t *) (messages + max_size);

	/*
	 *	Service loop... receive messages and process them.
	 */

	for (;;) {
		/* receive first message */

	    receive_msg:
		r = mach_msg(in_msg, MACH_RCV_MSG, 0, max_size, rcv_set,
			     MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
		if (r == MACH_MSG_SUCCESS)
			break;

		printf("%s: receive failed, 0x%x.\n", SERVER_NAME, r);
	}

	for (;;) {
		/* process request message */

		(void) SERVER_DISPATCH(in_msg, out_msg);

		/* send reply and receive next request */

		if (out_msg->msgh_remote_port == MACH_PORT_NULL)
			goto receive_msg;

		r = mach_msg(out_msg, MACH_SEND_MSG|MACH_RCV_MSG,
			     out_msg->msgh_size, max_size, rcv_set,
			     MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
		if (r != MACH_MSG_SUCCESS) {
			printf("%s: send/receive failed, 0x%x.\n",
			       SERVER_NAME, r);
			goto receive_msg;
		}

		/* swap message buffers */

		tmp_msg = in_msg; in_msg = out_msg; out_msg = tmp_msg;
	}
}
