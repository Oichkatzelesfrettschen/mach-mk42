/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log:	context.s,v $
 * Revision 2.8  91/02/05  17:47:39  mrt
 * 	Added author notices
 * 	[91/02/04  11:21:38  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:25:11  mrt]
 * 
 * Revision 2.7  91/01/08  15:49:15  rpd
 * 	Added mips_stack_base.
 * 	[91/01/02            rpd]
 * 
 * 	Split mips_machine_state off of mips_kernel_state.
 * 	Moved stack_switch to mips/pcb.c.
 * 	[90/12/30            rpd]
 * 	Removed load_context_ipc, save_context.
 * 	Added switch_task_context, switch_thread_context.
 * 	[90/12/08            rpd]
 * 
 * 	Added stack_switch, load_context_ipc.
 * 	[90/11/29            rpd]
 * 
 * 	Changed save_context/load_context for pcb reorganization.
 * 	Removed stack wiring code from load_context.
 * 	Disabled pmap_pcache code.
 * 	[90/11/12            rpd]
 * 
 * Revision 2.4  90/08/07  22:29:08  rpd
 * 	Support for pmap_pcache.
 * 	[90/08/07  15:25:14  af]
 * 
 * Revision 2.3  90/01/22  23:06:19  af
 * 	Added optional return value to longjmp(), as needed by KDB.
 * 	Which is the only user, btw.
 * 	[90/01/20  17:04:35  af]
 * 
 * Revision 2.2  89/11/29  14:12:54  af
 * 	Forgot to preserve a1 across the pmap_set_modify() call.
 * 	How comes the im-pure kernel never stomped into this ?
 * 	[89/11/27            af]
 * 
 * 	Support emulated syscalls by caching the dispatch pointer.
 * 	Support scheduling of the FPA by exporting the pcb pointer.
 * 	[89/11/16  14:42:52  af]
 * 
 * 	Kernel stack grew to 2 pages. Sigh.
 * 	[89/11/03  16:36:51  af]
 * 
 * 	Pure kernel: kernel stack reduced to 1 page.
 * 	U_ADDRESS hack is gone.
 * 	[89/10/29            af]
 * 
 * Revision 2.7  89/09/22  13:56:30  af
 * 	Now that the EAGER_PG_M thing is gone from pmap.c, need to make
 * 	sure kernel stack pages are writable. This is peculiar to a soft
 * 	managed tlb like ours, as we need a writable stack to take tlbmod
 * 	exceptions in software.
 * 	[89/09/01  09:52:12  af]
 * 
 * Revision 2.6  89/09/09  16:23:02  rvb
 * 	Have load_context make kernel stack writable, or we are in deep
 * 	trouble [af].
 * 	[89/09/09            rvb]
 * 
 * Revision 2.5  89/08/28  22:38:42  af
 * 	Polished and optimized. Moved probe_and_wire in tlb module where
 * 	it belongs. Made thread's PCB pointer a global, so that we do not
 * 	need the third wired entry anymore. Documented functions
 * 	and made comments (possibly) more useful.
 * 	[89/08/06            af]
 * 
 * Revision 2.4  89/08/08  21:48:41  jsb
 * 	Unbelievable bug in load_context(), which always restored
 * 	a status register of 1. Hard to believe, but the kernel still
 * 	worked.	Cleanups.
 * 	[89/08/02            af]
 * 
 * Revision 2.3  89/07/14  15:27:43  rvb
 * 	Call load_context_data for U_ADDRESS hack.
 * 	[89/07/05            rvb]
 * 
 * Revision 2.2  89/05/31  12:29:03  rvb
 * 	Changes. [af]
 * 
 * Revision 2.1  89/05/30  12:55:30  rvb
 * Under source control.
 * 
 * 13-Jan-89  Alessandro Forin (af) at Carnegie-Mellon University
 *	Created.
 *
 */
/* 
 *	File: context.s
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	1/89
 *
 *	Context switching and context save/restore primitives
 *
 */


#include <mach/mips/asm.h>

#include <mips/mips_cpu.h>
#include <mach/mips/vm_param.h>
#include <mips/pmap.h>
#include <mips/thread.h>
#include <mips/context.h>

#include <assym.s>

	.set	noreorder		# unless overridden

/*
 *	Object:
 *		active_threads			EXPORTED variable
 *		current_kstack			EXPORTED variable
 *		current_dispatch		EXPORTED variable
 *		current_pcb			EXPORTED variable
 *
 *	- Pointer to current thread
 *	- Pointer to top of kernel stack for current thread
 *	- Pointer to emulation dispatch structure for same
 *	- Pointer to PCB for current thread
 */
BSS(active_threads,4)
BSS(current_kstack,4)
BSS(current_dispatch,4)
BSS(current_pcb,4)

/*
 *	Object:
 *		load_context			EXPORTED function
 *
 *	 	Reload state of the target thread
 *
 *	Arguments:
 *		a0				thread_t
 *
 *	Installs this as the current thread on all the various
 *	globals: active_threads, current_kstack, current_pcb.
 *	Enables use of the FPA (when back to user) if nobody
 *	has changed its content, but only if we ever used it.
 *	Restores the state that save_context() has saved in the PCB
 *	and returns control to the thread [which will then return
 *	from save_context()]
 */
LEAF(load_context)
	/*
	 * There are various possible accidents we must prevent here.
	 * Basically we might screw up by taking a tlb miss when we
	 * are not ready, or by not setting things up right so that
	 * going into user mode and back fails.
	 *
	 * The first is only really a problem when inside this function.
	 * It is avoided by collectiong all the informations we
	 * need in registers when still in a state that allows
	 * tlb misses, and then entering a small critical section where
	 * misses are NOT allowed. CAVEAT MAINTAINER!
	 *
	 * The second requires that the thread's kernel stack and pcb
	 * be mapped at all times, as we need to save the user's
	 * registers before we clobber them.
	 * As an optimization, the current pcb pointer is held in a
	 * local variable to reduce tlb misses.
	 * A further optimization is to keep the kernel stack pointer
	 * of the current thread in a global variable so that we can
	 * get it quickly when we switch back to kernel mode.
	 */

	mtc0	zero,c0_status		# disable interrupts

	/*
	 *	Part 1: Load registers
	 */
	lw	s0,THREAD_KERNEL_STACK(a0)	# get kernel stack, might miss
	lw	s1,THREAD_PCB(a0)		# PCB pointer, might miss

	/*
	 *	Part 2: Critical section
	 *
	 *	Setup important global variables.
	 *	Relevant globals are bracketed in comments
	 *
	 *	>>> TLB MISSES NOT ALLOWED HERE <<<
	 */

	sw	a0,the_current_thread	# >>active_threads<<

	addu	s0,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE-MEL_SIZE
	sw	s0,current_kstack	# >>current_kstack<<

	lw	sp,MEL_SIZE+MKS_SP(s0)	# get new sp value
	sw	s1,current_pcb		# cache >>pcb pointer<<

	/*
	 *	>> END OF CRITICAL SECTION <<
	 *
	 *	We can take misses again now, but we're not done yet.
	 */
					# cache emulation table
	lw	a2,THREAD_TASK(a0)	# thread->task->dispatch
	nop
	lw	a2,EML_DISPATCH(a2)	# up to 2 misses here. Sigh.
	nop
	sw	a2,current_dispatch	# >>current_dispatch<<

	/*
	 *	Part 3: Coprocessor(s) usability.
	 *
	 *	Only enable if nobody stole it in the meantime.
	 */

	.set	reorder
	lw	a2,fpa_owner		# fpowner == me ?
	lw	t0,MSS_SR(s1)		# from user's exception frame
	bne	a0,a2,1f
	or	t0,SR_CU1		# yep, enable
	b	2f
1:	and	t0,~SR_CU1
2:	sw	t0,MSS_SR(s1)
	.set	noreorder

	/*
	 *	Part 4: Reload thread's registers and return
	 */
	lw	ra,MEL_SIZE+MKS_PC(s0)
	lw	fp,MEL_SIZE+MKS_FP(s0)
	lw	s1,MEL_SIZE+MKS_S1(s0)
	lw	s2,MEL_SIZE+MKS_S2(s0)
	lw	s3,MEL_SIZE+MKS_S3(s0)
	lw	s4,MEL_SIZE+MKS_S4(s0)
	lw	s5,MEL_SIZE+MKS_S5(s0)
	lw	s6,MEL_SIZE+MKS_S6(s0)
	lw	v0,MEL_SIZE+MKS_SR(s0)
	lw	s7,MEL_SIZE+MKS_S7(s0)
	lw	s0,MEL_SIZE+MKS_S0(s0)
	j	ra
	mtc0	v0,c0_status
	END(load_context)

/*
 *	Object:
 *		switch_thread_context		EXPORTED function
 *		switch_task_context		EXPORTED function
 *
 *	 	Save state of the current thread,
 *		call thread_continue, and resume new thread.
 *
 *	Arguments:
 *		a0				old thread_t
 *		a1				function
 *		a2				new thread_t
 *
 *	Installs this as the current thread on all the various
 *	globals: active_threads, current_kstack, current_pcb.
 *	Enables use of the FPA (when back to user) if nobody
 *	has changed its content, but only if we ever used it.
 *	Restores the state that save_context() has saved in the PCB
 *	and returns control to the thread [which will then return
 *	from save_context()]
 *
 *	switch_thread_context could be a separate function,
 *	optimized for the case of switching between threads in a task.
 */
LEAF(switch_task_context)
XLEAF(switch_thread_context)
	/*
	 * There are various possible accidents we must prevent here.
	 * Basically we might screw up by taking a tlb miss when we
	 * are not ready, or by not setting things up right so that
	 * going into user mode and back fails.
	 *
	 * The first is only really a problem when inside this function.
	 * It is avoided by collectiong all the informations we
	 * need in registers when still in a state that allows
	 * tlb misses, and then entering a small critical section where
	 * misses are NOT allowed. CAVEAT MAINTAINER!
	 *
	 * The second requires that the thread's kernel stack and pcb
	 * be mapped at all times, as we need to save the user's
	 * registers before we clobber them.
	 * As an optimization, the current pcb pointer is held in a
	 * local variable to reduce tlb misses.
	 * A further optimization is to keep the kernel stack pointer
	 * of the current thread in a global variable so that we can
	 * get it quickly when we switch back to kernel mode.
	 */

	mfc0	t1,c0_status		# save SR and disable interrupts
	lw	t2,THREAD_KERNEL_STACK(a0)	# get old kernel stack
	mtc0	zero,c0_status

	/*
	 *	Part 1: Save context
	 *
	 * 	We only need to save those registers that are callee-saved
	 *	in C, everything else is already on the stack.
	 */
	sw	sp,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_SP(t2)
	sw	ra,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_PC(t2)
	sw	t1,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_SR(t2)

	sw	s0,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S0(t2)
	sw	s1,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S1(t2)
	sw	s2,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S2(t2)
	sw	s3,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S3(t2)
	sw	s4,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S4(t2)
	sw	s5,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S5(t2)
	sw	s6,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S6(t2)
	sw	s7,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_S7(t2)
	sw	fp,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE+MKS_FP(t2)

	/* we want this value to be preserved across thread_continue */
	lw	s0,THREAD_KERNEL_STACK(a2)	# get new kernel stack
	lw	t2,THREAD_PCB(a2)		# get new thread's PCB

	/*
	 *	Part 2: Critical section
	 *
	 *	Setup important global variables.
	 *	Relevant globals are bracketed in comments
	 *
	 *	>>> TLB MISSES NOT ALLOWED HERE <<<
	 */

	sw	a2,the_current_thread	# >>active_threads<<

	addu	s0,KERNEL_STACK_SIZE-MSB_SIZE-MKS_SIZE-MEL_SIZE
	sw	s0,current_kstack	# >>current_kstack<<

	lw	sp,MEL_SIZE+MKS_SP(s0)	# get new sp value
	sw	t2,current_pcb		# cache >>pcb pointer<<

	/*
	 *	>> END OF CRITICAL SECTION <<
	 *
	 *	We can take misses again now, but we're not done yet.
	 */
					# cache emulation table
	lw	t0,THREAD_TASK(a2)	# thread->task->dispatch
	nop
	lw	t0,EML_DISPATCH(t0)	# up to 2 misses here. Sigh.
	nop
	sw	t0,current_dispatch	# >>current_dispatch<<

	/*
	 *	Part 3: Coprocessor(s) usability.
	 *
	 *	Only enable if nobody stole it in the meantime.
	 */

	.set	reorder
	lw	t0,fpa_owner		# fpowner == me ?
	lw	t1,MSS_SR(t2)		# from user's exception frame
	bne	a2,t0,1f
	or	t1,SR_CU1		# yep, enable
	b	2f
1:	and	t1,~SR_CU1
2:	sw	t1,MSS_SR(t2)

	/*
	 *	Part 4: Call thread_continue
	 *
	 *	a0 and a1 are already set up.
	 *	We don't need to reserve stack space for them,
	 *	because the stack pointer we restored must
	 *	already allow for at least two arguments.
	 */

	jal	thread_continue
	.set	noreorder

	/*
	 *	Part 5: Reload thread's registers and return
	 */
	lw	ra,MEL_SIZE+MKS_PC(s0)
	lw	fp,MEL_SIZE+MKS_FP(s0)
	lw	s1,MEL_SIZE+MKS_S1(s0)
	lw	s2,MEL_SIZE+MKS_S2(s0)
	lw	s3,MEL_SIZE+MKS_S3(s0)
	lw	s4,MEL_SIZE+MKS_S4(s0)
	lw	s5,MEL_SIZE+MKS_S5(s0)
	lw	s6,MEL_SIZE+MKS_S6(s0)
	lw	v0,MEL_SIZE+MKS_SR(s0)
	lw	s7,MEL_SIZE+MKS_S7(s0)
	lw	s0,MEL_SIZE+MKS_S0(s0)
	j	ra
	mtc0	v0,c0_status
	END(switch_task_context)

/*
 *	Object:
 *		setjmp				EXPORTED function
 *		_setjmp				EXPORTED function alias
 *
 *		Save current context for non-local goto's
 *
 *	Arguments:
 *		a0				jmp_buf *
 *
 *	Saves all registers that are callee-saved in the
 *	given longjmp buffer.  Same as user level _setjmp,
 *	but kernel does not use fpa.
 * 	Return 0
 */
LEAF(setjmp)
XLEAF(_setjmp)
	sw	ra,JB_PC(a0)
	sw	sp,JB_SP(a0)
	sw	fp,JB_FP(a0)
	sw	s0,JB_S0(a0)
	sw	s1,JB_S1(a0)
	sw	s2,JB_S2(a0)
	sw	s3,JB_S3(a0)
	sw	s4,JB_S4(a0)
	sw	s5,JB_S5(a0)
	sw	s6,JB_S6(a0)
	mfc0	v0,c0_status
	sw	s7,JB_S7(a0)
	sw	v0,JB_SR(a0)
	j	ra
	move	v0,zero			# return zero
	END(setjmp)


/*
 *	Object:
 *		longjmp				EXPORTED function
 *		_longjmp			EXPORTED function
 *
 *		Perform a non-local goto
 *
 *	Arguments:
 *		a0				jmp_buf *
 *		a1				unsigned
 *
 *	Restores all registers that are callee-saved from the
 *	given longjmp buffer.  Same as user level _longjmp
 * 	Return the second argument.
 */
LEAF(longjmp)
XLEAF(_longjmp)
	lw	ra,JB_PC(a0)
	lw	sp,JB_SP(a0)
	lw	fp,JB_FP(a0)
	lw	s0,JB_S0(a0)
	lw	s1,JB_S1(a0)
	lw	s2,JB_S2(a0)
	lw	s3,JB_S3(a0)
	lw	s4,JB_S4(a0)
	lw	s5,JB_S5(a0)
	lw	s6,JB_S6(a0)
	lw	v0,JB_SR(a0)
	lw	s7,JB_S7(a0)
	mtc0	v0,c0_status
	j	ra
	move	v0,a1			# return a1
	END(longjmp)
