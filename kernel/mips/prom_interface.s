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
 * $Log:	prom_interface.s,v $
 * Revision 2.3  91/02/05  17:50:54  mrt
 * 	Added author notices
 * 	[91/02/04  11:24:37  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:28:57  mrt]
 * 
 * Revision 2.2  89/11/29  14:15:18  af
 * 	Created.
 * 	[89/10/06            af]
 */
/*
 *	File: prom_interface.s
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	PROM entrypoints of interest to kernel
 */

#include <mach/mips/asm.h>
#include <mips/mips_cpu.h>
#include <mips/prom_interface.h>

	.text
	.align	2

/*
 * reset reboots as indicated by $bootmode and looks for warm start block
 */
EXPORT(prom_reset)
	li	v0,PROM_RESET
	j	v0

/*
 * restart always enters prom command mode
 */
EXPORT(prom_restart)
	li	v0,PROM_RESTART
	j	v0

/*
 * reboot does action indicated by $bootmode
 */
EXPORT(prom_reboot)
	li	v0,PROM_REBOOT
	j	v0

/*
 * prom flow-controlled console io
 */
EXPORT(prom_getchar)
	li	v0,PROM_GETCHAR
	j	v0

EXPORT(prom_putchar)
	li	v0,PROM_PUTCHAR
	j	v0

/*
 * prom read-modify-write routines
 * NOTE: on the Pmax they should not be called
 */
EXPORT(andb_rmw)
	li	v0,PROM_ANDB_RMW
	j	v0

EXPORT(andh_rmw)
	li	v0,PROM_ANDH_RMW
	j	v0

EXPORT(andw_rmw)
	li	v0,PROM_ANDW_RMW
	j	v0

EXPORT(orb_rmw)
	li	v0,PROM_ORB_RMW
	j	v0

EXPORT(orh_rmw)
	li	v0,PROM_ORH_RMW
	j	v0

EXPORT(orw_rmw)
	li	v0,PROM_ORW_RMW
	j	v0

/*
 * Other prom operations
 */

EXPORT(prom_getenv)
	li	v0,PROM_GETENV
	j	v0

EXPORT(prom_setenv)
	li	v0,PROM_SETENV
	j	v0

EXPORT(prom_autoboot)
	li	v0,PROM_AUTOBOOT
	j	v0

