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
 * $Log:	rz.h,v $
 * Revision 2.4  91/02/05  17:43:42  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:33  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:29  mrt]
 * 
 * Revision 2.3  90/12/05  23:33:55  af
 * 
 * 
 * Revision 2.1.1.1  90/11/01  03:43:37  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Mapping between U*x-like indexing and controller+slave
 *	Each controller handles at most 8 slaves, few controllers.
 */

#define	controller(dev)	((dev)>>6)&0x3
#define	slave(dev)	((dev)>>3)&0x7
#define	partition(dev)	((dev)&0x7)

