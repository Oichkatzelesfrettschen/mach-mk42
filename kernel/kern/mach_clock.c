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
 * $Log:	mach_clock.c,v $
 * Revision 2.11  91/02/05  17:27:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:47  mrt]
 * 
 * Revision 2.10  91/01/08  15:16:22  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.9  90/11/05  14:31:27  rpd
 * 	Unified untimeout and untimeout_try.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.8  90/10/12  18:07:29  rpd
 * 	Fixed calls to thread_bind in host_set_time.
 * 	Fix from Philippe Bernadat.
 * 	[90/10/10            rpd]
 * 
 * Revision 2.7  90/09/09  14:32:18  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.6  90/08/27  22:02:48  dbg
 * 	Add untimeout_try for multiprocessors.  Reduce lint.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.5  90/06/02  14:55:04  rpd
 * 	Converted to new IPC and new host port technology.
 * 	[90/03/26  22:10:04  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:31  dbg
 * 	Switch to master CPU in host_set_time.
 * 	[90/01/03            dbg]
 * 
 * Revision 2.3  89/08/09  14:33:09  rwd
 * 	Include mach/vm_param.h and use PAGE_SIZE instead of NBPG.
 * 	[89/08/08            rwd]
 * 	Removed timemmap to machine/model_dep.c
 * 	[89/08/08            rwd]
 * 
 * Revision 2.2  89/08/05  16:07:11  rwd
 * 	Added mappable time code.
 * 	[89/08/02            rwd]
 * 
 * 14-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Split into two new files: mach_clock (for timing) and priority
 *	(for priority calculation).
 *
 *  8-Dec-88  David Golub (dbg) at Carnegie-Mellon University
 *	Use sentinel for root of timer queue, to speed up search loops.
 *
 * 30-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */ 
/*
 *	File:	clock_prim.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Clock primitives.
 */
#include <cpus.h>
#include <stat_time.h>

#include <mach/boolean.h>
#include <mach/machine.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/cpu_number.h>
#include <kern/host.h>
#include <kern/lock.h>
#include <kern/mach_param.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <kern/time_stamp.h>
#include <vm/vm_kern.h>
#include <sys/time.h>
#include <machine/mach_param.h>	/* HZ */
#include <machine/machparam.h>

extern void	thread_quantum_update();

void softclock();		/* forward */

int		hz = HZ;		/* number of ticks per second */
int		tick = (1000000 / HZ);	/* number of usec per tick */
time_value_t	time = { 0, 0 };	/* time since bootup (uncorrected) */
time_value_t	*mtime = (time_value_t *) 0;	/* a mappable timer */
unsigned long	elapsed_ticks = 0;	/* ticks elapsed since bootup */


decl_simple_lock_data(,	timer_lock)	/* lock for ... */
timer_elt_data_t	timer_head;	/* ordered list of timeouts */
					/* (doubles as end-of-list) */
zone_t			timer_zone;	/* zone for free timers */

int	n_timer_max = 60;	/* 60 */
int	n_timer_alloc = 60;	/* allocate this many to start */

/*
 *	Handle clock interrupts.
 *
 *	The clock interrupt is assumed to be called at a (more or less)
 *	constant rate.  The rate must be identical on all CPUS (XXX - fix).
 *
 *	Usec is the number of microseconds that have elapsed since the
 *	last clock tick.  It may be constant or computed, depending on
 *	the accuracy of the hardware clock.
 *
 */
void clock_interrupt(usec, usermode, basepri)
	register int	usec;		/* microseconds per tick */
	boolean_t	usermode;	/* executing user code */
	boolean_t	basepri;	/* at base priority */
{
	register int		my_cpu = cpu_number();
	register thread_t	thread = current_thread();

#if	STAT_TIME
	/*
	 *	Increment the thread time, if using
	 *	statistical timing.
	 */
	if (usermode) {
	    timer_bump(&thread->user_timer, usec);
	}
	else {
	    timer_bump(&thread->system_timer, usec);
	}
#endif	STAT_TIME

	/*
	 *	Increment the CPU time statistics.
	 */
	{
	    register int	state;

	    if (usermode)
		state = CPU_STATE_USER;
	    else if (!cpu_idle(my_cpu))
		state = CPU_STATE_SYSTEM;
	    else
		state = CPU_STATE_IDLE;

	    machine_slot[my_cpu].cpu_ticks[state]++;

	    /*
	     *	Adjust the thread's priority and check for
	     *	quantum expiration.
	     */

	    thread_quantum_update(my_cpu, thread, 1, state);
	}

	/*
	 *	Time-of-day and time-out list are updated only
	 *	on the master CPU.
	 */
	if (my_cpu == master_cpu) {

	    register int s;
	    register timer_elt_t	telt;
	    boolean_t	needsoft = FALSE;

#if	TS_FORMAT == 1
	    /*
	     *	Increment the tick count for the timestamping routine.
	     */
	    ts_tick_count++;
#endif	TS_FORMAT == 1

	    /*
	     *	Update the tick count since bootup, and handle
	     *	timeouts.
	     */

	    s = splsched();
	    simple_lock(&timer_lock);

	    elapsed_ticks++;

	    telt = (timer_elt_t)queue_first(&timer_head.chain);
	    if (telt->ticks <= elapsed_ticks)
		needsoft = TRUE;
	    simple_unlock(&timer_lock);
	    splx(s);

	    /*
	     *	Increment the time-of-day clock.
	     */
	    time_value_add_usec(&time, usec);
	    if (mtime)
		*mtime = time;

	    /*
	     *	Schedule soft-interupt for timeout if needed
	     */
	    if (needsoft) {
		if (basepri) {
		    (void) splsoftclock();
		    softclock();
		}
		else {
		    setsoftclock();
		}
	    }
	}
}


void softclock()
{
	/*
	 *	Handle timeouts.
	 */
	int	s;
	register timer_elt_t	telt;
	register int	(*fcn)();
	register char	*param;

	while (TRUE) {
	    s = splsched();
	    simple_lock(&timer_lock);
	    telt = (timer_elt_t) queue_first(&timer_head.chain);
	    if (telt->ticks > elapsed_ticks) {
		simple_unlock(&timer_lock);
		splx(s);
		break;
	    }
	    fcn = telt->fcn;
	    param = telt->param;

	    remqueue(&timer_head.chain, (queue_entry_t)telt);
	    queue_init((queue_t)telt);	/* XXX - need 'timer_set' */
	    simple_unlock(&timer_lock);

	    if (!telt->private)
		zfree(timer_zone, (vm_offset_t)telt);
	    splx(s);

	    assert(fcn != 0);
	    (*fcn)(param);
	}
}

/*
 *	Set timeout.
 *
 *	Parameters:
 *		telt	 timer element.  Function and param are already set.
 *		interval time-out interval, in hz.
 */
void set_timeout(telt, interval)
	register timer_elt_t	telt;	/* already loaded */
	register unsigned int	interval;
{
	int	s;
	register timer_elt_t	next;

	s = splsched();
	simple_lock(&timer_lock);

	interval += elapsed_ticks;

	for (next = (timer_elt_t)queue_first(&timer_head.chain);
	     ;
	     next = (timer_elt_t)queue_next((queue_entry_t)next)) {

	    if (next->ticks > interval)
		break;
	}
	telt->ticks = interval;
	/*
	 * Insert new timer element before 'next'
	 * (after 'next'->prev)
	 */
	insque((queue_entry_t) telt, ((queue_entry_t)next)->prev);
	simple_unlock(&timer_lock);
	splx(s);
}

/*
 *	Set timeout.
 *
 *	fcn:		function to call
 *	param:		parameter to pass to function
 *	interval:	timeout interval, in hz.
 */
void timeout(fcn, param, interval)
	int	(*fcn)(/* char * param */);
	char *	param;
	int	interval;
{
	int	s;
	register timer_elt_t	telt;

	s = splsched();
	telt = (timer_elt_t) zalloc(timer_zone);
	splx(s);
	if (telt == 0)
	    panic("timeout");

	telt->private = FALSE;
	telt->fcn = fcn;
	telt->param = param;

	set_timeout(telt, (unsigned int)interval);
}

void reset_timeout(telt)
	register timer_elt_t	telt;
{
	int	s;

	s = splsched();
	simple_lock(&timer_lock);
	remqueue(&timer_head.chain, (queue_entry_t)telt);
	queue_init((queue_t)telt);	/* XXX - need 'timer_set' */
	simple_unlock(&timer_lock);
	splx(s);
}

/*
 * Returns a boolean indicating whether the timeout element was found
 * and removed.
 */
boolean_t untimeout(fcn, param)
	register int	(*fcn)();
	register char *	param;
{
	int	s;
	register timer_elt_t	telt;

	s = splsched();
	simple_lock(&timer_lock);
	queue_iterate(&timer_head.chain, telt, timer_elt_t, chain) {

	    if (fcn == telt->fcn && param == telt->param) {
		/*
		 *	Found it.
		 */
		remqueue(&timer_head.chain, (queue_entry_t)telt);
		queue_init((queue_t)telt);	/* XXX - need 'timer_set' */

		simple_unlock(&timer_lock);
		splx(s);

		if (!telt->private) {
		    s = splsched();
		    zfree(timer_zone, (vm_offset_t) telt);
		    splx(s);
		}
		return (TRUE);
	    }
	}
	simple_unlock(&timer_lock);
	splx(s);
	return (FALSE);
}

void init_timeout()
{
	simple_lock_init(&timer_lock);
	queue_init(&timer_head.chain);
	timer_head.ticks = ~0;	/* MAXUINT - sentinel */

	elapsed_ticks = 0;

	timer_zone = zinit(
			(vm_size_t)sizeof(timer_elt_data_t),
			(vm_size_t)(sizeof(timer_elt_data_t) * n_timer_max),
			(vm_size_t)(sizeof(timer_elt_data_t) * n_timer_alloc),
			FALSE,
			"time-out");

}

/*
 * Read the time.
 */
kern_return_t
host_get_time(host, current_time)
	host_t		host;
	time_value_t	*current_time;	/* OUT */
{
	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

	/*
	 * we are NOT synchronized unless we are on the master cpu,
	 * but we do NOT want to spend the time switching back and
	 * forth.
	 */
	*current_time = time;

	return (KERN_SUCCESS);
}

/*
 * Set the time.  Only available to privileged users.
 */
kern_return_t
host_set_time(host, new_time)
	host_t		host;
	time_value_t	new_time;
{
	int	s;

	if (host == HOST_NULL)
		return(KERN_INVALID_HOST);

#if	NCPUS > 1
	/*
	 * Switch to the master CPU to synchronize correctly.
	 */
	thread_bind(current_thread(), master_processor);
	if (current_processor() != master_processor)
	    thread_block((void (*)) 0);
#endif	NCPUS > 1

	s = splhigh();
	time = new_time;
	resettodr();
	splx(s);

#if	NCPUS > 1
	/*
	 * Switch off the master CPU.
	 */
	thread_bind(current_thread(), PROCESSOR_NULL);
#endif	NCPUS > 1

	return (KERN_SUCCESS);
}

/*
 * Adjust the time gradually.
 */
kern_return_t
host_adjust_time(host, new_adjustment, old_adjustment)
	host_t		host;
	time_value_t	new_adjustment;
	time_value_t	*old_adjustment;	/* OUT */
{
	if (host == HOST_NULL)
		return (KERN_INVALID_HOST);

#ifdef lint
	new_adjustment = *old_adjustment;
#endif lint
	return (KERN_FAILURE);	/* NOT YET XXX */
}

mapable_time_init()
{
	mtime = (time_value_t *) kmem_alloc(kernel_map, PAGE_SIZE);
	*mtime = time;
}

timeopen()
{
	return(0);
}
timeclose()
{
	return(0);
}
