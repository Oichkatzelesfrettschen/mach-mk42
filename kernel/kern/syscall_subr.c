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
 * $Log:	syscall_subr.c,v $
 * Revision 2.5  91/02/05  17:29:34  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:18:14  mrt]
 * 
 * Revision 2.4  91/01/08  15:17:15  rpd
 * 	Added continuation argument to thread_run.
 * 	[90/12/11            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.3  90/11/05  14:31:36  rpd
 * 	Restored missing multiprocessor untimeout failure code.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.2  90/06/02  14:56:17  rpd
 * 	Updated to new scheduling technology.
 * 	[90/03/26  22:19:48  rpd]
 * 
 * Revision 2.1  89/08/03  15:52:39  rwd
 * Created.
 * 
 *  3-Aug-88  David Golub (dbg) at Carnegie-Mellon University
 *	Removed all non-MACH code.
 *
 * 01-Mar-88  Douglas Orr (dorr) at Carnegie-Mellon University
 *	Added htg_u??x_syscall
 *
 * 18-Dec-87  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Added an extra pair of parens to make map_fd work correctly.
 *
 *  6-Dec-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Changed calling sequence to inode_pager_setup().
 *	Removed old history.
 *
 * 19-Nov-87  Michael Jones (mbj) at Carnegie-Mellon University
 *	Make sure fd passed to map_fd references a local inode before
 *	calling getinode, since getinode will panic for map_fd of RFS files.
 *
 * 19-Jun-87  David Black (dlb) at Carnegie-Mellon University
 *	MACH_TT: boolean for swtch and swtch_pri is now whether there is
 *	other work that the kernel could run instead of this thread.
 *
 *  7-May-87  David Black (dlb) at Carnegie-Mellon University
 *	New versions of swtch and swtch_pri for MACH_TT.  Both return a
 *	boolean indicating whether a context switch was done.  Documented.
 *
 * 31-Jul-86  Rick Rashid (rfr) at Carnegie-Mellon University
 *	Changed TPswtch_pri to set p_pri to 127 to make sure looping
 *	processes which want to simply reschedule do not monopolize the
 *	cpu.
 *
 *  3-Jul-86  Fil Alleva (faa) at Carnegie-Mellon University
 *	Added TPswtch_pri().  [Added to Mach, 20-jul-86, mwyoung.]
 *
 */

#include <mach_fixpri.h>
#include <cpus.h>

#include <mach/boolean.h>
#include <mach/thread_switch.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_sched.h>
#include <kern/processor.h>
#include <kern/sched.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/time_out.h>
#include <machine/machparam.h>	/* for splsched */

#if	MACH_FIXPRI
#include <mach/policy.h>
#endif	MACH_FIXPRI

/*
 *	Note that we've usurped the name "swtch" from its
 *	previous use.
 */

/*
 *	swtch and swtch_pri both attempt to context switch (logic in
 *	thread_block no-ops the context switch if nothing would happen).
 *	A boolean is returned that indicates whether there is anything
 *	else runnable.
 *
 *	This boolean can be used by a thread waiting on a
 *	lock or condition:  If FALSE is returned, the thread is justified
 *	in becoming a resource hog by continuing to spin because there's
 *	nothing else useful that the processor could do.  If TRUE is
 *	returned, the thread should make one more check on the
 *	lock and then be a good citizen and really suspend.
 */

void thread_depress_priority();
kern_return_t thread_depress_abort();

boolean_t swtch()
{
	register processor_t	myprocessor;

#if	NCPUS > 1
	myprocessor = current_processor();
	if (myprocessor->runq.count == 0 &&
	    myprocessor->processor_set->runq.count == 0)
		return(FALSE);
#endif	NCPUS > 1

	thread_block((void (*)()) 0);

	myprocessor = current_processor();
	return(myprocessor->runq.count > 0 ||
	    myprocessor->processor_set->runq.count > 0);
}

boolean_t  swtch_pri(pri)
	int pri;
{
	register boolean_t	result;
	register thread_t	thread = current_thread();
#ifdef	lint
	pri++;
#endif	lint

	/*
	 *	XXX need to think about depression duration.
	 *	XXX currently using min quantum.
	 */
	thread_depress_priority(thread, min_quantum);
	result = swtch();
	if (thread->depress_priority >= 0)
		thread_depress_abort(thread);
	return(result);
}

extern int hz;

/*
 *	thread_switch:
 *
 *	Context switch.  User may supply thread hint.
 *
 *	Fixed priority threads that call this get what they asked for
 *	even if that violates priority order.
 */
kern_return_t thread_switch(thread_name, option, option_time)
int	thread_name, option, option_time;
{
    register thread_t		cur_thread = current_thread();
    register processor_t	myprocessor;
    ipc_port_t			port;

    /*
     *	Process option.
     */
    switch (option) {
	case SWITCH_OPTION_NONE:
	    /*
	     *	Nothing to do.
	     */
	    break;

	case SWITCH_OPTION_DEPRESS:
	    /*
	     *	Depress priority for given time.
	     */
	    thread_depress_priority(cur_thread, option_time);
	    break;

	case SWITCH_OPTION_WAIT:
	    ipc_thread_will_wait_with_timeout(cur_thread, option_time);
	    break;

	default:
	    return(KERN_INVALID_ARGUMENT);
    }
    
    /*
     *	Check and act on thread hint if appropriate.
     */
    if ((thread_name != 0) &&
	(ipc_port_translate_send(cur_thread->task->itk_space,
				 thread_name, &port) == KERN_SUCCESS)) {
	    /* port is locked, but it might not be active */

	    /*
	     *	Get corresponding thread.
	     */
	    if (ip_active(port) && (ip_kotype(port) == IKOT_THREAD)) {
		register thread_t thread;
		register int s;

		thread = (thread_t) port->ip_kobject;
		/*
		 *	Check if the thread is in the right pset. Then
		 *	pull it off its run queue.  If it
		 *	doesn't come, then it's not eligible.
		 */
		s = splsched();
		thread_lock(thread);
		if ((thread->processor_set == cur_thread->processor_set)
		    && (rem_runq(thread) != RUN_QUEUE_NULL)) {
			/*
			 *	Hah, got it!!
			 */
			thread_unlock(thread);
			(void) splx(s);
			ip_unlock(port);
			/* XXX thread might disappear on us now? */
#if	MACH_FIXPRI
			if (thread->policy == POLICY_FIXEDPRI) {
			    myprocessor = current_processor();
			    myprocessor->quantum = thread->sched_data;
			    myprocessor->first_quantum = TRUE;
			}
#endif	MACH_FIXPRI
			thread_run((void (*)()) 0, thread);
			/*
			 *  Restore depressed priority			 
			 */
			if (cur_thread->depress_priority >= 0)
				thread_depress_abort(cur_thread);

			return(KERN_SUCCESS);
		}
		thread_unlock(thread);
		(void) splx(s);
	    }
	    ip_unlock(port);
    }

    /*
     *	No handoff hint supplied, or hint was wrong.  Call thread_block() in
     *	hopes of running something else.  If nothing else is runnable,
     *	thread_block will detect this.  WARNING: thread_switch with no
     *	option will not do anything useful if the thread calling it is the
     *	highest priority thread (can easily happen with a collection
     *	of timesharing threads).
     */
#if	NCPUS > 1
    myprocessor = current_processor();
    if (myprocessor->processor_set->runq.count > 0 ||
	myprocessor->runq.count > 0)
#endif	NCPUS > 1
	    thread_block((void (*)()) 0);

    /*
     *  Restore depressed priority			 
     */
    if (cur_thread->depress_priority >= 0)
	thread_depress_abort(cur_thread);
    return(KERN_SUCCESS);
}

/*
 *	thread_depress_priority
 *
 *	Depress thread's priority to lowest possible for specified period.
 *	Intended for use when thread wants a lock but doesn't know which
 *	other thread is holding it.  As with thread_switch, fixed
 *	priority threads get exactly what they asked for.  Users access
 *	this by the SWITCH_OPTION_DEPRESS option to thread_switch.
 */
void
thread_depress_priority(thread, depress_time)
register thread_t thread;
int	depress_time;
{
    int		s;
    void	thread_depress_timeout();

    /* convert from milliseconds to ticks */
    depress_time = (depress_time * hz) / 1000;

    s = splsched();
    thread_lock(thread);

    /*
     *	If thread is already depressed, override previous depression.
     */
    if (thread->depress_priority >= 0) {
	if (!untimeout(thread_depress_timeout, thread)) {
	    /*
	     *	Handle multiprocessor race condition.  Some other processor
	     *	is trying to timeout the old depress.  This should be rare.
	     */
	    thread_unlock(thread);
	    (void) splx(s);

	    /*
	     *	Wait for the timeout to do its thing.
	     */
	    while (thread->depress_priority >= 0)
	       continue;

	    /*
	     * Relock the thread and depress its priority.
	     */
	    s = splsched();
	    thread_lock(thread);

	    thread->depress_priority = thread->priority;
	    thread->priority = 31;
	    thread->sched_pri = 31;
	}
    }
    else {
	/*
	 *	Save current priority, then set priority and
	 *	sched_pri to their lowest possible values.
	 */
	thread->depress_priority = thread->priority;
	thread->priority = 31;
        thread->sched_pri = 31;
    }
    timeout(thread_depress_timeout, (char *)thread, depress_time);

    thread_unlock(thread);
    (void) splx(s);
}	
    
/*
 *	thread_depress_timeout:
 *
 *	Timeout routine for priority depression.
 */
void
thread_depress_timeout(thread)
register thread_t thread;
{
    int s;

    s = splsched();
    thread_lock(thread);
    /*
     *	Make sure thread is depressed, then undepress it.
     */
    if (thread->depress_priority >= 0) {
	thread->priority = thread->depress_priority;
	thread->depress_priority = -1;
	compute_priority(thread);
    }
    else {
	panic("thread_depress_timeout: thread not depressed!");
    }
    thread_unlock(thread);
    (void) splx(s);
}

/*
 *	thread_depress_abort:
 *
 *	Prematurely abort priority depression if there is one.
 */
kern_return_t
thread_depress_abort(thread)
register thread_t	thread;
{
    kern_return_t	ret = KERN_SUCCESS;
    int	s;

    if (thread == THREAD_NULL) {
	return(KERN_INVALID_ARGUMENT);
    }

    s = splsched();
    thread_lock(thread);
    
    /*
     *	Only restore priority if thread is depressed and we can
     *	grab the depress timeout off of the callout queue.
     */
    if (thread->depress_priority >= 0) {
	if (untimeout(thread_depress_timeout, thread)) {
	    thread->priority = thread->depress_priority;
	    thread->depress_priority = -1;
	    compute_priority(thread);
	}
	else {
	    /*
	     *	Can only happen on a multiprocessor.
	     */
	    ret = KERN_FAILURE;
	}
    }

    thread_unlock(thread);
    (void) splx(s);
    return(ret);
}
