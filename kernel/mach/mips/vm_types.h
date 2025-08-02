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
 * $Log:	vm_types.h,v $
 * Revision 2.3  91/02/05  17:35:03  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:13:23  mrt]
 * 
 * Revision 2.2  89/11/29  14:09:59  af
 * 	Copied for pure kernel.
 * 	[89/10/28  09:58:03  af]
 * 
 * Revision 2.1  89/05/30  16:55:50  rvb
 * Created.
 * 
 *  3-Jan-89  Alessandro Forin (af) at Carnegie-Mellon University
 *	Copied for Mips from Vax version.
 *
 * 23-Apr-87  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Changed things to "unsigned int" to appease the user community :-).
 *
 * 13-Jun-85  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created.
 */
/*
 *	File:	vm_types.h
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1985
 *
 *	Header file for VM data types.  Mips version.
 *
 */

#ifndef	_MACH_MIPS_VM_TYPES_H_
#define	_MACH_MIPS_VM_TYPES_H_	1

#ifdef	ASSEMBLER
#else	ASSEMBLER
typedef	unsigned int	vm_offset_t;
typedef	unsigned int	vm_size_t;
#endif	ASSEMBLER

#endif	_MACH_MIPS_VM_TYPES_H_
