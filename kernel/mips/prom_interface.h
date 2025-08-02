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
 * $Log:	prom_interface.h,v $
 * Revision 2.6  91/02/05  17:50:47  mrt
 * 	Added author notices
 * 	[91/02/04  11:24:31  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:28:51  mrt]
 * 
 * Revision 2.5  90/12/05  23:38:40  af
 * 
 * 
 * Revision 2.4  90/12/05  20:50:11  af
 * 	Added decl for prom_getenv(), dropped some unused defs.
 * 	[90/12/03  23:04:48  af]
 * 
 * Revision 2.3  90/09/09  23:21:11  rpd
 * 	Removed depends on MIPSco compiler defines.
 * 	[90/09/05  10:30:34  af]
 * 
 * Revision 2.2  89/11/29  14:15:12  af
 * 	Created.
 * 	[89/10/06            af]
 */
/*
 *	File: prom_interface.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	Functions and data structures that link the kernel
 *	to the prom environment.
 */

#define	PROM_ENTRY(x)	(VEC_RESET+((x)*8))	/* Prom jump table */

#define	PROM_RESET	PROM_ENTRY(0)	/* run diags,check bootmode,reinit */
#define	PROM_RESTART	PROM_ENTRY(2)	/* re-enter monitor command loop */
#define	PROM_REBOOT	PROM_ENTRY(4)	/* check bootmode, no config */
#define	PROM_AUTOBOOT	PROM_ENTRY(5)	/* autoboot the system */

#ifdef	PMAX
#define	PROM_GETCHAR	PROM_ENTRY(12)	/* getchar from console */
#define	PROM_PUTCHAR	PROM_ENTRY(13)	/* putchar to console */
#endif	PMAX

#ifdef	MSERIES
#define	PROM_GETCHAR	PROM_ENTRY(11)	/* getchar from console */
#define	PROM_PUTCHAR	PROM_ENTRY(12)	/* putchar to console */
#endif	MSERIES

#define	PROM_ORW_RMW	PROM_ENTRY(22)	/* r-m-w version of or word */
#define	PROM_ORH_RMW	PROM_ENTRY(23)	/* r-m-w version of or halfword */
#define	PROM_ORB_RMW	PROM_ENTRY(24)	/* r-m-w version of or byte */
#define	PROM_ANDW_RMW	PROM_ENTRY(25)	/* r-m-w version of and word */
#define	PROM_ANDH_RMW	PROM_ENTRY(26)	/* r-m-w version of and halfword */
#define	PROM_ANDB_RMW	PROM_ENTRY(27)	/* r-m-w version of and byte */

#define	PROM_GETENV	PROM_ENTRY(33)	/* get environment variable */
#define	PROM_SETENV	PROM_ENTRY(34)	/* set environment variable */

#define	PROM_NOTIMPLEMENT PROM_ENTRY(52)/* guaranteed to be not implemented */

/*
 * Restart block -- monitor support for "warm" starts
 *
 * prom will perform "warm start" if restart_blk is properly set-up:
 *	rb_magic == RESTART_MAGIC
 *	rb_occurred == 0
 *	rb_checksum == 2's complement, 32-bit sum of first 32, 32-bit words 
 */
#define	RESTART_ADDR	0xa0000400	/* prom restart block */
#define	RESTART_MAGIC	0xfeedface
#define	RESTART_CSUMCNT	32		/* chksum 32 words of restart routine */
#define	RB_BPADDR	(RESTART_ADDR+24)/* address of rb_bpaddr */

#ifndef	ASSEMBLER
struct restart_blk {
	int	rb_magic;		/* magic pattern */
	int	(*rb_restart)();	/* restart routine */
	int	rb_occurred;		/* to avoid loops on restart failure */
	int	rb_checksum;		/* checksum of 1st 32 wrds of restrt */
	char	*rb_fbss;		/* start of prom bss and stack area */
	char	*rb_ebss;		/* end of prom bss and stack area */
	int	(*rb_bpaddr)();		/* breakpoint handler */
	int	(*rb_vtop)();		/* virtual to physical conversion rtn */
};

extern char	*prom_getenv();
#endif ASSEMBLER
