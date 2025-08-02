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
 * $Log:	thread_swap.h,v $
 * Revision 2.3  91/02/05  17:30:46  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:20:24  mrt]
 * 
 * Revision 2.2  90/06/02  14:57:23  rpd
 * 	Updated for new swapping technology.
 * 	[90/03/26  22:27:27  rpd]
 * 
 * Revision 2.1  89/08/03  15:54:23  rwd
 * Created.
 * 
 * 21-Jan-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	File:	kern/thread_swap.h
 *
 *	Declarations of thread swap_states and swapping routines.
 */

/*
 *	Swap states for threads.
 */

#ifndef	_KERN_THREAD_SWAP_H_
#define _KERN_THREAD_SWAP_H_

#define TH_SW_UNSWAPPABLE	1	/* not swappable */
#define TH_SW_IN		2	/* swapped in */
#define TH_SW_GOING_OUT		3	/* being swapped out */
#define TH_SW_WANT_IN		4	/* being swapped out, but should
					   immediately be swapped in */
#define TH_SW_OUT		5	/* swapped out */
#define TH_SW_COMING_IN		6	/* queued for swapin, or being
					   swapped in */
#define TH_SW_MAKE_UNSWAPPABLE	0x10	/* flag for WANT_IN, COMING_IN
					   to make thread unswappable */

#define TH_SW_STATE		0xf	/* Mask for state */

/*
 *	exported routines
 */
extern void	swapper_init();
extern void	thread_swapin( /* thread_t thread */ );
extern void	swapin_thread();
extern void	swapout_threads();
extern void	swapout_thread();
extern void	thread_swappable( /* thread_t thread, boolean_t swappable */ );
extern void	thread_doswapin( /* thread_t thread */ );

#endif	_KERN_THREAD_SWAP_H_
