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
 * $Log:	ipc_table.c,v $
 * Revision 2.3  91/02/05  17:24:15  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:52:05  mrt]
 * 
 * Revision 2.2  90/06/02  14:51:58  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:04:20  rpd]
 * 
 */
/*
 *	File:	ipc/ipc_table.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Functions to manipulate tables of IPC capabilities.
 */

#include <mach/vm_param.h>
#include <mach/machine/vm_types.h>
#include <ipc/ipc_table.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_entry.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>

ipc_table_size_t ipc_table_entries;
unsigned int ipc_table_entries_size = 128;

ipc_table_size_t ipc_table_dnrequests;
unsigned int ipc_table_dnrequests_size = 64;

void
ipc_table_fill(its, num, min, elemsize)
	ipc_table_size_t its;	/* array to fill */
	unsigned int num;	/* size of array */
	unsigned int min;	/* at least this many elements */
	vm_size_t elemsize;	/* size of elements */
{
	unsigned int index;
	vm_size_t minsize = min * elemsize;
	vm_size_t size;
	vm_size_t incrsize;

	/* first use powers of two, up to the page size */

	for (index = 0, size = 1;
	     (index < num) && (size < PAGE_SIZE);
	     size <<= 1) {
		if (size >= minsize) {
			its[index].its_size = size / elemsize;
			its[index].its_zone = ZONE_NULL;
			index++;
		}
	}

	/* then increments of a page, then two pages, etc. */

	for (incrsize = PAGE_SIZE; index < num; incrsize <<= 1) {
		unsigned int period;

		for (period = 0;
		     (period < 15) && (index < num);
		     period++, size += incrsize) {
			if (size >= minsize) {
				its[index].its_size = size / elemsize;
				its[index].its_zone = ZONE_NULL;
				index++;
			}
		}
	}
}

void
ipc_table_init()
{
	ipc_table_size_t last;

	ipc_table_entries = (ipc_table_size_t)
		kalloc(sizeof(struct ipc_table_size) *
		       ipc_table_entries_size);
	assert(ipc_table_entries != ITS_NULL);

	ipc_table_fill(ipc_table_entries, ipc_table_entries_size - 1,
		       4, sizeof(struct ipc_entry));

	/* the last two elements should have the same size */

	ipc_table_entries[ipc_table_entries_size - 1].its_size =
		ipc_table_entries[ipc_table_entries_size - 2].its_size;


	ipc_table_dnrequests = (ipc_table_size_t)
		kalloc(sizeof(struct ipc_table_size) *
		       ipc_table_dnrequests_size);
	assert(ipc_table_dnrequests != ITS_NULL);

	ipc_table_fill(ipc_table_dnrequests, ipc_table_dnrequests_size - 1,
		       2, sizeof(struct ipc_port_request));

	/* the last element should have zero size */

	ipc_table_dnrequests[ipc_table_dnrequests_size - 1].its_size = 0;
}

vm_offset_t
ipc_table_alloc(its, esize)
	ipc_table_size_t its;
	vm_size_t esize;
{
	return ipc_table_alloc_macro(its, esize);
}

void
ipc_table_free(its, esize, table)
	ipc_table_size_t its;
	vm_size_t esize;
	vm_offset_t table;
{
	ipc_table_free_macro(its, esize, table);
}
