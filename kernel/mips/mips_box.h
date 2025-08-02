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
 * $Log:	mips_box.h,v $
 * Revision 2.4  91/02/05  17:49:09  mrt
 * 	Added author notices
 * 	[91/02/04  11:23:10  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:26:48  mrt]
 * 
 * Revision 2.3  90/08/07  22:29:27  rpd
 * 	Support for 3max, removed some 'required' defs.
 * 	[90/08/07  15:23:49  af]
 * 
 * Revision 2.2.1.1  90/05/30  16:38:58  af
 * 	Support for 3max, removed some 'required' defs.
 * 
 * Revision 2.2  89/11/29  14:14:23  af
 * 	Created.
 * 	[89/10/05            af]
 */
/*
 *	File: mips_box.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	General definitions for all MIPS implementations
 */

#define	MIN_CACHE_SIZE		(4*1024)
#define	MAX_CACHE_SIZE		(64*1024)

/*
 *	Specific definitions for particular boxes.
 *
 *	Required definitions are:
 *
 *		RDCLR_BUSERR	must be read to ack a bus error
 *		WRCLR_BUSERR	must be written to ack a bus error
 *		INT_LEV_CONSOLE	console interrupt level
 *		IP_LEV_FPA	fpa interrupt pending
 *
 */


#ifdef	MSERIES
#include <mips/MSERIES/mips_box.h>
#endif	MSERIES
#ifdef	PMAX
#include <mips/PMAX/mips_box.h>
#endif	PMAX

#ifndef	CONFIG_INVAL
#define	CONFIG_INVAL	0
#endif	CONFIG_INVAL

/*
 * Meanings of the board id in prom, if any
 */
#define BRDTYPE_R2300		1
#define BRDTYPE_R2600		2
#define BRDTYPE_R2800		3

#define BRDTYPE_DEC3100		0x81
#define BRDTYPE_DEC5000		0x82
#define BRDTYPE_DEC5400		0x85
#define BRDTYPE_DEC5800		0x86


#define FPA_IMP_MASK		0x0000ff00

#define FPA_IMP_NONE		0x00000000
#define FPA_IMP_R2360		0x00000100
#define FPA_IMP_R2010		0x00000200
#define FPA_IMP_R3010		0x00000300
