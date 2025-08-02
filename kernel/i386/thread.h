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
 * $Log:	thread.h,v $
 * Revision 2.5  91/02/05  17:15:03  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:38:28  mrt]
 * 
 * Revision 2.4  91/01/09  22:41:49  rpd
 * 	Added dummy switch_thread_context macro.
 * 	Added ktss to i386_machine_state.
 * 	Removed user_regs, k_stack_top.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.3  91/01/08  15:11:12  rpd
 * 	Added i386_machine_state.
 * 	[91/01/03  22:05:01  rpd]
 * 
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.2  90/05/03  15:37:59  dbg
 * 	Created.
 * 	[90/02/08            dbg]
 * 
 */

/*
 *	File:	machine/thread.h
 *
 *	This file contains the structure definitions for the thread
 *	state as applied to I386 processors.
 */

#ifndef	_I386_THREAD_H_
#define _I386_THREAD_H_

#include <mach/boolean.h>
#include <mach/i386/vm_param.h>
#include <i386/fp_reg.h>
#include <i386/tss.h>

/*
 *	i386_saved_state:
 *
 *	This structure corresponds to the state of user registers
 *	as saved upon kernel entry.  It lives in the pcb.
 *	It is also pushed onto the stack for exceptions in the kernel.
 */

struct i386_saved_state {
	unsigned int	gs;
	unsigned int	fs;
	unsigned int	es;
	unsigned int	ds;
	unsigned int	edi;
	unsigned int	esi;
	unsigned int	ebp;
	unsigned int	unused;		/* kernel esp stored by pusha -
					   irrelevant */
	unsigned int	ebx;
	unsigned int	edx;
	unsigned int	ecx;
	unsigned int	eax;
	unsigned int	trapno;
	unsigned int	err;
	unsigned int	eip;
	unsigned int	cs;
	unsigned int	efl;
	unsigned int	uesp;
	unsigned int	ss;
	struct v86_segs {
	    unsigned int v86_es;	/* virtual 8086 segment registers */
	    unsigned int v86_ds;
	    unsigned int v86_fs;
	    unsigned int v86_gs;
	} v86_segs;
};

/*
 *	i386_kernel_state:
 *
 *	This structure corresponds to the state of kernel registers
 *	as saved in a context-switch.  It lives at the base of the stack.
 */

struct i386_kernel_state {
	int			k_ebx;	/* kernel context */
	int			k_esp;
	int			k_ebp;
	int			k_edi;
	int			k_esi;
	int			k_eip;
	int			k_ipl;
};

/*
 *	i386_machine_state:
 *
 *	This structure corresponds to special machine state.
 *	It lives in the pcb.  It is not saved by default.
 */

struct i386_machine_state {
	struct i386_tss		*ktss;
	boolean_t		fp_valid;
	struct i386_fp_save	fp_save_state;
	struct i386_fp_regs	fp_regs;
};

typedef struct pcb {
	struct i386_saved_state iss;
	struct i386_machine_state ims;
} *pcb_t;

#define STACK_IKS(stack)	\
	((struct i386_kernel_state *)((stack) + KERNEL_STACK_SIZE) - 1)
#define STACK_ISS(stack)	\
	((struct i386_saved_state *)STACK_IKS(stack) - 1)

#define USER_REGS(thread)			\
	(((thread)->kernel_stack == 0) ?	\
	 &(thread)->pcb->iss :			\
	 STACK_ISS((thread)->kernel_stack))

#define switch_thread_context(new, continuation, old) \
	switch_task_context((new), (continuation), (old))

#endif	_I386_THREAD_H_
