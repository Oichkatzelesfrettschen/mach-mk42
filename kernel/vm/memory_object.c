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
 * $Log:	memory_object.c,v $
 * Revision 2.12  91/02/05  17:57:25  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:30:46  mrt]
 * 
 * Revision 2.11  91/01/08  16:44:11  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.10  90/10/25  14:49:30  rwd
 * 	Clean and not flush pages to lock request get moved to the
 * 	inactive queue.
 * 	[90/10/24            rwd]
 * 
 * Revision 2.9  90/08/27  22:15:49  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/08/06  15:08:16  rwd
 * 	Fix locking problems in memory_object_lock_request.
 * 	[90/07/12            rwd]
 * 	Fix memory_object_lock_request to only send contiguous pages as
 * 	one message.  If dirty pages were seperated by absent pages,
 * 	then the wrong thing was done.
 * 	[90/07/11            rwd]
 * 
 * Revision 2.7  90/06/19  23:01:38  rpd
 * 	Bring old single_page version of memory_object_data_provided up
 * 	to date.
 * 	[90/06/05            dbg]
 * 
 * 	Correct object locking in memory_object_lock_request.
 * 	[90/06/05            dbg]
 * 
 * Revision 2.6  90/06/02  15:10:14  rpd
 * 	Changed memory_object_lock_request/memory_object_lock_completed calls
 * 	to allow both send and send-once right reply-to ports.
 * 	[90/05/31            rpd]
 * 
 * 	Added memory_manager_default_port.
 * 	[90/04/29            rpd]
 * 	Converted to new IPC.  Purged MACH_XP_FPD.
 * 	[90/03/26  23:11:14  rpd]
 * 
 * Revision 2.5  90/05/29  18:38:29  rwd
 * 	New memory_object_lock_request from dbg.
 * 	[90/05/18  13:04:36  rwd]
 * 
 * 	Picked up rfr MACH_PAGEMAP changes.
 * 	[90/04/12  13:45:43  rwd]
 * 
 * Revision 2.4  90/05/03  15:58:23  dbg
 * 	Pass should_flush to vm_pageout_page: don't flush page if not
 * 	requested.
 * 	[90/03/28            dbg]
 * 
 * Revision 2.3  90/02/22  20:05:10  dbg
 * 	Pick up changes from mainline:
 * 
 * 		Fix placeholder page handling in memory_object_data_provided.
 * 		Old code was calling zalloc while holding a lock.
 * 		[89/12/13  19:58:28  dlb]
 * 
 * 		Don't clear busy flags on any pages in memory_object_lock_page
 * 		(from memory_object_lock_request)!!  Implemented by changing
 * 		PAGE_WAKEUP to not clear busy flag and using PAGE_WAKEUP_DONE
 * 		when it must be cleared.  See vm/vm_page.h.  With dbg's help.
 * 		[89/12/13            dlb]
 * 
 * 		Don't activate fictitious pages after freeing them in
 * 		memory_object_data_{unavailable,error}.  Add missing lock and
 * 		unlock of page queues when activating pages in same routines.
 * 		[89/12/11            dlb]
 * 		Retry lookup after using CLEAN_DIRTY_PAGES in
 * 		memory_object_lock_request().  Also delete old version of
 * 		memory_object_data_provided().  From mwyoung.
 * 		[89/11/17            dlb]
 * 
 * 		Save all page-cleaning operations until it becomes necessary
 * 		to block in memory_object_lock_request().
 * 		[89/09/30  18:07:16  mwyoung]
 * 
 * 		Split out a per-page routine for lock requests.
 * 		[89/08/20  19:47:42  mwyoung]
 * 
 * 		Verify that the user memory used in
 * 		memory_object_data_provided() is valid, even if it won't
 * 		be used to fill a page request.
 * 		[89/08/01  14:58:21  mwyoung]
 * 
 * 		Make memory_object_data_provided atomic, interruptible,
 * 		and serializable when handling multiple pages.  Add
 * 		optimization for single-page operations.
 * 		[89/05/12  16:06:13  mwyoung]
 * 
 * 		Simplify lock/clean/flush sequences memory_object_lock_request.
 * 		Correct error in call to pmap_page_protect() there.
 * 		Make error/absent pages eligible for pageout.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:10  dbg
 * 	Pass keep_wired argument to vm_move.  Disabled
 * 	host_set_memory_object_default.
 * 	[89/07/14            dbg]
 * 
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Clean up fast_pager_data option.  Remove pager_data_provided_inline.
 *
 * Revision 2.18  89/04/23  13:25:30  gm0w
 * 	Fixed typo to pmap_page_protect in memory_object_lock_request().
 * 	[89/04/23            gm0w]
 * 
 * Revision 2.17  89/04/22  15:35:09  gm0w
 * 	Commented out check/uprintf if memory_object_data_unavailable
 * 	was called on a permanent object.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.16  89/04/18  21:24:24  mwyoung
 * 	Recent history:
 * 	 	Add vm_set_default_memory_manager(),
 * 		 memory_object_get_attributes().
 * 		Whenever asked to clean a page, use pmap_is_modified, even
 * 		 if not flushing the data.
 * 		Handle fictitious pages when accepting data (or error or
 * 		 unavailable).
 * 		Avoid waiting in memory_object_data_error().
 * 
 * 	Previous history has been integrated into the documentation below.
 * 	[89/04/18            mwyoung]
 * 
 */
/*
 *	File:	vm/memory_object.c
 *	Author:	Michael Wayne Young
 *
 *	External memory management interface control functions.
 */

/*
 *	Interface dependencies:
 */

#include <mach/std_types.h>	/* For pointer_t */
#include <mach/mach_types.h>

#include <mach/kern_return.h>
#include <vm/vm_object.h>
#include <mach/memory_object.h>
#include <mach/memory_object_user.h>
#include <mach/memory_object_default.h>
#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/message.h>

/*
 *	Implementation dependencies:
 */
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <vm/pmap.h>		/* For copy_to_phys, pmap_clear_modify */
#include <kern/thread.h>		/* For current_thread() */
#include <kern/host.h>
#include <vm/vm_kern.h>		/* For kernel_map, vm_move */
#include <vm/vm_map.h>		/* For vm_map_pageable */
#include <ipc/ipc_port.h>

#include <mach_pagemap.h>
#if	MACH_PAGEMAP
#include <vm/vm_external.h>	
#endif	MACH_PAGEMAP

#include <kern/xpr.h>

int		memory_object_debug = 0;

/*
 */

memory_object_t	memory_manager_default = MACH_PORT_NULL;
decl_simple_lock_data(,memory_manager_default_lock)

/*
 *	Important note:
 *		All of these routines gain a reference to the
 *		object (first argument) as part of the automatic
 *		argument conversion. Explicit deallocation is necessary.
 */

/*
 *	If successful, destroys the map copy object.
 */
kern_return_t memory_object_data_provided(object, offset, data, data_cnt, lock_value)
	vm_object_t	object;
	vm_offset_t	offset;
	pointer_t	data;
	unsigned int	data_cnt;
	vm_prot_t	lock_value;
{
	kern_return_t	result = KERN_FAILURE;
	vm_page_t	m;
	vm_offset_t	kernel_data;

	XPR(XPR_MEMORY_OBJECT, ("memory_object_data_provided, object 0x%x, offset 0x%x",
				object, offset, 0, 0));

	/*
	 *	Look for bogus arguments
	 */

	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (lock_value & ~VM_PROT_ALL) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	if (data_cnt != PAGE_SIZE) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Adjust the offset from the memory object to the offset
	 *	within the vm_object.  Only accept whole pages.
	 */

	vm_object_lock(object);
	offset -= object->paging_offset;
	vm_object_paging_begin(object);
	vm_object_unlock(object);

	if (vm_map_copyout(ipc_kernel_map, &kernel_data, (vm_map_copy_t) data)
						!= KERN_SUCCESS)
		panic("modp: can't copyout data");

	if (vm_map_pageable(ipc_kernel_map,
			    kernel_data, kernel_data + PAGE_SIZE,
			    VM_PROT_READ|VM_PROT_WRITE) != KERN_SUCCESS)
		panic("modp: can't wire data");

	/*
	 *	Find the page waiting for our data.  If there's no
	 *	page, see whether we can allocate one, but don't bother
	 *	waiting.
	 */

	vm_object_lock(object);
	if ((memory_object_debug & 0x2) && (object->ref_count <= 1))
		printf("memory_object_data_provided: supplying data to a dead object");

	if ((m = vm_page_lookup(object, offset)) == VM_PAGE_NULL) {
		if (memory_object_debug & 0x4) {
			printf("memory_object_data_provided: object providing spurious data");
			printf("; object = 0x%x, offset = 0x%x\n", object, offset);
		}
		if ((m = vm_page_alloc(object, offset)) != VM_PAGE_NULL) {
			m->busy = m->absent = TRUE;
			object->absent_count++;
		} else
			result = KERN_RESOURCE_SHORTAGE;
	}

	/*
	 *	Only overwrite pages that are "absent".
	 *	Maybe someday we'll allow memory managers
	 *	to overwrite live data,	but not yet.
	 */

	if ((m != VM_PAGE_NULL) && m->busy && m->absent) {

		/*
		 *	If the requestor aborted its page fault,
		 *	then we merely throw this page away.
		 */

		if (m->fictitious) {
			VM_PAGE_FREE(m);
		} else {
			/*
			 *	Turn off the absent indication, but leave
			 *	the page busy, so we're the only thread
			 *	that can use it.  [The page was already busy,
			 *	so we don't have to take a paging reference
			 *	for ourselves.]
			 */

			m->absent = FALSE;

			/*
			 *	Copy in the data.  Since we're holding the
			 *	page busy, we can unlock the object.
			 *	[If MACH_XP_FPD, this is essential, because
			 *	the copy may fault on the user's memory.]
			 */

			vm_object_unlock(object);

			copy_to_phys(kernel_data, m->phys_addr, PAGE_SIZE);

			/*
			 *	Pick the object lock back up, and set the
			 *	page parameters.
			 */

			vm_object_lock(object);
			pmap_clear_modify(m->phys_addr);
			m->page_lock = lock_value;
			m->unlock_request = VM_PROT_NONE;

			/*
			 *	Clear the busy indication, and perform wakeups.
			 */

			PAGE_WAKEUP_DONE(m);
			result = KERN_SUCCESS;

			/*
			 *	Reactivate the page
			 */

			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();

			/*
			 *	Release absent reference
			 */
			vm_object_absent_release(object);

		}
	}

	vm_object_paging_end(object);
	vm_object_unlock(object);

	vm_deallocate(ipc_kernel_map, kernel_data, PAGE_SIZE);
	vm_object_deallocate(object);
	return(result);
}

#if	0

kern_return_t memory_object_data_provided(object, offset, data, data_cnt, lock_value)
	register
	vm_object_t	object;
	register
	vm_offset_t	offset;
	vm_offset_t	data;
	unsigned int	data_cnt;
	vm_prot_t	lock_value;
{
	kern_return_t	result = KERN_SUCCESS;
	register
	vm_page_t	m;
	register
	vm_page_t	placeholder_page;
	queue_head_t	placeholder_queue;
	vm_object_t	tmp_object;
	int		absent_count = 0;
	boolean_t	undo = TRUE;
	vm_offset_t	kernel_data;
	vm_size_t	kernel_data_cnt;

	XPR(XPR_MEMORY_OBJECT, ("memory_object_data_provided, object 0x%x, offset 0x%x",
				object, offset, 0, 0));

	/*
	 *	Look for bogus arguments
	 */

	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (lock_value & ~VM_PROT_ALL) {
		vm_object_deallocate(object);
		return(KERN_INVALID_ARGUMENT);
	}

	/*
	 *	Adjust the offset from the memory object to the offset
	 *	within the vm_object.
	 *
	 *	Once we look at the paging_offset, we must ensure that
	 *	the object cannot be collapsed.  This means taking a
	 *	paging reference for now.
	 */

	vm_object_lock(object);
	offset -= object->paging_offset;
	vm_object_paging_begin(object);
	vm_object_unlock(object);

	/*
	 *	Get a temporary object to hold the pages as we
	 *	copy them in.  The pages in this object are known
	 *	only to this thread and the pageout daemon.
	 */

	if ((tmp_object = current_thread()->tmp_object) == VM_OBJECT_NULL) {
		tmp_object = current_thread()->tmp_object =
			vm_object_allocate((vm_size_t) 0);
	}

	/*
	 *	Copy the data into the kernel
	 */

	if (vm_map_copyout(ipc_kernel_map, &kernel_data, (vm_map_copy_t) data)
						!= KERN_SUCCESS) {
		printf("memory_object_data_provided: %s",
			"memory_object_data_provided: cannot move data");
		vm_object_deallocate(object);
		return(KERN_RESOURCE_SHORTAGE);
	}
	data = kernel_data;
	kernel_data_cnt = data_cnt;

	/*
	 *	First pass: copy in the data
	 */

	queue_init(&placeholder_queue);
	for (;
	     data_cnt >= PAGE_SIZE;
	     data_cnt -= PAGE_SIZE, data += PAGE_SIZE, offset += PAGE_SIZE) {

		/*
		 *	See whether there is a page waiting for our data.
		 *	Get a placeholder page now before grabbing locks.
		 *	Initialization postponed until page is used.
		 */
		placeholder_page =
			(vm_page_t) zalloc(vm_page_fictitious_zone);
		vm_object_lock(object);
		if ((memory_object_debug & 0x2) && (object->ref_count <= 1))
			printf("memory_object_data_provided: supplying data to a dead object");

		if ((m = vm_page_lookup(object, offset)) == VM_PAGE_NULL) {
			/*
			 *	No page waiting -- see whether we can allocate
			 *	one, but don't bother waiting.
			 */

			if (memory_object_debug & 0x4) {
				printf("memory_object_data_provided: object providing spurious data");
				printf("; object = 0x%x, offset = 0x%x\n", object, offset);
			}
			if ((m = vm_page_alloc(object, offset)) == VM_PAGE_NULL) {
				char		verify_data;

				result = KERN_RESOURCE_SHORTAGE;

			 VerifyPage:

				/*
				 *	Won't actually page in this page,
				 *	so free placeholder that we won't use.
				 */

				vm_object_unlock(object);
				zfree(vm_page_fictitious_zone,
					(vm_offset_t)placeholder_page);

				/*
				 *	Even if we cannot use the data, we
				 *	must verify that the data being
				 *	supplied is valid, in order to
				 *	preserve the appearance of a msg_send
				 *	operation.
				 *
				 *	Note that the user-supplied data
				 *	may span more than one page.
				 */

				if (copyin(((char *) data),
					   &verify_data, 1) ||
				    (!page_aligned((vm_offset_t) data) &&
				     copyin(((char *) data) + (PAGE_SIZE-1),
					   &verify_data, 1))) {
					/*
					 *	Indicate that an underlying
					 *	IPC error should have
					 *	occurred.
					 */

					current_thread()->ith_state = MACH_SEND_INVALID_MEMORY;
					result = KERN_FAILURE;

					/*
					 *	Remember to undo changes in the second
					 *	pass, rather than committing them.
					 */

					undo = TRUE;

					/*
					 *	There's no point in continuing the first
					 *	pass at this point.
					 */

					break;
				}

				continue;
			}
		} else {
			/*
			 *	Only overwrite pages that are "absent".
			 */

			if (!(m->busy && m->absent))
				goto VerifyPage;
		}

		/*
		 *	At this point, the page in question should be
		 *	"busy".  If it was previously requested, then it
		 *	will also be "absent".
		 */

		/*
		 *	Move the page to the alternate object,
		 *	and install a (busy) placeholder page in
		 *	the original object.
		 *
		 *	XXXO This step could be combined with the
		 *	lookup/allocate step above.
		 */

		vm_object_lock(tmp_object);
		vm_page_rename(m, tmp_object, offset);
		vm_object_unlock(tmp_object);

		vm_page_init(placeholder_page, object, offset, (vm_offset_t) 0);
		placeholder_page->fictitious = TRUE;

		/*
		 *	Remember whether the page has been
		 *	explicitly requested, in case we later abort.
		 */

		if (m->absent) {
			placeholder_page->was_absent = TRUE;
			m->absent = FALSE;
			absent_count++;
		}

		/*
		 *	Enqueue the placeholder page so that we can
		 *	quickly make a second pass.
		 *
		 *	WARNING: we are using the pageout queue
		 *	chain for this list.  This is not a serious
		 *	problem, since the placeholder is wired,
		 *	but if the pageout system changes, this
		 *	will require re-engineering.
		 */

		queue_enter(&placeholder_queue, placeholder_page,
					 vm_page_t, pageq);

		/*
		 *	Copy the data into the page (now in the
		 *	temporary object).
		 *
		 *	Note that we cannot write into fictitious
		 *	pages.
		 *
		 *	XXXO We could be optimistic and try to
		 *	allocate a new page again.
		 */

		if (m->fictitious)
			goto VerifyPage;

		/*
		 *	Copy in the data.
		 *
		 *	Note that we must unlock the object, because
		 *	the copyin may fault (and may be interrupted).
		 */

		vm_object_unlock(object);

		copy_to_phys((vm_offset_t) data, m->phys_addr, data_cnt);

		pmap_clear_modify(m->phys_addr);

		/*
		 *	Make this page eligible for pageout if it
		 *	wasn't already requested.
		 *
		 *	This prevents our thread from consuming
		 *	several physical pages and then faulting on
		 *	other bogus user-managed data.
		 */

		if (!placeholder_page->was_absent) {
			m->busy = FALSE;
			vm_page_lock_queues();
			if (!m->active)
				vm_page_activate(m);
			vm_page_unlock_queues();
		}
	}
	
	if (data_cnt != 0)
		uprintf("memory_object_data_provided: partial page discarded\n");

	/*
	 *	Second pass: atomically make the data available.
	 *	[If an error occurred in the first pass, this pass *undoes*
	 *	the changes done in the first pass.]
	 */

	vm_object_lock(object);
	vm_object_lock(tmp_object);
	for (
	    placeholder_page = (vm_page_t) queue_first(&placeholder_queue);
	    !queue_end(&placeholder_queue, (queue_entry_t) placeholder_page);
	    placeholder_page = (vm_page_t) queue_first(&placeholder_queue)
	    ) {
		boolean_t	was_absent = placeholder_page->was_absent;

		/*
		 *	We must look up the real page, as it may have
		 *	disappeared.
		 *
		 *	XXXO We could step through the alternate object's
		 *	page queue, since it should never be reordered.
		 *	[Even if it is reordered, we can always throw
		 *	away pages that we don't want anyway.]
		 */

		offset = placeholder_page->offset;
		queue_remove(&placeholder_queue, placeholder_page,
					vm_page_t, pageq);
		VM_PAGE_FREE(placeholder_page);

		if ((m = vm_page_lookup(tmp_object, offset)) != VM_PAGE_NULL) {
			assert(m == (vm_page_t) queue_first(&tmp_object->memq));

			if (undo) {
				/*
				 *	Put back the page if it was originally
				 *	requested; otherwise, throw it away.
				 */

				if (was_absent) {
					vm_page_rename(m, object, offset);
					m->absent = TRUE;
				} else
					VM_PAGE_FREE(m);

			} else if (m->fictitious) {
				VM_PAGE_FREE(m);
			} else {
				/*
				 *	Put the real page back
				 */

				vm_page_rename(m, object, offset);

				/*
				 *	If the page was explicitly requested,
				 *	it wasn't made eligible for pageout in
				 *	the first pass, so it must be now.
				 */

				if (was_absent) {
					m->busy = FALSE;
					vm_page_lock_queues();
					if (!m->active)
						vm_page_activate(m);
					vm_page_unlock_queues();
				} else {
					vm_page_lock_queues();
					assert(m->active || m->inactive);
					vm_page_unlock_queues();
					assert(!m->busy && !m->absent);
				}

				/*
				 *	Set the page parameters.
				 */

				m->page_lock = lock_value;
				m->unlock_request = VM_PROT_NONE;

				PAGE_WAKEUP(m);
			}
		}
	}
	assert(queue_empty(&tmp_object->memq));
	vm_object_unlock(tmp_object);

	/*
	 *	Release all of the absent_count references at once.
	 */

	if (absent_count && !undo) {
		object->absent_count -= absent_count;
		vm_object_wakeup(object, VM_OBJECT_EVENT_ABSENT_COUNT);
	}

	vm_object_paging_end(object);

	vm_object_unlock(object);

	vm_object_deallocate(object);

	if (kernel_data_cnt != 0)
		(void) vm_deallocate(ipc_kernel_map,
				     kernel_data, kernel_data_cnt);

	return(result);
}

#endif	0

kern_return_t memory_object_data_error(object, offset, size, error_value)
	vm_object_t	object;
	vm_offset_t	offset;
	vm_size_t	size;
	kern_return_t	error_value;
{
	XPR(XPR_MEMORY_OBJECT, ("memory_object_data_error, object 0x%x, offset 0x%x",
				object, offset, 0, 0));
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size))
		return(KERN_INVALID_ARGUMENT);

#if	defined(hc) || defined(lint)
	/* Error value is ignored at this time */
	error_value++;
#endif	defined(hc) || defined(lint)

	vm_object_lock(object);
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		if (((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL)) {
			if (m->absent) {
				if (m->fictitious) {
					VM_PAGE_FREE(m);
				} else {
					m->busy = FALSE;
					m->absent = FALSE;
					m->error = TRUE;
					vm_object_absent_release(object);
					PAGE_WAKEUP(m);

					vm_page_lock_queues();
					if (!m->active)
						vm_page_activate(m);
					vm_page_unlock_queues();
				}
			}
		}
		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	 }
	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

kern_return_t memory_object_data_unavailable(object, offset, size)
	vm_object_t	object;
	vm_offset_t	offset;
	vm_size_t	size;
{
#if	MACH_PAGEMAP
	vm_external_t	existence_info = VM_EXTERNAL_NULL;
#endif	MACH_PAGEMAP

	XPR(XPR_MEMORY_OBJECT, ("memory_object_data_unavailable, object 0x%x, offset 0x%x",
				object, offset, 0, 0));

	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size != round_page(size))
		return(KERN_INVALID_ARGUMENT);

/*
	if (!object->temporary) {
		uprintf("memory_object_data_unavailable: called on a permanent object -- converted to memory_object_data_error\n");
		return(memory_object_data_error(object, offset, size, KERN_SUCCESS));
	}
*/
#if	MACH_PAGEMAP
	if ((offset == 0) && (size > VM_EXTERNAL_LARGE_SIZE) && 
	    (object->existence_info == VM_EXTERNAL_NULL)) {
		existence_info = vm_external_create(VM_EXTERNAL_SMALL_SIZE);
	}
#endif	MACH_PAGEMAP

	vm_object_lock(object);
#if	MACH_PAGEMAP
 	if (existence_info != VM_EXTERNAL_NULL) {
		object->existence_info = existence_info;
	}
	if ((offset == 0) && (size > VM_EXTERNAL_LARGE_SIZE)) {
		vm_object_unlock(object);
		vm_object_deallocate(object);
		return(KERN_SUCCESS);
	}
#endif	MACH_PAGEMAP
	offset -= object->paging_offset;

	while (size != 0) {
		register vm_page_t m;

		if (((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL)) {
			/*
			 *	We're looking for pages that are both busy and
			 *	absent (waiting to be filled), converting them
			 *	to just absent.
			 *
			 *	Pages that are just busy can be ignored entirely.
			 */
			if (m->busy && m->absent) {
				if (m->fictitious) {
					VM_PAGE_FREE(m);
				}
				else {
					PAGE_WAKEUP_DONE(m);
					vm_page_lock_queues();
					if (!m->active)
						vm_page_activate(m);
					vm_page_unlock_queues();
				}
			}
		}
		size -= PAGE_SIZE;
		offset += PAGE_SIZE;
	}

	vm_object_unlock(object);

	vm_object_deallocate(object);
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_object_lock_page
 *
 *	Description:
 *		Perform the appropriate lock operations on the
 *		given page.  See the description of
 *		"memory_object_lock_request" for the meanings
 *		of the arguments.
 *
 *		Returns an indication that the operation
 *		completed, blocked, or that the page must
 *		be cleaned.
 */
typedef	int		memory_object_lock_result_t;
#define	MEMORY_OBJECT_LOCK_RESULT_DONE		0
#define	MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK	1
#define	MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN	2

memory_object_lock_result_t memory_object_lock_page(m, should_clean, should_flush, prot)
	vm_page_t	m;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	prot;
{
	/*
	 *	Don't worry about pages for which the kernel
	 *	does not have any data.
	 */

	if (m->absent)
		return(MEMORY_OBJECT_LOCK_RESULT_DONE);

	/*
	 *	If we cannot change access to the page,
	 *	either because a mapping is in progress
	 *	(busy page) or because a mapping has been
	 *	wired, then give up.
	 */

	if (m->busy || (m->wire_count != 0)) {
		/*
		 *	If no change would take place
		 *	anyway, return successfully.
		 */

		if (!should_flush &&
		    (m->page_lock == prot)) {
			if (!should_clean ||
			    ((prot & VM_PROT_WRITE) &&
			     !m->dirty &&
			     !pmap_is_modified(m->phys_addr)
			    )
			   ) {
				/*
				 *	Restart page unlock requests,
				 *	even though no change took place.
				 *	[Memory managers may be expecting
				 *	to see new requests.]
				 */
				m->unlock_request = VM_PROT_NONE;
				PAGE_WAKEUP(m);

				return(MEMORY_OBJECT_LOCK_RESULT_DONE);
			}
		}

		return(MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK);
	}

	/*
	 *	If the page is to be flushed, allow
	 *	that to be done as part of the protection.
	 */

	if (should_flush)
		prot = VM_PROT_ALL;

	/*
	 *	Set the page lock.
	 *
	 *	If we are decreasing permission, do it now;
	 *	else, let the fault handler take care of it.
	 */

	if ((m->page_lock ^ prot) & prot)
		pmap_page_protect(m->phys_addr, VM_PROT_ALL & ~prot);
	m->page_lock = prot;


	/*
	 *	Restart any past unlock requests, even if no
	 *	change was made.
	 */

	m->unlock_request = VM_PROT_NONE;
	PAGE_WAKEUP(m);

	/*
	 *	Handle cleaning.
	 */

	if (should_clean) {
		/*
		 *	Check whether the page is dirty.  If
		 *	write permission has not been removed,
		 *	this may have unpredictable results.
		 */

		if (!m->dirty)
			m->dirty = pmap_is_modified(m->phys_addr);

		if (m->dirty) {
			/*
			 *	If we weren't planning
			 *	to flush the page anyway,
			 *	we may need to remove the
			 *	page from the pageout
			 *	system and from physical
			 *	maps now.
			 */

			vm_page_lock_queues();
			VM_PAGE_QUEUES_REMOVE(m);
			vm_page_unlock_queues();

			if (!should_flush)
				pmap_page_protect(m->phys_addr,
						VM_PROT_NONE);

			/*
			 *	Cleaning a page will cause
			 *	it to be flushed.
			 */

			return(MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN);
		}
	}

	/*
	 *	Handle flushing
	 */

	if (should_flush) {
		VM_PAGE_FREE(m);
	} else {
		if (should_clean) {
			vm_page_lock_queues();
			vm_page_deactivate(m);
			vm_page_unlock_queues();
		}
	}

	return(MEMORY_OBJECT_LOCK_RESULT_DONE);
}

/*
 *	Routine:	memory_object_lock_request [user interface]
 *
 *	Description:
 *		Control use of the data associated with the given
 *		memory object.  For each page in the given range,
 *		perform the following operations, in order:
 *			1)  restrict access to the page (disallow
 *			    forms specified by "prot");
 *			2)  write back modifications (if "should_clean"
 *			    is asserted, and the page is dirty); and,
 *			3)  flush the cached copy (if "should_flush"
 *			    is asserted).
 *		The set of pages is defined by a starting offset
 *		("offset") and size ("size").  Only pages with the
 *		same page alignment as the starting offset are
 *		considered.
 *
 *		A single acknowledgement is sent (to the "reply_to"
 *		port) when these actions are complete.  If successful,
 *		the naked send right for reply_to is consumed.
 */

kern_return_t
memory_object_lock_request(object, offset, size,
			should_clean, should_flush, prot,
			reply_to, reply_to_type)
	register vm_object_t	object;
	register vm_offset_t	offset;
	register vm_size_t	size;
	boolean_t		should_clean;
	boolean_t		should_flush;
	vm_prot_t		prot;
	mach_port_t		reply_to;
	mach_msg_type_name_t	reply_to_type;
{
	register vm_page_t	m;
	vm_offset_t		original_offset = offset;
	vm_size_t		original_size = size;
	vm_offset_t		paging_offset = 0;
	vm_object_t		new_object = VM_OBJECT_NULL;
	vm_offset_t		new_offset = 0;
	vm_offset_t		last_offset = offset;

#define	DATA_WRITE_MAX	32
	vm_page_t		holding_pages[DATA_WRITE_MAX];

	/*
	 *	Check for bogus arguments.
	 */
	if (object == VM_OBJECT_NULL ||
		(prot & ~VM_PROT_ALL) != 0)
	    return (KERN_INVALID_ARGUMENT);

	size = round_page(size);

	/*
	 *	Lock the object, and acquire a paging reference to
	 *	prevent the memory_object and control ports from
	 *	being destroyed.
	 */

	vm_object_lock(object);
	vm_object_paging_begin(object);
	offset -= object->paging_offset;

	/*
	 *	To avoid blocking while scanning for pages, save
	 *	dirty pages to be cleaned all at once.
	 *
	 *	XXXO A similar strategy could be used to limit the
	 *	number of times that a scan must be restarted for
	 *	other reasons.  Those pages that would require blocking
	 *	could be temporarily collected in another list, or
	 *	their offsets could be recorded in a small array.
	 */

#define	CLEAN_DIRTY_PAGES						\
MACRO_BEGIN								\
	vm_map_copy_t		copy;					\
	register int		i;					\
	register vm_page_t	hp;					\
									\
	vm_object_unlock(object);					\
									\
	(void) vm_map_copyin_object(new_object, 0, new_offset, &copy);	\
									\
	(void) memory_object_data_write(				\
			object->pager,					\
			object->pager_request,				\
			paging_offset,					\
			(pointer_t) copy,				\
			new_offset);					\
									\
	vm_object_lock(object);						\
									\
	for (i = 0; i < atop(new_offset); i++) {			\
	    hp = holding_pages[i];					\
	    if (hp != VM_PAGE_NULL)					\
		VM_PAGE_FREE(hp);					\
	}								\
									\
	new_object = VM_OBJECT_NULL;					\
MACRO_END

	for (;
	     size != 0;
	     size -= PAGE_SIZE, offset += PAGE_SIZE)
	{
	    /*
	     *	Limit the number of pages to be cleaned at once.
	     */
	    if (new_object != VM_OBJECT_NULL &&
		    new_offset >= PAGE_SIZE * DATA_WRITE_MAX)
	    {
		CLEAN_DIRTY_PAGES;
	    }

	    while ((m = vm_page_lookup(object, offset)) != VM_PAGE_NULL) {
		switch (memory_object_lock_page(m,
					should_clean,
					should_flush,
					prot))
		{
		    case MEMORY_OBJECT_LOCK_RESULT_DONE:
			/*
			 *	End of a cluster of dirty pages.
			 */
			if (new_object != VM_OBJECT_NULL) {
			    CLEAN_DIRTY_PAGES;
			    continue;
			}
			break;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_BLOCK:
			/*
			 *	Since it is necessary to block,
			 *	clean any dirty pages now.
			 */
			if (new_object != VM_OBJECT_NULL) {
			    CLEAN_DIRTY_PAGES;
			    continue;
			}

			PAGE_ASSERT_WAIT(m, FALSE);
			vm_object_unlock(object);
			thread_block((void (*)()) 0);
			vm_object_lock(object);
			continue;

		    case MEMORY_OBJECT_LOCK_RESULT_MUST_CLEAN:
			/*
			 * Mark the page busy since we unlock the
			 * object below.
			 */
			m->busy = TRUE;

			/*
			 * if this would form a discontiguous block,
			 * clean the old pages and start anew.
			 */
			if (last_offset != offset &&
			    new_object != VM_OBJECT_NULL) {
			    CLEAN_DIRTY_PAGES;
			}

			vm_object_unlock(object);

			/*
			 *	If we have not already allocated an object
			 *	for a range of pages to be written, do so
			 *	now.
			 */
			if (new_object == VM_OBJECT_NULL) {
			    new_object = vm_object_allocate(original_size);
			    new_offset = 0;
			    paging_offset = m->offset +
					object->paging_offset;
			}

			/*
			 *	Move or copy the dirty page into the
			 *	new object.
			 */
			m = vm_pageout_setup(m,
					m->offset + object->paging_offset,
					new_object,
					new_offset,
					should_flush);

			/*
			 *	Save the holding page if there is one.
			 */
			holding_pages[atop(new_offset)] = m;
			new_offset += PAGE_SIZE;
			last_offset = offset + PAGE_SIZE;

			vm_object_lock(object);
			break;
		}
		break;
	    }
	}

	/*
	 *	We have completed the scan for applicable pages.
	 *	Clean any pages that have been saved.
	 */
	if (new_object != VM_OBJECT_NULL) {
	    CLEAN_DIRTY_PAGES;
	}

	if (MACH_PORT_VALID(reply_to)) {

		vm_object_unlock(object);

		/* consumes our naked send-once/send right for reply_to */
		(void) memory_object_lock_completed(reply_to, reply_to_type,
			object->pager_request, original_offset, original_size);

		vm_object_lock(object);
	}

	vm_object_paging_end(object);
	vm_object_unlock(object);
	vm_object_deallocate(object);

	return (KERN_SUCCESS);
}

kern_return_t	memory_object_set_attributes(object, object_ready,
						may_cache, copy_strategy)
	vm_object_t	object;
	boolean_t	object_ready;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	/*
	 *	Verify the attributes of importance
	 */

	switch(copy_strategy) {
		case MEMORY_OBJECT_COPY_NONE:
		case MEMORY_OBJECT_COPY_CALL:
		case MEMORY_OBJECT_COPY_DELAY:
			break;
		default:
			vm_object_deallocate(object);
			return(KERN_INVALID_ARGUMENT);
	}

	if (object_ready)
		object_ready = TRUE;
	if (may_cache)
		may_cache = TRUE;

	vm_object_lock(object);

	/*
	 *	Wake up anyone waiting for the ready attribute
	 *	to become asserted.
	 */

	if (object_ready && !object->pager_ready)
		vm_object_wakeup(object, VM_OBJECT_EVENT_PAGER_READY);

	/*
	 *	Copy the attributes
	 */

	object->can_persist = may_cache;
	object->pager_ready = object_ready;
	object->copy_strategy = copy_strategy;

	vm_object_unlock(object);

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

kern_return_t	memory_object_get_attributes(object, object_ready,
						may_cache, copy_strategy)
	vm_object_t	object;
	boolean_t	*object_ready;
	boolean_t	*may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	if (object == VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	vm_object_lock(object);
	*may_cache = object->can_persist;
	*object_ready = object->pager_ready;
	*copy_strategy = object->copy_strategy;
	vm_object_unlock(object);

	vm_object_deallocate(object);

	return(KERN_SUCCESS);
}

/*
 *	If successful, consumes the supplied naked send right.
 */
kern_return_t	vm_set_default_memory_manager(host, default_manager)
	host_t		host;
	memory_object_t	*default_manager;
{
	ipc_port_t current_manager;
	ipc_port_t new_manager;
	ipc_port_t returned_manager;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	new_manager = (ipc_port_t) *default_manager;
	simple_lock(&memory_manager_default_lock);
	current_manager = (ipc_port_t) memory_manager_default;

	if (new_manager == IP_NULL) {
		/*
		 *	Retrieve the current value.
		 */

		returned_manager = ipc_port_copy_send(current_manager);
	} else {
		/*
		 *	Retrieve the current value,
		 *	and replace it with the supplied value.
		 *	We consume the supplied naked send right.
		 */

		returned_manager = current_manager;
		memory_manager_default = (memory_object_t) new_manager;

		/*
		 *	In case anyone's been waiting for a memory
		 *	manager to be established, wake them up.
		 */

		thread_wakeup((int) &memory_manager_default);
	}

	simple_unlock(&memory_manager_default_lock);

	*default_manager = (memory_object_t) returned_manager;
	return(KERN_SUCCESS);
}

/*
 *	Routine:	memory_manager_default_reference
 *	Purpose:
 *		Returns a naked send right for the default
 *		memory manager.  The returned right is always
 *		valid (not IP_NULL or IP_DEAD).
 */

memory_object_t		memory_manager_default_reference()
{
	ipc_port_t current_manager;

	simple_lock(&memory_manager_default_lock);

	while (current_manager =
		ipc_port_copy_send((ipc_port_t) memory_manager_default),
	       !IP_VALID(current_manager)) {
		thread_sleep((int) &memory_manager_default,
			     simple_lock_addr(memory_manager_default_lock),
			     FALSE);
		simple_lock(&memory_manager_default_lock);
	}

	simple_unlock(&memory_manager_default_lock);

	return((memory_object_t) current_manager);
}

/*
 *	Routine:	memory_manager_default_port
 *	Purpose:
 *		Returns true if the receiver for the port
 *		is the default memory manager.
 *
 *		This is a hack to let ds_read_done
 *		know when it should keep memory wired.
 */

boolean_t	memory_manager_default_port(port)
	ipc_port_t port;
{
	ipc_port_t current;
	boolean_t result;

	simple_lock(&memory_manager_default_lock);
	current = (ipc_port_t) memory_manager_default;
	if (IP_VALID(current)) {
		/*
		 *	There is no point in bothering to lock
		 *	both ports, which would be painful to do.
		 *	If the receive rights are moving around,
		 *	we might be inaccurate.
		 */

		result = port->ip_receiver == current->ip_receiver;
	} else
		result = FALSE;
	simple_unlock(&memory_manager_default_lock);

	return result;
}

void		memory_manager_default_init()
{
	memory_manager_default = MACH_PORT_NULL;
	simple_lock_init(&memory_manager_default_lock);
}
