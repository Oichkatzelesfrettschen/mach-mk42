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
 * $Log:	pm_defs.h,v $
 * Revision 2.4  91/02/05  17:43:06  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:49  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:35  mrt]
 * 
 * Revision 2.3  90/12/05  23:33:14  af
 * 	Cleanups.
 * 	[90/12/03  23:30:06  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:36:24  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: pm_defs.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Definitions specific to the "pm" simple framebuffer driver,
 *	exported across sub-modules.  Some other framebuffer drivers
 *	that share code with pm use these defs also.
 */

/* Screen Resolution */
#define ScanCols	1024
#define ScanRows	864
#define CharCols	128
#define CharRows	57
#define CharWidth	8
#define CharHeight	15
#define BytesPerLine	(ScanCols >> 3)

/* Hardware state (to be held in the screen descriptor) */

#include <mips/PMAX/bt478.h>

typedef struct {
	char		*cursor_registers;	/* opaque, for sharing */
	unsigned short	cursor_state;		/* some regs are W-only */
	short		unused;			/* padding, free */
	unsigned char	*framebuffer;
	unsigned char	*plane_mask;
	bt478_regmap_t	*bt478;			/* VDAC */
} pm_softc_t;

extern pm_softc_t	*pm_alloc(/* unit, regs, framebuf, planem, flags*/);

/* flags values, in addition to those in screen.h */
#define	PM_SOMETHING	SCFLAG_UNSPEC
#define	PM_SOMEMORE	(SCFLAG_UNSPEC<<1)

/* user mapping sizes */
#define	USER_INFO_SIZE	PAGE_SIZE
#define	PMASK_SIZE	PAGE_SIZE
#define	BITMAP_SIZE(sc)	(1024 * ((sc->flags & COLOR_SCREEN) ? 1024 : 256))

#define	PM_SIZE(sc)	(USER_INFO_SIZE+PMASK_SIZE+BITMAP_SIZE(sc))
