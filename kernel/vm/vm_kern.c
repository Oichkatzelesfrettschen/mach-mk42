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
 * $Log:	vm_kern.c,v $
 * Revision 2.12  91/02/05  17:58:22  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:32:19  mrt]
 * 
 * Revision 2.11  91/01/08  16:44:59  rpd
 * 	Changed VM_WAIT to VM_PAGE_WAIT.
 * 	[90/11/13            rpd]
 * 
 * Revision 2.10  90/10/12  13:05:35  rpd
 * 	Only activate the page returned by vm_fault_page if it isn't
 * 	already on a pageout queue.
 * 	[90/10/09  22:33:09  rpd]
 * 
 * Revision 2.9  90/06/19  23:01:54  rpd
 * 	Picked up vm_submap_object.
 * 	[90/06/08            rpd]
 * 
 * Revision 2.8  90/06/02  15:10:43  rpd
 * 	Purged MACH_XP_FPD.
 * 	[90/03/26  23:12:33  rpd]
 * 
 * Revision 2.7  90/02/22  20:05:39  dbg
 * 	Update to vm_map.h.
 * 	Remove kmem_alloc_wait, kmem_free_wakeup, vm_move.
 * 	Fix copy_user_to_physical_page to test for kernel tasks.
 * 	Simplify v_to_p allocation.
 * 	Change PAGE_WAKEUP to PAGE_WAKEUP_DONE to reflect the
 * 	fact that it clears the busy flag.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.6  90/01/22  23:09:12  af
 * 	Undone VM_PROT_DEFAULT change, moved to vm_prot.h
 * 	[90/01/20  17:28:57  af]
 * 
 * Revision 2.5  90/01/19  14:35:57  rwd
 * 	Get new version from rfr
 * 	[90/01/10            rwd]
 * 
 * Revision 2.4  90/01/11  11:47:44  dbg
 * 	Remove kmem_mb_alloc and mb_map.
 * 	[89/12/11            dbg]
 * 
 * Revision 2.3  89/11/29  14:17:43  af
 * 	Redefine VM_PROT_DEFAULT locally for mips.
 * 	Might migrate in the final place sometimes.
 * 
 * Revision 2.2  89/09/08  11:28:19  dbg
 * 	Add special wiring code for IO memory.
 * 	[89/08/10            dbg]
 * 
 * 	Add keep_wired argument to vm_move.
 * 	[89/07/14            dbg]
 * 
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Optimize kmem_alloc.  Add kmem_alloc_wired.
 *	. Remove non-MACH include files.
 *	. Change vm_move to call vm_map_move.
 *	. Clean up fast_pager_data option.
 *
 * Revision 2.14  89/04/22  15:35:28  gm0w
 * 	Added code in kmem_mb_alloc to verify that requested allocation
 * 	will fit in the map.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.13  89/04/18  21:25:45  mwyoung
 * 	Recent history:
 * 	 	Add call to vm_map_simplify to reduce kernel map fragmentation.
 * 	History condensation:
 * 		Added routines for copying user data to physical
 * 		 addresses.  [rfr, mwyoung]
 * 		Added routines for sleep/wakeup forms, interrupt-time
 * 		 allocation. [dbg]
 * 		Created.  [avie, mwyoung, dbg]
 * 
 */
/*
 *	File:	vm/vm_kern.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Kernel memory management.
 */

#include <kern/assert.h>
#include <mach/kern_return.h>

#include <vm/vm_fault.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <mach/vm_param.h>

/*
 *	Variables exported by this module.
 */

vm_map_t	kernel_map;
vm_map_t	kernel_pageable_map;

/*
 *	kmem_alloc_pageable:
 *
 *	Allocate pageable memory to the kernel's address map.
 */

vm_offset_t kmem_alloc_pageable(map, size)
	vm_map_t		map;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	register kern_return_t	result;

	size = round_page(size);

	addr = vm_map_min(map);
	result = vm_map_find(map, VM_OBJECT_NULL, (vm_offset_t) 0,
				&addr, size, TRUE);
	if (result != KERN_SUCCESS) {
		return(0);
	}

	return(addr);
}

/*
 *	Allocate new wired pages in an object.
 *	The object is assumed to be mapped into the kernel map or
 *	a submap.
 *	The map must be locked.
 */
void kmem_alloc_pages(object, offset, start, end, protection)
	register vm_object_t	object;
	register vm_offset_t	offset;
	register vm_offset_t	start, end;
	vm_prot_t		protection;
{
	/*
	 *	Mark the pmap region as not pageable.
	 */
	pmap_pageable(kernel_pmap, start, end, FALSE);

	/*
	 *	Lock the object, and loop through the range...
	 */
	vm_object_lock(object);
	while (start < end) {
	    register vm_page_t	mem;

	    /*
	     *	Allocate a page
	     */
	    while ((mem = vm_page_alloc(object, offset))
			 == VM_PAGE_NULL) {
		vm_object_unlock(object);
		VM_PAGE_WAIT();
		vm_object_lock(object);
	    }

	    /*
	     *	Wire it down
	     */
	    vm_page_lock_queues();
	    vm_page_wire(mem);
	    vm_page_unlock_queues();

	    /*
	     *	Enter it in the kernel pmap
	     */
	    pmap_enter(kernel_pmap, start, VM_PAGE_TO_PHYS(mem),
			protection, TRUE);

	    mem->busy = FALSE;

	    start += PAGE_SIZE;
	    offset += PAGE_SIZE;
	}
	vm_object_unlock(object);
}

extern kern_return_t vm_map_find_entry();

/*
 *	Allocate wired-down memory in the kernel's address map
 *	or a submap.  The memory is zero-filled.
 *
 *	The memory is allocated in the kernel_object; therefore,
 *	it may NOT be copied with vm_map_copy.
 */
vm_offset_t kmem_alloc(map, size)
	register vm_map_t	map;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	register kern_return_t	result;
	register vm_offset_t	offset;
	extern vm_object_t	kernel_object;
	vm_map_entry_t		entry;

	size = round_page(size);

	/*
	 *	Use the kernel object for wired-down kernel pages.
	 *	Assume that no region of the kernel object is
	 *	referenced more than once.
	 */

	addr = vm_map_min(map);
	vm_map_lock(map);
	result = vm_map_find_entry(map,
				&addr,
				size,
				(vm_offset_t)0,	/* mask */
				TRUE,		/* anywhere */
				VM_OBJECT_NULL,
				(vm_offset_t)0,
				&entry);
	if (result != KERN_SUCCESS) {
	    vm_map_unlock(map);
	    return(0);
	}

	/*
	 *	Since we didn't know where the new region would
	 *	start, we couldn't supply the correct offset into
	 *	the kernel object.  Fill it in now.
	 */

	offset = addr - VM_MIN_KERNEL_ADDRESS;
	vm_object_reference(kernel_object);

	entry->object.vm_object = kernel_object;
	entry->offset = offset;

	/*
	 *	The memory is wired down.
	 */
	entry->wired_count++;

	/*
	 *	Since we have not given out this address yet,
	 *	it is safe to unlock the map.
	 */
	vm_map_unlock(map);

	/*
	 *	Allocate wired-down memory in the kernel_object,
	 *	for this entry, and enter it in the kernel pmap.
	 */
	kmem_alloc_pages(kernel_object,
			offset,
			entry->vme_start,
			entry->vme_end,
			entry->protection);

	/*
	 *	Zero the memory.
	 */
	blkclr((char *)addr, size);

	return(addr);
}

/*
 *	kmem_free:
 *
 *	Release a region of kernel virtual memory allocated
 *	with kmem_alloc, and return the physical pages
 *	associated with that region.
 */
void kmem_free(map, addr, size)
	vm_map_t		map;
	register vm_offset_t	addr;
	vm_size_t		size;
{
	(void) vm_map_remove(map, trunc_page(addr), round_page(addr + size));
}

/*
 *	kmem_suballoc:
 *
 *	Allocates a map to manage a subrange
 *	of the kernel virtual address space.
 *
 *	Arguments are as follows:
 *
 *	parent		Map to take range from
 *	size		Size of range to find
 *	min, max	Returned endpoints of map
 *	pageable	Can the region be paged
 */
vm_map_t kmem_suballoc(parent, min, max, size, pageable)
	register vm_map_t	parent;
	vm_offset_t		*min, *max;
	register vm_size_t	size;
	boolean_t		pageable;
{
	register kern_return_t	ret;
	vm_map_t	result;

	size = round_page(size);
	*min = (vm_offset_t) vm_map_min(parent);
	/*
	 *	Need reference on submap object because it is internal
	 *	to the vm_system.  vm_object_enter will never be called
	 *	on it (usual source of reference for vm_map_find).
	 */
	vm_object_reference(vm_submap_object);

	ret = vm_map_find(parent, vm_submap_object, (vm_offset_t) 0,
				min, size, TRUE);
	if (ret != KERN_SUCCESS) {
		printf("kmem_suballoc: bad status return of %d.\n", ret);
		panic("kmem_suballoc");
	}
	*max = *min + size;
	pmap_reference(vm_map_pmap(parent));
	result = vm_map_create(vm_map_pmap(parent), *min, *max, pageable);
	if (result == VM_MAP_NULL)
		panic("kmem_suballoc: cannot create submap");
	if ((ret = vm_map_submap(parent, *min, *max, result)) != KERN_SUCCESS)
		panic("kmem_suballoc: unable to change range to submap");
	return(result);
}

/*
 *	kmem_alloc_wired:
 *
 *	Allocate wired-down memory in the kernel's address map
 *	or a submap.  The memory is NOT zero-filled.  A new
 *	object is allocated for the memory; it may be copied
 * 	with vm_map_copy.
 *
 */
vm_offset_t kmem_alloc_wired(map, size)
	register vm_map_t	map;
	register vm_size_t	size;
{
	vm_offset_t		addr;
	register kern_return_t	result;
	register vm_object_t	object;
	vm_map_entry_t		entry;

	size = round_page(size);

	addr = vm_map_min(map);
	vm_map_lock(map);
	result = vm_map_find_entry(map,
				&addr,
				size,
				(vm_offset_t)0,	/* mask */
				TRUE,		/* anywhere */
				VM_OBJECT_NULL,
				(vm_offset_t)0,
				&entry);
	if (result != KERN_SUCCESS) {
	    vm_map_unlock(map);
	    return(0);
	}

	/*
	 *	Allocate a new object.
	 */
	object = vm_object_allocate(size);
	entry->object.vm_object = object;
	entry->offset = 0;

	/*
	 *	The memory is wired down.
	 */
	entry->wired_count++;

	/*
	 *	Since we have not given out this address yet,
	 *	it is safe to unlock the map.
	 */
	vm_map_unlock(map);

	/*
	 *	Allocate wired-down memory in the kernel_object,
	 *	for this entry, and enter it in the kernel pmap.
	 */
	kmem_alloc_pages(object,
			entry->offset,
			entry->vme_start,
			entry->vme_end,
			entry->protection);

	/*
	 *	Return the memory, not zeroed.
	 */
	return (addr);
}

/*
 *	kmem_init:
 *
 *	Initialize the kernel's virtual memory map, taking
 *	into account all memory allocated up to this time.
 */
void kmem_init(start, end)
	vm_offset_t	start;
	vm_offset_t	end;
{
	vm_offset_t	addr;
	extern vm_map_t	kernel_map;

	kernel_map = vm_map_create(pmap_kernel(), VM_MIN_KERNEL_ADDRESS, end,
				FALSE);

	addr = VM_MIN_KERNEL_ADDRESS;
	(void) vm_map_find(kernel_map, VM_OBJECT_NULL, (vm_offset_t) 0,
				&addr, (start - VM_MIN_KERNEL_ADDRESS),
				FALSE);
}

/*

 */
/*
 * Special wiring code for IO memory.
 */

/*
 * Fault in and wire down the pages in a map entry.
 */
void
kmem_fault_wire(map, entry, start, end, fault_type, page_list_p)
	vm_map_t		map;
	register vm_map_entry_t	entry;
	vm_offset_t		start;
	vm_offset_t		end;
	vm_prot_t		fault_type;
	vm_page_t		**page_list_p;
{
	register vm_offset_t	va = start;
	register vm_offset_t	end_addr = end;
	register vm_object_t	object = entry->object.vm_object;
	vm_offset_t		offset = entry->offset +
					(start - entry->vme_start);
	vm_prot_t		prot = entry->protection;

	vm_prot_t		req_fault_type;
	vm_prot_t		result_prot;
	vm_page_t		result_page;
	vm_page_t		top_page;

	/*
	 * Keep an extra reference to the object.
	 */
	vm_object_reference(object);

	for (;
	     va < end_addr;
	     va += PAGE_SIZE, offset += PAGE_SIZE) {

	    /*
	     * Fault in the page.
	     */
	    {
		register vm_fault_return_t	result;

		req_fault_type = prot & fault_type;

		do {
		    vm_object_lock(object);
		    vm_object_paging_begin(object);

		    result_prot = req_fault_type;

		    result = vm_fault_page(object,
					   offset,
					   req_fault_type,
					   FALSE,	/* not resident */
					   FALSE,	/* not interruptible */
					   &result_prot,
					   &result_page,
					   &top_page);
		    if (result == VM_FAULT_MEMORY_SHORTAGE) {
			VM_PAGE_WAIT();
		    }
		    /*
		     * If the object is a permanent object, the original
		     * data will move when the owner takes the copy-on-
		     * write fault.  If this is the case, force the
		     * copy now.
		     */
		    else if (result == VM_FAULT_SUCCESS) {
			if (result_page->object->copy != VM_OBJECT_NULL &&
				(fault_type & VM_PROT_WRITE) == 0)
			{
			    register vm_page_t	page = result_page;

			    /*
			     * Release the page.
			     */
			    vm_page_lock_queues();
			    if (!page->active && !page->inactive)
				vm_page_activate(page);
			    vm_page_unlock_queues();
			    PAGE_WAKEUP_DONE(page);
			    vm_object_paging_end(page->object);
			    vm_object_unlock(page->object);

			    if (top_page != VM_PAGE_NULL) {
				vm_object_lock(object);
				VM_PAGE_FREE(top_page);
				vm_object_paging_end(object);
				vm_object_unlock(object);
			    }

			    req_fault_type |= VM_PROT_WRITE;
			    result = VM_FAULT_RETRY;
			}
		    }
		} while (result != VM_FAULT_SUCCESS);
	    }

	    /*
	     * Enter a wired mapping.  Leave the page busy and
	     * its object with a paging reference.
	     */
	    {
		register vm_page_t	page = result_page;

		vm_object_unlock(page->object);
		PMAP_ENTER(map->pmap, va, page, result_prot, TRUE);

		/*
		 * Put the page in the page list.
		 */
		*(*page_list_p) = page;
		(*page_list_p)++;
	    }

	    if (top_page != VM_PAGE_NULL) {
		vm_object_lock(object);
		VM_PAGE_FREE(top_page);
		vm_object_paging_end(object);
		vm_object_unlock(object);
	    }
	}
	vm_object_deallocate(object);
}

/*
 * Wire kernel memory for IO.
 * Assumes that no other thread will be playing with the memory,
 * so we can unlock the map while faulting.
 *
 * Fault_type is the type of access for which no page faults may occur.
 * Because of problems with copy-on-write and wiring, leaves the pages
 * busy.  If wiring for read, read accesses may still proceed; copy-on-write
 * faults will block until the IO is complete.
 *
 * XXX ONLY WORKS FOR READ! XXX
 */
kern_return_t
io_wire(map, start, end, fault_type, page_list)
	register vm_map_t	map;
	vm_offset_t		start;
	vm_offset_t		end;
	vm_prot_t		fault_type;
	vm_page_t		*page_list;	/* pointer to OUT array */
{
	register vm_map_entry_t	entry;
	vm_map_entry_t		temp_entry;

	if (fault_type != VM_PROT_READ) {
	    panic("io_wire can only be used for VM_PROT_READ");
	}

	/*
	 * Inform the pmap module that this range of addresses
	 * will be wired down.
	 */
	pmap_pageable(map->pmap, start, end, FALSE);

	vm_map_lock(map);

	if (vm_map_lookup_entry(map, start, &temp_entry)) {
	    entry = temp_entry;
	}
	else
	    entry = temp_entry->vme_next;

	while (entry != vm_map_to_entry(map) && entry->vme_start < end) {

	    vm_offset_t	e_start, e_end;

	    /*
	     * This is only used on kernel maps, which never
	     * point to sharing maps.
	     */
	    if (entry->is_a_map) {
		panic("io_wire: kernel map has a sharing entry");
	    }

	    /*
	     * Now fault in the entry.  We unlock the map,
	     * assuming that (because kernel threads are
	     * well-behaved) no other thread will fault on
	     * this region, change its attributes, or
	     * (horrors) delete it.
	     */
	    if (entry->vme_start > start)
		e_start = entry->vme_start;
	    else
		e_start = start;

	    if (entry->vme_end < end)
		e_end = entry->vme_end;
	    else
		e_end = end;

	    /*
	     * Grab the map version...
	     */
	    vm_map_unlock(map);	/* trust me... */

	    kmem_fault_wire(map,
			    entry,
			    e_start,
			    e_end,
			    fault_type,
			    &page_list);

	    /*
	     * Lock the map again.  We trust that the entry
	     * still really exists.
	     */
	    vm_map_lock(map);

	    entry = entry->vme_next;
	}

	vm_map_unlock(map);

	return (KERN_SUCCESS);
}

void
io_unwire(map, start, end, page_list)
	vm_map_t	map;
	vm_offset_t	start;
	vm_offset_t	end;
	vm_page_t	page_list[];	/* in */
{
	register vm_offset_t	va;
	register vm_page_t	m;
	register vm_object_t	object;

	for (va = start; va < end; page_list++, va += PAGE_SIZE) {
	    m = *page_list;

	    pmap_change_wiring(map->pmap, va, FALSE);

	    object = m->object;

	    vm_object_lock(object);

	    vm_page_lock_queues();
	    if (!m->active && !m->inactive)
		vm_page_activate(m);
	    vm_page_unlock_queues();

	    PAGE_WAKEUP_DONE(m);

	    vm_object_paging_end(object);
	    vm_object_unlock(object);
	}

	pmap_pageable(map->pmap, start, end, FALSE);
}
