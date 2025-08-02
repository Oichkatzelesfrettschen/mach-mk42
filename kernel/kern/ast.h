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
 * $Log:	ast.h,v $
 * Revision 2.3  91/02/05  17:25:38  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:14  mrt]
 * 
 * Revision 2.2  90/06/02  14:53:34  rpd
 * 	Merged with mainline.
 * 	[90/03/26  22:02:55  rpd]
 * 
 * Revision 2.1  89/08/03  15:45:04  rwd
 * Created.
 * 
 *  6-Sep-88  David Golub (dbg) at Carnegie-Mellon University
 *	Adapted to MACH_KERNEL and VAX.
 *
 * 11-Aug-88  David Black (dlb) at Carnegie-Mellon University
 *	Created.  dbg gets equal credit for the design.
 *
 */

/*
 *	kern/ast.h: Definitions for Asynchronous System Traps.
 */

#ifndef	_KERN_AST_H_
#define _KERN_AST_H_

/*
 *	There are two types of AST's:
 *		1.  This thread must context switch [call thread_block()]
 *		2.  This thread must do something bizarre
 *			[call thread_halt_self()]
 *
 *	Type 2 ASTs are kept in a field in the thread which encodes the
 *	bizarre thing the thread must do.
 *
 *	The need_ast array (per processor) records whether ASTs are needed
 *	for each processor.  For now each processor only has access to its
 *	own cell in that array.  [May change when we record which 
 *	processor each thread is executing on.]
 *
 *	need_ast is initialized from the thread's ast field at context
 *	switch.  Type 1 ASTs are entered directly into the field
 *	by aston().  The actual values in need_ast do not matter, 
 *	an ast is required if it is non-zero.
 */

#include <cpus.h>

#include <kern/cpu_number.h>
#include <machine/ast.h>

/*
 *	Bits for reasons
 */

#define	AST_ZILCH	0x0
#define AST_HALT	0x1
#define AST_TERMINATE	0x2
/*#define AST_PROFILE	0x4  For future use */

#ifndef	MACHINE_AST
/*
 *	Type 1 ASTs
 *
 *	If machine/ast.h defines MACHINE_AST, then it is responsible
 *	for defining this functionality.
 */

extern int	need_ast[NCPUS];

#define	aston()		need_ast[cpu_number()] = 1
#define astoff()	need_ast[cpu_number()] = 0

/*
 *	Macros to propagate thread asts to need_ast at context switch and
 *	clock interrupts.  (Context switch replaces old ast requests,
 *	clock interrupt reflects new requests from thread to need_ast.
 *
 *	NOTE: cpu is always the current cpu.  It is in these macros
 *	solely to avoid recalculating it on machines where that may
 *	be expensive.
 */

#define	ast_context(thread, cpu)	need_ast[(cpu)] = (thread)->ast
#define	ast_propagate(thread, cpu)	need_ast[(cpu)] |= (thread)->ast
#define	ast_needed(cpu)			need_ast[(cpu)]
#endif	MACHINE_AST

/*
 *	Type 2 ASTs
 */
#define	thread_ast_set(thread, reason)	(thread)->ast |= (reason)
#define thread_ast_clear(thread, reason)	(thread)->ast &= ~(reason)
#define thread_ast_clear_all(thread)	(thread)->ast = AST_ZILCH

/*
 *	NOTE: if thread is the current thread, thread_ast_set should
 *	be followed by aston() 
 */

#endif	_KERN_AST_H_
