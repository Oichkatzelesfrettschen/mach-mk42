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
 * $Log:	ipc_sched.h,v $
 * Revision 2.4  91/02/05  17:26:58  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:13:32  mrt]
 * 
 * Revision 2.3  91/01/08  15:15:58  rpd
 * 	Removed ipc_thread_go_and_block.
 * 	Added ipc_thread_switch.
 * 	Added continuation argument to ipc_thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.2  90/06/02  14:54:27  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:46:16  rpd]
 * 
 */

#ifndef	_KERN_IPC_SCHED_H_
#define	_KERN_IPC_SCHED_H_

#include <kern/macro_help.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>

extern void
ipc_thread_go(/* thread_t */);

extern void
ipc_thread_will_wait(/* thread_t */);

extern void
ipc_thread_will_wait_with_timeout(/* thread_t, mach_msg_timeout_t */);

#define ipc_thread_block(thread, continuation)		\
MACRO_BEGIN						\
	assert(current_thread() == (thread));		\
	thread_block(continuation);			\
MACRO_END

extern boolean_t
ipc_thread_switch(/* thread_t, thread_t */);

#endif	_KERN_IPC_SCHED_H_
