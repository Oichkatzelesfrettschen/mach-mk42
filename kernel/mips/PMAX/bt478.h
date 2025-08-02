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
 * $Log:	bt478.h,v $
 * Revision 2.4  91/02/05  17:39:55  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:11  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:09:53  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:30  af
 * 	Works on pmaxen.
 * 	[90/12/03  23:10:15  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:37:05  af
 * 	Created, from DEC specs and Brooktree data sheets:
 * 	"Product Databook 1989"
 * 	"Bt478 80 MHz 256 Color Palette RAMDAC"
 * 	Brooktree Corp. San Diego, CA
 * 	LA78001 Rev. M
 * 	[90/10/10            af]
 */
/*
 *	File: bt478.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Defines for the bt478 Cursor/RAMDAC chip
 */

typedef struct {
	unsigned char	addr_mapwa;
	char						pad0[3];
	unsigned char	addr_map;
	char						pad1[3];
	unsigned char	addr_mask;
	char						pad2[3];
	unsigned char	addr_mapra;
	char						pad3[3];
	unsigned char	addr_overwa;
	char						pad4[3];
	unsigned char	addr_over;
	char						pad5[3];
	unsigned char	addr_overra;
	char						pad6[3];
} bt478_regmap_t;

