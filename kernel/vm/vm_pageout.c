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
 * $Log:	vm_pageout.c,v $
 * Revision 2.13  91/02/05  17:59:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:34:17  mrt]
 * 
 * Revision 2.12  91/01/08  16:45:57  rpd
 * 	Added net_kmsg_collect.
 * 	[91/01/05            rpd]
 * 	Added consider_task_collect, consider_thread_collect.
 * 	[91/01/03            rpd]
 * 
 * 	Added stack_collect.
 * 	[90/12/31            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Ensure that vm_page_free_target is at least five pages
 * 	larger than vm_page_free_min, to avoid vm_page_wait deadlock.
 * 	[90/11/19            rpd]
 * 
 * 	Replaced swapout_threads with consider_zone_gc.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.11  90/11/05  14:35:03  rpd
 * 	Modified vm_pageout_scan for new vm_page_deactivate protocol.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.10  90/10/12  13:06:53  rpd
 * 	Fixed vm_pageout_page to take busy pages.
 * 	[90/10/09            rpd]
 * 
 * 	In vm_pageout_scan, check for new software reference bit
 * 	in addition to using pmap_is_referenced.  Remove busy pages
 * 	found on the active and inactive queues.
 * 	[90/10/08            rpd]
 * 
 * Revision 2.9  90/08/27  22:16:02  dbg
 * 	Fix error in initial assumptions: vm_pageout_setup must take a
 * 	BUSY page, to prevent the page from being scrambled by pagein.
 * 	[90/07/26            dbg]
 * 
 * Revision 2.8  90/06/19  23:03:22  rpd
 * 	Locking fix for vm_pageout_page from dlb/dbg.
 * 	[90/06/11            rpd]
 * 
 * 	Correct initial condition in vm_pageout_page (page is NOT busy).
 * 	Fix documentation for vm_pageout_page and vm_pageout_setup.
 * 	[90/06/05            dbg]
 * 
 * 	Fixed vm_object_unlock type in vm_pageout_page.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.7  90/06/02  15:11:56  rpd
 * 	Removed pageout_task and references to kernel_vm_space.
 * 	[90/04/29            rpd]
 * 
 * 	Made vm_pageout_burst_max, vm_pageout_burst_wait tunable.
 * 	[90/04/18            rpd]
 * 	Converted to new IPC and vm_map_copyin_object.
 * 	[90/03/26  23:18:10  rpd]
 * 
 * Revision 2.6  90/05/29  18:39:52  rwd
 * 	Picked up new vm_pageout_page from dbg.
 * 	[90/05/17            rwd]
 * 	Rfr change to send multiple pages to pager at once.
 * 	[90/04/12  13:49:13  rwd]
 * 
 * Revision 2.5  90/05/03  15:53:21  dbg
 * 	vm_pageout_page flushes page only if asked; otherwise, it copies
 * 	the page.
 * 	[90/03/28            dbg]
 * 
 * 	If an object's pager is not initialized, don't page out to it.
 * 	[90/03/21            dbg]
 * 
 * Revision 2.4  90/02/22  20:06:48  dbg
 * 		PAGE_WAKEUP --> PAGE_WAKEUP_DONE to reflect the fact that it
 * 		clears the busy flag.
 * 		[89/12/13            dlb]
 * 
 * Revision 2.3  90/01/11  11:48:27  dbg
 * 	Pick up recent changes from mainline:
 * 
 * 		Eliminate page lock when writing back a page.
 * 		[89/11/09            mwyoung]
 * 
 * 		Account for paging_offset when setting external page state.
 * 		[89/10/16  15:29:08  af]
 * 
 * 		Improve reserve tuning... it was a little too low.
 * 
 * 		Remove laundry count computations, as the count is never used.
 * 		[89/10/10            mwyoung]
 * 
 * 		Only attempt to collapse if a memory object has not
 * 		been initialized.  Don't bother to PAGE_WAKEUP in
 * 		vm_pageout_scan() before writing back a page -- it
 * 		gets done in vm_pageout_page().
 * 		[89/10/10            mwyoung]
 * 
 * 		Don't reactivate a page when creating a new memory
 * 		object... continue on to page it out immediately.
 * 		[89/09/20            mwyoung]
 * 
 * 		Reverse the sensing of the desperately-short-on-pages tests.
 * 		[89/09/19            mwyoung]
 * 		Check for absent pages before busy pages in vm_pageout_page().
 * 		[89/07/10  00:01:22  mwyoung]
 * 
 * 		Allow dirty pages to be reactivated.
 * 		[89/04/22            mwyoung]
 * 
 * 		Don't clean pages that are absent, in error, or not dirty in
 * 		vm_pageout_page().  These checks were previously issued
 * 		elsewhere.
 * 		[89/04/22            mwyoung]
 * 
 * Revision 2.2  89/09/08  11:28:55  dbg
 * 	Reverse test for internal_only pages.  Old sense left pageout
 * 	daemon spinning.
 * 	[89/08/15            dbg]
 * 
 * 18-Jul-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Removed non-XP code.
 *	Count page wiring when sending page to default pager.
 *	Increase reserved page count.
 *	Make 'internal-only' count LARGER than reserved page count.
 *
 * Revision 2.18  89/06/12  14:53:05  jsb
 * 	Picked up bug fix (missing splimps) from Sequent via dlb.
 * 	[89/06/12  14:39:28  jsb]
 * 
 * Revision 2.17  89/04/18  21:27:08  mwyoung
 * 	Recent history [mwyoung]:
 * 		Keep hint when pages are written out (call
 * 		 vm_external_state_set).
 * 		Use wired-down fictitious page data structure for "holding_page".
 * 	History condensation:
 * 		Avoid flooding memory managers by using timing [mwyoung].
 * 		New pageout daemon for external memory management
 * 		 system [mwyoung].
 * 	[89/04/18            mwyoung]
 * 
 */
/*
 *	File:	vm/vm_pageout.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	The proverbial page-out daemon.
 */

#include <mach_pagemap.h>

#include <mach/mach_types.h>
#include <mach/memory_object.h>
#include <mach/memory_object_default.h>
#include <mach/memory_object_user.h>
#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <kern/xpr.h>
#include <kern/zalloc.h>
#include <vm/pmap.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <machine/vm_tuning.h>

int	vm_pageout_debug = 0;
int	vm_pageout_double_laundry_count = 0;

int	vm_pageout_reserved_internal = 0;
int	vm_pageout_reserved_external = 0;

int	vm_pageout_burst_max = 0;
unsigned int	vm_pageout_burst_wait = 0;

boolean_t	vm_pageout_reactivate_dirty_pages = TRUE;

/*
 *	Routine:	vm_pageout_setup
 *	Purpose:
 *		Set up a page for pageout.
 *
 *		Move or copy the page to a new object, as part
 *		of which it will be sent to its memory manager
 *		in a memory_object_data_write or memory_object_initialize
 *		message.
 *
 *		The "paging_offset" argument specifies the offset
 *		of the page within its external memory object.
 *
 *		The "new_object" and "new_offset" arguments
 *		indicate where the page should be moved.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from its object.  If not, a
 *		copy of the page is moved to the new object.
 *
 *	In/Out conditions:
 *		The page in question must not be on any pageout queues,
 *		and must be busy.  The object to which it belongs
 *		must be unlocked, and the caller must hold a paging
 *		reference to it.  The new_object must not be locked.
 *
 *		If the page is flushed from its original object,
 *		this routine returns a pointer to a place-holder page,
 *		inserted at the same offset, to block out-of-order
 *		requests for the page.  The place-holder page must
 *		be freed after the data_write or initialize message
 *		has been sent.  If the page is copied,
 *		the holding page is VM_PAGE_NULL.
 *
 *		The original page is put on a paging queue and marked
 *		not busy on exit.
 */
vm_page_t
vm_pageout_setup(m, paging_offset, new_object, new_offset, flush)
	register vm_page_t	m;
	vm_offset_t		paging_offset;
	register vm_object_t	new_object;
	vm_offset_t		new_offset;
	boolean_t		flush;
{
	register vm_object_t	old_object = m->object;
	kern_return_t	rc;
	register vm_page_t	holding_page;
	register vm_page_t	new_m;

	XPR(XPR_VM_PAGEOUT, ("vm_pageout_page: begin %x", m));
	assert(m->busy);

	/*
	 *	If we are not flushing the page, allocate a
	 *	page in the object.  If we cannot get the
	 *	page, flush instead.
	 */
	if (!flush) {
		vm_object_lock(new_object);
		new_m = vm_page_alloc(new_object, new_offset);
		if (new_m == VM_PAGE_NULL)
			flush = TRUE;
		vm_object_unlock(new_object);
	}

	if (flush) {
		/*
		 *	Create a place-holder page where the old one was,
		 *	to prevent anyone from attempting to page in this
		 *	page while we`re unlocked.
		 */
		holding_page = (vm_page_t) zalloc(vm_page_fictitious_zone);

		vm_object_lock(old_object);
		vm_page_remove(m);
		PAGE_WAKEUP_DONE(m);

		vm_page_init(holding_page,
			old_object,
			m->offset,
			(vm_offset_t) 0);
		holding_page->fictitious = TRUE;

		/*
		 *	Record that this page has been written out
		 */
#if	MACH_PAGEMAP
		vm_external_state_set(old_object->existence_info,
					paging_offset,
					VM_EXTERNAL_STATE_EXISTS);
#endif	MACH_PAGEMAP

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Move this page into the new object
		 */

		vm_page_insert(m, new_object, new_offset);
		m->dirty = TRUE;
		m->page_lock = VM_PROT_NONE;
		m->unlock_request = VM_PROT_NONE;
	}
	else {
		/*
		 *	Copy the data into the new page,
		 *	and mark the new page as clean.
		 */
		vm_page_copy(m, new_m);

		vm_object_lock(old_object);
		m->dirty = FALSE;
		pmap_clear_modify(m->phys_addr);

		/*
		 *	Deactivate old page.
		 */
		vm_page_lock_queues();
		vm_page_deactivate(m);
		vm_page_unlock_queues();

		PAGE_WAKEUP_DONE(m);

		/*
		 *	Record that this page has been written out
		 */

#if	MACH_PAGEMAP
		vm_external_state_set(old_object->existence_info,
					paging_offset,
					VM_EXTERNAL_STATE_EXISTS);
#endif	MACH_PAGEMAP

		vm_object_unlock(old_object);

		vm_object_lock(new_object);

		/*
		 *	Use the new page below.
		 */
		m = new_m;
		m->dirty = TRUE;
		PAGE_WAKEUP_DONE(m);
	}

	/*
	 *	Make the old page eligible for replacement again; if a
	 *	user-supplied memory manager fails to release the page,
	 *	it will be paged out again to the default memory manager.
	 *
	 *	Note that pages written to the default memory manager
	 *	must be wired down -- in return, it guarantees to free
	 *	this page, rather than reusing it.
	 */

	vm_page_lock_queues();
	vm_stat.pageouts++;
	if (old_object->internal)
		vm_page_wire(m);
	 else
		vm_page_activate(m);
	vm_page_unlock_queues();

	/*
	 *	Mark the page as in the laundry, so we can detect
	 *	pages which aren't released, and so we can decide
	 *	when to stop the pageout scan.
	 */

	if (m->laundry) {
		if (vm_pageout_debug)
			printf("vm_pageout_page: double laundry, object 0x%x offset 0x%x page 0x%x physical 0x%x!\n",
			       old_object, paging_offset, m, m->phys_addr);
		vm_pageout_double_laundry_count++;
	}
	m->laundry = TRUE;
	
	/*
	 *	Since IPC operations may block, we drop locks now.
	 *	[The placeholder page is busy, and we still have
	 *	paging_in_progress incremented.]
	 */

	vm_object_unlock(new_object);

	/*
	 *	Return the placeholder page to simplify cleanup.
	 */
	return (flush ? holding_page : VM_PAGE_NULL);
}

/*
 *	Routine:	vm_pageout_page
 *	Purpose:
 *		Causes the specified page to be written back to
 *		the appropriate memory object.
 *
 *		The "initial" argument specifies whether this
 *		data is an initialization only, and should use
 *		memory_object_data_initialize instead of
 *		memory_object_data_write.
 *
 *		The "flush" argument specifies whether the page
 *		should be flushed from the object.  If not, a
 *		copy of the data is sent to the memory object.
 *
 *	In/out conditions:
 *		The page in question must not be on any pageout queues.
 *		The object to which it belongs must be locked.
 *	Implementation:
 *		Move this page to a completely new object, if flushing;
 *		copy to a new page in a new object, if not.
 */
void
vm_pageout_page(m, initial, flush)
	register vm_page_t	m;
	boolean_t		initial;
	boolean_t		flush;
{
	vm_map_copy_t		copy;
	register vm_object_t	old_object;
	register vm_object_t	new_object;
	register vm_page_t	holding_page;
	vm_offset_t		paging_offset;
	kern_return_t		rc;

	assert(m->busy);

	/*
	 *	Verify that we really want to clean this page
	 */

	if (m->absent || m->error || !m->dirty) {
		VM_PAGE_FREE(m);
		return;
	}

	/*
	 *	Create a paging reference to let us play with the object.
	 */
	old_object = m->object;
	paging_offset = m->offset + old_object->paging_offset;
	vm_object_paging_begin(old_object);
	vm_object_unlock(old_object);

	/*
	 *	Allocate a new object into which we can put the page.
	 */
	new_object = vm_object_allocate(PAGE_SIZE);

	/*
	 *	Move the page into the new object.
	 */
	holding_page = vm_pageout_setup(m,
				paging_offset,
				new_object,
				0,		/* new offset */
				flush);		/* flush */

	rc = vm_map_copyin_object(new_object, 0, PAGE_SIZE, &copy);
	assert(rc == KERN_SUCCESS);

	if ((rc = (*(initial ? memory_object_data_initialize
			     : memory_object_data_write))
			(old_object->pager,
			 old_object->pager_request,
			 paging_offset, (pointer_t) copy, PAGE_SIZE))
					!= KERN_SUCCESS) {
		if (vm_pageout_debug)
			printf("vm_pageout_page: memory_object_data_write failed, %d, page 0x%x\n", rc, m);
		vm_object_deallocate(new_object);
	}

	/*
	 *	Clean up.
	 */
	vm_object_lock(old_object);
	if (holding_page != VM_PAGE_NULL)
	    vm_page_free(holding_page);
	vm_object_paging_end(old_object);
}

extern zone_t vm_map_copy_zone;

/*
 *	vm_pageout_scan does the dirty work for the pageout daemon.
 */
void		vm_pageout_scan()
{
	register vm_page_t	m;
	int		page_shortage;
	int		pages_moved;
	int		burst_count;
	int		free;

	/*
	 *	Only continue when we want more pages to be "free"
	 */

	simple_lock(&vm_page_queue_free_lock);
	free = vm_page_free_count;
	simple_unlock(&vm_page_queue_free_lock);

	if (free < vm_page_free_target) {
		/*
		 *	Try to recover internal memory.
		 *	consider_zone_gc should be last, because the
		 *	other operations might return memory to zones.
		 */

		stack_collect();
		net_kmsg_collect();
		consider_task_collect();
		consider_thread_collect();
		consider_zone_gc();

		/*
		 *	Be sure the pmap system is updated so
		 *	we can scan the inactive queue.
		 */

		pmap_update();
	}

	/*
	 *	Start scanning the inactive queue for pages we can free.
	 *	We keep scanning until we have enough free pages or
	 *	we have scanned through the entire queue.  If we
	 *	encounter dirty pages, we start cleaning them.
	 *
	 *	NOTE:	The page queues lock is not held at loop entry,
	 *		but *is* held upon loop exit.
	 */

	pages_moved = burst_count = 0;

	while (TRUE) {
		register
		vm_object_t	object;

		vm_page_lock_queues();

		m = (vm_page_t) queue_first(&vm_page_queue_inactive);
		if (queue_end(&vm_page_queue_inactive, (queue_entry_t) m))
			break;
		assert(m->inactive && !m->active);

		/*
		 *	If there are enough free pages, stop anyway.
		 */

		simple_lock(&vm_page_queue_free_lock);
		free = vm_page_free_count;
		simple_unlock(&vm_page_queue_free_lock);

		if (free >= vm_page_free_target)
			break;

		/*
		 *	If we've moved a bunch of pages, pause to let
		 *	the memory manager(s) catch up.  This should
		 *	be dynamically tuned, but isn't yet.
		 */

		if (burst_count >= vm_pageout_burst_max) {
			assert_wait(0, TRUE);
			thread_set_timeout(vm_pageout_burst_wait);
			vm_page_unlock_queues();
			thread_block((void (*)()) 0);
			burst_count = 0;
			continue;
		}

		object = m->object;

		/*
		 *	If we're extremely tight on pages, we can only
		 *	send pages to the (trusted) default pager.
		 */

		if (free < vm_pageout_reserved_internal) {
			if (!object->internal) {
				/*
				 *	[Don't count this as a reactivation.]
				 */

				vm_page_activate(m);
				vm_page_unlock_queues();
				continue;
			}

			/*
			 *	Stop working entirely when down to that
			 *	portion of the reserved pool that
			 *	is required by the default memory manager.
			 */

			if (free < vm_pageout_reserved_external)
			{
				/* XXX If the burst_wait is computed
				 * dynamically, it would probably be
				 * wise to change this one to a fixed
				 * timeout.
				 */
				assert_wait(0, TRUE);
				thread_set_timeout(vm_pageout_burst_wait);
				vm_page_unlock_queues();
				thread_block((void (*)()) 0);
				continue;
			}
		}

		/*
		 *	If it's being used, merely reactivate.
		 */

		if (m->reference || pmap_is_referenced(m->phys_addr)) {
			vm_page_activate(m);
			vm_stat.reactivations++;
			vm_page_unlock_queues();
			continue;
		}

		/*
		 *	Try to lock object; since we've got the
		 *	page queues lock, we can only try for this one.
		 */
		if (!vm_object_lock_try(object)) {
			/*
			 *	Move page to end and continue.
			 */
			queue_remove(&vm_page_queue_inactive, m,
				     vm_page_t, pageq);
			queue_enter(&vm_page_queue_inactive, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			continue;
		}

		/*
		 *	Remove the page from the inactive list.
		 */

		queue_remove(&vm_page_queue_inactive, m, vm_page_t, pageq);
		vm_page_inactive_count--;
		m->inactive = FALSE;

		if (m->busy) {
			/*
			 *	Somebody is already playing with this page.
			 *	Leave it off the pageout queues.
			 */
			vm_object_unlock(object);
			vm_page_unlock_queues();
			continue;
		}

		m->busy = TRUE;

		/*
		 *	Eliminate all mappings
		 */

		pmap_page_protect(m->phys_addr, VM_PROT_NONE);
		if (!m->dirty)
			m->dirty = pmap_is_modified(m->phys_addr);

		/*
		 *	If it's clean, we can merely free the page.
		 */

		if (!m->dirty) {
			vm_page_free(m);
			pages_moved++;
			vm_object_unlock(object);
			vm_page_unlock_queues();
			continue;
		}

		vm_page_unlock_queues();

		/*
		 *	If there is no memory object for the page, create
		 *	one and hand it to the default pager.
		 *	[First try to collapse, so we don't create
		 *	one unnecessarily.]
		 */

		if (!object->pager_initialized)
			vm_object_collapse(object);
		if (!object->pager_initialized)
			vm_object_pager_create(object);
		if (!object->pager_initialized) {
			/*
			 *	Still no pager for the object.
			 *	Reactivate the page.
			 *
			 *	Should only happen if there is no
			 *	default pager.
			 */
			vm_page_lock_queues();
			vm_page_activate(m);
			vm_page_unlock_queues();

			/*
			 *	And we are done with it.
			 */
			PAGE_WAKEUP_DONE(m);
			vm_object_unlock(object);
			continue;
		}

		vm_pageout_page(m, FALSE, TRUE);	/* flush it */
		vm_object_unlock(object);
		pages_moved++;
		burst_count++;
	}

	/*
	 *	Compute the page shortage.  If we are still very low on memory
	 *	be sure that we will move a minimal number of pages from
	 *	active to inactive.
	 */

	page_shortage = vm_page_inactive_target - vm_page_inactive_count;

	if ((page_shortage <= 0) && (pages_moved == 0))
		page_shortage = 1;

	while ((page_shortage > 0) && !queue_empty(&vm_page_queue_active)) {
		register vm_object_t object;

		/*
		 *	Move some more pages from active to inactive.
		 */

		m = (vm_page_t) queue_first(&vm_page_queue_active);
		assert(m->active && !m->inactive);

		object = m->object;
		if (!vm_object_lock_try(object)) {
			/*
			 *	Move page to end and continue.
			 */
			queue_remove(&vm_page_queue_active, m,
				     vm_page_t, pageq);
			queue_enter(&vm_page_queue_active, m,
				    vm_page_t, pageq);
			vm_page_unlock_queues();
			vm_page_lock_queues();
			continue;
		}

		vm_page_deactivate(m);
		vm_object_unlock(object);
		page_shortage--;
	}

	vm_page_unlock_queues();
}

/*
 *	vm_pageout is the high level pageout daemon.
 */

void vm_pageout()
{
	int		free_after_reserve;
	int		target_less_min;

	current_thread()->vm_privilege = TRUE;

	(void) spl0();

	/*
	 *	Initialize some paging parameters.
	 */

#ifndef	VM_PAGEOUT_BURST_MAX
#define	VM_PAGEOUT_BURST_MAX	10
#endif	VM_PAGEOUT_BURST_MAX
	if (vm_pageout_burst_max == 0)
		vm_pageout_burst_max = VM_PAGEOUT_BURST_MAX;

#ifndef	VM_PAGEOUT_BURST_WAIT
#define	VM_PAGEOUT_BURST_WAIT	30
#endif	VM_PAGEOUT_BURST_WAIT
	if (vm_pageout_burst_wait == 0)
		vm_pageout_burst_wait = VM_PAGEOUT_BURST_WAIT;

#ifndef	VM_PAGE_FREE_RESERVED
#define	VM_PAGE_FREE_RESERVED			15
#endif	VM_PAGE_FREE_RESERVED
	if (vm_page_free_reserved == 0)
		vm_page_free_reserved = VM_PAGE_FREE_RESERVED;

#ifndef	VM_PAGEOUT_RESERVED_EXTERNAL
#define	VM_PAGEOUT_RESERVED_EXTERNAL(reserve)	((reserve) - 10)
#endif	VM_PAGEOUT_RESERVED_EXTERNAL
	if (vm_pageout_reserved_external == 0)
		vm_pageout_reserved_external =
			VM_PAGEOUT_RESERVED_EXTERNAL(vm_page_free_reserved);

#ifndef	VM_PAGEOUT_RESERVED_INTERNAL
#define	VM_PAGEOUT_RESERVED_INTERNAL(reserve)	((reserve) - 5)
#endif	VM_PAGEOUT_RESERVED_INTERNAL
	if (vm_pageout_reserved_internal == 0)
		vm_pageout_reserved_internal =
			VM_PAGEOUT_RESERVED_INTERNAL(vm_page_free_reserved);

	free_after_reserve = vm_page_free_count - vm_page_free_reserved;

#ifndef	VM_PAGE_FREE_MIN
#define	VM_PAGE_FREE_MIN(free)	(10 + (free) / 100)
#endif	VM_PAGE_FREE_MIN
	if (vm_page_free_min == 0)
		vm_page_free_min = vm_page_free_reserved +
			VM_PAGE_FREE_MIN(free_after_reserve);

#ifndef	VM_PAGE_FREE_TARGET
#define	VM_PAGE_FREE_TARGET(free)	(15 + (free) / 80)
#endif	VM_PAGE_FREE_TARGET
	if (vm_page_free_target == 0)
		vm_page_free_target = vm_page_free_reserved +
			VM_PAGE_FREE_TARGET(free_after_reserve);

	/*
	 *	The default inactive target is rather high.
	 *	A machine that must simulate reference bits should
	 *	choose a small inactive target.
	 */

#ifndef	VM_PAGE_INACTIVE_TARGET
#define	VM_PAGE_INACTIVE_TARGET(free)	((free) * 2 / 3)
#endif	VM_PAGE_INACTIVE_TARGET
	if (vm_page_inactive_target == 0)
		vm_page_inactive_target = 
			VM_PAGE_INACTIVE_TARGET(free_after_reserve);

	if (vm_page_free_target < vm_page_free_min + 5)
		vm_page_free_target = vm_page_free_min + 5;

	target_less_min = vm_page_free_target - vm_page_free_min;
	if (vm_page_inactive_target <= target_less_min)
		vm_page_inactive_target = target_less_min + 1;

	/*
	 *	The pageout daemon is never done, so loop
	 *	forever.
	 */

	simple_lock(&vm_page_queue_free_lock);
	while (TRUE) {
		if (vm_page_free_wanted)
			simple_unlock(&vm_page_queue_free_lock);
		 else
			thread_sleep((int) &vm_page_free_wanted,
				     simple_lock_addr(vm_page_queue_free_lock),
				     FALSE);
		vm_pageout_scan();
		simple_lock(&vm_page_queue_free_lock);
	}
}
