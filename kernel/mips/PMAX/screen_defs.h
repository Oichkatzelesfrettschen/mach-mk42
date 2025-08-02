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
 * $Log:	screen_defs.h,v $
 * Revision 2.4  91/02/05  17:44:18  mrt
 * 	Added author notices
 * 	[91/02/04  11:17:42  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:16:17  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:22  af
 * 
 * 
 * Revision 2.1.1.1  90/11/13  15:05:29  af
 * 	Created.
 * 	[90/11/07            af]
 */
/*
 *	File: screen_defs.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	11/90
 *
 *	Definitions for the Generic Screen Driver.
 */

#include <mips/PMAX/screen.h>
#include <mips/PMAX/screen_switch.h>

/*
 * Driver state
 */
typedef struct screen_softc {
	user_info_t		*up;
	char			**hw_state;	/* semi-opaque */
	struct screen_switch	sw;
	char			flags;
	char			mapped;
	char			blitc_state;
	char			standout;
	short			save_row;
	short			save_col;

/* This is used by all screens, therefore it is sized maximally */
#	define MaxCharRows	68	/* 2DA screen */
#	define MaxCharCols	158
#	define MinCharRows	57	/* pmax */
	unsigned char		ascii_screen[MaxCharRows*MaxCharCols];

} *screen_softc_t;

extern screen_softc_t	screen_up(/* unit, up */);

/*
 * This global says if we have a graphic console
 * and where it is and if it is enabled
 */
extern short	screen_console;
#define SCREEN_CONS_ENBL	(0x0100)
#define	SCREEN_ISA_CONSOLE()	(screen_console & SCREEN_CONS_ENBL)
#define SCREEN_CONS_UNIT()	(screen_console & 0x00ff)

/* kernel font */
extern unsigned char kfont_7x14[];


