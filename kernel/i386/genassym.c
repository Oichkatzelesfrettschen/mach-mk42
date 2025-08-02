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
 * $Log:	genassym.c,v $
 * Revision 2.7  91/02/05  17:11:56  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:22  mrt]
 * 
 * Revision 2.6  91/01/09  22:41:19  rpd
 * 	Removed user_regs, k_stack_top.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.5  91/01/08  15:10:36  rpd
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.4  90/12/04  14:45:58  jsb
 * 	Changes for merged intel/pmap.{c,h}.
 * 	[90/12/04  11:15:00  jsb]
 * 
 * Revision 2.3  90/08/27  21:56:44  dbg
 * 	Use new names from new seg.h.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:27:32  dbg
 * 	Created.
 * 	[90/02/11            dbg]
 * 
 */

#include <mach_kdb.h>

/*
 * Pass field offsets to assembly code.
 */
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/syscall_emulation.h>
#include <i386/thread.h>
#include <mach/i386/vm_param.h>
#include <i386/seg.h>
#include <i386/pmap.h>
#include <i386/trap.h>
#include <i386/tss.h>
#if	MACH_KDB
#include <sys/reboot.h>
#endif	MACH_KDB

/*ARGSUSED*/
main(argc,argv)
	int	argc;
	char	**argv;
{
	struct i386_kernel_state	*ks = (struct i386_kernel_state *)0;
	struct i386_kernel_state	*kss = STACK_IKS(0);
	struct i386_saved_state		*us = (struct i386_saved_state *)0;
	thread_t			th = (thread_t)0;
	task_t				task = (task_t)0;
	eml_dispatch_t			eml = (eml_dispatch_t)0;
	struct i386_tss			*tss = (struct i386_tss *)0;

	printf("#define	TH_PCB\t%d\n", &th->pcb);
	printf("#define	TH_TASK\t%d\n", &th->task);
	printf("#define	TH_RECOVER\t%d\n", &th->recover);
	printf("#define TH_KERNEL_STACK\t%d\n", &th->kernel_stack);

	printf("#define	TASK_EMUL\t%d\n", &task->eml_dispatch);

	printf("#define	EML_MIN_SYSCALL\t%d\n", EML_MIN_SYSCALL);
	printf("#define	DISP_COUNT\t%d\n", &eml->disp_count);
	printf("#define	DISP_VECTOR\t%d\n", &eml->disp_vector[0]);

	printf("#define K_SIZE\t%d\n", sizeof *ks);
	printf("#define	K_EBX\t%d\n", &ks->k_ebx);
	printf("#define	K_ESP\t%d\n", &ks->k_esp);
	printf("#define	K_EBP\t%d\n", &ks->k_ebp);
	printf("#define	K_EDI\t%d\n", &ks->k_edi);
	printf("#define	K_ESI\t%d\n", &ks->k_esi);
	printf("#define	K_EIP\t%d\n", &ks->k_eip);
	printf("#define	K_IPL\t%d\n", &ks->k_ipl);

	printf("#define	KSS_EBX\t%d\n", &kss->k_ebx);
	printf("#define	KSS_ESP\t%d\n", &kss->k_esp);
	printf("#define	KSS_EBP\t%d\n", &kss->k_ebp);
	printf("#define	KSS_EDI\t%d\n", &kss->k_edi);
	printf("#define	KSS_ESI\t%d\n", &kss->k_esi);
	printf("#define	KSS_EIP\t%d\n", &kss->k_eip);
	printf("#define	KSS_IPL\t%d\n", &kss->k_ipl);

	printf("#define R_SIZE\t%d\n", sizeof *us);
	printf("#define	R_CS\t%d\n", &us->cs);
	printf("#define	R_UESP\t%d\n", &us->uesp);
	printf("#define	R_EAX\t%d\n", &us->eax);
	printf("#define	R_TRAPNO\t%d\n", &us->trapno);
	printf("#define	R_ERR\t%d\n", &us->err);
	printf("#define	R_EFLAGS\t%d\n", &us->efl);
	printf("#define	R_EIP\t%d\n", &us->eip);

	printf("#define	NBPG\t%d\n", I386_PGBYTES);
	printf("#define	VM_MIN_ADDRESS\t%d\n", VM_MIN_ADDRESS);
	printf("#define	VM_MAX_ADDRESS\t%d\n", VM_MAX_ADDRESS);
	printf("#define	KERNELBASE\t%d\n", VM_MIN_KERNEL_ADDRESS);
	printf("#define KERNEL_STACK_SIZE\t%d\n", KERNEL_STACK_SIZE);

	printf("#define	PDESHIFT\t%d\n", PDESHIFT);
	printf("#define	PTESHIFT\t%d\n", PTESHIFT);
	printf("#define	PTEMASK\t%d\n", PTEMASK);

	printf("#define	PTE_PFN\t%d\n", INTEL_PTE_PFN);
	printf("#define	PTE_V\t%d\n", INTEL_PTE_VALID);
	printf("#define	PTE_W\t%d\n", INTEL_PTE_WRITE);
	printf("#define	PTE_INVALID\t%d\n", ~INTEL_PTE_VALID);

	printf("#define	IDTSZ\t%d\n", IDTSZ);
	printf("#define	GDTSZ\t%d\n", GDTSZ);
	printf("#define	LDTSZ\t%d\n", LDTSZ);

	printf("#define	KERNEL_CS\t%d\n", KERNEL_CS);
	printf("#define	KERNEL_DS\t%d\n", KERNEL_DS);
	printf("#define	KERNEL_TSS\t%d\n", KERNEL_TSS);
	printf("#define	KERNEL_LDT\t%d\n", KERNEL_LDT);

	printf("#define	PTES_PER_PAGE\t%d\n", NPTES);
	printf("#define	INTEL_PTE_KERNEL\t%d\n",
		INTEL_PTE_VALID|INTEL_PTE_WRITE);

	printf("#define	KERNELBASEPDE\t%d\n",
		(VM_MIN_KERNEL_ADDRESS >> PDESHIFT)
		*sizeof(pt_entry_t));

	printf("#define	T_INVALID_OPCODE\t%d\n", T_INVALID_OPCODE);
	printf("#define	T_PAGE_FAULT\t%d\n", T_PAGE_FAULT);
	printf("#define	T_GP_FAULT\t%d\n", T_GENERAL_PROTECTION);

	printf("#define	T_PF_USER_READ\t%d\n",
			T_PF_PROT|T_PF_USER);

	printf("#define	TSS_ESP0\t%d\n", &tss->esp0);
	printf("#define	TSS_SS0\t%d\n", &tss->ss0);

#if	MACH_KDB
	printf("#define	RB_KDB\t%d\n", RB_KDB);
#endif	MACH_KDB

	printf("#define	K_TRAP_GATE\t%d\n",
		ACC_P|ACC_PL_K|ACC_TRAP_GATE);
	printf("#define	U_TRAP_GATE\t%d\n",
		ACC_P|ACC_PL_U|ACC_TRAP_GATE);
	printf("#define	K_INTR_GATE\t%d\n",
		ACC_P|ACC_PL_K|ACC_INTR_GATE);

	return (0);
}

