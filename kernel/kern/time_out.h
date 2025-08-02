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
 * $Log:	time_out.h,v $
 * Revision 2.3  91/02/05  17:30:51  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:20:31  mrt]
 * 
 * Revision 2.2  90/11/05  14:32:00  rpd
 * 	Changed untimeout to return boolean.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.1  89/08/03  15:57:24  rwd
 * Created.
 * 
 * 14-Jun-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */

#ifndef	_KERN_TIME_OUT_H_
#define	_KERN_TIME_OUT_H_

/*
 * Mach time-out and time-of-day facility.
 */

#include <mach/boolean.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/zalloc.h>

/*
 *	Timers in kernel:
 */
extern unsigned long elapsed_ticks; /* number of ticks elapsed since bootup */
extern int	hz;		/* number of ticks per second */
extern int	tick;		/* number of usec per tick */

/*
 *	Time-out element.
 */
struct timer_elt {
	queue_chain_t	chain;		/* chain in order of expiration */
	int		(*fcn)();	/* function to call */
	char *		param;		/* with this parameter */
	unsigned long	ticks;		/* expiration time, in ticks */
	boolean_t	private;	/* owned by another structure -
					   do not zfree on expiration */
};
typedef	struct timer_elt	timer_elt_data_t;
typedef	struct timer_elt	*timer_elt_t;

/* for 'private' timer elements */
extern void		set_timeout();
extern void		reset_timeout();

/* for public timer elements */
extern void		timeout();
extern boolean_t	untimeout();

#define	set_timeout_setup(telt,fcn,param,interval)	\
	((telt)->fcn = (fcn),				\
	 (telt)->param = (param),			\
	 (telt)->private = TRUE,			\
	 set_timeout((telt), (interval)))

#endif	_KERN_TIME_OUT_H_
