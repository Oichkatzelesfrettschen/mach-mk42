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
 * $Log:	hardclock.c,v $
 * Revision 2.4  91/02/05  17:12:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:29  mrt]
 * 
 * Revision 2.3  91/01/08  17:32:02  rpd
 * 	EFL_VM => user_mode
 * 	[90/12/21  10:50:54  rvb]
 * 
 * Revision 2.2  90/05/03  15:27:35  dbg
 * 	Created.
 * 	[90/03/06            dbg]
 * 
 */

/*
 * Clock interrupt.
 */
#include <kern/time_out.h>
#include <i386/thread.h>
#include <i386/ipl.h>
#include <i386/eflags.h>

extern void	clock_interrupt();

hardclock(ivect, old_ipl, regs)
	int	ivect;		/* interrupt number */
	int	old_ipl;	/* old interrupt level */
	struct i386_saved_state regs;
				/* saved registers */
{
	clock_interrupt(tick,				/* usec per tick */
			(regs.efl & EFL_VM) ||		/* user mode */
			((regs.cs & 0x03) != 0),	/* user mode */
			old_ipl == SPL0);		/* base priority */
}
