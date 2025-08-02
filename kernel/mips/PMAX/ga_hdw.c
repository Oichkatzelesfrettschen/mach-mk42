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
 * $Log:	ga_hdw.c,v $
 * Revision 2.5  91/02/14  14:33:49  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:43:54  af]
 * 
 * Revision 2.4  91/02/05  17:40:46  mrt
 * 	Added author notices
 * 	[91/02/04  11:13:20  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:11:12  mrt]
 * 
 * Revision 2.3  90/12/05  23:31:08  af
 * 	Created, (almost) empty.
 * 	[90/12/03  23:17:23  af]
 */
/*
 *	File: ga_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Hardware-level routines for the 3max 2D display.
 */

#include <sys/types.h>
#include <mips/busses.h>

/*
 * Definition of the driver for the auto-configuration program.
 */

int	ga_probe(), ga_attach(), ga_intr();

caddr_t	ga_std[] = { 0 };
struct	bus_device *ga_info[1];
struct	bus_driver ga_driver = 
        { ga_probe, 0, ga_attach, 0, ga_std, "ga", ga_info,};


/*
 * Probe/Attach functions
 */

ga_probe( /* reg, ui */)
{
#if	supported
	/*
	 * Probing was really done sweeping the TC long ago
	 */
	return tc_probe("ga");
#else
	return 0;
#endif
}

ga_attach(ui)
	struct bus_device *ui;
{
	/* ... */
	printf(": 2D color display");
	printf(", tell DEC to get CMU the specs if you want to use it");
}


/*
 * Interrupt routine
 */

ga_intr(unit,spllevel)
{
}

ga_mem_need()
{return 4096;}

ga_cold_init(unit, up)
{}
