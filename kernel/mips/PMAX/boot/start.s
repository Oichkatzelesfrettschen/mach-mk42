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
 * $Log:	start.s,v $
 * Revision 2.4  91/02/05  17:39:16  mrt
 * 	Added author notices
 * 	[91/02/04  11:11:06  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:06:30  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:13  af
 * 	Created.
 * 	[90/12/02            af]
 * 
 */
/*
 *	File: start.s
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	12/90
 *
 *	Entry point for Bootstrap.
 */

#include <mach/mips/asm.h>
#include <mips/PMAX/boot/asm_misc.h>

/*
 *	Object:
 *		start				EXPORTED function
 *
 *		Entry point for standalone pgms (boots)
 *
 *	Arguments:
 *		a0				int
 *		a1				char **
 *		a2				char **
 *		a3				boolean_t
 *
 *	This is where the prom comes to.
 * 	Leaves all exception and interrupts to prom,
 * 	runs off prom's stack too.
 *
 * 	a3 says if we should NOT return to caller but
 * 	rather get out via prom_restart().
 * 	Other args as in U*x.
 *
 * 	Returns whatever main() returns, or the arg to exit()
 */
/*
 */
IMPORT(prom_restart,4)

	BSS(ret_buf, LJMP_SIZE)

	.text
	.set	noreorder

#define ENTRY_FRAME	1024		/* XXX does prom really use fault stack ? */

NESTED(start, ENTRY_FRAME, zero)

	la	gp,_gp			# load up GP register

					# setup return info
	la	t9,ret_buf
	sw	ra,LJMP_PC(t9)
	sw	sp,LJMP_SP(t9)
	sw	a3,LJMP_SR(t9)		# safe spot

					# adjust stack
	subu	sp,ENTRY_FRAME

					# put args in save area
	sw	a0,ENTRY_FRAME(sp)
	sw	a1,ENTRY_FRAME+4(sp)
	sw	a2,ENTRY_FRAME+8(sp)

					# xfer to C
	jal	main
	sw	zero,ENTRY_FRAME-4(sp)	# backtrace stop

	move	a0,v0			# ret code for exit()

	/* FALL THROUGH */

/*
 *	Object:
 *		exit				EXPORTED function
 *
 *		Exit point, back to prom
 *
 *	Arguments:
 *		a0				int
 */
XLEAF(exit)
					# recover initial state
	la	t9,ret_buf
	lw	a3,LJMP_SR(t9)
	lw	ra,LJMP_PC(t9)
	bne	a3,zero,restart		# should we just return
	move	v0,a0

	j	ra
	lw	sp,LJMP_SP(t9)

restart:
	j	prom_restart
	nop
	END(start)
