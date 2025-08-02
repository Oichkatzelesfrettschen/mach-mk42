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
 * $Log:	ast.h,v $
 * Revision 2.4  91/02/05  17:46:53  mrt
 * 	Added author notices
 * 	[91/02/04  11:20:46  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:24:19  mrt]
 * 
 * Revision 2.3  90/06/02  15:01:53  rpd
 * 	Cleaned up.
 * 	[90/03/26  22:47:25  rpd]
 * 
 * Revision 2.2  89/11/29  14:12:36  af
 * 	Created.
 * 	[89/10/11            af]
 * 
 */
/*
 *	File: ast.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	11/89
 *
 *	Machine-dependent AST file for MIPS.
 */

#ifndef	_MIPS_AST_H_
#define	_MIPS_AST_H_

/*
 *	The MIPS cpu does not have a hardware AST.  The software
 *	implementation uses a pseudo-exception code to be passed
 *	to trap(), quite similarly to the VAX case.
 *
 */
#define		EXC_AST		0x00000040

#endif	_MIPS_AST_H_
