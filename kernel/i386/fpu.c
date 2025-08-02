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
 * $Log:	fpu.c,v $
 * Revision 2.4  91/02/05  17:11:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:33:57  mrt]
 * 
 * Revision 2.3  91/01/08  15:10:29  rpd
 * 	Split i386_machine_state off of i386_kernel_state.
 * 	[90/12/31            rpd]
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.2  90/05/03  15:25:18  dbg
 * 	Created.
 * 	[90/02/11            dbg]
 * 
 */

/*
 * Support for 80387 floating point or FP emulator.
 */
#include <cputypes.h>

#include <mach/exception.h>
#include <kern/thread.h>
#include <i386/thread.h>
#include <i386/fp_reg.h>

thread_t	fp_thread = THREAD_NULL;

/*
 * Initialize FPU.
 */
void fpinit()
{
	int	control;

	_fninit();
	control = _fstcw();
	control &= ~(FPC_IE|FPC_ZE|FPC_OE|FPC_PE);
	control |= (FPC_PC_53 | FPC_IC_AFF);
	_fldcw(control);
}

/*
 * Coprocessor not present.
 */
fpnoextflt(regs)
	struct i386_saved_state *regs;
{
	/*
	 * Give FPU to this thread.
	 */
	_clts();

	if (fp_thread != current_thread()) {
	    if (fp_thread != THREAD_NULL) {
		fp_save(fp_thread);
	    }
	    fp_thread = current_thread();
	    fp_load(fp_thread);
	}
}

/*
 * FPU overran end of segment.
 */
fpextovrflt(regs)
	struct i386_saved_state *regs;
{
	exception(EXC_BAD_ACCESS, VM_PROT_READ|VM_PROT_EXECUTE, 0);
	/*NOTREACHED*/
}

/*
 * FPU error.
 */
fpexterrflt()
{
	int	status;

	/*
	 * FPU error is lower priority than task-switched;
	 * we should have switched threads first.
	 */
	assert(fp_thread == current_thread());

	/*
	 * Get status word, then clear exceptions.
	 */
	status = _fnstsw();
	_fnclex();

	/*
	 * Save status word in PCB
	 */
	current_thread()->pcb->ims.fp_save_state.fp_status = status;

	/*
	 * Raise FPU exception.
	 */
	exception(EXC_ARITHMETIC, EXC_I386_EXTERR, status);
	/*NOTREACHED*/
}

/*
 * Save FPU state.
 */
fp_save(thread)
	register thread_t	thread;
{
	register struct i386_machine_state *ims = &thread->pcb->ims;

	ims->fp_valid = TRUE;
	_fpsave(&ims->fp_save_state);
}

/*
 * Restore FPU state from PCB.
 */
fp_load(thread)
	register thread_t	thread;
{
	register struct i386_machine_state *ims = &thread->pcb->ims;

	if (ims->fp_valid) {
	    _fprestore(&ims->fp_save_state);
	}
	else {
	    fpinit();
	}
}

#if	AT386
/*
 *	Handle a coprocessor error interrupt on the AT386.
 *	This comes in on line 5 of the slave PIC at SPL1
 */
fpintr()
{
	outb(0xf0, 0);		/* Turn off 'busy' to coprocessor */
	fpexterrflt();		/* handle the error */
}
#endif	AT386
