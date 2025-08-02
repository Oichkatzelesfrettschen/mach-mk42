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
 * Revision 2.6  91/02/05  17:51:38  mrt
 * 	Added author notices
 * 	[91/02/04  11:24:52  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:29:38  mrt]
 * 
 * Revision 2.5  90/12/05  23:38:49  af
 * 
 * 
 * Revision 2.4  90/12/05  20:50:13  af
 * 	Simplified a bit more.  I love killing code.
 * 	[90/12/03  23:05:12  af]
 * 
 * Revision 2.3  90/09/09  23:21:16  rpd
 * 	Removed environ thing.  It's not there and not worth the pain.
 * 	Use prom_getenv() if you need to query the prom.
 * 	[90/09/05            af]
 * 
 * Revision 2.2  89/11/29  14:15:20  af
 * 	Zero initial status, let mips_init go where it has to.
 * 	[89/11/03  16:28:19  af]
 * 
 * 	Rewritten for pure kernel.
 * 	[89/10/04            af]
 * 
 * Revision 2.3  89/07/14  15:27:50  rvb
 * 	Make _argv external for, check_debug.c
 * 	[89/07/14            rvb]
 * 
 * Revision 2.2  89/05/31  12:29:26  rvb
 * 	Flush unused "entry points". [af]
 * 
 * Revision 2.1  89/05/30  12:55:34  rvb
 * Rcs'ed
 * 
 * 13-Jan-89  Alessandro Forin (af) at Carnegie-Mellon University
 *	Created.
 */
/*
 *	File: start.s
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	1/89
 *
 *	Kernel startup code.
 *	This is where we receive control from the PROM.
 */

#include <mach_kdb.h>

#include <mach/mips/asm.h>
#include <mach/mips/vm_param.h>
#include <mips/mips_cpu.h>

#include <assym.s>


/*
 * We better get a stack for ourselves quickly.
 * Note: the "intstack" misnomer is because the machine-independent
 * code knows about this symbol, which therefore must be defined.
 */
	.text				# so that it goes upfront
	.align	2
	.globl	intstack
intstack:
	.space	INTSTACK_SIZE-4
	.globl	boot_stack
boot_stack:
	.space	4

/*
 *	This is the initial kernel entry point.
 *	We are somehow vulnerable to what the loader on the
 *	given machine does, but in general we should be called
 *	just like normal U*x programs.
 */
#define ENTRY_FRAME	(4*4)+4
					/* 4 argsaves + ra */

EXPORT(eprol)				/* profiling */
NESTED(start, ENTRY_FRAME, zero)

	move	a2,zero			# no environ, make clear
	move	ra,zero			# no going back

	la	gp,_gp			# load up GP register
	la	sp,boot_stack		# switch stack
	subu	sp,ENTRY_FRAME

	sw	a0,0(sp)
	sw	a1,4(sp)
	sw	a2,8(sp)
	sw	ra,(ENTRY_FRAME-4)(sp)

#if	MACH_KDB
	/*
	 * Preserve symtable, among other things.
	 */
	jal	kdb_init

	lw	a0,0(sp)
	lw	a1,4(sp)
	lw	a2,8(sp)
#endif	MACH_KDB

	mtc0	zero,c0_status		# running in kernel mode

	sw	zero,ENTRY_FRAME-4(sp)	# zero old ra for kdb

	mtc0	zero,c0_tlbhi		# kernel's ptes
	li	t0,KPTEADDR
	mtc0	t0,c0_tlbcxt

	jal	mips_init		# cold initialization
	/* NOTREACHED */
	END(start)
