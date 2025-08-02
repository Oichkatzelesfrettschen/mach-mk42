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
 * $Log:	pcb.c,v $
 * Revision 2.7  91/02/05  17:50:17  mrt
 * 	Added author notices
 * 	[91/02/04  11:24:22  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:28:14  mrt]
 * 
 * Revision 2.6  91/01/08  15:51:19  rpd
 * 	Removed pcb_synch.  Added pcb_collect.
 * 	[91/01/03            rpd]
 * 
 * 	Added mips_stack_base.
 * 	[91/01/02            rpd]
 * 
 * 	Split mips_machine_state off of mips_kernel_state.
 * 	Moved stack_switch here from mips/context.s.
 * 	[90/12/30            rpd]
 * 	Eliminated thread_bootstrap_user, thread_bootstrap_kernel.
 * 	[90/12/19            rpd]
 * 	Added pcb_alloc/pcb_free.  Use vm_page_grab/vm_page_free
 * 	for kernel stacks.  Reorganized the pcb layout.
 * 	Removed thread_private.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.3  90/01/22  23:07:50  af
 * 	Added single-stepping interface, via setstatus of the exception
 * 	flavor.  Sounded like an "exception" state is the closest thing
 * 	to a vax with a T bit set.
 * 	[89/12/13            af]
 * 
 * Revision 2.2  89/11/29  14:15:00  af
 * 	Added exception flavor, read-only cuz setting it is useless.
 * 	Fixed get_status to return zeroes and not affect the thread
 * 	for floats flavor when the thread did not use the FPA.
 * 	[89/11/27            af]
 * 
 * 	User state is only identified by KUo=1.
 * 	[89/11/03  16:30:51  af]
 * 
 * 	Moved over to pure kernel, removed old history.
 * 	[89/10/29            af]
 */
/*
 *	File: pcb.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	Mips PCB management
 */

#include <mach_debug.h>

#include <mach/thread_status.h>
#include <mach/vm_param.h>
#include <kern/thread.h>
#include <kern/zalloc.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>
#include <mips/mips_cpu.h>
#include <mips/thread.h>

/*
 *	stack_attach:
 *
 *	Attach a kernel stack to a thread.
 */

void stack_attach(thread, stack)
	register thread_t thread;
	register vm_offset_t stack;
{
	thread->kernel_stack = stack;

	/*
	 *	Setup the saved kernel state to run thread->swap_func.
	 *	Point the exception link back at the exception frame.
	 */
	STACK_MKS(stack)->sr = INT_LEV0|SR_IEc;		/* spl0/kernel mode */
	STACK_MKS(stack)->pc = (int) thread->swap_func;
	STACK_MKS(stack)->sp = (int) STACK_MEL(stack);
	STACK_MEL(stack)->eframe = &thread->pcb->mss;
}

/*
 *	stack_switch:
 *
 *	Move the kernel stack from the old thread to the new thread.
 *	Installs the new thread in various globals.
 *	Assumes interrupts are disabled.
 */

void stack_switch(old, new)
	register thread_t old;
	register thread_t new;
{
	register pcb_t pcb;
	register vm_offset_t stack;

	/*
	 *	Move the stack.
	 */

	stack = old->kernel_stack;
	old->kernel_stack = 0;
	pcb = new->pcb;
	new->kernel_stack = stack;

	/*
	 *	Attach the exception link to the new pcb.
	 */

	STACK_MEL(stack)->eframe = &pcb->mss;

	/*
	 *	Update global variables.
	 */

    {
	extern pcb_t current_pcb;
	extern struct eml_dispatch *current_dispatch;

	register task_t task;
	register struct eml_dispatch *dispatch;

	task = new->task;
	active_threads[cpu_number()] = new;
	dispatch = task->eml_dispatch;
	current_pcb = pcb;
	current_dispatch = dispatch;
    }

	/*
	 *	Only enable FPA if nobody stole it.
	 */

    {
	extern thread_t fpa_owner;
	register unsigned sr = pcb->mss.sr;

	if (new == fpa_owner)
		sr |= SR_CU1;	/* enable */
	else
		sr &= ~SR_CU1;	/* disable */
	pcb->mss.sr = sr;
    }
}

/*
 *	The stack_free_list can only be accessed at splsched,
 *	because stack_alloc_try/thread_swapin operate at splsched.
 */
vm_offset_t stack_free_list;		/* splsched only */
unsigned int stack_free_count;		/* splsched only */
unsigned int stack_free_limit = 3;	/* patchable */
unsigned int stack_alloc_hits;		/* debugging */
unsigned int stack_alloc_misses;	/* debugging */

/*
 *	The next field is at the base of the stack,
 *	so the low end is left unsullied.  The page
 *	field must be preserved.
 */

#define stack_next(stack) STACK_MSB(stack)->next

/*
 *	stack_alloc_try:
 *
 *	Non-blocking attempt to allocate a kernel stack.
 *	Called at splsched, so we must be quick.
 */

boolean_t stack_alloc_try(thread)
	register thread_t thread;
{
	register vm_offset_t stack;

	if ((stack = stack_free_list) == 0) {
		stack_alloc_misses++;
		return FALSE;
	}

	stack_free_list = stack_next(stack);
	stack_free_count--;

	stack_attach(thread, stack);
	stack_alloc_hits++;
	return TRUE;
}

/*
 *	stack_alloc:
 *
 *	Allocate a kernel stack for a thread.
 *	May block.
 */

void stack_alloc(thread)
	register thread_t thread;
{
	register vm_offset_t stack;
	int s;

	/*
	 *	We first try the free list.  It is probably empty,
	 *	or stack_alloc_try would have succeeded, but possibly
	 *	a stack was freed before the swapin thread got to us.
	 */

	s = splsched();
	if ((stack = stack_free_list) != 0) {
		stack_free_list = stack_next(stack);
		stack_free_count--;
	}
	(void) splx(s);

	if (stack == 0) {
		register vm_page_t m;

		while ((m = vm_page_grab()) == VM_PAGE_NULL)
			VM_PAGE_WAIT();

		stack = PHYS_TO_K0SEG(m->phys_addr);
#if	MACH_DEBUG
		stack_init(stack);
#endif	MACH_DEBUG
		STACK_MSB(stack)->page = m;
	}

	stack_attach(thread, stack);
}

/*
 *	stack_free:
 *
 *	Free a thread's kernel stack.
 *	May *not* block.  For any reason.
 */

void stack_free(thread)
	register thread_t thread;
{
	register vm_offset_t stack;
	int s;

	stack = thread->kernel_stack;
	thread->kernel_stack = 0;

	s = splsched();
	stack_next(stack) = stack_free_list;
	stack_free_list = stack;
	stack_free_count++;
	(void) splx(s);
}

/*
 *	stack_collect:
 *
 *	Free excess kernel stacks.
 *	May block.
 */

void stack_collect()
{
	extern vm_page_t vm_page_array;
	extern int first_page;

	register vm_offset_t stack;
	int s;

	s = splsched();
	while (stack_free_count > stack_free_limit) {
		stack = stack_free_list;
		stack_free_list = stack_next(stack);
		stack_free_count--;
		(void) splx(s);

#if		MACH_DEBUG
		stack_finalize(stack);
#endif		MACH_DEBUG
		vm_page_release(STACK_MSB(stack)->page);

		s = splsched();
	}
	(void) splx(s);
}

#if	MACH_DEBUG
extern boolean_t stack_check_usage;

/*
 *	stack_statistics:
 *
 *	Return statistics on cached kernel stacks
 *	kept by this machine-dependent module.
 *	*maxusagep must be initialized by the caller.
 */

void stack_statistics(totalp, maxusagep)
	unsigned int *totalp;
	vm_size_t *maxusagep;
{
	int s;

	s = splsched();
	if (stack_check_usage) {
		register vm_offset_t stack;

		/*
		 *	This is pretty expensive to do at splsched,
		 *	but it only happens when someone makes
		 *	a debugging call, so it should be OK.
		 */

		for (stack = stack_free_list;
		     stack != 0;
		     stack = stack_next(stack)) {
			vm_size_t usage = stack_usage(stack);

			if (usage > *maxusagep)
				*maxusagep = usage;
		}
	}

	*totalp = stack_free_count;
	(void) splx(s);
}
#endif	MACH_DEBUG

pcb_t pcb_free_list;		/* list of unused pcb structures */
unsigned int pcb_free_count;	/* size of the list, for debugging */
unsigned int pcb_wasted_count;	/* number of unusable pcbs allocated */
zone_t pcb_zone;		/* used when free list is empty */

pcb_t pcb_alloc()
{
	pcb_t pcb;

	if (pcb = pcb_free_list) {
		pcb_free_list = * (pcb_t *) pcb;
		pcb_free_count--;
		return pcb;
	}

	for (;;) {
		vm_offset_t va, pa;

		/*
		 *	We use the zone package to get more pcbs.
		 *	This is tricky, because we need k0seg memory.
		 */

		va = zalloc(pcb_zone);
		if (ISA_K0SEG(va))
			return (pcb_t) va;

		/*
		 *	We can convert the virtual address to a
		 *	physical address, if the pcb lies entirely in
		 *	one physical page.
		 */

		if (mips_trunc_page(va) + MIPS_PGBYTES ==
		    mips_round_page((vm_offset_t) ((pcb_t) va + 1))) {
			pa = pmap_resident_extract(kernel_pmap, va);
			if (pa == 0)
				panic("pcb_alloc", va);

			return (pcb_t) PHYS_TO_K0SEG(pa);
		}

		/*
		 *	Discard this pcb and try again.
		 */

		pcb_wasted_count++;
	}
}

void pcb_free(pcb)
	pcb_t pcb;
{
	pcb_free_count++;
	* (pcb_t *) pcb = pcb_free_list;
	pcb_free_list = pcb;
}

/*
 *	pcb_init:
 *
 *	Initialize the pcb for a thread.  For Mips,
 *	also initializes the coprocessor(s).
 *
 */
void pcb_init(thread)
	register thread_t	thread;
{
	register pcb_t pcb;

	/*
	 *	Allocate a pcb.
	 */
	pcb = pcb_alloc();
	thread->pcb = pcb;

	/*
	 *	We can't let the user see random values
	 *	in his registers.  They might not be so random.
	 */
	bzero(pcb, sizeof *pcb);

	/*
	 *	Make the thread run in user mode,
	 *	if it ever comes back out of the kernel.
	 */
	pcb->mss.sr = INT_LEV0|SR_KUo|SR_IEo;	/* user mode */

	/*
	 * The floating point coprocessor is reloaded as
	 * needed from the pcb.  Zeroing the pcb is enough.
	 * Same applies to other coprocessors, if any.
	 */
}

/*
 *  	pcb_terminate:
 *
 *	Shutdown any state associated with a thread's pcb.
 *	Also, release any coprocessor(s) register state.
 */
void pcb_terminate(thread)
	register struct thread *thread;
{
	check_fpa(thread, 1);
	pcb_free(thread->pcb);
	thread->pcb = 0;
}

/*
 *	pcb_collect:
 *
 *	Attempt to free excess pcb memory.
 */

void pcb_collect(thread)
	thread_t thread;
{
}


/*
 *	thread_setstatus:
 *
 *	Set the status of the given thread.
 */

kern_return_t thread_setstatus(thread, flavor, tstate, count)
	thread_t		thread;
	int			flavor;
	thread_state_t		tstate;
	unsigned int		count;
{
	register struct mips_thread_state	*state;
	register struct mips_saved_state	*saved_state;


	switch (flavor) {

	case MIPS_THREAD_STATE:

		if (count < MIPS_THREAD_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		state = (struct mips_thread_state *) tstate;
		saved_state = USER_REGS(thread);

		saved_state->at = state->r1;
		saved_state->v0 = state->r2;
		saved_state->v1 = state->r3;
		saved_state->a0 = state->r4;
		saved_state->a1 = state->r5;
		saved_state->a2 = state->r6;
		saved_state->a3 = state->r7;
		saved_state->t0 = state->r8;
		saved_state->t1 = state->r9;
		saved_state->t2 = state->r10;
		saved_state->t3 = state->r11;
		saved_state->t4 = state->r12;
		saved_state->t5 = state->r13;
		saved_state->t6 = state->r14;
		saved_state->t7 = state->r15;
		saved_state->s0 = state->r16;
		saved_state->s1 = state->r17;
		saved_state->s2 = state->r18;
		saved_state->s3 = state->r19;
		saved_state->s4 = state->r20;
		saved_state->s5 = state->r21;
		saved_state->s6 = state->r22;
		saved_state->s7 = state->r23;
		saved_state->t8 = state->r24;
		saved_state->t9 = state->r25;
		saved_state->k0 = state->r26;
		saved_state->k1 = state->r27;
		saved_state->gp = state->r28;
		saved_state->sp = state->r29;
		saved_state->fp = state->r30;
		saved_state->ra = state->r31;
		saved_state->mdlo = state->mdlo;
		saved_state->mdhi = state->mdhi;
		saved_state->pc = state->pc;

		break;

	case MIPS_FLOAT_STATE:

		if (count < MIPS_FLOAT_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		thread->pcb->mms.fpa_used = 1;
		check_fpa(thread, 0);	/* make sure we'll reload fpu */

		/* NOTE: this stmt knows about the pcb reg layout */
		bcopy(tstate, thread->pcb->mms.fpa_registers,
		      sizeof(struct mips_float_state));

		break;
	case MIPS_EXC_STATE:

#define cast_state ((struct mips_exc_state*)tstate)
		if (count < MIPS_EXC_STATE_COUNT ||
		    cast_state->cause != EXC_SST)
			return(KERN_INVALID_ARGUMENT);
#undef	cast_state

		thread->pcb->mms.sstepping = 1;
		break;

	default:
		return(KERN_INVALID_ARGUMENT);
	}

	return(KERN_SUCCESS);

}

/*
 *	thread_getstatus:
 *
 *	Get the status of the specified thread.
 */

kern_return_t thread_getstatus(thread, flavor, tstate, count)
	register thread_t	thread;
	int			flavor;
	register thread_state_t	tstate;		/* pointer to OUT array */
	unsigned int		*count;		/* IN/OUT */
{
	register struct mips_saved_state	*saved_state;

	switch (flavor) {
	case THREAD_STATE_FLAVOR_LIST:
		if (*count < 3)
			return(KERN_INVALID_ARGUMENT);
		tstate[0] = MIPS_THREAD_STATE;
		tstate[1] = MIPS_FLOAT_STATE;
		tstate[2] = MIPS_EXC_STATE;
		*count = 3;

		break;
	
	case MIPS_THREAD_STATE:

		if (*count < MIPS_THREAD_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		saved_state = USER_REGS(thread);

#define cast_state ((struct mips_thread_state*)tstate)
		cast_state->r1 = saved_state->at;
		cast_state->r2 = saved_state->v0;
		cast_state->r3 = saved_state->v1;
		cast_state->r4 = saved_state->a0;
		cast_state->r5 = saved_state->a1;
		cast_state->r6 = saved_state->a2;
		cast_state->r7 = saved_state->a3;
		cast_state->r8 = saved_state->t0;
		cast_state->r9 = saved_state->t1;
		cast_state->r10 = saved_state->t2;
		cast_state->r11 = saved_state->t3;
		cast_state->r12 = saved_state->t4;
		cast_state->r13 = saved_state->t5;
		cast_state->r14 = saved_state->t6;
		cast_state->r15 = saved_state->t7;
		cast_state->r16 = saved_state->s0;
		cast_state->r17 = saved_state->s1;
		cast_state->r18 = saved_state->s2;
		cast_state->r19 = saved_state->s3;
		cast_state->r20 = saved_state->s4;
		cast_state->r21 = saved_state->s5;
		cast_state->r22 = saved_state->s6;
		cast_state->r23 = saved_state->s7;
		cast_state->r24 = saved_state->t8;
		cast_state->r25 = saved_state->t9;
		cast_state->r26 = saved_state->k0;
		cast_state->r27 = saved_state->k1;
		cast_state->r28 = saved_state->gp;
		cast_state->r29 = saved_state->sp;
		cast_state->r30 = saved_state->fp;
		cast_state->r31 = saved_state->ra;
		cast_state->mdlo  = saved_state->mdlo;
		cast_state->mdhi  = saved_state->mdhi;
		cast_state->pc  = saved_state->pc;
#undef	cast_state

		*count = MIPS_THREAD_STATE_COUNT;
		break;

	case MIPS_FLOAT_STATE:

		if (*count < MIPS_FLOAT_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		if (thread->pcb->mms.fpa_used) {
			check_fpa(thread,0);
			/* NOTE: this knows about the pcb reg layout */
			bcopy(thread->pcb->mms.fpa_registers, tstate,
			      sizeof(struct mips_float_state));
		} else
			bzero(tstate, sizeof(struct mips_float_state));

		*count = MIPS_FLOAT_STATE_COUNT;

		return(KERN_SUCCESS);

		break;

	case MIPS_EXC_STATE:

		if (*count < MIPS_EXC_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		saved_state = USER_REGS(thread);

#define cast_state ((struct mips_exc_state*)tstate)

		cast_state->cause   = saved_state->cause;
		cast_state->address = saved_state->bad_address;
		cast_state->coproc_state = MIPS_STATUS_USE_COP0;
		if (thread->pcb->mms.fpa_used)
			cast_state->coproc_state |= MIPS_STATUS_USE_COP1;
#undef	cast_state

		*count = MIPS_EXC_STATE_COUNT;

		return(KERN_SUCCESS);

		break;

	default:
		return(KERN_INVALID_ARGUMENT);
	}
	return(KERN_SUCCESS);
}
