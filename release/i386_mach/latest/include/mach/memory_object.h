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
 * $Log:	memory_object.h,v $
 * Revision 2.3  91/02/05  17:34:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:19:11  mrt]
 * 
 * Revision 2.2  90/06/02  14:58:56  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  22:35:26  rpd]
 * 
 * Revision 2.1  89/08/03  16:02:52  rwd
 * Created.
 * 
 * Revision 2.5  89/02/25  18:38:23  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.4  89/02/07  00:54:07  mwyoung
 * Relocated from vm/memory_object.h
 * 
 * Revision 2.3  89/01/30  22:08:42  rpd
 * 	Updated includes to the new style.  Fixed log.
 * 	Made variable declarations use "extern".
 * 	[89/01/25  15:25:20  rpd]
 */
/*
 *	File:	memory_object.h
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface definition.
 */

#ifndef	_MACH_MEMORY_OBJECT_H_
#define _MACH_MEMORY_OBJECT_H_

/*
 *	User-visible types used in the external memory
 *	management interface:
 */

#include <mach/port.h>

typedef	mach_port_t	memory_object_t;
					/* Represents a memory object ... */
					/*  Used by user programs to specify */
					/*  the object to map; used by the */
					/*  kernel to retrieve or store data */

typedef	mach_port_t	memory_object_control_t;
					/* Provided to a memory manager; ... */
					/*  used to control a memory object */

typedef	mach_port_t	memory_object_name_t;
					/* Used to describe the memory ... */
					/*  object in vm_regions() calls */

typedef	int		memory_object_copy_strategy_t;
					/* How memory manager handles copy: */
#define		MEMORY_OBJECT_COPY_NONE		0
					/* ... No special support */
#define		MEMORY_OBJECT_COPY_CALL		1
					/* ... Make call on memory manager */
#define		MEMORY_OBJECT_COPY_DELAY 	2
					/* ... Memory manager doesn't ... */
					/*     change data externally. */


#define		MEMORY_OBJECT_NULL	MACH_PORT_NULL

#ifdef	KERNEL
extern memory_object_t	memory_manager_default;
#endif	KERNEL
#endif	_MACH_MEMORY_OBJECT_H_
