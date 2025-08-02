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
 * $Log:	gq_hdw.c,v $
 * Revision 2.5  91/02/14  14:33:55  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:44:15  af]
 * 
 * Revision 2.4  91/02/05  17:40:52  mrt
 * 	Added author notices
 * 	[91/02/04  11:13:25  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:11:21  mrt]
 * 
 * Revision 2.3  90/12/05  23:31:10  af
 * 	Created (almost) empty.
 * 	[90/12/03  23:17:49  af]
 */
/*
 *	File: gq_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Hardware-level routines for the 3max 3D display.
 */


#include <sys/types.h>
#include <mips/busses.h>

/*
 * Definition of the driver for the auto-configuration program.
 */

int	gq_probe(), gq_attach(), gq_intr();

caddr_t	gq_std[] = { 0 };
struct	bus_device *gq_info[1];
struct	bus_driver gq_driver = 
        { gq_probe, 0, gq_attach, 0, gq_std, "gq", gq_info,};


/*
 * Probe/Attach functions
 */

gq_probe( /* reg, ui */)
{
#if	supported
	/*
	 * Probing was really done sweeping the TC long ago
	 */
	return tc_probe("gq");
#else
	return 0;
#endif
}

gq_attach(ui)
	struct bus_device *ui;
{
	/* ... */
	printf(": 3D color display");
	printf(", tell DEC to get CMU the specs if you want to use it");
}


/*
 * Interrupt routine
 */

gq_intr(unit,spllevel)
{
}

gq_mem_need()
{ return 4096;}

gq_cold_init(unit, up)
{
}
