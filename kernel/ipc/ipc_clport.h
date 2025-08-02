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
 * $Log:	ipc_clport.h,v $
 * Revision 2.4  91/02/05  17:21:12  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:44:05  mrt]
 * 
 * Revision 2.3  90/12/14  11:00:02  jsb
 * 	Made clport uids 32 bits instead of 64. Remoted unused IP_IS_REMOTE.
 * 	[90/12/13  22:10:14  jsb]
 * 
 * Revision 2.2  90/09/28  16:54:39  jsb
 * 	First checkin.
 * 	[90/09/28  14:25:59  jsb]
 * 
 */
/*
 *	File:	ipc/ipc_clport.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Definitions for cluster ports.
 */

#ifndef	_IPC_IPC_CLPORT_H_
#define _IPC_IPC_CLPORT_H_

#include <mach_clport.h>

#if	MACH_CLPORT
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <kern/lock.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_port.h>

#define	IP_CLPORT_NULL	((ipc_clport_t) 0)

typedef struct ipc_clport {
	unsigned long		clport_uid;
	struct ipc_port	*	clport_port;	/* XXX */
	struct ipc_clport *	clport_next;	/* XXX */
} *ipc_clport_t;

extern int ipc_mynode;

#endif	MACH_CLPORT

#endif	_IPC_IPC_CLPORT_H_
