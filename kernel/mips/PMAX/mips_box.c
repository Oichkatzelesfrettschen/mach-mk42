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
 * $Log:	mips_box.c,v $
 * Revision 2.7  91/02/14  14:34:52  mrt
 * 	In clock interrupt routine, drop priority as now required.
 * 	[91/02/12  12:59:26  af]
 * 
 * 	Defined pmax_memcheck() and related default implementation.
 * 	[91/01/03  02:10:32  af]
 * 
 * Revision 2.6  91/02/05  17:42:47  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:22  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:01  mrt]
 * 
 * Revision 2.5  91/01/09  19:49:54  rpd
 * 	Defined pmax_memcheck() and related default implementation.
 * 	[91/01/03  02:10:32  af]
 * 
 * Revision 2.4  90/12/05  23:32:43  af
 * 	Moved a number of pmax-specific handlers in kn01.c
 * 	[90/12/03  23:28:42  af]
 * 
 * Revision 2.2.1.2  90/11/01  03:47:10  af
 * 	Mostly renames for new, copyright free drivers.
 * 	Added "pm" driver.
 * 
 * Revision 2.2.1.1  90/10/03  11:54:25  af
 * 	Reflected changes in new autoconf TC code.  Also, moved wildcarded
 * 	"se" driver entry after non-wildcarded ones.
 * 
 * Revision 2.2  90/08/07  22:25:14  rpd
 * 	Created.
 * 	[90/08/07  15:49:22  af]
 * 
 * Revision 2.1.1.1  90/05/30  15:49:07  af
 * 	Created.
 * 	[90/05/28            af]
 * 
 */
/*
 *	File: mips_box.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	5/90
 *
 *	Box-specific routines and data required by the MI code
 */

#include <gx.h>
#include <kern/time_out.h>
#include <mips/mips_cpu.h>
#include <mips/PMAX/mips_box.h>
#include <sys/types.h>
#include <mips/busses.h>


extern	kn01_scsi_intr(), kn01_se_intr(),
	kn01_dz_intr(), kn01_err_intr();

/* Common clock interrupt routine for pmax/3max */
mc_intr(st,spllevel)
{
	ackrtclock(spllevel);
	splx(spllevel);
	clock_interrupt(tick, st&SR_KUo, (st&SR_INT_MASK)==INT_LEV0);
}

int (*pmax_intr2)() = kn01_scsi_intr;		/* SCSI */
int (*pmax_intr3)() = kn01_se_intr;		/* Lance */
int (*pmax_intr4)() = kn01_dz_intr;		/* DZ */
int (*pmax_intr5)() = mc_intr;			/* RTC */
int (*pmax_intr6)() = kn01_err_intr;		/* Write timeouts */


static
boolean_t null_memcheck(addr,pc) { return FALSE; }
boolean_t (*pmax_memcheck)() = null_memcheck;

extern struct bus_driver
	asc_driver, sii_driver,
	se_driver, dz_driver,
	pm_driver, ga_driver, gq_driver, cfb_driver;
extern int
	asc_intr(), sii_intr(),
	se_intr(), dz_intr(),
	pm_intr(), ga_intr(), gq_intr(), cfb_intr();

struct bus_ctlr bus_master_init[] = {
/*driver,	name,	unit, intr, address+am, phys, adpt, alive, flags, */
{ &asc_driver,	"asc",	0,    asc_intr,  0x0,0,	0,    '?',    0,     0, },
{ &sii_driver,	"sii",	0,    sii_intr,  0x0,0,	0,    '?',    0,     0, },
	0
};

struct bus_device bus_device_init[] = {
/* driver,      name, unit,intr,addr+am,phys, adaptor,alive,ctlr,slave,flags,*/
{ &asc_driver,	"rz",   0,  0,  0x0,0,	0,    '?',     0,   0,   0,    0, },
{ &asc_driver,	"rz",   1,  0,  0x0,0,	0,    '?',     0,   0,   1,    0, },
{ &asc_driver,	"rz",   2,  0,  0x0,0,	0,    '?',     0,   0,   2,    0, },
{ &asc_driver,	"rz",   3,  0,  0x0,0,	0,    '?',     0,   0,   3,    0, },
{ &asc_driver,	"rz",   4,  0,  0x0,0,	0,    '?',     0,   0,   4,    0, },
{ &asc_driver,	"rz",   5,  0,  0x0,0,	0,    '?',     0,   0,   5,    0, },
{ &asc_driver,	"rz",   6,  0,  0x0,0,	0,    '?',     0,   0,   6,    0, },
{ &asc_driver,	"rz",   7,  0,  0x0,0,	0,    '?',     0,   0,   7,    0, },
{ &asc_driver,	"tz",   0,  0,  0x0,0,	0,    '?',     0,   0,   0,    0, },
{ &asc_driver,	"tz",   1,  0,  0x0,0,	0,    '?',     0,   0,   1,    0, },
{ &asc_driver,	"tz",   2,  0,  0x0,0,	0,    '?',     0,   0,   2,    0, },
{ &asc_driver,	"tz",   3,  0,  0x0,0,	0,    '?',     0,   0,   3,    0, },
{ &asc_driver,	"tz",   4,  0,  0x0,0,	0,    '?',     0,   0,   4,    0, },
{ &asc_driver,	"tz",   5,  0,  0x0,0,	0,    '?',     0,   0,   5,    0, },
{ &asc_driver,	"tz",   6,  0,  0x0,0,	0,    '?',     0,   0,   6,    0, },
{ &asc_driver,	"tz",   7,  0,  0x0,0,	0,    '?',     0,   0,   7,    0, },

{ &sii_driver,	"rz",   0,  0,  0x0,0,	0,    '?',     0,   0,   0,    0, },
{ &sii_driver,	"rz",   1,  0,  0x0,0,	0,    '?',     0,   0,   1,    0, },
{ &sii_driver,	"rz",   2,  0,  0x0,0,	0,    '?',     0,   0,   2,    0, },
{ &sii_driver,	"rz",   3,  0,  0x0,0,	0,    '?',     0,   0,   3,    0, },
{ &sii_driver,	"rz",   4,  0,  0x0,0,	0,    '?',     0,   0,   4,    0, },
{ &sii_driver,	"rz",   5,  0,  0x0,0,	0,    '?',     0,   0,   5,    0, },
{ &sii_driver,	"rz",   6,  0,  0x0,0,	0,    '?',     0,   0,   6,    0, },
{ &sii_driver,	"rz",   7,  0,  0x0,0,	0,    '?',     0,   0,   7,    0, },
{ &sii_driver,	"tz",   0,  0,  0x0,0,	0,    '?',     0,   0,   0,    0, },
{ &sii_driver,	"tz",   1,  0,  0x0,0,	0,    '?',     0,   0,   1,    0, },
{ &sii_driver,	"tz",   2,  0,  0x0,0,	0,    '?',     0,   0,   2,    0, },
{ &sii_driver,	"tz",   3,  0,  0x0,0,	0,    '?',     0,   0,   3,    0, },
{ &sii_driver,	"tz",   4,  0,  0x0,0,	0,    '?',     0,   0,   4,    0, },
{ &sii_driver,	"tz",   5,  0,  0x0,0,	0,    '?',     0,   0,   5,    0, },
{ &sii_driver,	"tz",   6,  0,  0x0,0,	0,    '?',     0,   0,   6,    0, },
{ &sii_driver,	"tz",   7,  0,  0x0,0,	0,    '?',     0,   0,   7,    0, },

{ &se_driver,	"se",   1,se_intr,0x0,0, 0,     0,     0,  -1,  -1,    0, },
{ &se_driver,	"se",   1,se_intr,0x0,0, 0,     1,     0,  -1,  -1,    0, },
{ &se_driver,	"se",   1,se_intr,0x0,0, 0,     2,     0,  -1,  -1,    0, },
{ &se_driver,	"se",   0,se_intr,0x0,0, 0,   '?',     0,  -1,  -1,    0, },

{ &dz_driver,	"dz",   0,dz_intr,0x0,0, 0,    '?',     0,  -1,  -1,    0, },
#if	NGX>0
{ &gq_driver,	"gq",   0,gq_intr,0x0,0, 0,    '?',     0,  -1,  -1,    0, },
{ &ga_driver,	"ga",   0,ga_intr,0x0,0, 0,    '?',     0,  -1,  -1,    0, },
#endif	NGX>0
{ &cfb_driver,	"cfb",  0,cfb_intr,0x0,0,0,    '?',     0,  -1,  -1,    0, },
{ &pm_driver,   "pm",   0,pm_intr,0x0,0, 0,    '?',	0,  -1,  -1,	0, },
	0
};

