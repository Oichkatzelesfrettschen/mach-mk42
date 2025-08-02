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
 * $Log:	gdt.c,v $
 * Revision 2.4  91/02/05  17:11:52  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:15  mrt]
 * 
 * Revision 2.3  90/08/27  21:56:36  dbg
 * 	Collapsed GDT.  Use new segmentation definitions.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:27:29  dbg
 * 	Created.
 * 	[90/02/15            dbg]
 * 
 */

/*
 * Global descriptor table.
 */
#include <i386/seg.h>
#include <i386/tss.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>

extern struct fake_descriptor	ldt[];
extern struct i386_tss		ktss;

struct fake_descriptor gdt[GDTSZ] = {
/* 0x000 */	{ 0, 0, 0, 0 },		/* always NULL */
/* 0x008 */	{ 0, 0, 0, 0 },		/* boot descriptor range */
/* 0x010 */	{ 0, 0, 0, 0 },
/* 0x018 */	{ 0, 0, 0, 0 },
/* 0x020 */	{ 0, 0, 0, 0 },
/* 0x028 */	{ 0, 0, 0, 0 },
/* 0x030 */	{ 0, 0, 0, 0 },
/* 0x038 */	{ 0, 0, 0, 0 },
/* 0x040 */	{ VM_MIN_ADDRESS,
		  (VM_MAX_KERNEL_ADDRESS-1-VM_MIN_ADDRESS)>>12,
		  SZ_32|SZ_G,
		  ACC_P|ACC_PL_K|ACC_CODE_R
		},			/* kernel code */
/* 0x048 */	{ VM_MIN_ADDRESS,
		  (VM_MAX_KERNEL_ADDRESS-1-VM_MIN_ADDRESS)>>12,
		  SZ_32|SZ_G,
		  ACC_P|ACC_PL_K|ACC_DATA_W
		},			/* kernel data */
/* 0x050 */	{ (unsigned int)ldt,
		  LDTSZ*sizeof(struct fake_descriptor)-1,
		  0,
		  ACC_P|ACC_PL_K|ACC_LDT
		},			/* local descriptor table */
/* 0x058 */	{ (unsigned int)&ktss,
		  sizeof(struct i386_tss),
		  0,
		  ACC_P|ACC_PL_K|ACC_TSS
		}			/* TSS for uniprocessor */
};


