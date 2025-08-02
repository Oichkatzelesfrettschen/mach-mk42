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
 * $Log:	thread_swap.c,v $
 * Revision 2.5  91/02/05  17:30:40  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:20:14  mrt]
 * 
 * Revision 2.4  91/01/08  15:18:20  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * 	Removed swapout_thread, swapout_threads,
 * 	swapout_scan, thread_swapout.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.3  90/06/02  14:57:18  rpd
 * 	In thread_swapout, free the thread's cached message buffer.
 * 	[90/04/23            rpd]
 * 	Converted to new processor set technology.
 * 	[90/03/26  22:26:32  rpd]
 * 
 * Revision 2.2  89/12/08  19:52:35  rwd
 * 	Added call to zone_gc()
 * 	[89/11/21            rwd]
 * 
 * Revision 2.1  89/08/03  15:48:24  rwd
 * Created.
 * 
 * Revision 2.4  88/10/27  10:50:40  rpd
 * 	Changed includes to the new style.  Removed extraneous semis
 * 	from the swapper_lock/swapper_unlock macros.
 * 	[88/10/26  14:49:09  rpd]
 * 
 * 15-Jun-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Fix improper handling of swapper_lock() in swapin_thread().
 *	Problem discovery and elegant recoding due to Richard Draves.
 *
 *  4-May-88  David Golub (dbg) at Carnegie-Mellon University
 *	Remove vax-specific code.
 *
 *  1-Mar-88  David Black (dlb) at Carnegie-Mellon University
 *	Logic change due to replacement of wait_time field in thread
 *	with sched_stamp.  Extra argument to thread_setrun().
 *
 * 25-Jan-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Notify pcb module that pcb is about to be unwired by calling
 *	pcb_synch(thread).
 *	
 * 21-Jan-88  David Golub (dbg) at Carnegie-Mellon University
 *	Fix lots more race conditions (thread_swapin called during
 *	swapout, thread_swapin called twice) by adding a swapper state
 *	machine to each thread.  Moved thread_swappable here from
 *	kern/thread.c.
 *
 * 12-Nov-87  David Golub (dbg) at Carnegie-Mellon University
 *	Fix race condition in thread_swapout: mark thread as swapped
 *	before swapping out its stack, so that an intervening wakeup
 *	will put it on the swapin list.
 *
 *  5-Oct-87  David Golub (dbg) at Carnegie-Mellon University
 *	Changed to new scheduling state machine.
 *
 * 15-Sep-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	De-linted.
 *
 *  5-Sep-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Added check for THREAD_SWAPPABLE in swapout_scan().
 *
 * 14-Jul-87  David Golub (dbg) at Carnegie-Mellon University
 *	Truncate the starting address and round up the size given to
 *	vm_map_pageable, when wiring/unwiring kernel stacks.
 *	KERNEL_STACK_SIZE is not necessarily a multiple of page_size; if
 *	it isn't, forgetting to round the address and size to page
 *	boundaries results in panic.  Kmem_alloc and kmem_free, used in
 *	thread.c to allocate and free kernel stacks, already round to
 *	page boundaries.
 *
 * 26-Jun-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Add thread_swapout_allowed flag to make it easy to turn
 *	off swapping when debugging.
 *
 *  4-Jun-87  David Golub (dbg) at Carnegie-Mellon University
 *	Pass correct number of parameters to lock_init - initialize
 *	swap_lock as sleepable instead of calling lock_sleepable
 *	separately.
 *
 *  1-Apr-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Include vm_param.h to pick up KERNEL_STACK_SIZE definition.
 *
 * 20-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Lower ipl before calling thread_swapout().
 *
 * 19-Mar-87  David Golub (dbg) at Carnegie-Mellon University
 *	Fix one race condition in this (not so buggy) version - since
 *	thread_swapin can be called from interrupts, must raise IPL when
 *	locking swapper_lock.
 *
 * 09-Mar-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created, based somewhat loosely on the earlier (which was a highly
 *	buggy, race condition filled version).
 *
 */
/*
 *
 *	File:	kern/thread_swap.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1987
 *
 *	Mach thread swapper:
 *		Find idle threads to swap, freeing up kernel stack resources
 *		at the expense of allowing them to execute.
 *
 *		Swap in threads that need to be run.  This is done here
 *		by the swapper thread since it cannot be done (in general)
 *		when the kernel tries to place a thread on a run queue.
 *
 *	Note: The act of swapping a thread in Mach does not mean that
 *	its memory gets forcibly swapped to secondary storage.  The memory
 *	for the task corresponding to a swapped thread is paged out
 *	through the normal paging mechanism.
 *
 */

#include <ipc/ipc_kmsg.h>
#include <kern/thread.h>
#include <kern/lock.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <kern/sched_prim.h>
#include <kern/processor.h>
#include <kern/thread_swap.h>
#include <machine/machparam.h>		/* for splsched */

queue_head_t		swapin_queue;
decl_simple_lock_data(,	swapper_lock_data)

#define swapper_lock()		simple_lock(&swapper_lock_data)
#define swapper_unlock()	simple_unlock(&swapper_lock_data)

/*
 *	swapper_init: [exported]
 *
 *	Initialize the swapper module.
 */
void swapper_init()
{
	queue_init(&swapin_queue);
	simple_lock_init(&swapper_lock_data);
}

/*
 *	thread_swapin: [exported]
 *
 *	Place the specified thread in the list of threads to swapin.  It
 *	is assumed that the thread is locked, therefore we are at splsched.
 *	make_unswappable argument is used to ask swapin thread to make
 *	thread unswappable.
 */

void thread_swapin(thread, make_unswappable)
	thread_t	thread;
	boolean_t	make_unswappable;
{
	switch (thread->swap_state & TH_SW_STATE) {
	    case TH_SW_OUT:
		/*
		 *	Swapped out - queue for swapin thread
		 *	if we can't handle the swapin request now.
		 */
		thread->swap_state = TH_SW_COMING_IN;
		if (stack_alloc_try(thread)) {
			if (make_unswappable)
				thread->swap_state = TH_SW_UNSWAPPABLE;
			else
				thread->swap_state = TH_SW_IN;
			thread->state &= ~TH_SWAPPED;
			if (thread->state & TH_RUN)
				thread_setrun(thread, TRUE);
			return;
		}

		swapper_lock();
		enqueue_tail(&swapin_queue, (queue_entry_t) thread);
		swapper_unlock();
		thread_wakeup((int) &swapin_queue);
		break;

	    case TH_SW_GOING_OUT:
		/*
		 *	Being swapped out - wait until swapped out,
		 *	then queue for swapin thread (in thread_swapout).
		 */
		thread->swap_state = TH_SW_WANT_IN;
		break;

	    case TH_SW_WANT_IN:
	    case TH_SW_COMING_IN:
		/*
		 *	Already queued for swapin thread, or being
		 *	swapped in
		 */
		break;

	    default:
		/*
		 *	Swapped in or unswappable
		 */
		panic("thread_swapin");
	}

	/*
	 *	Set make unswappable flag if asked to.  swapin thread
	 *	will make thread unswappable.
	 */
	if (make_unswappable)
		thread->swap_state |= TH_SW_MAKE_UNSWAPPABLE;
}

/*
 *	thread_doswapin:
 *
 *	Swapin the specified thread, if it should be runnable, then put
 *	it on a run queue.  No locks should be held on entry, as it is
 *	likely that this routine will sleep (waiting for stack allocation).
 */
void thread_doswapin(thread)
	thread_t	thread;
{
	register vm_offset_t	addr;
	register int		s;

	/*
	 *	Allocate the kernel stack.
	 */

	stack_alloc(thread);

	/*
	 *	Make unswappable and wake up waiting thread(s) if needed.
	 *	Place on run queue if appropriate.  
	 */

	s = splsched();
	thread_lock(thread);
	if (thread->swap_state & TH_SW_MAKE_UNSWAPPABLE) {
		thread->swap_state = TH_SW_UNSWAPPABLE;
		thread_wakeup(&thread->swap_state);
	}
	else {
		thread->swap_state = TH_SW_IN;
	}
	thread->state &= ~TH_SWAPPED;
	if (thread->state & TH_RUN)
		thread_setrun(thread, TRUE);
	thread_unlock(thread);
	(void) splx(s);
}

/*
 *	swapin_thread: [exported]
 *
 *	This procedure executes as a kernel thread.  Threads that need to
 *	be swapped in are swapped in by this thread.
 */
void swapin_thread()
{
	while (TRUE) {
		register thread_t	thread;
		register int		s;

		s = splsched();
		swapper_lock();

		while ((thread = (thread_t) dequeue_head(&swapin_queue))
							== THREAD_NULL) {
			assert_wait((int) &swapin_queue, FALSE);
			swapper_unlock();
			splx(s);
			thread_block((void (*)()) 0);
			s = splsched();
			swapper_lock();
		}

		swapper_unlock();
		splx(s);

		thread_doswapin(thread);
	}
}

/*
 *	Mark a thread as swappable or unswappable.  May be called at
 *	any time.  No longer assumes thread is swapped in.
 */
void thread_swappable(thread, is_swappable)
	thread_t	thread;
	boolean_t	is_swappable;
{
	int	s = splsched();
	thread_lock(thread);
	if (is_swappable) {
	    if (thread->swap_state == TH_SW_UNSWAPPABLE)
		thread->swap_state = TH_SW_IN;
	}
	else {
	    switch(thread->swap_state) {
		case TH_SW_UNSWAPPABLE:
		    break;

		case TH_SW_IN:
		    thread->swap_state = TH_SW_UNSWAPPABLE;
		    break;

		default:
		    do {
			thread_swapin(thread, TRUE);
			thread_sleep(&thread->swap_state,
				     simple_lock_addr(thread->lock),
				     FALSE);
			thread_lock(thread);
		    } while (thread->swap_state != TH_SW_UNSWAPPABLE);
		    break;
	    }
	}
	thread_unlock(thread);
	(void) splx(s);
}

void thread_swapout(thread, func)
	thread_t thread;
	void (*func)();
{
	int state;

	thread->state |= TH_SWAPPED;
	thread->swap_state = TH_SW_GOING_OUT;
	thread_unlock(thread);

	stack_free(thread);

	thread_lock(thread);
	state = thread->swap_state;
	thread->swap_state = TH_SW_OUT;
	thread->swap_func = func;

	if ((state & TH_SW_STATE) == TH_SW_WANT_IN)
		thread_swapin(thread, state & TH_SW_MAKE_UNSWAPPABLE);
}
