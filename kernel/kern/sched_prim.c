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
 * $Log:	sched_prim.c,v $
 * Revision 2.11  91/02/05  17:28:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:16:51  mrt]
 * 
 * Revision 2.10  91/01/08  15:16:38  rpd
 * 	Added KEEP_STACKS support.
 * 	[91/01/06            rpd]
 * 	Added thread_continue_calls counter.
 * 	[91/01/03  22:07:43  rpd]
 * 
 * 	Added continuation argument to thread_run.
 * 	[90/12/11            rpd]
 * 	Added continuation argument to thread_block/thread_continue.
 * 	Removed FAST_CSW conditionals.
 * 	[90/12/08            rpd]
 * 
 * 	Removed thread_swap_tick.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.9  90/09/09  14:32:36  rpd
 * 	Removed do_pset_scan call from sched_thread.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.8  90/08/07  22:22:54  rpd
 * 	Fixed casting of a volatile comparison: the non-volatile should be casted or else.
 * 	Removed silly set_leds() mips thingy.
 * 	[90/08/07  15:56:08  af]
 * 
 * Revision 2.7  90/08/07  17:58:55  rpd
 * 	Removed sched_debug; converted set_pri, update_priority,
 * 	and compute_my_priority to real functions.
 * 	Picked up fix for thread_block, to check for processor set mismatch.
 * 	Record last processor info on all multiprocessors.
 * 	[90/08/07            rpd]
 * 
 * Revision 2.6  90/06/02  14:55:49  rpd
 * 	Updated to new scheduling technology.
 * 	[90/03/26  22:16:03  rpd]
 * 
 * Revision 2.5  90/01/11  11:43:51  dbg
 * 	Check for cpu shutting down on exit from idle loop - next_thread
 * 	will be THREAD_NULL in this case.
 * 	[90/01/03            dbg]
 * 
 * 	Make sure cpu is marked active on all exits from idle loop.
 * 	[89/12/11            dbg]
 * 
 * 	Removed more lint.
 * 	[89/12/05            dbg]
 * 
 * 	DLB's scheduling changes in thread_block don't work if partially
 * 	applied; a thread can run in two places at once.  Revert to old
 * 	code, pending a complete merge.
 * 	[89/12/04            dbg]
 * 
 * Revision 2.4  89/11/29  14:09:11  af
 * 	On Mips, delay setting of active_threads inside load_context,
 * 	or we might take exceptions in an embarassing state.
 * 	[89/11/03  17:00:04  af]
 * 
 * 	Long overdue fix: the pointers that the idle thread uses to check
 * 	for someone to become runnable are now "volatile".  This prevents
 * 	smart compilers from overoptimizing (e.g. Mips).
 * 
 * 	While looking for someone to run in the idle_thread(), rotate
 * 	console lights on Mips to show we're alive [useful when machine
 * 	becomes catatonic].
 * 	[89/10/28            af]
 * 
 * Revision 2.3  89/09/08  11:26:22  dbg
 * 	Add extra thread state cases to thread_switch, since it may now
 * 	be called by a thread about to block.
 * 	[89/08/22            dbg]
 * 
 * 19-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Import timing definitions from sys/time_out.h.
 *	. Split uses of PMAP_ACTIVATE and PMAP_DEACTIVATE into
 *	  separate _USER and _KERNEL macros.
 *
 * Revision 2.8  88/12/19  02:46:33  mwyoung
 * 	Corrected include file references.  Use <kern/macro_help.h>.
 * 	[88/11/22            mwyoung]
 * 	
 * 	In thread_wakeup_with_result(), only lock threads that have the
 * 	appropriate wait_event.  Both the wait_event and the hash bucket
 * 	links are only modified with both the thread *and* hash bucket
 * 	locked, so it should be safe to read them with either locked.
 * 	
 * 	Documented the wait event mechanism.
 * 	
 * 	Summarized ancient history.
 * 	[88/11/21            mwyoung]
 * 
 * Revision 2.7  88/08/25  18:18:00  mwyoung
 * 	Corrected include file references.
 * 	[88/08/22            mwyoung]
 * 	
 * 	Avoid unsigned computation in wait_hash.
 * 	[88/08/16  00:29:51  mwyoung]
 * 	
 * 	Add priority check to thread_check; make queue index unsigned,
 * 	so that checking works correctly at all.
 * 	[88/08/11  18:47:55  mwyoung]
 * 
 * 11-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Support ast mechanism for threads.  Thread from local_runq gets
 *	minimum quantum to start.
 *
 *  9-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Moved logic to detect and clear next_thread[] dispatch to
 *	idle_thread() from thread_block().
 *	Maintain first_quantum field in thread instead of runrun.
 *	Changed preempt logic in thread_setrun.
 *	Avoid context switch if current thread is still runnable and
 *	processor would go idle as a result.
 *	Added scanner to unstick stuck threads.
 *
 * Revision 2.6  88/08/06  18:25:03  rpd
 * Eliminated use of kern/mach_ipc_defs.h.
 * 
 * 10-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Check for negative priority (BUG) in thread_setrun.
 *
 * Revision 2.5  88/07/20  16:39:35  rpd
 * Changed "NCPUS > 1" conditionals that were eliminating dead
 * simple locking code to MACH_SLOCKS conditionals.
 * 
 *  7-Jul-88  David Golub (dbg) at Carnegie-Mellon University
 *	Split uses of PMAP_ACTIVATE and PMAP_DEACTIVATE into separate
 *	_USER and _KERNEL macros.
 *
 * 15-Jun-88  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Removed excessive thread_unlock() occurrences in thread_wakeup.
 *	Problem discovered and solved by Richard Draves.
 *
 * Historical summary:
 *
 *	Redo priority recomputation. [dlb, 29 feb 88]
 *	New accurate timing. [dlb, 19 feb 88]
 *	Simplified choose_thread and thread_block. [dlb, 18 dec 87]
 *	Add machine-dependent hooks in idle loop. [dbg, 24 nov 87]
 *	Quantum scheduling changes. [dlb, 14 oct 87]
 *	Replaced scheduling logic with a state machine, and included
 *	 timeout handling. [dbg, 05 oct 87]
 *	Deactivate kernel pmap in idle_thread. [dlb, 23 sep 87]
 *	Favor local_runq in choose_thread. [dlb, 23 sep 87]
 *	Hacks for master processor handling. [rvb, 12 sep 87]
 *	Improved idle cpu and idle threads logic. [dlb, 24 aug 87]
 *	Priority computation improvements. [dlb, 26 jun 87]
 *	Quantum-based scheduling. [avie, dlb, apr 87]
 *	Improved thread swapper. [avie, 13 mar 87]
 *	Lots of bug fixes. [dbg, mar 87]
 *	Accurate timing support. [dlb, 27 feb 87]
 *	Reductions in scheduler lock contention. [dlb, 18 feb 87]
 *	Revise thread suspension mechanism. [avie, 17 feb 87]
 *	Real thread handling [avie, 31 jan 87]
 *	Direct idle cpu dispatching. [dlb, 19 jan 87]
 *	Initial processor binding. [avie, 30 sep 86]
 *	Initial sleep/wakeup. [dbg, 12 jun 86]
 *	Created. [avie, 08 apr 86]
 */
/*
 *	File:	sched_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Scheduling primitives
 *
 */

#include <cpus.h>
#include <simple_clock.h>
#include <mach_fixpri.h>
#include <mach_host.h>
#include <hw_footprint.h>

#include <mach/machine.h>
#include <kern/ast.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/macro_help.h>
#include <kern/processor.h>
#include <kern/queue.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>
#include <kern/thread_swap.h>
#include <kern/time_out.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <machine/machparam.h>	/* For def'n of splsched() */

#if	MACH_FIXPRI
#include <mach/policy.h>
#endif	MACH_FIXPRI

extern int hz;

int		min_quantum;	/* defines max context switch rate */

unsigned	sched_tick;

#if	SIMPLE_CLOCK
int		sched_usec;
#endif SIMPLE_CLOCK

thread_t	sched_thread_id;

thread_t	choose_pset_thread();

/*
 *	State machine
 *
 * states are combinations of:
 *  R	running
 *  W	waiting (or on wait queue)
 *  S	suspended (or will suspend)
 *  N	non-interruptible
 *  O	swapped out
 *
 * init	action 
 *	assert_wait	thread_block	clear_wait	suspend	resume	swapout	swapin
 *
 * R	RW, RWN		R;   setrun	-		RS	-	-
 * RS	RWS, RWNS	S;  wake_active	-		-	R	-
 * RN	RWN		RN;  setrun	-		RNS	-	-
 * RNS	RWNS		RNS; setrun	-		-	RN	-
 *
 * RW			W		R		RWS	-	-
 * RWN			WN		RN		RWNS	-	-
 * RWS			WS; wake_active	RS		-	RW	-
 * RWNS			WNS		RNS		-	RWN	-
 *
 * W					R;   setrun	WS	-	WO
 * WN					RN;  setrun	WNS	-	-
 * WNS					RNS; setrun	-	WN	-
 * WO					RO;  swapin	WSO	-	-
 *
 * S					-		-	R	SO
 * SO					-		-	RO	-
 * WS					S		-	W	WSO
 * WSO					SO		-	WO	-
 *
 * RO					-		RSO	-		R
 * RSO					-		-	RO		RS
 */

/*
 *	Waiting protocols and implementation:
 *
 *	Each thread may be waiting for exactly one event; this event
 *	is set using assert_wait().  That thread may be awakened either
 *	by performing a thread_wakeup_prim() on its event,
 *	or by directly waking that thread up with clear_wait().
 *
 *	The implementation of wait events uses a hash table.  Each
 *	bucket is queue of threads having the same hash function
 *	value; the chain for the queue (linked list) is the run queue
 *	field.  [It is not possible to be waiting and runnable at the
 *	same time.]
 *
 *	Locks on both the thread and on the hash buckets govern the
 *	wait event field and the queue chain field.  Because wakeup
 *	operations only have the event as an argument, the event hash
 *	bucket must be locked before any thread.
 *
 *	Scheduling operations may also occur at interrupt level; therefore,
 *	interrupts below splsched() must be prevented when holding
 *	thread or hash bucket locks.
 *
 *	The wait event hash table declarations are as follows:
 */

#define NUMQUEUES	59

queue_head_t		wait_queue[NUMQUEUES];
decl_simple_lock_data(,	wait_lock[NUMQUEUES])

#define wait_hash(event) \
	(((int)((event) < 0 ? ((event) ^ -1) : (event)))%NUMQUEUES)

void sched_init()
{
	min_quantum = hz / 10;		/* context switch 10 times/second */
	wait_queue_init();
	pset_sys_bootstrap();		/* initialize processer mgmt. */
	queue_init(&action_queue);
	simple_lock_init(&action_lock);
	sched_tick = 0;
#if	SIMPLE_CLOCK
	sched_usec = 0;
#endif	SIMPLE_CLOCK
	ast_init();
}

wait_queue_init()
{
	register int i;

	for (i = 0; i < NUMQUEUES; i++) {
		queue_init(&wait_queue[i]);
		simple_lock_init(&wait_lock[i]);
	}
}

/*
 *	Thread timeout routine, called when timer expires.
 *	Called at splsoftclock.
 */
thread_timeout(_thread, t)
	int	_thread;
	int	t;
{
	register thread_t	thread = (thread_t) _thread;
	int	s;
#ifdef	lint
	t++;
#endif	lint
	s = splsched();
	thread_lock(thread);
	thread->timer_set = FALSE;
	thread_unlock(thread);
	splx(s);

	clear_wait(thread, THREAD_TIMED_OUT, FALSE);
}

/*
 *	thread_set_timeout:
 *
 *	Set a timer for the current thread, if the thread
 *	is ready to wait.  Must be called between assert_wait()
 *	and thread_block().
 */
 
void thread_set_timeout(t)
	int	t;	/* timeout interval in ticks */
{
	register thread_t	thread = current_thread();
	register int s;

	s = splsched();
	thread_lock(thread);
	if ((thread->state & TH_WAIT) != 0) {
		thread->timer_set = TRUE;
		set_timeout(&thread->timer, t);
	}
	thread_unlock(thread);
	splx(s);
}

/*
 * Set up thread timeout element when thread is created.
 */
void thread_timeout_setup(thread)
	register thread_t	thread;
{
	thread->timer.private = TRUE;
	thread->timer.fcn = thread_timeout;
	thread->timer.param = (char *)thread;
}

/*
 *	assert_wait:
 *
 *	Assert that the current thread is about to go to
 *	sleep until the specified event occurs.
 */
void assert_wait(event, interruptible)
	int		event;
	boolean_t	interruptible;
{
	register queue_t	q;
	register int		index;
	register thread_t	thread;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif	MACH_SLOCKS
	int			s;

	thread = current_thread();
	if (thread->wait_event != 0) {
		printf("assert_wait: already asserted event 0x%x\n",
			thread->wait_event);
		panic("assert_wait");
	}
 	s = splsched();
	if (event != 0) {
		index = wait_hash(event);
		q = &wait_queue[index];
#if	MACH_SLOCKS
		lock = &wait_lock[index];
#endif	MACH_SLOCKS
		simple_lock(lock);
		thread_lock(thread);
		enqueue_tail(q, (queue_entry_t) thread);
		thread->wait_event = event;
		thread->state |= TH_WAIT;
		thread->interruptible = interruptible;
		thread_unlock(thread);
		simple_unlock(lock);
	}
	else {
		thread_lock(thread);
		thread->state |= TH_WAIT;
		thread->interruptible = interruptible;
		thread_unlock(thread);
	}
	splx(s);
}

/*
 *	clear_wait:
 *
 *	Clear the wait condition for the specified thread.  Start the thread
 *	executing if that is appropriate.
 *
 *	parameters:
 *	  thread		thread to awaken
 *	  result		Wakeup result the thread should see
 *	  interrupt_only	Don't wake up the thread if it isn't
 *				interruptible.
 */
void clear_wait(thread, result, interrupt_only)
	register thread_t	thread;
	int			result;
	boolean_t		interrupt_only;
{
	register int		index;
	register queue_t	q;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif	MACH_SLOCKS
	register int		event;
	int			s;

	s = splsched();
	thread_lock(thread);
	if (interrupt_only && !thread->interruptible) {
		/*
		 *	can't interrupt thread
		 */
		thread_unlock(thread);
		splx(s);
		return;
	}

	event = thread->wait_event;
	if (event != 0) {
		thread_unlock(thread);
		index = wait_hash(event);
		q = &wait_queue[index];
#if	MACH_SLOCKS
		lock = &wait_lock[index];
#endif	MACH_SLOCKS
		simple_lock(lock);
		/*
		 *	If the thread is still waiting on that event,
		 *	then remove it from the list.  If it is waiting
		 *	on a different event, or no event at all, then
		 *	someone else did our job for us.
		 */
		thread_lock(thread);
		if (thread->wait_event == event) {
			remqueue(q, (queue_entry_t)thread);
			thread->wait_event = 0;
			event = 0;		/* cause to run below */
		}
		simple_unlock(lock);
	}
	if (event == 0) {
		register int	state = thread->state;

		if (thread->timer_set) {
			thread->timer_set = FALSE;
			reset_timeout(&thread->timer);
		}
		switch (state) {
		    case TH_WAIT | TH_SUSP:
			/*
			 *	Suspend thread if interruptible
			 */
			if (thread->interruptible) {
			    thread->state = TH_SUSP;
			    thread->wait_result = result;
			    break;
			}
			/* fall through */
		    case TH_WAIT:
			/*
			 *	Sleeping and not suspendable - put
			 *	on run queue.
			 */
			thread->state = (state & ~TH_WAIT) | TH_RUN;
			thread->wait_result = result;
			thread_setrun(thread, TRUE);
			break;

		    case TH_WAIT | TH_SWAPPED:
			/*
			 *	Thread is swapped out, but runnable
			 */
			thread->state = TH_RUN | TH_SWAPPED;
			thread->wait_result = result;
			thread_swapin(thread, FALSE);
			break;

		    default:
			/*
			 *	Either already running, or suspended.
			 */
			if (state & TH_WAIT) {
				thread->state = state & ~TH_WAIT;
				thread->wait_result = result;
				thread->interruptible = TRUE;
			}
			break;
		}
	}
	thread_unlock(thread);
	splx(s);
}

/*
 *	thread_wakeup_prim:
 *
 *	Common routine for thread_wakeup, thread_wakeup_with_result,
 *	and thread_wakeup_one.
 *
 */
void thread_wakeup_prim(event, one_thread, result)
	register int	event;
	boolean_t	one_thread;
	int		result;
{
	register queue_t	q;
	register int		index;
	register thread_t	thread, next_th;
#if	MACH_SLOCKS
	register simple_lock_t	lock;
#endif	MACH_SLOCKS
	int			s;
	register int		state;

	index = wait_hash(event);
	q = &wait_queue[index];
	s = splsched();
#if	MACH_SLOCKS
	lock = &wait_lock[index];
#endif	MACH_SLOCKS
	simple_lock(lock);
	thread = (thread_t) queue_first(q);
	while (!queue_end(q, (queue_entry_t)thread)) {
		next_th = (thread_t) queue_next((queue_t) thread);

		if (thread->wait_event == event) {
			thread_lock(thread);
			remqueue(q, (queue_entry_t) thread);
			thread->wait_event = 0;
			if (thread->timer_set) {
				thread->timer_set = FALSE;
				reset_timeout(&thread->timer);
			}
			state = thread->state;
			switch (state) {
			    case TH_WAIT | TH_SUSP:
				if (thread->interruptible) {
				    thread->state = TH_SUSP;
				    thread->wait_result = result;
				    break;
				}
				/* fall through */
			    case TH_WAIT:
				thread->state = (state & ~TH_WAIT) | TH_RUN;
				thread->wait_result = result;
				thread_setrun(thread, TRUE);
				break;
				
			    case TH_WAIT | TH_SWAPPED:
				simple_unlock(lock);
				thread->state = TH_RUN | TH_SWAPPED;
				thread->wait_result = result;
				thread_swapin(thread, FALSE);
				thread_unlock(thread);

				if (one_thread) {
					/*
					 *	Done!
					 */
					splx(s);
					return;
				}

				/*
				 * Restart the search, since we had
				 * to unlock for thread_swapin (it calls
				 * thread_wakeup, which may find the
				 * swapin thread on the same queue).
				 */
				simple_lock(lock);
				thread = (thread_t) queue_first(q);
				continue;

			    default:
				if ((state & TH_WAIT) == 0)
				    panic("thread_wakeup");
				thread->state = state & ~TH_WAIT;
				thread->wait_result = result;
				thread->interruptible = TRUE;
				break;
			}
			thread_unlock(thread);
			if (one_thread)
				break;
		}
		thread = next_th;
	}
	simple_unlock(lock);
	splx(s);
}

/*
 *	thread_sleep:
 *
 *	Cause the current thread to wait until the specified event
 *	occurs.  The specified lock is unlocked before releasing
 *	the cpu.  (This is a convenient way to sleep without manually
 *	calling assert_wait).
 */
void thread_sleep(event, lock, interruptible)
	int		event;
	simple_lock_t	lock;
	boolean_t	interruptible;
{
	assert_wait(event, interruptible);	/* assert event */
	simple_unlock(lock);			/* release the lock */
	thread_block((void (*)()) 0);		/* block ourselves */
}

/*
 *	thread_bind:
 *
 *	Force a thread to execute on the specified processor.
 *	If the thread is currently executing, it may wait until its
 *	time slice is up before switching onto the specified processor.
 *
 *	A processor of PROCESSOR_NULL causes the thread to be unbound.
 *	xxx - DO NOT export this to users.
 */
thread_bind(thread, processor)
	register thread_t	thread;
	processor_t		processor;
{
	int		s;

	s = splsched();
	thread_lock(thread);
	thread->bound_processor = processor;
	thread_unlock(thread);
	(void) splx(s);
}

#ifdef	KEEP_STACKS
extern void thread_continue();

void switch_task_context(old_thread, continuation, new_thread)
	thread_t	old_thread;
	void		(*continuation)();
	thread_t	new_thread;
{
	if (save_context() == 0) {
		active_threads[cpu_number()] = new_thread;
		thread_continue(old_thread, continuation);
		load_context(new_thread);
		/*NOTREACHED*/
	}
}

#define switch_thread_context(old_thread, continuation, new_thread) \
	switch_task_context(old_thread, continuation, new_thread)
#endif	KEEP_STACKS

/*
 *	thread_block:
 *
 *	Block the current thread.  If the thread is runnable
 *	then someone must have woken it up between its request
 *	to sleep and now.  In this case, it goes back on a
 *	run queue.
 *
 *	If a continuation is specified, then thread_block will
 *	attempt to discard the thread's kernel stack.  When the
 *	thread resumes, it will execute the continuation function
 *	on a new kernel stack.
 */
void thread_block(continuation)
	void (*continuation)();
{
	register thread_t thread = current_thread();
	register thread_t	new_thread;
	register int		mycpu = cpu_number();
	register processor_t	myprocessor;
	int		s;

	myprocessor = cpu_to_processor(mycpu);

	s = splsched();
	
	myprocessor->first_quantum = TRUE;
	/*
	 *	Check for obvious simple case; local runq is
	 *	empty and global runq has entry at hint.
	 */
	if (myprocessor->runq.count > 0) {
		new_thread = choose_thread(myprocessor);
		myprocessor->quantum = min_quantum;
	}
	else {
		register run_queue_t	rq;

#if	MACH_HOST
		rq = &(myprocessor->processor_set->runq);
#else	MACH_HOST
		rq = &default_pset.runq;
#endif	MACH_HOST
		simple_lock(&rq->lock);
		if (rq->count == 0) {
			/*
			 *	Nothing else runnable.  Return if this
			 *	thread is still runnable on this processor.
			 *	Check for priority update if required.
			 */
			if ((thread->state == TH_RUN) &&
#if	MACH_HOST
			    (thread->processor_set == (processor_set_t) rq) &&
#endif	MACH_HOST
			    ((thread->bound_processor == PROCESSOR_NULL) ||
			     (thread->bound_processor == myprocessor))) {
				simple_unlock(&rq->lock);
				thread_lock(thread);
				if (thread->sched_stamp != sched_tick)
				    update_priority(thread);
				thread_unlock(thread);
				splx(s);
				return;
			}
			else {
				new_thread =
					choose_pset_thread(myprocessor, rq);
			}
		}
		else {
			register queue_t	q;
		
			/*
			 *	If there is a thread at hint, grab it,
			 *	else call choose_pset_thread.
			 */
			q = rq->runq + rq->low;

			if (queue_empty(q)) {
				rq->low++;
				new_thread =
					choose_pset_thread(myprocessor, rq);
			}
			else {
				new_thread = (thread_t) dequeue_head(q);
				new_thread->runq = RUN_QUEUE_NULL;
				rq->count--;
#if	MACH_FIXPRI
				/*
				 *	Cannot lazy evaluate rq->low for
				 *	fixed priority policy
				 */
				if ((rq->count > 0) &&
				    (myprocessor->processor_set->policies &
					POLICY_FIXEDPRI)) {
					    while (queue_empty(q)) {
						rq->low++;
						q++;
					    }
				}
#endif	MACH_FIXPRI
				simple_unlock(&rq->lock);
			}
		}

#if	MACH_FIXPRI
		if (new_thread->policy == POLICY_TIMESHARE) {
#endif	MACH_FIXPRI
#if	MACH_HOST
			myprocessor->quantum =
				myprocessor->processor_set->set_quantum;
#else	MACH_HOST
			myprocessor->quantum = default_pset.set_quantum;
#endif	MACH_HOST
#if	MACH_FIXPRI
		}
		else {
			/*
			 *	POLICY_FIXEDPRI
			 */
			myprocessor->quantum = new_thread->sched_data;
		}
#endif	MACH_FIXPRI
	}

	/*
	 *	Thread is now interruptible.
	 */
	new_thread->interruptible = TRUE;
#if	NCPUS > 1
	new_thread->last_processor = myprocessor;
#endif	NCPUS > 1

	/*
	 *	Set up ast context of new thread and switch to its timer.
	 */
	ast_context(new_thread, mycpu);
	timer_switch(&new_thread->system_timer);

	/*
	 *	WARNINGS:
	 *	    1.  switch_{task,thread}_context must do
	 *		active_threads[mycpu] = new_thread;
	 *
	 *	    2.  On hardware that can take page faults during
	 *		a context_save, PMAP_DEACTIVATE and PMAP_ACTIVATE
	 *		must not change the pmap.  Assembly language should
	 *		only change it after all operations that could
	 *		cause a page fault are completed.
	 *
	 *	All of this nonsense is necessitated by bizarre hardware
	 *	on which saving context can cause a trap or fault.  Handlers
	 *	for these traps and faults must not block (e.g. don't call
	 *	vm_fault).
	 */

	if (thread->task == new_thread->task) {
		PMAP_CONTEXT(vm_map_pmap(new_thread->task->map), new_thread);
		switch_thread_context(thread, continuation, new_thread);
	} else {
		PMAP_DEACTIVATE_USER(vm_map_pmap(thread->task->map),
				     thread, mycpu);
		PMAP_ACTIVATE_USER(vm_map_pmap(new_thread->task->map),
				   new_thread, mycpu);

		switch_task_context(thread, continuation, new_thread);
	}
	splx(s);
}

/*
 *	Thread continue dispatches a running thread that is not
 *	on a runq.  It is called by machine-dependent context switch
 *	code.  Caller must disable interrupts.
 *
 *	If a continuation is specified, then thread_continue will
 *	attempt to discard the thread's kernel stack.  When the
 *	thread resumes, it will execute the continuation function
 *	on a new kernel stack.
 */

unsigned int thread_continue_calls = 0;

void thread_continue(thread, continuation)
	thread_t	thread;
	void		(*continuation)();
{
	register int	state;

	thread_continue_calls++;

	/*
	 *	See if we should go back on a run queue.
	 */

	thread_lock(thread);
	state = thread->state;
	switch (state) {

	    case TH_RUN | TH_SUSP:
		if (thread->interruptible) {
		    /*
		     *	Suspend the thread
		     */
		    thread->state = TH_SUSP;
		    if (thread->wake_active) {
			thread->wake_active = FALSE;
			thread_unlock(thread);
			thread_wakeup((int)&thread->wake_active);
			thread_lock(thread);
		    }
		    break;
		}
		/* fall through */

	    case TH_RUN:
		/*
		 *	No reason to stop.  Put back on a run queue.
		 */
		thread_setrun(thread, FALSE);
		break;

	    case TH_RUN | TH_WAIT | TH_SUSP:
		if (thread->interruptible) {
		    /*
		     *	Suspended and interruptible.
		     */
		    thread->state = TH_WAIT | TH_SUSP;
		    if (thread->wake_active) {
			/*
			 *	Someone wants to know when this thread
			 *	really stops.
			 */
			thread->wake_active = FALSE;
			thread_unlock(thread);
			thread_wakeup((int)&thread->wake_active);
			thread_lock(thread);
		    }
		    break;
		}
		/* fall through */

	    case TH_RUN | TH_WAIT:
		/*
		 *	Waiting, and not suspended or not interruptible.
		 */
		thread->state = state & ~TH_RUN;
		break;

	    default:
		/*
		 *	Drop idle thread -- it is already in
		 *	idle_thread_array.
		 */
		if (state != (TH_RUN | TH_IDLE)) {
		    /*
		     *	Not running - oops
		     */
		    panic("thread_continue");
		}
		break;
	}
#ifndef	KEEP_STACKS
	if ((continuation != 0) &&
	    (thread->swap_state == TH_SW_IN) &&
	    ((thread->state & TH_RUN) == 0))
		thread_swapout(thread, continuation);
#endif	KEEP_STACKS
	thread_unlock(thread);
}

/*
 *	thread_run:
 *
 *	Switch directly from the current thread to a specified
 *	thread.  Both the current and new threads must be
 *	runnable.
 *
 *	If a continuation is specified, then thread_block will
 *	attempt to discard the current thread's kernel stack.  When the
 *	thread resumes, it will execute the continuation function
 *	on a new kernel stack.
 */
void thread_run(continuation, new_thread)
	void			(*continuation)();
	register thread_t	new_thread;
{
	register thread_t	thread = current_thread();
	register int		s, mycpu;

	mycpu = cpu_number();

	s = splsched();

	/*
	 *	Thread is now interruptible.
	 */
	new_thread->interruptible = TRUE;
#if	NCPUS > 1
	new_thread->last_processor = current_processor();
#endif	NCPUS > 1

	/*
	 *	New thread inherits old one's quantum and first_quantum flag.
	 *	Fixed priority threads must be fixed by caller.
	 */

	timer_switch(&new_thread->system_timer);

	ast_context(new_thread, mycpu);

	if (thread->task == new_thread->task) {
		PMAP_CONTEXT(vm_map_pmap(new_thread->task->map), new_thread);
		switch_thread_context(thread, continuation, new_thread);
	} else {
		PMAP_DEACTIVATE_USER(vm_map_pmap(thread->task->map),
				     thread, mycpu);
		PMAP_ACTIVATE_USER(vm_map_pmap(new_thread->task->map),
				   new_thread, mycpu);

		switch_task_context(thread, continuation, new_thread);
	}
	splx(s);
}


/*
 *	Define shifts for simulating (5/8)**n
 */

shift_data_t	wait_shift[32] = {
	{1,1},{1,3},{1,-3},{2,-7},{3,5},{3,-5},{4,-8},{5,7},
	{5,-7},{6,-10},{7,10},{7,-9},{8,-11},{9,12},{9,-11},{10,-13},
	{11,14},{11,-13},{12,-15},{13,17},{13,-15},{14,-17},{15,19},{16,18},
	{16,-19},{17,22},{18,20},{18,-20},{19,26},{20,22},{20,-22},{21,-27}};

/*
 *	set_pri:
 *
 *	Set the priority of the specified thread to the specified
 *	priority.  This may cause the thread to change queues.
 *
 *	The thread *must* be locked by the caller.
 */

set_pri(th,pri)
thread_t	th;
int		pri;
{
	register struct run_queue	*rq;

	rq = rem_runq(th);
	th->sched_pri = pri;
	if (rq != RUN_QUEUE_NULL) {
		thread_setrun(th, TRUE);
	}
}

/*
 *	do_priority_computation:
 *
 *	Calculate new priority for thread based on its base priority plus
 *	accumulated usage.  PRI_SHIFT and PRI_SHIFT_2 convert from
 *	usage to priorities.  SCHED_SHIFT converts for the scaling
 *	of the sched_usage field by SCHED_SCALE.  This scaling comes
 *	from the multiplication by sched_load (thread_timer_delta)
 *	in sched.h.  sched_load is calculated as a scaled overload
 *	factor in compute_mach_factor (mach_factor.c).
 */

#ifdef	PRI_SHIFT_2
#if	PRI_SHIFT_2 > 0
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> (PRI_SHIFT + SCHED_SHIFT))		\
	    + ((th)->sched_usage >> (PRI_SHIFT_2 + SCHED_SHIFT));	\
	if ((pri) > 31) (pri) = 31;					\
	MACRO_END
#else	PRI_SHIFT_2
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> (PRI_SHIFT + SCHED_SHIFT))		\
	    - ((th)->sched_usage >> (SCHED_SHIFT - PRI_SHIFT_2));	\
	if ((pri) > 31) (pri) = 31;					\
	MACRO_END
#endif	PRI_SHIFT_2
#else	defined(PRI_SHIFT_2)
#define do_priority_computation(th, pri)				\
	MACRO_BEGIN							\
	(pri) = (th)->priority	/* start with base priority */		\
	    + ((th)->sched_usage >> PRI_SHIFT + SCHED_SHIFT);		\
	if ((pri) > 31) (pri) = 31;					\
	MACRO_END
#endif	defined(PRI_SHIFT_2)

/*
 *	compute_priority:
 *
 *	Compute the effective priority of the specified thread.
 *	The effective priority computation is as follows:
 *
 *	Take the base priority for this thread and add
 *	to it an increment derived from its cpu_usage.
 *
 *	The thread *must* be locked by the caller. 
 */

compute_priority(thread)
	register thread_t	thread;
{
	register int	pri;

#if	MACH_FIXPRI
	if (thread->policy == POLICY_TIMESHARE) {
#endif	MACH_FIXPRI
	    do_priority_computation(thread, pri);
	    if (thread->depress_priority < 0)
		set_pri(thread, pri);
	    else
		thread->depress_priority = pri;
#if	MACH_FIXPRI
	}
	else {
	    set_pri(thread, thread->priority);
	}
#endif	MACH_FIXPRI
}

/*
 *	compute_my_priority:
 *
 *	Version of compute priority for current thread or thread
 *	being manipulated by scheduler (going on or off a runq).
 *	Only used for priority updates.  Policy or priority changes
 *	must call compute_priority above.  Caller must have thread
 *	locked and know it is timesharing and not depressed.
 */

compute_my_priority(thread)
register thread_t	thread;
{
	register int temp_pri;

	do_priority_computation(thread,temp_pri);
	thread->sched_pri = temp_pri;
}

/*
 *	recompute_priorities:
 *
 *	Update the priorities of all threads periodically.
 */
recompute_priorities()
{
#if	SIMPLE_CLOCK
	int	new_usec;
#endif	SIMPLE_CLOCK

	sched_tick++;		/* age usage one more time */
	timeout(recompute_priorities, (char *) 0, hz);
#if	SIMPLE_CLOCK
	/*
	 *	Compensate for clock drift.  sched_usec is an
	 *	exponential average of the number of microseconds in
	 *	a second.  It decays in the same fashion as cpu_usage.
	 */
	new_usec = sched_usec_elapsed();
	sched_usec = (5*sched_usec + 3*new_usec)/8;
#endif	SIMPLE_CLOCK
	/*
	 *	Wakeup scheduler thread.
	 */
	if (sched_thread_id != THREAD_NULL) {
		clear_wait(sched_thread_id, THREAD_AWAKENED, FALSE);
	}
}

/*
 *	update_priority
 *
 *	Cause the priority computation of a thread that has been 
 *	sleeping or suspended to "catch up" with the system.  Thread
 *	*MUST* be locked by caller.  If thread is running, then this
 *	can only be called by the thread on itself.
 */
update_priority(thread)
register thread_t	thread;
{
	register unsigned int	ticks;
	register shift_t	shiftp;
	register int		temp_pri;

	ticks = sched_tick - thread->sched_stamp;

	assert(ticks != 0);

	/*
	 *	If asleep for more than 30 seconds forget all
	 *	cpu_usage, else catch up on missed aging.
	 *	5/8 ** n is approximated by the two shifts
	 *	in the wait_shift array.
	 */
	thread->sched_stamp += ticks;
	thread_timer_delta(thread);
	if (ticks >  30) {
		thread->cpu_usage = 0;
		thread->sched_usage = 0;
	}
	else {
		thread->cpu_usage += thread->cpu_delta;
		thread->sched_usage += thread->sched_delta;
		shiftp = &wait_shift[ticks];
		if (shiftp->shift2 > 0) {
		    thread->cpu_usage =
			(thread->cpu_usage >> shiftp->shift1) +
			(thread->cpu_usage >> shiftp->shift2);
		    thread->sched_usage =
			(thread->sched_usage >> shiftp->shift1) +
			(thread->sched_usage >> shiftp->shift2);
		}
		else {
		    thread->cpu_usage =
			(thread->cpu_usage >> shiftp->shift1) -
			(thread->cpu_usage >> -(shiftp->shift2));
		    thread->sched_usage =
			(thread->sched_usage >> shiftp->shift1) -
			(thread->sched_usage >> -(shiftp->shift2));
		}
	}
	thread->cpu_delta = 0;
	thread->sched_delta = 0;
	/*
	 *	Recompute priority if appropriate.
	 */
	if (
#if	MACH_FIXPRI
	    (thread->policy == POLICY_TIMESHARE) &&
#endif	MACH_FIXPRI
	    (thread->depress_priority < 0)) {
		do_priority_computation(thread, temp_pri);
		thread->sched_pri = temp_pri;
	}
}

/*
 *	run_queue_enqueue macro for thread_setrun().
 */
#if	DEBUG
#define run_queue_enqueue(rq, th)					\
	MACRO_BEGIN							\
	    register unsigned int	whichq;				\
									\
	    whichq = (th)->sched_pri;					\
	    if (whichq >= NRQS) {					\
	printf("thread_setrun: pri too high (%d)\n", (th)->sched_pri);  \
		whichq = NRQS - 1;					\
	    }								\
									\
	    simple_lock(&(rq)->lock);	/* lock the run queue */	\
	    enqueue_tail(&(rq)->runq[whichq], (queue_entry_t) (th));	\
									\
	    if (whichq < (rq)->low || (rq)->count == 0) 		\
		 (rq)->low = whichq;	/* minimize */			\
									\
	    (rq)->count++;						\
	    (th)->runq = (rq);						\
	    thread_check((th), (rq));					\
	    checkrq((rq));						\
	    simple_unlock(&(rq)->lock);					\
	MACRO_END
#else	DEBUG
#define run_queue_enqueue(rq, th)					\
	MACRO_BEGIN							\
	    register unsigned int	whichq;				\
									\
	    whichq = (th)->sched_pri;					\
	    if (whichq >= NRQS) {					\
	printf("thread_setrun: pri too high (%d)\n", (th)->sched_pri);  \
		whichq = NRQS - 1;					\
	    }								\
									\
	    simple_lock(&(rq)->lock);	/* lock the run queue */	\
	    enqueue_tail(&(rq)->runq[whichq], (queue_entry_t) (th));	\
									\
	    if (whichq < (rq)->low || (rq)->count == 0) 		\
		 (rq)->low = whichq;	/* minimize */			\
									\
	    (rq)->count++;						\
	    (th)->runq = (rq);						\
	    simple_unlock(&(rq)->lock);					\
	MACRO_END
#endif	DEBUG
/*
 *	thread_setrun:
 *
 *	Make thread runnable; dispatch directly onto an idle processor
 *	if possible.  Else put on appropriate run queue (processor
 *	if bound, else processor set.  Caller must have lock on thread.
 *	This is always called at splsched.
 */

void thread_setrun(th, may_preempt)
	register thread_t	th;
	boolean_t		may_preempt;
{
	register processor_t	processor;
	register run_queue_t	rq;
#if	NCPUS > 1
	register processor_set_t	pset;
#endif	NCPUS > 1

	/*
	 *	Update priority if needed.
	 */
	if (th->sched_stamp != sched_tick) {
		update_priority(th);
	}

	assert(th->runq == RUN_QUEUE_NULL);

#if	NCPUS > 1
	/*
	 *	Try to dispatch the thread directly onto an idle processor.
	 */
	if ((processor = th->bound_processor) == PROCESSOR_NULL) {
	    /*
	     *	Not bound, any processor in the processor set is ok.
	     */
	    pset = th->processor_set;
#if	HW_FOOTPRINT
	    /*
	     *	But first check the last processor it ran on.
	     */
	    processor = th->last_processor;
	    if (processor->state == PROCESSOR_IDLE) {
		    simple_lock(&processor->lock);
		    simple_lock(&pset->idle_lock);
		    if ((processor->state == PROCESSOR_IDLE)
#if	MACH_HOST
			&& (processor->processor_set == pset)
#endif	MACH_HOST
			) {
			    queue_remove(&pset->idle_queue, processor,
			        processor_t, processor_queue);
			    pset->idle_count--;
			    processor->next_thread = th;
			    processor->state = PROCESSOR_DISPATCHING;
			    simple_unlock(&pset->idle_lock);
			    simple_unlock(&processor->lock);
		            return;
		    }
		    simple_unlock(&pset->idle_lock);
		    simple_unlock(&processor->lock);
	    }
#endif	HW_FOOTPRINT

	    if (pset->idle_count > 0) {
		simple_lock(&pset->idle_lock);
		if (pset->idle_count > 0) {
		    processor = (processor_t) queue_first(&pset->idle_queue);
		    queue_remove(&(pset->idle_queue), processor, processor_t,
				processor_queue);
		    pset->idle_count--;
		    processor->next_thread = th;
		    processor->state = PROCESSOR_DISPATCHING;
		    simple_unlock(&pset->idle_lock);
		    return;
		}
		simple_unlock(&pset->idle_lock);
	    }
	    rq = &(pset->runq);
	    run_queue_enqueue(rq,th);
	    /*
	     * Preempt check
	     */
	    if (may_preempt &&
#if	MACH_HOST
		(pset == current_processor()->processor_set) &&
#endif	MACH_HOST
		(current_thread()->sched_pri > th->sched_pri)) {
			/*
			 *	Turn off first_quantum to allow csw.
			 */
			current_processor()->first_quantum = FALSE;
			aston();
	    }
	}
	else {
	    /*
	     *	Bound, can only run on bound processor.  Have to lock
	     *  processor here because it may not be the current one.
	     */
	    if (processor->state == PROCESSOR_IDLE) {
		simple_lock(&processor->lock);
		pset = processor->processor_set;
		simple_lock(&pset->idle_lock);
		if (processor->state == PROCESSOR_IDLE) {
		    queue_remove(&pset->idle_queue, processor,
			processor_t, processor_queue);
		    pset->idle_count--;
		    processor->next_thread = th;
		    processor->state = PROCESSOR_DISPATCHING;
		    simple_unlock(&pset->idle_lock);
		    simple_unlock(&processor->lock);
		    return;
		}
		simple_unlock(&pset->idle_lock);
		simple_unlock(&processor->lock);
	    }
	    rq = &(processor->runq);
	    run_queue_enqueue(rq,th);

	    /*
	     *	Cause ast on processor if processor is on line.
	     *
	     *	XXX Don't do this remotely to master because this will
	     *	XXX send an interprocessor interrupt, and that's too
	     *  XXX expensive for all the unparallelized U*x code.
	     */
	    if (processor == current_processor()) {
		aston();
	    }
	    else if ((processor != master_processor) &&
	    	     (processor->state != PROCESSOR_OFF_LINE)) {
			cause_ast_check(processor);
	    }
	}
#else	NCPUS > 1
	/*
	 *	XXX should replace queue with a boolean in this case.
	 */
	if (default_pset.idle_count > 0) {
	    processor = (processor_t) queue_first(&default_pset.idle_queue);
	    queue_remove(&default_pset.idle_queue, processor,
		processor_t, processor_queue);
	    default_pset.idle_count--;
	    processor->next_thread = th;
	    processor->state = PROCESSOR_DISPATCHING;
	    return;
	}
	if (th->bound_processor == PROCESSOR_NULL) {
	    	rq = &(default_pset.runq);
	}
	else {
		rq = &(master_processor->runq);
		aston();
	}
	run_queue_enqueue(rq,th);

	/*
	 * Preempt check
	 */
	if (may_preempt && (current_thread()->sched_pri > th->sched_pri)) {
		/*
		 *	Turn off first_quantum to allow context switch.
		 */
		current_processor()->first_quantum = FALSE;
		aston();
	}
#endif	NCPUS > 1
}

/*
 *	rem_runq:
 *
 *	Remove a thread from its run queue.
 *	The run queue that the process was on is returned
 *	(or RUN_QUEUE_NULL if not on a run queue).  Thread *must* be locked
 *	before calling this routine.  Unusual locking protocol on runq
 *	field in thread structure makes this code interesting; see thread.h.
 */

struct run_queue *rem_runq(th)
	thread_t		th;
{
	register struct run_queue	*rq;

	rq = th->runq;
	/*
	 *	If rq is RUN_QUEUE_NULL, the thread will stay out of the
	 *	run_queues because the caller locked the thread.  Otherwise
	 *	the thread is on a runq, but could leave.
	 */
	if (rq != RUN_QUEUE_NULL) {
		simple_lock(&rq->lock);
#if	DEBUG
		checkrq(rq);
#endif	DEBUG
		if (rq == th->runq) {
			/*
			 *	Thread is in a runq and we have a lock on
			 *	that runq.
			 */
#if	DEBUG
			checkrq(rq);
			thread_check(th, rq);
#endif	DEBUG
			remqueue(&rq->runq[0], (queue_entry_t) th);
			rq->count--;
#if	DEBUG
			checkrq(rq);
#endif	DEBUG
			th->runq = RUN_QUEUE_NULL;
			simple_unlock(&rq->lock);
		}
		else {
			/*
			 *	The thread left the runq before we could
			 * 	lock the runq.  It is not on a runq now, and
			 *	can't move again because this routine's
			 *	caller locked the thread.
			 */
			simple_unlock(&rq->lock);
			rq = RUN_QUEUE_NULL;
		}
	}

	return(rq);
}


/*
 *	choose_thread:
 *
 *	Choose a thread to execute.  The thread chosen is removed
 *	from its run queue.  Note that this requires only that the runq
 *	lock be held.
 *
 *	Strategy:
 *		Check processor runq first; if anything found, run it.
 *		Else check pset runq; if nothing found, return idle thread.
 *
 *	Second line of strategy is implemented by choose_pset_thread.
 *	This is only called on processor startup and when thread_block
 *	thinks there's something in the processor runq.
 */

thread_t choose_thread(myprocessor)
processor_t myprocessor;
{
	thread_t th;
	register queue_t q;
	register run_queue_t runq;
	register int i;

	runq = &myprocessor->runq;

	simple_lock(&runq->lock);
	if (runq->count > 0) {
	    q = runq->runq + runq->low;
	    for (i = runq->low; i < NRQS ; i++, q++) {
		if (!queue_empty(q)) {
		    th = (thread_t) dequeue_head(q);
		    th->runq = RUN_QUEUE_NULL;
		    runq->count--;
		    runq->low = i;
		    simple_unlock(&runq->lock);
		    return(th);
		}
	    }
	    panic("choose_thread");
	    /*NOTREACHED*/
	}
	simple_unlock(&runq->lock);

	runq = &(myprocessor->processor_set->runq);

	simple_lock(&runq->lock);
	return(choose_pset_thread(myprocessor,runq));
}

/*
 *	choose_pset_thread:  choose a thread from processor_set runq or
 *		set processor idle and choose its idle thread.
 *
 *	Caller must be at splsched and have a lock on the runq.  This
 *	lock is released by this routine.  myprocessor is always the current
 *	processor, and runq must be the runq for its processor set.
 *	This routine chooses and removes a thread from the runq if there
 *	is one (and returns it), else it sets the processor idle and
 *	returns its idle thread.
 */

thread_t choose_pset_thread(myprocessor,runq)
register processor_t myprocessor;
register run_queue_t runq;
{
	register thread_t th;
	register queue_t q;
	register int i;

	/*
	 *	This definition of pset works because the run queue
	 *	is at offset 0 in the pset structure.
	 */
#define	pset	((processor_set_t) runq)

	if (runq->count > 0) {
	    q = runq->runq + runq->low;
	    for (i = runq->low; i < NRQS ; i++, q++) {
		if (!queue_empty(q)) {
		    th = (thread_t) dequeue_head(q);
		    th->runq = RUN_QUEUE_NULL;
		    runq->count--;
		    /*
		     *	For POLICY_FIXEDPRI, runq->low must be
		     *	accurate!
		     */
#if	MACH_FIXPRI
		    if ((runq->count > 0) &&
			(pset->policies & POLICY_FIXEDPRI)) {
			    while (queue_empty(q)) {
				q++;
				i++;
			    }
		    }
#endif	MACH_FIXPRI
		    runq->low = i;
		    simple_unlock(&runq->lock);
		    return(th);
		}
	    }
	    panic("choose_pset_thread");
	    /*NOTREACHED*/
	}
	simple_unlock(&runq->lock);

	/*
	 *	Nothing is runnable, so set this processor idle if it
	 *	was running.  If it was in an assignment or shutdown,
	 *	leave it alone.  Return its idle thread.
	 */
	simple_lock(&pset->idle_lock);
	if (myprocessor->state == PROCESSOR_RUNNING) {
	    myprocessor->state = PROCESSOR_IDLE;
	    /*
	     *	XXX Until it goes away, put master on end of queue, others
	     *	XXX on front so master gets used last.
	     */
	    if (myprocessor == master_processor) {
		queue_enter(&(pset->idle_queue), myprocessor,
			processor_t, processor_queue);
	    }
	    else {
		queue_enter_first(&(pset->idle_queue), myprocessor,
			processor_t, processor_queue);
	    }

	    pset->idle_count++;
	}
	simple_unlock(&pset->idle_lock);

	return(myprocessor->idle_thread);

#undef	pset
}

/*
 *	This is the idle thread, which just looks for other threads
 *	to execute.
 */


/*
 *	no_dispatch_count counts number of times processors go non-idle
 *	without being dispatched.  This should be very rare.
 */
int	no_dispatch_count = 0;

void idle_thread()
{
	register volatile thread_t *threadp;
	register volatile int *gcount;
	register volatile int *lcount;
	register thread_t new_thread;
	register processor_t myprocessor;
	register int state;
	thread_t th;
	int mycpu;

	(void) splsched();
	th = current_thread();			/* who am I? */
	mycpu = cpu_number();			/* where am I? */
	myprocessor = cpu_to_processor(mycpu);

	th->priority = 31;
	th->sched_pri = 31;

	/*
	 *	Set the idle flag to indicate that this is an idle thread,
	 *	enter ourselves in the idle array, and thread_block() to get
	 *	out of the run queues (and set the processor idle when we
	 *	run next time).
	 */
	thread_lock(th);
	th->state |= TH_IDLE;
	thread_unlock(th);
	myprocessor->idle_thread = th;
	thread_block((void (*)()) 0);

	threadp = (volatile thread_t *) &myprocessor->next_thread;
	lcount = (volatile int *) &myprocessor->runq.count;

#if	!MACH_HOST
	gcount = (volatile int *) &default_pset.runq.count;
#endif	!MACH_HOST

	while (TRUE) {

#ifdef	MARK_CPU_IDLE
		MARK_CPU_IDLE(mycpu);
#endif	MARK_CPU_IDLE

#if	MACH_HOST
		gcount = (volatile int *)
			&myprocessor->processor_set->runq.count;
#endif	MACH_HOST

		(void) spl0();	/* for idle loop */
/*
 *	This cpu will be dispatched (by thread_setrun) by setting next_thread
 *	to the value of the thread to run next.  Also check runq counts
 *	and should_exit.
 *
 */
		while ((*threadp == (volatile thread_t)THREAD_NULL) &&
		       (*gcount == 0) && (*lcount == 0))
				continue;		/* wait */

		(void) splsched();

#ifdef	MARK_CPU_ACTIVE
		MARK_CPU_ACTIVE(mycpu);
#endif	MARK_CPU_ACTIVE

		/*
		 *	This is not a switch statement to avoid the
		 *	bounds checking code in the common case.
		 */
retry:
		state = myprocessor->state;
		if (state == PROCESSOR_DISPATCHING) {
			/*
			 *	Commmon case -- cpu dispatched.
			 */
			new_thread = (thread_t) *threadp;
			*threadp = (volatile thread_t) THREAD_NULL;
			myprocessor->state = PROCESSOR_RUNNING;
			/*
			 *	set up quantum for new thread.
			 */
#if	MACH_FIXPRI
			if (new_thread->policy == POLICY_TIMESHARE) {
#endif	MACH_FIXPRI
				/*
				 *  Just use set quantum.  No point in
				 *  checking for shorter local runq quantum;
				 *  csw_needed will handle correctly.
				 */
#if	MACH_HOST
				myprocessor->quantum = new_thread->
					processor_set->set_quantum;
#else	MACH_HOST
				myprocessor->quantum =
					default_pset.set_quantum;
#endif	MACH_HOST

#if	MACH_FIXPRI
			}
			else {
				/*
				 *	POLICY_FIXEDPRI
				 */
				myprocessor->quantum = new_thread->sched_data;
			}
#endif	MACH_FIXPRI
			myprocessor->first_quantum = TRUE;
			thread_run((void (*)()) 0, new_thread);
		}
		else if (state == PROCESSOR_IDLE) {
			register processor_set_t pset;

			pset = myprocessor->processor_set;
			simple_lock(&pset->idle_lock);
			if (myprocessor->state != PROCESSOR_IDLE) {
				/*
				 *	Something happened, try again.
				 */
				simple_unlock(&pset->idle_lock);
				goto retry;
			}
			/*
			 *	Processor was not dispatched (Rare).
			 *	Set it running again.
			 */
			no_dispatch_count++;
			pset->idle_count--;
			queue_remove(&pset->idle_queue, myprocessor,
				processor_t, processor_queue);
			myprocessor->state = PROCESSOR_RUNNING;
			simple_unlock(&pset->idle_lock);
			thread_block((void (*)()) 0);
		}
		else if ((state == PROCESSOR_ASSIGN) ||
			 (state == PROCESSOR_SHUTDOWN)) {
			/*
			 *	Changing processor sets, or going off-line.
			 *	Release next_thread if there is one.  Actual
			 *	thread to run in on a runq.
			 */
			if ((new_thread = (thread_t)*threadp)!= THREAD_NULL) {
				*threadp = (volatile thread_t) THREAD_NULL;
				thread_setrun(new_thread, FALSE);
			}

			thread_block((void (*)()) 0);
		}
		else {
			printf(" Bad processor state %d (Cpu %d)\n",
				cpu_state(mycpu), mycpu);
			panic("idle_thread");
		}
	}
}
		
/*
 *	sched_thread: scheduler thread.
 *
 *	This thread handles periodic calculations in the scheduler that
 *	we don't want to do at interrupt level.  This allows us to
 *	avoid blocking 
 */
void sched_thread()
{
    sched_thread_id = current_thread();

    while (TRUE) {
	/*
	 *	Sleep on event 0, recompute_priorities() will awaken
	 *	us by calling clear_wait().
	 */
	assert_wait(0, FALSE);
	thread_block((void (*)()) 0);
	(void) compute_mach_factor();
	/*
	 *	Check for stuck threads.  This and the pset scan can't be
	 *	done off of the callout queue because they require operations
	 *	that can't be used from interrupt level.
	 */
	if (sched_tick & 1) {
	    	do_thread_scan();
	}
    }
}

#define	MAX_STUCK_THREADS	16

/*
 *	do_thread_scan: scan for stuck threads.  A thread is stuck if
 *	it is runnable but its priority is so low that it has not
 *	run for several seconds.  It's priority should be higher, but
 *	won't be until it runs and calls update_priority.  The scanner
 *	finds these threads and does the updates.
 *
 *	Scanner runs in two passes.  Pass one squirrels likely
 *	thread id's away in an array  (takes out references for them).
 *	Pass two does the priority updates.  This is necessary because
 *	the run queue lock is required for the candidate scan, but
 *	cannot be held during updates [set_pri will deadlock].
 *
 *	Array length should be enough so that restart isn't necessary,
 *	but restart logic is included.  Does not scan processor runqs.
 *
 */

thread_t		stuck_threads[MAX_STUCK_THREADS];
int			stuck_count = 0;

/*
 *	do_runq_scan is the guts of pass 1.  It scans a runq for
 *	stuck threads.  A boolean is returned indicating whether
 *	it ran out of space.
 */

boolean_t
do_runq_scan(runq)
run_queue_t	runq;
{
	register int		s;
	register queue_t	q;
	register thread_t	thread;
	register int		count;

	s = splsched();
	simple_lock(&runq->lock);
	if((count = runq->count) > 0) {
	    q = runq->runq + runq->low;
	    while (count > 0) {
		thread = (thread_t) queue_first(q);
		while(!queue_end(q, (queue_entry_t)thread)) {
		    if (thread->state == TH_RUN &&
			sched_tick - thread->sched_stamp > 1) {
			    /*
			     *	Stuck, save its id for later.
			     */
			    if (stuck_count == MAX_STUCK_THREADS) {
				/*
				 *	!@#$% No more room.
				 */
				simple_unlock(&runq->lock);
				splx(s);
				return(TRUE);
			    }
			    /*
			     *	Inline version of thread_reference
			     */
			    thread_lock(thread);
			    thread->ref_count++;
			    thread_unlock(thread);
			    stuck_threads[stuck_count++] = thread;
		    }
		    count--;
		    thread = (thread_t) queue_next((queue_t)thread);
		}
		q++;
	    }
	}
	simple_unlock(&runq->lock);
	splx(s);

	return(FALSE);
}

do_thread_scan()
{
	register int		s;
	register boolean_t	restart_needed;
	register thread_t	thread;
#if	MACH_HOST
	register processor_set_t	pset;
#endif	MACH_HOST

	do {
#if	MACH_HOST
	    simple_lock(&all_psets_lock);
	    pset = (processor_set_t) queue_first(&all_psets);
	    while (!queue_end(&all_psets, (queue_entry_t) pset)) {
		if (restart_needed = do_runq_scan(&pset->runq))
			break;
		pset = (processor_set_t) queue_next(&pset->all_psets);
	    }
	    simple_unlock(&all_psets_lock);
#else	MACH_HOST
	    restart_needed = do_runq_scan(&default_pset.runq);
#endif	MACH_HOST
	    if (!restart_needed)
	    	restart_needed = do_runq_scan(&master_processor->runq);

	    /*
	     *	Ok, we now have a collection of candidates -- fix them.
	     */

	    while (stuck_count > 0) {
		thread = stuck_threads[--stuck_count];
		stuck_threads[stuck_count] = THREAD_NULL;
		s = splsched();
		thread_lock(thread);
		if (thread->state == TH_RUN &&
		    sched_tick - thread->sched_stamp > 1) {
			/*
			 *	Do the priority update.  Have to do
			 *	separate set_pri because thread is probably
			 *	on wrong queue for new priority.
			 */
			update_priority(thread);
#if	MACH_FIXPRI
			if (thread->policy != POLICY_FIXEDPRI)
#endif	MACH_FIXPRI
				set_pri(thread, thread->sched_pri);
		}
		thread_unlock(thread);
		splx(s);
		thread_deallocate(thread);
	    }

	} while (restart_needed);
}
		
/*
 *	Just in case someone doesn't use the macro
 */
#undef	thread_wakeup
void		thread_wakeup(x)
	register int	x;
{
	thread_wakeup_with_result(x, THREAD_AWAKENED);
}

#if	someday
/*
 *	create_wait(event, persistent)
 *
 *	Create and initialize a new wait structure corresponding to the
 *	specified event.  A persistence is specified to determine if the
 *	wait structure should ever be deallocated.  (Persistent wait
 *	structures are used for frequently used events).
 *
 *	The wait structure is entered into the event hash table.
 */

wait_t create_wait(event, persistent)
	event_t		event;
	boolean_t	persistent;
{
	wait_t		wait;
	wait_bucket_t	bucket;

	/*
	 *	Allocate and initialize and wait structure.
	 */

	wait = (wait_t) zalloc(wait_zone);
	wait->event = event;
	wait->persistent = persistent;
	queue_init(&wait->thread_list);
	simple_lock_init(&wait->lock);

	/*
	 *	Insert the wait structure into the hash table.
	 */

	bucket = wait_buckets[wait_hash(event)];
	lock_write(&bucket->lock);
	queue_enter(&bucket->waitq, wait, wait_t, listq);
	lock_write_done(&bucket->lock);
}

/*
 *	assert_wait(event)
 *
 *	assert that the current thread wants to wait for the specified
 *	event.  The thread does not actually wait at this time, but it
 *	should wait sometime in the near future.
 */

assert_wait(event)
	event_t		event;
{
	wait_t		wait;
	wait_bucket_t	bucket;
	boolean_t	found;
	thread_t	thread;

	/*
	 *	Find the bucket for this event.
	 */

	bucket = wait_buckets[wait_hash(event)];
	found = FALSE;

	/*
	 *	See if there is already a wait structure for the
	 *	event.
	 */

	lock_read(&bucket->lock);
	wait = (wait_t) queue_first(&bucket->waitq);
	while (!queue_end(&bucket->waitq, (queue_entry_t) wait)) {
		if (wait->event == event) {
			found = TRUE;
			break;
		}
		wait = (wait_t) queue_next(&wait->listq);
	}
	lock_read_done(&bucket->lock);

	/*
	 *	If there was no such entry, then create (and insert)
	 *	a non-persistent wait structure.
	 */

	if (!found)
		wait = create_wait(FALSE);
	}

	/*
	 *	Now we have a wait structure corresponding to our event
	 *	(which is in the hash table).  We must now insert our thread
	 *	into the list of threads waiting for this event, which means
	 *	we create yet another structure to represent it (because a
	 *	thread may be waiting for more than one event).
	 *
	 *	Then, link the thread wait structure into the thread list
	 *	of what it is waiting on.
	 */

	thread = current_thread();
	twait = (thread_wait_t) zalloc(thread_wait_zone);
	twait->thread = thread;

	simple_lock(&wait->lock);
	queue_enter(&wait->thread_list, twait, thread_wait_t, threadq);
	queue_enter(&thread->wait_list, twait, thread_wait_t, waitq);
	simple_unlock(&wait->lock);
}
#endif	someday
#if	DEBUG
checkrq(rq)
	run_queue_t	rq;
{
	register queue_t	q1;
	register int		i, j;
	register queue_entry_t	e;

	j = 0;
	q1 = rq->runq;
	for (i = 0; i < NRQS; i++) {
		if (q1->next == q1) {
			if (q1->prev != q1->next)
				panic("checkrq");
		}
		for (e = q1->next; e != q1; e = e->next) {
			j++;
			if (e->next->prev != e)
				panic("checkrq-2");
			if (e->prev->next != e)
				panic("checkrq-3");
		}
		q1++;
	}
	if (j != rq->count)
		panic("checkrq-count");
}

thread_check(th, rq)
	register thread_t	th;
	register run_queue_t	rq;
{
	register unsigned int 	whichq;

	whichq = th->sched_pri;
	if (whichq >= NRQS) {
		printf("thread_check: priority too high\n");
		whichq = NRQS-1;
	}
	if ((th->links.next == &rq->runq[whichq]) &&
		(rq->runq[whichq].prev != (queue_entry_t)th))
			panic("thread_check");
}
#endif	DEBUG
