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
 * $Log:	ipc_table.h,v $
 * Revision 2.3  91/02/05  17:24:19  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:52:19  mrt]
 * 
 * Revision 2.2  90/06/02  14:52:02  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:04:35  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_table.h
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Definitions for tables, used for IPC capabilities (ipc_entry_t)
 *	and dead-name requests (ipc_port_request_t).
 */

#ifndef	_IPC_IPC_TABLE_H_
#define	_IPC_IPC_TABLE_H_

#include <mach/machine/vm_types.h>
#include <kern/macro_help.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>

/*
 *	The is_table_next field of an ipc_space_t points to
 *	an ipc_table_size structure.  These structures must
 *	be elements of an array, ipc_table_entries.
 *
 *	The array must end with two elements with the same its_size value.
 *	Except for the terminating element, the its_size values must
 *	be strictly increasing.  The largest (last) its_size value
 *	must be less than or equal to MACH_PORT_INDEX(MACH_PORT_DEAD).
 *	This ensures that
 *		1) MACH_PORT_INDEX(MACH_PORT_DEAD) isn't a valid index
 *		in the table, so ipc_entry_get won't allocate it.
 *		2) MACH_PORT_MAKE(index+1, 0) and MAKE_PORT_MAKE(size, 0)
 *		won't ever overflow.
 *
 *
 *	The ipr_size field of the first element in a table of
 *	dead-name requests (ipc_port_request_t) points to the
 *	ipc_table_size structure.  The structures must be elements
 *	of ipc_table_dnrequests.  ipc_table_dnrequests must end
 *	with an element with zero its_size, and except for this last
 *	element, the its_size values must be strictly increasing.
 *
 *	The is_table_next field points to the ipc_table_size structure
 *	for the next larger size of table, not the one currently in use.
 *	The ipr_size field points to the currently used ipc_table_size.
 */

typedef unsigned int ipc_table_index_t;	/* index into tables */
typedef unsigned int ipc_table_elems_t;	/* size of tables */

typedef struct ipc_table_size {
	ipc_table_elems_t its_size;	/* number of elements in table */
	zone_t its_zone;		/* zone for allocating tables */
} *ipc_table_size_t;

#define	ITS_NULL	((ipc_table_size_t) 0)

extern ipc_table_size_t ipc_table_entries;
extern ipc_table_size_t ipc_table_dnrequests;

#define	ipc_table_alloc_macro(its, esize)				\
	(((its)->its_zone == ZONE_NULL) ?				\
	 kalloc((its)->its_size * (esize)) :				\
	 zalloc((its)->its_zone))

#define	ipc_table_free_macro(its, esize, table)				\
MACRO_BEGIN								\
	zone_t _zone = (its)->its_zone;					\
									\
	if (_zone == ZONE_NULL)						\
		kfree((table), (its)->its_size * (esize));		\
	else								\
		zfree(_zone, (table));					\
MACRO_END

extern void
ipc_table_init();

extern vm_offset_t
ipc_table_alloc(/* ipc_table_size_t, vm_size_t */);

extern void
ipc_table_free(/* ipc_table_size_t, vm_size_t, vm_offset_t */);

#define	it_entries_alloc(its)						\
	((ipc_entry_t) ipc_table_alloc((its),				\
			sizeof(struct ipc_entry)))

#define	it_entries_free(its, table)					\
	ipc_table_free((its), sizeof(struct ipc_entry),			\
			(vm_offset_t) (table))

#define	it_dnrequests_alloc(its)					\
	((ipc_port_request_t) ipc_table_alloc((its),			\
			sizeof(struct ipc_port_request)))

#define	it_dnrequests_free(its, table)					\
	ipc_table_free((its), sizeof(struct ipc_port_request),		\
			(vm_offset_t) (table))

#endif	_IPC_IPC_TABLE_H_
