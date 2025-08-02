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
 * $Log:	mips_instruction.c,v $
 * Revision 2.8  91/02/14  14:35:58  mrt
 * 	Added branch_delay() for FPA emulation.
 * 	[91/02/12  12:23:19  af]
 * 
 * Revision 2.7  91/02/05  17:49:48  mrt
 * 	Added author notices
 * 	[91/02/04  11:23:50  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:27:37  mrt]
 * 
 * Revision 2.6  91/01/08  15:50:54  rpd
 * 	Added isa_load, isa_store.
 * 	[90/11/27            rpd]
 * 
 * Revision 2.5  90/06/02  15:02:45  rpd
 * 	Added missing include.
 * 	[90/03/26  22:50:53  rpd]
 * 
 * Revision 2.4  90/01/22  23:07:34  af
 * 	Added an argument to branch_taken, used when calling the getreg function.
 * 	Needed for single-step support.
 * 	Made half-harted float support replacable by the MIPS-copyrighted one.
 * 	[90/01/20  16:45:54  af]
 * 
 * Revision 2.3  89/12/08  19:48:12  rwd
 * 	Another hack for floats.
 * 	[89/12/06  10:23:14  af]
 * 
 * Revision 2.2  89/11/29  14:14:45  af
 * 	Added emulate_fpa(), which is now just enough to get up multiuser
 * 	the Unix server.
 * 	[89/11/26  10:39:36  af]
 * 
 * 	Added isa_load_store(), to disentangle DBE exceptions.
 * 	[89/11/03  16:39:46  af]
 * 
 * 	Created.
 * 	[89/10/10            af]
 */
/*
 *	File: mips_instruction.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	Functions that operate on MIPS instructions,
 *	such as branch prediction and opcode predicates.
 */

#include <mach_kdb.h>
#include <mips_code.h>

#include <mach/mips/mips_instruction.h>
#include <mach/exception.h>
#include <mips/mips_cpu.h>
#include <mach/mach_types.h>
#include <kern/thread.h>

/*
 *	Object:
 *		isa_call			EXPORTED function
 *
 *		Function call predicate
 *
 */
boolean_t
isa_call(ins)
	register mips_instruction ins;
{
	return ((ins.j_format.opcode == op_jal) ||
		((ins.j_format.opcode == op_special) &&
		 (ins.r_format.func == op_jalr)));
}


/*
 *	Object:
 *		isa_ret				EXPORTED function
 *
 *		Function return predicate
 *
 */
boolean_t
isa_ret(ins)
	register mips_instruction ins;
{
	return ((ins.j_format.opcode == op_special) &&
		(ins.r_format.func == op_jr) &&
		(ins.r_format.rs == 31));
}


/*
 *	Object:
 *		isa_rei				EXPORTED function
 *
 *		Return from interrupt predicate
 *
 */
boolean_t
isa_rei(ins)
	register mips_instruction ins;
{
	return ((ins.j_format.opcode == op_cop0) &&
		(ins.f_format.func == op_rfe));
}

/*
 *	Object:
 *		isa_branch			EXPORTED function
 *
 *		Branch predicate
 *
 *	Does NOT include function calls, use isa_call() for that.
 *	Includes all other jump and branch instructions.
 */
boolean_t
isa_branch(ins)
	register mips_instruction ins;
{
	switch (ins.j_format.opcode) {
	    case op_special:
	    	return (ins.r_format.func == op_jr);
	    case op_bcond:
	    case op_j:
	    case op_beq:
	    case op_bne:
	    case op_blez:
	    case op_bgtz:
		return TRUE;

	    case op_cop0:
	    case op_cop1:
	    case op_cop2:
	    case op_cop3:
		return (ins.r_format.rs == op_bc);
	    default:
		return FALSE;
	}
}

/*
 *	Object:
 *		isa_sstep			EXPORTED function
 *
 *		Single step predicate
 *
 */
isa_sstep(ins)
	register mips_instruction ins;
{
	return ((ins.r_format.opcode == op_special) &&
		(ins.r_format.func == op_break) &&
		(((*((unsigned*)&ins)) >> 16) == BREAK_SSTEP));
}

/*
 *	Object:
 *		isa_spill			EXPORTED function
 *
 *		Register save (spill) predicate
 *
 */
boolean_t
isa_spill(ins,regn)
	register mips_instruction ins;
	register unsigned regn;
{
	return ((ins.j_format.opcode == op_sw) &&
		(ins.i_format.rt == regn));
}

/*
 *	Object:
 *		isa_load			EXPORTED function
 *
 *		Memory load predicate.
 *
 */
boolean_t
isa_load(ins)
	register mips_instruction ins;
{
	return (((op_lb <= ins.i_format.opcode) &&
		 (ins.i_format.opcode <= op_lwr)) ||
		((op_lwc0 <= ins.i_format.opcode) &&
		 (ins.i_format.opcode <= op_lwc3)));
}

/*
 *	Object:
 *		isa_store			EXPORTED function
 *
 *		Memory store predicate.
 *
 */
boolean_t
isa_store(ins)
	register mips_instruction ins;
{
	return (((op_sb <= ins.i_format.opcode) &&
		 (ins.i_format.opcode <= op_swr)) ||
		((op_swc0 <= ins.i_format.opcode) &&
		 (ins.i_format.opcode <= op_swc3)));
}

/*
 *	Object:
 *		isa_load_store			EXPORTED function
 *
 *		Memory load/store predicate
 *
 *	If the instruction is a load or store instruction
 *	returns the destination address.
 */
boolean_t
isa_load_store(ins, dest_addr, getreg, arg)
	register mips_instruction   ins;
	unsigned		   *dest_addr;
	unsigned 		  (*getreg)();
	unsigned		    arg;
{
	if (ins.j_format.opcode == op_special &&
	    ins.j_format.target == op_tas) {
		/*
		 * Special software test-and-set.
		 * The only valid register is a0
		 */
		*dest_addr = (*getreg)(4, arg);
		return TRUE;
	}
	/*
	 *	Well now, there are a few reserved operands here and there,
	 *	but basically all opcodes greater than 'lb' are load/store
	 *	operations. So that's what we check.
	 */
	if ((ins.i_format.opcode < op_lb) ||
	    (ins.i_format.opcode > op_swc3))
		return FALSE;

	/*
	 *	The only address calculation is register+displacement
	 */
	*dest_addr = (unsigned)((int)ins.i_format.simmediate +
		(*getreg)(ins.i_format.rs, arg));

	return TRUE;
}

#if	!MIPS_CODE
/*
 *	Object:
 *		emulate_fpa			EXPORTED function
 *
 *		Floating point emulation
 *
 *	This function is called upon FP instructions that caused
 *	an Unimplemented Operation exception in the FPA chip.
 *	Assembly code already checked that this is indeed an FPA
 *	instruction, enabled access to the FPA for us, saved all
 *	GP registers on the stack.
 *	We just have to do the rest now.
 *	NOTE: we currently refuse interrupts in the BD slot of a
 *	instructions and just deliver an FPE exception.  It will
 *	be necessary to write some more code (which MIPS already
 *	wrote...) to change this.  Maybe over Christmas time...
 *
 *	NOTE: This is all a brick-a-brack, which should be turned
 *	into a full fledged simulator. [Addition traps in strange
 *	ways...]
 */
boolean_t
emulate_fpa(ss_ptr, inst, csr)
	struct mips_saved_state *ss_ptr;
	mips_instruction	 inst;
	unsigned		 csr;
{
	register unsigned	 ft,fs,fd;
	register unsigned	 ft1 = 0,fs1 = 0,fd1;
	register unsigned	 ret = 0;
	char			 fmt = inst.f_format.fmt;

	if (fmt == fmt_double) {
		ft1 = fpa_get_reg(inst.f_format.ft);
		fs1 = fpa_get_reg(inst.f_format.fs);
		ft  = fpa_get_reg(inst.f_format.ft+1);
		fs  = fpa_get_reg(inst.f_format.fs+1);
	} else {
		ft = fpa_get_reg(inst.f_format.ft);
		fs = fpa_get_reg(inst.f_format.fs);
	}

	switch (inst.f_format.func) {
	case	op_fsub:
			if (ft == fs && ft1 == fs1) {
				fd1 = 0;
				fd = 0;
			}
			break;
	case	op_fdiv:
			if (ft == 0 && ft1 == 0) {
				if (fs == 0) {
					/* NaN */
					if (fmt == fmt_double) {
						fd  = 0x7ff7ffff;
						fd1 = 0xffffffff;
					} else
						fd = 0x7f80ffff;
					csr |= FPA_CSR_SV;
				} else {
					/* Infinity */
					if (fmt == fmt_double)
						fd  = 0x7ff00000;
					else
						fd  = 0x7f800000;
					csr |= FPA_CSR_SZ;
				}
				fpa_set_csr(csr);
			}
			break;
	case	op_fadd:
			if (ft == 0x3fe00000 || fs == 0x3fe00000) {
				fd1 = 0;
				fd = 0x3fe00000;
			}
			break;
	case	op_fmul:
	case	op_fabs:
	case	op_fmov:
	case	op_fneg:
	case	op_cvts:
	case	op_cvtd:
	case	op_cvtw:
	case	op_fcomp_f:
	case	op_fcomp_un:
	case	op_fcomp_eq:
	case	op_fcomp_ueq:
	case	op_fcomp_olt:
	case	op_fcomp_ult:
	case	op_fcomp_ole:
	case	op_fcomp_ule:
	case	op_fcomp_sf:
	case	op_fcomp_ngle:
	case	op_fcomp_seq:
	case	op_fcomp_ngl:
	case	op_fcomp_lt:
	case	op_fcomp_nge:
	case	op_fcomp_le:
	case	op_fcomp_ngt:
		pcb_synch(current_thread());
		kdb_trap(ss_ptr, 1);
	default:
		return EXC_MIPS_FLT_UNIMP;
	}
	if (fmt == fmt_double) {
		fpa_put_reg(inst.f_format.fd+1, fd);
		fpa_put_reg(inst.f_format.fd,  fd1);
	} else
		fpa_put_reg(inst.f_format.fd, fd);
	return ret;
}
#endif	!MIPS_CODE

/*
 *	Object:
 *		branch_taken			EXPORTED function
 *
 *		Branch prediction
 *
 *	Returns the address where the instruction might branch,
 *	if the branch is taken.
 *	Needs the address where the instruction is located and
 *	a function returning the current value of some register.
 *	See [Kane], pag 3-9, 5-19.
 *
 *	The instruction must be a call or branch, or we panic.
 */
unsigned
branch_taken(ins, addr, getreg, arg)
	register mips_instruction ins;
	unsigned addr;
	unsigned (*getreg)();
	int	 arg;
{

	switch (ins.j_format.opcode) {
	    case op_special:
		if ((ins.r_format.func == op_jr) ||
		    (ins.r_format.func == op_jalr))
			return (*getreg) (ins.r_format.rs, arg);
		break;

	    case op_j:
	    case op_jal:
		return ((ins.j_format.target << 2) | ((addr+4) & 0xf0000000));

	    case op_bcond:
	    case op_beq:
	    case op_bne:
	    case op_blez:
	    case op_bgtz:
		return ((ins.i_format.simmediate << 2) + (addr+4));

	    case op_cop0:
	    case op_cop1:
	    case op_cop2:
	    case op_cop3:
		if (ins.r_format.rs == op_bc)
			return ((ins.i_format.simmediate << 2) + (addr+4));
		break;
	}
	panic("branch_taken");
}


/*
 *	Object:
 *		branch_delay			EXPORTED function
 *
 *		Branch emulation
 *
 *	Generally, if an exception or interrupt hits the
 *	branch delay slot we can just restart the instruction.
 *	But if the instruction is a floating point one that
 *	requires emulation we are in trouble, we have to
 *	emulate the instruction and therefore the branch
 *	that preceeded it as well. We are only called if the
 *	branch did take place, I suppose.
 */
unsigned
branch_delay(ss_ptr, ins)
	struct mips_saved_state *ss_ptr;
	register mips_instruction ins;
{
	/*
	 * The hardware updates the RA
	 */
	extern getreg_val();
	return branch_taken(ins, ss_ptr->pc, getreg_val, ss_ptr);
}


#if	MACH_KDB
/*
 *	Object:
 *		stack_modified			EXPORTED function
 *
 *		Does the instruction affect the stack pointer and how
 *
 *	Returns the amount by which the instruction changes the
 *	stack pointer, or 0.
 *	Needs a function returning the current value of some
 *	register, but this needs not be precise as the C compiler
 *	typically uses immediate values.
 */
int stack_modified(ins, getreg)
	register mips_instruction ins;
	unsigned (*getreg)();
{
#define SP 29
	/* frame is mods by add and sub instructions. else you loose */
	switch (ins.j_format.opcode) {
	    case op_special:
		switch (ins.r_format.func) {
		    case op_add:
		    case op_addu:
			if (ins.r_format.rd != SP)
				return 0;
			if (ins.r_format.rs == SP)
				return (*getreg)(ins.r_format.rt);
			if (ins.r_format.rt == SP)
				return (*getreg)(ins.r_format.rs);
			/* approximate */
			return (*getreg)(ins.r_format.rs) + (*getreg)(ins.r_format.rt) - (*getreg)(SP);
		    case op_sub:
		    case op_subu:
			if (ins.r_format.rd != SP)
				return 0;
			if (ins.r_format.rd != SP)
				return 0;
			if (ins.r_format.rs == SP)
				return (*getreg)(ins.r_format.rt);
			if (ins.r_format.rt == SP)
				return (*getreg)(ins.r_format.rs);
			/* approximate */
			return (*getreg)(ins.r_format.rs) - (*getreg)(ins.r_format.rt) - (*getreg)(SP);
			break;
		    default:
			return 0;
		}
		break;
	    case op_addi:
	    case op_addiu:
		if (ins.i_format.rt != SP)
			return 0;
		return ins.i_format.simmediate;
	    default:
		return 0;
	}
}
#endif	MACH_KDB
