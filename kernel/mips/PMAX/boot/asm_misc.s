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
 * $Log:	asm_misc.s,v $
 * Revision 2.4  91/02/05  17:38:31  mrt
 * 	Added author notices
 * 	[91/02/04  11:09:18  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:04:02  mrt]
 * 
 * Revision 2.3  90/12/05  23:29:22  af
 * 	Created.
 * 	[90/12/02            af]
 */
/*
 *	File: asm_misc.s
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	12/90
 *
 *	Miscellaneous assembly code.
 */

#include <mach/mips/asm.h>
#include <mips/mips_cpu.h>
#include <mips/PMAX/boot/asm_misc.h>

	.text
	.set	noreorder

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
 *	but we do not use fpa.  Differs from kernel's cuz
 *	it does not preserve the status register.
 * 	Return 0
 */
LEAF(setjmp)
XLEAF(_setjmp)
	sw	ra,LJMP_PC(a0)
	sw	sp,LJMP_SP(a0)
	sw	fp,LJMP_FP(a0)
	sw	s0,LJMP_S0(a0)
	sw	s1,LJMP_S1(a0)
	sw	s2,LJMP_S2(a0)
	sw	s3,LJMP_S3(a0)
	sw	s4,LJMP_S4(a0)
	sw	s5,LJMP_S5(a0)
	sw	s6,LJMP_S6(a0)
	sw	s7,LJMP_S7(a0)
	j	ra
	move	v0,zero
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
 *		a1				unsigned (optional)
 *
 *	Restores all registers that are callee-saved from the
 *	given longjmp buffer.  Same as user level _longjmp
 * 	Return a non-zero value.
 */
LEAF(longjmp)
XLEAF(_longjmp)
	.set	noat
	move	AT,a1
	bne	AT,zero,1f
	li	AT,1
1:	lw	ra,LJMP_PC(a0)
	lw	sp,LJMP_SP(a0)
	lw	fp,LJMP_FP(a0)
	lw	s0,LJMP_S0(a0)
	lw	s1,LJMP_S1(a0)
	lw	s2,LJMP_S2(a0)
	lw	s3,LJMP_S3(a0)
	lw	s4,LJMP_S4(a0)
	lw	s5,LJMP_S5(a0)
	lw	s6,LJMP_S6(a0)
	lw	s7,LJMP_S7(a0)
	j	ra
	move	v0,AT		/* return non-zero */
	.set	at
	END(longjmp)


/*
 *	PROM interface jump table
 *
 *	Object:
 *		prom_restart			EXPORTED function
 */
	.text

EXPORT(prom_restart)
	li	v0,PROM_RESTART
	j	v0
	nop
EXPORT(prom_open)
	li	v0,PROM_OPEN
	j	v0
	nop
EXPORT(prom_lseek)
	li	v0,PROM_LSEEK
	j	v0
	nop
EXPORT(prom_read)
	li	v0,PROM_READ
	j	v0
	nop
EXPORT(prom_write)
	li	v0,PROM_WRITE
	j	v0
	nop
EXPORT(prom_close)
	li	v0,PROM_CLOSE
	j	v0
	nop
EXPORT(prom_gets)
	li	v0,PROM_GETS
	j	v0
	nop
EXPORT(prom_printf)
	li	v0,PROM_PRINTF
	j	v0
	nop
EXPORT(prom_getenv)
	li	v0,PROM_GETENV
	j	v0
	nop
