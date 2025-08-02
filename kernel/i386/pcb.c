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
 * $Log:	pcb.c,v $
 * Revision 2.7  91/02/05  17:13:19  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:36:24  mrt]
 * 
 * Revision 2.6  91/01/09  22:41:41  rpd
 * 	Revised the pcb yet again.
 * 	Picked up i386_ISA_PORT_MAP_STATE flavors.
 * 	Added load_context, switch_task_context cover functions.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.5  91/01/08  15:10:58  rpd
 * 	Removed pcb_synch.  Added pcb_collect.
 * 	[91/01/03            rpd]
 * 
 * 	Split i386_machine_state off of i386_kernel_state.
 * 	Set k_stack_top correctly for V8086 threads.
 * 	[90/12/31            rpd]
 * 	Added stack_switch.  Moved stack_alloc_try, stack_alloc,
 * 	stack_free, stack_statistics to kern/thread.c.
 * 	[90/12/14            rpd]
 * 
 * 	Reorganized the pcb.
 * 	Added stack_attach, stack_alloc, stack_alloc_try,
 * 	stack_free, stack_statistics.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.4  90/08/27  21:57:34  dbg
 * 	Return correct count from thread_getstatus.
 * 	[90/08/22            dbg]
 * 
 * Revision 2.3  90/08/07  14:24:47  rpd
 * 	Include seg.h for segment names.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:35:51  dbg
 * 	Created.
 * 	[90/02/08            dbg]
 * 
 */

#include <mach_debug.h>

#include <mach/kern_return.h>
#include <mach/thread_status.h>
#include <mach/vm_param.h>

#include <kern/thread.h>
#include <vm/vm_kern.h>

#include <i386/thread.h>
#include <i386/eflags.h>
#include <i386/seg.h>
#include <i386/tss.h>

extern struct i386_tss		ktss;

zone_t pcb_zone;
zone_t ktss_zone;

/*
 *	stack_attach:
 *
 *	Attach a kernel stack to a thread.
 */

void stack_attach(thread, stack)
	register thread_t thread;
	register vm_offset_t stack;
{
	if (thread->kernel_stack != 0)
		panic("stack_attach");

	thread->kernel_stack = stack;

	/*
	 *	Make the thread run thread->swap_func.
	 *	This function will not return normally,
	 *	so we don't have to worry about a return address.
	 */
	STACK_IKS(stack)->k_eip = (int) thread->swap_func;
	STACK_IKS(stack)->k_esp = (int) STACK_ISS(stack);
	STACK_IKS(stack)->k_ipl = 0;	/* kernel mode, interrupts on */

	/*
	 *	XXX Copy user's registers from pcb to stack.
	 */
	bcopy((char *) &thread->pcb->iss, (char *) STACK_ISS(stack),
	      sizeof(struct i386_saved_state));
}

/*
 *	stack_detach:
 *
 *	Detaches a kernel stack from a thread, returning the old stack.
 */

vm_offset_t stack_detach(thread)
	register thread_t	thread;
{
	register vm_offset_t	stack;

	stack = thread->kernel_stack;
	thread->kernel_stack = 0;

	if (stack == 0)
		panic("stack_detach");

	/*
	 *	XXX Copy user's registers from stack to pcb.
	 */
	bcopy((char *) STACK_ISS(stack), (char *) &thread->pcb->iss,
	      sizeof(struct i386_saved_state));

	return stack;
}

void switch_ktss(stack, tss)
	register vm_offset_t stack;
	register struct i386_tss *tss;
{
	/*
	 *	Save a pointer to the top of the kernel stack.
	 *	The location depends on V8086 mode.  If we are
	 *	not in V8086 mode, then a trap into the kernel
	 *	won't save the v86 segments, so we leave room.
	 */

	ktss.esp0 = ((STACK_ISS(stack)->efl & EFL_VM) ?
		     (int) STACK_IKS(stack) :
		     (int) &STACK_ISS(stack)->v86_segs);

	if (tss == 0) {
		/*
		 *	No per-thread IO permissions.
		 */

		ktss.io_bit_map_offset = 0xffff;
	} else {
		/*
		 *	Set the IO permissions.
		 */

		bcopy((char *) tss->t_bitmap,
		      (char *) ktss.t_bitmap,
		      sizeof ktss.t_bitmap);
		ktss.io_bit_map_offset = tss->io_bit_map_offset;
	}
}

/*
 *	stack_switch:
 *
 *	Move the current thread's kernel stack to the new thread.
 */

void stack_switch(old, new)
	register thread_t	old;
	register thread_t	new;
{
	register vm_offset_t	stack;

	stack = old->kernel_stack;
	old->kernel_stack = 0;
	new->kernel_stack = stack;

	active_threads[cpu_number()] = new;

	/*
	 *	XXX Save old user registers back to pcb,
	 *	and replace with new user registers.
	 */

	bcopy((char *) STACK_ISS(stack), &old->pcb->iss,
	      sizeof(struct i386_saved_state));
	bcopy((char *) &new->pcb->iss, (char *) STACK_ISS(stack),
	      sizeof(struct i386_saved_state));

	switch_ktss(stack, new->pcb->ims.ktss);
}

void load_context(new)
	register thread_t	new;
{
	switch_ktss(new->kernel_stack, new->pcb->ims.ktss);
	Load_context(new);
}

void switch_task_context(old, continuation, new)
	register thread_t	old;
	void (*continuation)();
	register thread_t	new;
{
	switch_ktss(new->kernel_stack, new->pcb->ims.ktss);
	Switch_task_context(old, continuation, new);
}

void pcb_init(thread)
	register thread_t	thread;
{
	register pcb_t		pcb;

	pcb = (pcb_t) zalloc(pcb_zone);
	if (pcb == 0)
		panic("pcb_init");

	/*
	 *	We can't let random values leak out to the user.
	 */
	bzero((char *) pcb, sizeof *pcb);

	/*
	 *	Guarantee that the bootstrapped thread will be in user
	 *	mode (this psl assignment above executes the bootstrap
	 *	code in kernel mode.
	 */
	pcb->iss.cs = USER_CS;
	pcb->iss.ss = USER_DS;
	pcb->iss.ds = USER_DS;
	pcb->iss.es = USER_DS;
	pcb->iss.fs = USER_DS;
	pcb->iss.gs = USER_DS;
	pcb->iss.efl = EFL_USER_SET;

	thread->pcb = pcb;
}

void pcb_terminate(thread)
	register thread_t	thread;
{
	register pcb_t		pcb = thread->pcb;
	register struct i386_tss *tss = pcb->ims.ktss;

	if (tss != 0)
		zfree(ktss_zone, (vm_offset_t) tss);
	zfree(pcb_zone, (vm_offset_t) pcb);
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
 *	Set the status of the specified thread.
 */

kern_return_t thread_setstatus(thread, flavor, tstate, count)
	thread_t		thread;
	int			flavor;
	thread_state_t		tstate;
	unsigned int		count;
{
	switch (flavor) {
	    case i386_THREAD_STATE: {
		register struct i386_thread_state	*state;
		register struct i386_saved_state	*saved_state;

		if (count < i386_THREAD_STATE_COUNT) {
		    return(KERN_INVALID_ARGUMENT);
		}
		state = (struct i386_thread_state *) tstate;

		saved_state = USER_REGS(thread);

		/*
		 * General registers
		 */
		saved_state->edi = state->edi;
		saved_state->esi = state->esi;
		saved_state->ebp = state->ebp;
		saved_state->uesp = state->uesp;
		saved_state->ebx = state->ebx;
		saved_state->edx = state->edx;
		saved_state->ecx = state->ecx;
		saved_state->eax = state->eax;
		saved_state->eip = state->eip;
		saved_state->efl = (state->efl & ~EFL_USER_CLEAR)
				    | EFL_USER_SET;

		/*
		 * Segment registers.  Set differently in V8086 mode.
		 */
		if (saved_state->efl & EFL_VM) {
		    /*
		     * V8086 mode.
		     */
		    saved_state->cs = state->cs;
		    saved_state->ss = state->ss;
		    saved_state->v86_segs.v86_ds = state->ds;
		    saved_state->v86_segs.v86_es = state->es;
		    saved_state->v86_segs.v86_fs = state->fs;
		    saved_state->v86_segs.v86_gs = state->gs;

		    /*
		     * Zero segment registers
		     */
		    saved_state->ds = 0;
		    saved_state->es = 0;
		    saved_state->fs = 0;
		    saved_state->gs = 0;
		}
		else {
		    /*
		     * 386 mode.  Set segment registers for flat
		     * 32-bit address space.
		     */
		    saved_state->cs = USER_CS;
		    saved_state->ss = USER_DS;
		    saved_state->ds = USER_DS;
		    saved_state->es = USER_DS;
		    saved_state->fs = USER_DS;
		    saved_state->gs = USER_DS;
		}

		break;
	    }

#if	0
	    case i386_FLOAT_STATE: {
		if (count < i386_FLOAT_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		if (fp_kind == FP_NO)
			return(KERN_FAILURE);
		state = (struct i386_float_state *)tstate;
		/* 
		 * If the named thread has ownership of the fp unit, throw out
		 * the old fp state and give up ownership of the fp unit.
		 */
		if (fp_thread == thread)
			fp_unowned();

		pcbstate = (struct i387_state *)thread->pcb->pcb_fps.state;
		userstate = (struct i387_state *)state->hw_state;
		thread->pcb->pcb_fpvalid = state->initialized;

		bcopy(userstate->stack, pcbstate->stack, sizeof(userstate->stack));

		/* Ensure that reserved parts of the environment are 0. */
		bzero((char *)&pcbstate->env, sizeof(struct i387_env));
		pcbstate->env.control = userstate->env.control;
		pcbstate->env.status = userstate->env.status;
		pcbstate->env.tags = userstate->env.tags;
		pcbstate->env.ip = userstate->env.ip;
		pcbstate->env.cs_sel = userstate->env.cs_sel;
		pcbstate->env.opcode = userstate->env.opcode;
		pcbstate->env.data = userstate->env.data;
		pcbstate->env.data_sel = userstate->env.data_sel;

		thread->pcb->pcb_fps.status = 0;

		break;
	    }
#endif	0

	    case i386_ISA_PORT_MAP_STATE: {
		register struct i386_isa_port_map_state *state;
		register struct i386_tss *tss;

		if (count < i386_ISA_PORT_MAP_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		/*
		 *	If the thread has no ktss yet,
		 *	we must allocate one.
		 */

		state = (struct i386_isa_port_map_state *) tstate;
		tss = thread->pcb->ims.ktss;
		if (tss == 0) {
			tss = (struct i386_tss *) zalloc(ktss_zone);
			if (tss == 0)
				panic("thread_setstatus: ktss");
			thread->pcb->ims.ktss = tss;

			/*
			 *	Initialize from the global ktss.
			 */

			*tss = ktss;
			tss->io_bit_map_offset = (unsigned short)
				&((struct i386_tss *) 0)->t_bitmap;
		}

		bcopy((char *) state->pm,
		      (char *) tss->t_bitmap,
		      sizeof state->pm);
		break;
	    }

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
	thread_state_t		tstate;	/* pointer to OUT array */
	unsigned int		*count;		/* IN/OUT */
{
	switch (flavor)  {
	    case THREAD_STATE_FLAVOR_LIST:
		if (*count < 3)
		    return (KERN_INVALID_ARGUMENT);
		tstate[0] = i386_THREAD_STATE;
		tstate[1] = i386_FLOAT_STATE;
		tstate[2] = i386_ISA_PORT_MAP_STATE;
		*count = 3;
		break;

	    case i386_THREAD_STATE: {
		register struct i386_thread_state	*state;
		register struct i386_saved_state	*saved_state;

		if (*count < i386_THREAD_STATE_COUNT)
		    return(KERN_INVALID_ARGUMENT);

		state = (struct i386_thread_state *) tstate;
		saved_state = USER_REGS(thread);

		/*
		 * General registers.
		 */
		state->edi = saved_state->edi;
		state->esi = saved_state->esi;
		state->ebp = saved_state->ebp;
		state->ebx = saved_state->ebx;
		state->edx = saved_state->edx;
		state->ecx = saved_state->ecx;
		state->eax = saved_state->eax;
		state->eip = saved_state->eip;
		state->efl = saved_state->efl;
		state->uesp = saved_state->uesp;

		state->cs = saved_state->cs;
		state->ss = saved_state->ss;
		if (saved_state->efl & EFL_VM) {
		    /*
		     * V8086 mode.
		     */
		    state->ds = saved_state->v86_segs.v86_ds;
		    state->es = saved_state->v86_segs.v86_es;
		    state->fs = saved_state->v86_segs.v86_fs;
		    state->gs = saved_state->v86_segs.v86_gs;
		}
		else {
		    /*
		     * 386 mode.
		     */
		    state->ds = saved_state->ds;
		    state->es = saved_state->es;
		    state->fs = saved_state->fs;
		    state->gs = saved_state->gs;
		}
		*count = i386_THREAD_STATE_COUNT;
		break;
	    }

#if	0
	    case i386_FLOAT_STATE: {
		register struct i386_float_state	*state;
		struct i387_state	*pcbstate, *userstate;

		state = (struct i386_float_state *)tstate;

		if (*count < i386_FLOAT_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		/* Make sure we've got the latest fp state info */
		pcb_synch(thread);

		state->fpkind = fp_kind;
		state->initialized = thread->pcb->pcb_fpvalid;
		state->exc_status = thread->pcb->pcb_fps.status;
		pcbstate = (struct i387_state *)thread->pcb->pcb_fps.state;
		userstate = (struct i387_state *)state->hw_state;

		bcopy(pcbstate->stack, userstate->stack, sizeof(userstate->stack));

		/* Ensure that reserved parts of the environment are 0. */
		bzero((char *)&userstate->env, sizeof(struct i387_env));
		userstate->env.control = pcbstate->env.control;
		userstate->env.status = pcbstate->env.status;
		userstate->env.tags = pcbstate->env.tags;
		userstate->env.ip = pcbstate->env.ip;
		userstate->env.cs_sel = pcbstate->env.cs_sel;
		userstate->env.opcode = pcbstate->env.opcode;
		userstate->env.data = pcbstate->env.data;
		userstate->env.data_sel = pcbstate->env.data_sel;

		*count = i386_FLOAT_STATE_COUNT;
		break;
	    }
#endif	0

	    case i386_ISA_PORT_MAP_STATE: {
		register struct i386_isa_port_map_state *state;
		register struct i386_tss *tss;

		if (*count < i386_ISA_PORT_MAP_STATE_COUNT)
			return(KERN_INVALID_ARGUMENT);

		state = (struct i386_isa_port_map_state *) tstate;
		tss = thread->pcb->ims.ktss;

		if (tss == 0) {
		    int i;

		    /*
		     *	The thread has no ktss, so no IO permissions.
		     */

		    for (i = 0; i < sizeof state->pm; i++)
			state->pm[i] = 0xff;
		} else {
		    /*
		     *	The thread has its own ktss.
		     */

		    bcopy((char *) tss->t_bitmap,
			  (char *) state->pm,
			  sizeof state->pm);
		}

		*count = i386_ISA_PORT_MAP_STATE_COUNT;
		break;
	    }

	    default:
		return(KERN_INVALID_ARGUMENT);
	}

	return(KERN_SUCCESS);
}
