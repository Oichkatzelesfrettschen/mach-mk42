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
 * $Log:	ipc_kobject.h,v $
 * Revision 2.3  91/02/05  17:26:40  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:12:58  mrt]
 * 
 * Revision 2.2  90/06/02  14:54:12  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:47:04  rpd]
 * 
 */
/*
 *	File:	kern/ipc_kobject.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Declarations for letting a port represent a kernel object.
 */

#ifndef	_KERN_IPC_KOBJECT_H_
#define _KERN_IPC_KOBJECT_H_

typedef unsigned int ipc_kobject_t;

#define	IKO_NULL	((ipc_kobject_t) 0)

typedef unsigned int ipc_kobject_type_t;

#define	IKOT_NONE		0
#define IKOT_THREAD		1
#define	IKOT_TASK		2
#define	IKOT_HOST		3
#define	IKOT_HOST_PRIV		4
#define	IKOT_PROCESSOR		5
#define	IKOT_PSET		6
#define	IKOT_PSET_NAME		7
#define	IKOT_PAGER		8
#define	IKOT_PAGING_REQUEST	9
#define	IKOT_DEVICE		10

extern struct ipc_kmsg *
ipc_kobject_server(/* ipc_kmsg_t */);

extern void
ipc_kobject_set(/* ipc_port_t, ipc_kobject_t, ipc_kobject_type_t */);

extern void
ipc_kobject_destroy(/* ipc_port_t */);

#define	null_conversion(port)	(port)

#endif	_KERN_IPC_KOBJECT_H_
