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
 * $Log:	dc503.c,v $
 * Revision 2.4  91/02/05  17:40:14  mrt
 * 	Added author notices
 * 	[91/02/04  11:12:34  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:24  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:42  af
 * 	Works now.
 * 	[90/12/03  23:12:48  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:21  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	[90/09/03            af]
 */
/*
 *	File: dc503.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Routines for the DEC DC503 Programmable Cursor Chip
 */

#include <mips/PMAX/dc503.h>

/*
 * The CMD register of the PCC is write-only
 * Pass a pointer to this structure when touching it:
 * "state" maintains a sw copy of the register
 */
struct pcc {
	dc503_regmap_t	*regs;
	unsigned short	state;
};

/*
 * Cursor
 */
dc503_pos_cursor( regs, x, y)
	dc503_regmap_t	*regs;
{
	regs->pcc_xpos = x + 212;
	regs->pcc_ypos = y +  34;
	wbflush();
}

dc503_load_cursor( pcc, cursor)
	struct pcc	*pcc;
	unsigned short	*cursor;
{
	dc503_regmap_t	*regs = pcc->regs;
	register int    i;

	pcc->state |= DC503_CMD_LODSA;
	regs->pcc_cmdr = pcc->state;
	wbflush();
	for (i = 0; i < 32; i++) {
		regs->pcc_memory = *cursor++;
		wbflush();
	}
	pcc->state &= ~DC503_CMD_LODSA;
	regs->pcc_cmdr = pcc->state;
}


unsigned short dc503_default_cursor[16+16] = {
/* Plane A */
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
/* Plane B */
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
	0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff
};

/*
 * Vert retrace interrupt
 */
dc503_vretrace( pcc, on)
	struct pcc	*pcc;
{
	if (on)
		pcc->state |= DC503_CMD_ENRG2;
	else
		pcc->state &= ~DC503_CMD_ENRG2;
	pcc->regs->pcc_cmdr = pcc->state;
}

/*
 * Video on/off
 */
dc503_video_on( pcc, up)
	struct pcc	*pcc;
{
	pcc->state = DC503_CMD_ENPA | (pcc->state & ~DC503_CMD_FOPB);
	pcc->regs->pcc_cmdr = pcc->state;
}

dc503_video_off( pcc, up)
	struct pcc	*pcc;
{
	pcc->state = DC503_CMD_FOPB | (pcc->state & ~DC503_CMD_ENPA);
	pcc->regs->pcc_cmdr = pcc->state;
}


/*
 * Initialization
 */
dc503_init( pcc )
	struct pcc	*pcc;
{
	dc503_regmap_t	*regs = pcc->regs;

	dc503_load_cursor( pcc, dc503_default_cursor);
	dc503_pos_cursor( regs, 0, 0);	/* XXX off screen */

	regs->pcc_xmin1 = 0;	/* test only */
	regs->pcc_xmax1 = 0;
	regs->pcc_ymin1 = 0;
	regs->pcc_ymax1 = 0;

	regs->pcc_xmin2 = 212;	/* vert retrace detector */
	regs->pcc_xmax2 = 212+1023;
	regs->pcc_ymin2 = 34+864;
	regs->pcc_ymax2 = 34+864;

#if 0
	regs->pcc_cmdr = DC503_CMD_FOPB | DC503_CMD_VBHI;/* reset */
#endif
	pcc->state = DC503_CMD_ENPA | DC503_CMD_ENPB;
	regs->pcc_cmdr = pcc->state;
}
