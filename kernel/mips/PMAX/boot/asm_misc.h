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
 * $Log:	asm_misc.h,v $
 * Revision 2.4  91/02/05  17:38:27  mrt
 * 	Added author notices
 * 	[91/02/04  11:09:12  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:03:56  mrt]
 * 
 * Revision 2.3  90/12/05  23:29:20  af
 * 	Created.
 * 	[90/12/02            af]
 */
/*
 *	File: asm_misc.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	12/90
 *
 *	Miscellaneous definitions used in the assembly code.
 */

/* This is so that we do not need assym */
#define	LJMP_S0		0x00
#define	LJMP_S1		0x04
#define	LJMP_S2		0x08
#define	LJMP_S3		0x0c
#define	LJMP_S4		0x10
#define	LJMP_S5		0x14
#define	LJMP_S6		0x18
#define	LJMP_S7		0x1c
#define	LJMP_SP		0x20
#define	LJMP_FP		0x24
#define	LJMP_PC		0x28
#define	LJMP_SR		0x2c

#define	LJMP_SIZE	0x30

/*
 * PROM interface, PMAX specific
 */
#define	PROM_ENTRY(x)	(VEC_RESET+((x)*8))	/* Prom jump table */

#define	PROM_RESTART	PROM_ENTRY(2)
#define	PROM_OPEN	PROM_ENTRY(6)
#define	PROM_READ	PROM_ENTRY(7)
#define	PROM_WRITE	PROM_ENTRY(8)
#define	PROM_CLOSE	PROM_ENTRY(10)
#define	PROM_LSEEK	PROM_ENTRY(11)
#define	PROM_GETCHAR	PROM_ENTRY(12)
#define	PROM_PUTCHAR	PROM_ENTRY(13)
#define	PROM_GETS	PROM_ENTRY(15)
#define	PROM_PRINTF	PROM_ENTRY(17)
#define	PROM_GETENV	PROM_ENTRY(33)
#define	PROM_SETENV	PROM_ENTRY(34)


#ifndef	ASSEMBLER
extern char	*prom_getenv(/* char* */);
extern int	prom_open(/* char*, int */);
#endif	ASSEMBLER
