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
 * $Log:	ipc_sched.c,v $
 * Revision 2.5  91/02/05  17:26:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:13:23  mrt]
 * 
 * Revision 2.4  91/01/08  15:15:52  rpd
 * 	Added KEEP_STACKS support.
 * 	[91/01/06            rpd]
 * 	Added ipc_thread_switch_hits, ipc_thread_switch_misses counters.
 * 	[91/01/03  22:07:15  rpd]
 * 
 * 	Modified ipc_thread_switch to deal with pending timeouts.
 * 	[90/12/20            rpd]
 * 	Removed ipc_thread_go_and_block.
 * 	Added ipc_thread_switch.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.3  90/08/27  22:02:40  dbg
 * 	Pass correct number of arguments to thread_swapin.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.2  90/06/02  14:54:22  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:45:59  rpd]
 * 
 */

#include <cpus.h>
#include <hw_footprint.h>
#include <mach_host.h>

#include <mach/message.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#include <kern/processor.h>
#include <kern/time_out.h>
#include <kern/thread_swap.h>
#include <kern/ipc_sched.h>
#include <machine/machparam.h>	/* for splsched/splx */
#include <machine/pmap.h>

/*
 *	Routine:	ipc_thread_go
 *	Purpose:
 *		Start a thread running.
 *	Conditions:
 *		IPC locks may be held.
 */

void
ipc_thread_go(thread)
	thread_t thread;
{
	int s, state;

	s = splsched();
	thread_lock(thread);

	if (thread->timer_set) {
		thread->timer_set = FALSE;
		reset_timeout(&thread->timer);
	}

	state = thread->state;
	switch (state) {
	    case TH_WAIT | TH_SUSP:
		/*
		 *	Suspend thread if interruptible
		 */
		if (thread->interruptible) {
			thread->state = TH_SUSP;
			thread->wait_result = THREAD_AWAKENED;
			break;
		}
		/* fall through */

	    case TH_WAIT:
		/*
		 *	Sleeping and not suspendable - put
		 *	on run queue.
		 */
		thread->state = (state & ~TH_WAIT) | TH_RUN;
		thread->wait_result = THREAD_AWAKENED;
		thread_setrun(thread, TRUE);
		break;

	    case TH_WAIT | TH_SWAPPED:
		/*
		 *	Thread is swapped out, but runnable
		 */
		thread->state = TH_RUN | TH_SWAPPED;
		thread->wait_result = THREAD_AWAKENED;
		thread_swapin(thread, FALSE);
		break;

	    default:
		/*
		 *	Either already running, or suspended.
		 */
		if (state & TH_WAIT) {
			thread->state = state & ~TH_WAIT;
			thread->wait_result = THREAD_AWAKENED;
			break;
		}
	}

	thread_unlock(thread);
	splx(s);
}

/*
 *	Routine:	ipc_thread_will_wait
 *	Purpose:
 *		Assert that the thread intends to block.
 */

void
ipc_thread_will_wait(thread)
	thread_t thread;
{
	int s;

	s = splsched();
	thread_lock(thread);

	thread->state |= TH_WAIT;

	thread_unlock(thread);
	splx(s);
}

/*
 *	Routine:	ipc_thread_will_wait_with_timeout
 *	Purpose:
 *		Assert that the thread intends to block,
 *		with a timeout.
 */

void
ipc_thread_will_wait_with_timeout(thread, msecs)
	thread_t thread;
	mach_msg_timeout_t msecs;
{
	int ticks = (msecs * hz) / 1000;
	int s;

	s = splsched();
	thread_lock(thread);

	thread->state |= TH_WAIT;
	thread->timer_set = TRUE;
	set_timeout(&thread->timer, ticks);

	thread_unlock(thread);
	splx(s);
}

/*
 *	Routine:	ipc_thread_switch
 *	Purpose:
 *		Switch to a new thread (new), leaving the current
 *		thread (old) blocked.  If successful, moves the
 *		kernel stack from old to new and returns as the
 *		new thread.
 *	Conditions:
 *		The new thread is not waiting with a timeout.
 *		Both old and new threads are interruptible.
 */

unsigned int ipc_thread_switch_hits = 0;
unsigned int ipc_thread_switch_misses = 0;

boolean_t
ipc_thread_switch(old, new)
	register thread_t old;
	register thread_t new;
{
#ifdef	KEEP_STACKS
	ipc_thread_switch_misses++;
	return FALSE;
#else	KEEP_STACKS
	int s;

	assert(current_thread() == old);

	s = splsched();
	if ((int)old < (int)new) {
		thread_lock(old);
		thread_lock(new);
	} else {
		thread_lock(new);
		thread_lock(old);
	}

	/*
	 *	XXX Dubious things here:
	 *	I don't check the idle_count on the processor set.
	 *	No scheduling priority or policy checks.
	 *	I assume new->interruptible and old->interruptible are TRUE.
	 */

	assert(old->interruptible);
	assert(new->interruptible);

	if ((old->state != TH_RUN) ||
	    (old->swap_state != TH_SW_IN) ||
#if	MACH_HOST
	    (old->processor_set != new->processor_set) ||
#endif	MACH_HOST
	    (new->state != (TH_WAIT|TH_SWAPPED)) ||
	    (new->swap_state != TH_SW_OUT)) {
		thread_unlock(new);
		thread_unlock(old);
		(void) splx(s);

		ipc_thread_switch_misses++;
		return FALSE;
	}

	old->state = TH_WAIT|TH_SWAPPED;
	old->swap_state = TH_SW_OUT;

	if (new->timer_set) {
		new->timer_set = FALSE;
		reset_timeout(&new->timer);
	}

	new->state = TH_RUN;
	new->swap_state = TH_SW_IN;

	/*
	 *	stack_switch is machine-dependent.  It plays with
	 *	the kernel_stack and pcb fields to move the stack.
	 *	It updates active_threads.  It may do other
	 *	machine-dependent components of a context-switch.
	 */

	stack_switch(old, new);
	thread_unlock(new);
	thread_unlock(old);

	/*
	 *	Finish switching to the new thread.  New thread
	 *	inherits old one's quantum and first_quantum flag.
	 */

#if	HW_FOOTPRINT
	new->last_processor = current_processor();
#endif	HW_FOOTPRINT
	timer_switch(&new->system_timer);
	ast_context(new, cpu_number());

    {
	task_t newtask = new->task;
	task_t oldtask = old->task;

	if (oldtask == newtask) {
		PMAP_CONTEXT(vm_map_pmap(newtask->map), new);
	} else {
		register pmap_t pmap;

		pmap = vm_map_pmap(oldtask->map);
		PMAP_DEACTIVATE_USER(pmap, old, cpu_number());

		pmap = vm_map_pmap(newtask->map);
		PMAP_ACTIVATE_USER(pmap, new, cpu_number());
	}
    }

	(void) splx(s);
	ipc_thread_switch_hits++;
	return TRUE;
#endif	KEEP_STACKS
}
