/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log:	trap.c,v $
 * Revision 2.10  91/02/14  14:41:59  mrt
 * 	rfr's latest changes to v86 assist
 * 	[91/01/28  15:25:30  rvb]
 * 
 * Revision 2.9  91/02/05  17:15:21  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:38:41  mrt]
 * 
 * Revision 2.8  91/01/09  22:41:55  rpd
 * 	Fixed a merge bug.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.7  91/01/08  17:32:21  rpd
 * 	Add v86_hdw_assist().
 * 	[91/01/04  09:54:24  rvb]
 * 
 * 	Basically add trapv86()
 * 	[90/12/20  10:21:01  rvb]
 * 
 * Revision 2.6  91/01/08  15:11:18  rpd
 * 	Only need csw_needed in AST exit path.
 * 	[90/12/27            rpd]
 * 
 * 	Replaced thread_doexception with new exception interface.
 * 	[90/12/21            rpd]
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.5  90/10/25  14:44:56  rwd
 * 	Added watchpoint support.
 * 	[90/10/18            rpd]
 * 
 * Revision 2.4  90/06/02  14:48:58  rpd
 * 	Updated to new csw_needed macro.
 * 	[90/06/02            rpd]
 * 
 * Revision 2.3  90/05/21  13:26:49  dbg
 * 	Add hook for emulating IO instructions.
 * 	[90/05/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:38:07  dbg
 * 	V86 mode is also user mode.
 * 	[90/04/26            dbg]
 * 
 * 	Created (from VAX version).
 * 	[90/02/08            dbg]
 * 
 */
/*
 * Hardware trap/fault handler.
 */

#include <cpus.h>

#include <mach_kdb.h>

#include <sys/types.h>
#include <i386/eflags.h>
#include <i386/trap.h>

#include <mach/exception.h>
#include <mach/kern_return.h>
#include <mach/vm_param.h>

#include <vm/vm_kern.h>
#include <vm/vm_map.h>

#include <kern/thread.h>
#include <kern/task.h>
#include <kern/sched.h>
#include <kern/sched_prim.h>

extern void exception();
extern void thread_exception_return();

#if	MACH_KDB
boolean_t	debug_all_traps_with_kdb = FALSE;
extern struct db_watchpoint *db_watchpoint_list;
extern boolean_t db_watchpoints_inserted;

void
thread_kdb_return()
{
	register thread_t thread = current_thread();
	register struct i386_saved_state *regs = USER_REGS(thread);

	if (kdb_trap(regs->trapno, regs->err, regs)) {
		thread_exception_return();
		/*NOTREACHED*/
	}
}
#endif	MACH_KDB

/*
 * Fault recovery in copyin/copyout routines.
 */
struct recovery {
	int	fault_addr;
	int	recover_addr;
};

extern struct recovery	recover_table[];
extern struct recovery	recover_table_end[];

/*
 * Recovery from Successful fault in copyout does not
 * return directly - it retries the pte check, since
 * the 386 ignores write protection in kernel mode.
 */
extern char		copyout_copy[];		/* copy instruction */
extern char		copyout_retry[];	/* retry starting here */

char *	trap_type[] = {
	"Divide error",
	"Debug trap",
	"NMI",
	"Breakpoint",
	"Overflow",
	"Bounds check",
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack bounds",
	"General protection",
	"Page fault",
	"(reserved)",
	"Coprocessor error"
};
int	TRAP_TYPES = sizeof(trap_type)/sizeof(trap_type[0]);

/*
 * Trap handler.  Called from locore when a processor trap occurs.
 */
int brb = 1;
void trap(regs)
	register struct i386_saved_state *regs;
{
	int	exc;
	int	code;
	int	subcode;
	register int	type;
	vm_map_t	map;
	kern_return_t	result;
	register thread_t	thread;

	type = regs->trapno;
	code = regs->err;
	thread = current_thread();

	if ((regs->cs & 0x3) == 0 && (regs->efl & EFL_VM) == 0) {
	    /*
	     * Trap in system mode.  Only page-faults are valid,
	     * and then only in special circumstances.
	     */

	    switch (type) {
		case T_NO_FPU:
		    fpnoextflt(regs);
		    return;

		case T_FLOATING_POINT_ERROR:
		    fpextovrflt(regs);
		    return;

		case T_FPU_FAULT:
		    fpexterrflt();
		    return;

		case T_PAGE_FAULT:
		case T_SEGMENT_NOT_PRESENT:
		    /*
		     * If the current map is a submap of the kernel map,
		     * and the address is within that map, fault on that
		     * map.  If the same check is done in vm_fault
		     * (vm_map_lookup), we may deadlock on the kernel map
		     * lock.
		     */
		    subcode = get_cr2();	/* get faulting address */

		    if (thread == THREAD_NULL)
			map = kernel_map;
		    else {
			map = thread->task->map;
			if ((vm_offset_t)subcode < vm_map_min(map) ||
			    (vm_offset_t)subcode >= vm_map_max(map))
			    map = kernel_map;
		    }

		    /*
		     * Since the 386 ignores write protection in
		     * kernel mode, always try for write permission
		     * first.  If that fails and the fault was a
		     * read fault, retry with read permission.
		     */
		    result = vm_fault(map,
				      trunc_page((vm_offset_t)subcode),
				      VM_PROT_READ|VM_PROT_WRITE,
				      FALSE);
		    if (result == KERN_SUCCESS) {
#if	MACH_KDB
			if (db_watchpoint_list &&
			    db_watchpoints_inserted &&
			    (code & T_PF_WRITE) &&
			    db_find_watchpoint(
				(VM_MIN_ADDRESS <= (vm_offset_t)subcode) &&
				((vm_offset_t)subcode < VM_MAX_ADDRESS) &&
				(thread != THREAD_NULL) ?
				thread->task->map : kernel_map,
				(vm_offset_t)subcode, regs))
				kdb_trap(T_WATCHPOINT, 0, regs);
#endif	MACH_KDB
			if (regs->eip == (int)&copyout_copy)
			    regs->eip = (int)&copyout_retry;
			return;
		    }

		    if ((code & T_PF_WRITE) == 0 &&
			    result == KERN_PROTECTION_FAILURE) {
			/*
			 *	Must expand vm_fault by hand,
			 *	so that we can ask for read-only access
			 *	but enter a (kernel)writable mapping.
			 */
			result = i386_read_fault(map,
					  trunc_page((vm_offset_t)subcode));
			if (result == KERN_SUCCESS) {
			    if (regs->eip == (int)&copyout_copy)
				regs->eip = (int)&copyout_retry;
			    return;
			}
		    }

		    {
			register struct recovery *rp;

			for (rp = recover_table;
			     rp < recover_table_end;
			     rp++) {
			    if (regs->eip == rp->fault_addr) {
				regs->eip = rp->recover_addr;
				return;
			    }
			}
		    }
		    /* Unanticipated page-fault errors in kernel
		       should not happen. */
		    /* fall through */

		case T_NMI:
			if (brb) {
				printf("BRB\n");
				kdb_trap(type, code, regs);
				return;
			}
		default:
#if	MACH_KDB
		    if (kdb_trap(type, code, regs))
			return;
#endif	MACH_KDB
		    printf("trap type %d, code = %x, pc = %x\n",
			   type, code, regs->eip);
		    panic("trap");
		    return;
	    }
	}

	/*
	 *	Trap from user mode.
	 */

	switch (type) {

	    case T_DIVIDE_ERROR:
		exc = EXC_ARITHMETIC;
		code = EXC_I386_DIV;
		break;

	    case T_DEBUG:
		exc = EXC_BREAKPOINT;
		code = EXC_I386_SGL;
		break;

	    case T_INT3:
		exc = EXC_BREAKPOINT;
		code = EXC_I386_BPT;
		break;

	    case T_OVERFLOW:
		exc = EXC_ARITHMETIC;
		code = EXC_I386_INTO;
		break;

	    case T_OUT_OF_BOUNDS:
		exc = EXC_SOFTWARE;
		code = EXC_I386_BOUND;
		break;

	    case T_GENERAL_PROTECTION:
		if (emulate_io(regs))
		    return;
		/* fall through to ... */

	    case T_INVALID_OPCODE:
		exc = EXC_BAD_INSTRUCTION;
		code = EXC_I386_INVOP;
		break;

	    case T_NO_FPU:
		fpnoextflt(regs);
		return;

	    case T_FLOATING_POINT_ERROR:
		fpextovrflt(regs);
		return;

	    case T_PAGE_FAULT:
	    case T_SEGMENT_NOT_PRESENT:
		subcode = get_cr2();
		result = vm_fault(thread->task->map,
				trunc_page((vm_offset_t)subcode),
				(code & T_PF_WRITE)
				  ? VM_PROT_READ|VM_PROT_WRITE
				  : VM_PROT_READ,
				FALSE);
		if (result == KERN_SUCCESS) {
#if	MACH_KDB
		    if (db_watchpoint_list &&
			db_watchpoints_inserted &&
			(code & T_PF_WRITE) &&
			db_find_watchpoint(thread->task->map,
					   (vm_offset_t)subcode,
					   regs))
			    kdb_trap(T_WATCHPOINT, 0, regs);
#endif	MACH_KDB
		    return;
		}
		exc = EXC_BAD_ACCESS;
		code = result;
		break;

	    case T_FPU_FAULT:
		exc = EXC_ARITHMETIC;
		code = EXC_I386_EXTERR;
		break;

	    case T_NMI:
		if (brb) {
			printf("BRB\n");
			kdb_trap(type, 0, regs);
			return;
		}
#if	MACH_KDB
		if (kdb_trap(type, code, regs))
			return;
#endif	MACH_KDB
		printf("trap type %d, code = %x, pc = %x\n",
			type, code, regs->eip);
		panic("trap");
		return;
		break;
	}

#if	MACH_KDB
	if (debug_all_traps_with_kdb)
	    (void) kdb_trap(type, code, regs);
	else
#endif	MACH_KDB
	exception(exc, code, subcode);
	/*NOTREACHED*/
}

/*ARGSUSED*/
trapv86(regs)
	register struct i386_saved_state *regs;
{
	register thread_t	thread = current_thread();
	int			type = regs->trapno;
	int			code = regs->err;
	unsigned int		pc = regs->eip;
	int			exc_type = 0, exc_code = 0, exc_subcode = 0;
	kern_return_t		result;
			
	switch (type) {
	case T_DEBUG:
		exc_type = EXC_BREAKPOINT;
		exc_code = EXC_I386_SGL;
		break;

	case T_INT3:
		exc_type = EXC_BREAKPOINT;
		exc_code = EXC_I386_BPT;
		break;

	case T_GENERAL_PROTECTION:
	case T_INVALID_OPCODE:
		if (v86_hdw_assist(regs))
			return;
		exc_type = EXC_BAD_INSTRUCTION;
		exc_code = EXC_I386_INVOP;
		break;

	case T_STACK_FAULT:
		exc_type = EXC_BAD_INSTRUCTION;
		exc_code = EXC_I386_STKFLT;
		break;

	case T_OVERFLOW:
		exc_type = EXC_ARITHMETIC;
		exc_code = EXC_I386_INTO;
		break;
		
	case T_OUT_OF_BOUNDS:
		exc_type = EXC_ARITHMETIC;
		exc_code = EXC_I386_BOUND;
		break;
		
	case T_DIVIDE_ERROR:
		exc_type = EXC_ARITHMETIC;
		exc_code = EXC_I386_DIV;
		break;
		
	case T_FLOATING_POINT_ERROR:
		exc_type = EXC_ARITHMETIC;
		exc_code = EXC_I386_EXTERR;
		break;
		
	case 33:
		exc_type = EXC_ARITHMETIC;
		exc_code = EXC_I386_EMERR;
		break;
		
	case T_NO_FPU:
	case 32:			/* software floating trap */
		fpnoextflt(regs);
		return;

	case T_FPU_FAULT:
		fpextovrflt(regs);
		return;
		
	case T_PAGE_FAULT:
	case T_SEGMENT_NOT_PRESENT:
		exc_subcode = get_cr2();
		result = vm_fault(thread->task->map,
				trunc_page((vm_offset_t)exc_subcode),
				(code & T_PF_WRITE)
				  ? VM_PROT_READ|VM_PROT_WRITE
				  : VM_PROT_READ,
				FALSE);
		if (result == KERN_SUCCESS) {
#if	MACH_KDB
		    if (db_watchpoint_list &&
			db_watchpoints_inserted &&
			(code & T_PF_WRITE) &&
			db_find_watchpoint(thread->task->map,
					   (vm_offset_t)exc_subcode,
					   regs))
			    kdb_trap(T_WATCHPOINT, 0, regs);
#endif	MACH_KDB
		    return;
		}
		exc_type = EXC_BAD_ACCESS;
		exc_code = result;
		break;
		
	case T_NMI:
		if (brb) {
			printf("BRB\n");
			kdb_trap(type, 0, regs);
			return;
		}
	case T_DOUBLE_FAULT:
	case 10:
		printf("V86 trap type %d, code = %x, pc = %x\n",
		       type, code, pc);
		kdb_trap(type, regs);
		if ((unsigned) type < TRAP_TYPES)
			panic(trap_type[type]);
		else
			panic("trap");
		return;
		
	default:
		printf("trap(v86): unknown trap 0x%x\n", type);
#if	MACH_KDB
		kdb_trap(type, code, regs);
#endif	MACH_KDB
		return;
	}

#if	MACH_KDB
	if (debug_all_traps_with_kdb)
		(void) kdb_trap(type, code, regs);
	else
#endif	MACH_KDB
	exception(exc_type, exc_code, exc_subcode);
	/*NOTREACHED*/
}

#define EFL_ALLCC (EFL_CF|EFL_PF|EFL_AF|EFL_ZF|EFL_SF)

#define	EFL_TSAFE \
(EFL_TF|EFL_CF|EFL_PF|EFL_AF|EFL_ZF|EFL_SF|EFL_ALLCC|EFL_DF|EFL_OF)

#define V8(r,i,p) (((r->i&0xffff)<<4)+(0xffff&r->p))
#define ADDR8(i,p) (((i&0xffff)<<4)+(0xffff&p))

int do_iret = 1;
int do_ints = 0;
int do_pending_eip = 1;
int do_int_flag = 0;

int	ASSIST_ADDR  = 	0xff000;
#define	ASSIST_INTADDR	(ASSIST_ADDR+4)
#define	ASSIST_INTCNT	(ASSIST_ADDR+8)
#define	ASSIST_INTEIP	(ASSIST_ADDR+12)
#define	ASSIST_INTFLAG	(ASSIST_ADDR+16)

v86_hdw_assist(regs)
	register struct i386_saved_state *regs;
{
	int opcode = 0;
	struct assist_record {
		int		pending_eip;
		int		interrupt;
		int		count;
		int		int_eip;
		int		int_flag;
	} assist;

	if (copyin(V8(regs,cs,eip), &opcode, 1))
		return 0;			/* no opcode ?? */

	if (do_ints || do_pending_eip || do_int_flag) {
		if (copyin(ASSIST_ADDR, &assist, sizeof(assist))) return 0;
	}

	if (do_ints) {
		int i;
		struct iret_info_struct {
			u_short	ip;
			u_short cs;
			u_short efl;
		} iret_info;
		u_short * sp;
		u_short flags;

		if (assist.count <= 0) goto cont;
		if (assist.int_eip != 0) goto cont;

		iret_info.ip = regs->eip;
		iret_info.cs = regs->cs;
		iret_info.efl = regs->efl;

		assist.int_eip = V8(regs,cs,eip);

		regs->uesp -= 6;
		sp = (u_short *)V8(regs,ss,uesp);
		if (copyout(&iret_info, sp, 6)) {
			regs->uesp += 6;
			goto cont;
		}

	 	regs->cs  = (assist.interrupt>>16)&0xffff;
		regs->eip = assist.interrupt&0xffff;
		flags 	  = iret_info.efl;
		flags &=  (0xfff);
		flags |= EFL_IF;
		regs->efl = (regs->efl&0xfffff000)|flags;
		assist.count--;
		copyout(&assist,ASSIST_ADDR,sizeof(assist));
		return 1;
	}

cont:

	switch (opcode)
	{
	case 0xCF:				/* iret */
	    {
		if (do_iret) {
			u_short * sp;
			u_short flags;
			struct iret_info_struct {
				u_short	ip;
				u_short cs;
				u_short efl;
			} iret_info;
			sp = (u_short *)V8(regs,ss,uesp);
			if (copyin(sp, &iret_info, 6)) return 0;
			regs->eip = iret_info.ip;
			regs->cs  = iret_info.cs;
			if (do_pending_eip) {
				int return_eip;
				return_eip = V8(regs,cs,eip);
				if (return_eip == assist.pending_eip) {
					assist.pending_eip = 0;
					copyout(&assist.pending_eip,
						 ASSIST_ADDR, sizeof(int));
				}
				if (return_eip == assist.int_eip) {
					assist.int_eip = 0;
					copyout(&assist.int_eip,
						 ASSIST_INTEIP, sizeof(int));
				}
			}				
			flags 	  = iret_info.efl;
			if (do_int_flag) {
				assist.int_flag = flags & (EFL_IF);
				copyout(&assist.int_flag,
					 ASSIST_INTFLAG, sizeof(int));
			}
			flags &=  (0xfff);
			flags |= EFL_IF;
			regs->efl = (regs->efl&0xfffff000)|flags;
			regs->uesp += 6;
			goto handled;
		} else goto not_handled;
	    }
	case 0x9C:				/* pushf */
	    {
		int flags = 0;
		register u_short *uesp;
		regs->uesp -= 2;
		uesp  = (u_short *)V8(regs,ss,uesp);
		flags = regs->efl;
		if (do_int_flag) {
			flags &= ~EFL_IF;
			flags |= assist.int_flag;
		}
		if (copyout(&flags, uesp, 2)) {
			regs->uesp += 2;
			return 0;
		}
		regs->eip++;
		goto handled;
	    }
	case 0x9D:				/* popd */
	    {
		int flags = 0;
		register u_short *uesp = (u_short *)V8(regs,ss,uesp);

		if (copyin(uesp, &flags, 2)) return 0;

		regs->uesp += 2;
		if (do_int_flag) {
			assist.int_flag = flags & (EFL_IF);
			copyout(&assist.int_flag,
				 ASSIST_INTFLAG, sizeof(int));
		}
		flags &=  (0xfff);
		flags |= EFL_IF;
		regs->efl = (regs->efl&0xfffff000)|flags;
		regs->eip++;
		goto handled;
	    }
	case 0xfa:			/* CLI */
	    {
		if (do_int_flag) {
			assist.int_flag = 0;
			copyout(&assist.int_flag,
				 ASSIST_INTFLAG, sizeof(int));
			regs->eip++;
			goto handled;
		} else {
			goto not_handled;
		}
	    }
	case 0xfb:			/* STI */
	    {
		if (do_int_flag) {
			assist.int_flag = EFL_IF;
			copyout(&assist.int_flag,
				 ASSIST_INTFLAG, sizeof(int));
			regs->eip++;
			goto handled;
		} else {
			goto not_handled;
		}
	    }
	default:
		goto not_handled;
	}

handled:
	return 1;

not_handled:
	return 0;				/* give to mom */
}

/*ARGSUSED*/
void
astintr(regs)
	struct i386_saved_state *regs;
{
	register thread_t thread = current_thread();

	astoff();
	/*
	 * We only need to check for termination here.
	 */
	while (thread_should_halt(thread))
	    thread_halt_self();

	if (csw_needed(thread, current_processor()))
	    thread_block(thread_exception_return);
}
