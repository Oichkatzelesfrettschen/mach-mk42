/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log:	i386_init.c,v $
 * Revision 2.7  91/02/05  17:12:06  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:34  mrt]
 * 
 * Revision 2.6  90/12/04  14:46:01  jsb
 * 	iPSC2 -> iPSC386; ipsc2_foo -> ipsc_foo.
 * 	[90/12/04  11:15:57  jsb]
 * 
 * Revision 2.5  90/11/24  15:14:43  jsb
 * 	Moved boottype, esym, first_avail outside AT386 conditional.
 * 	Added definition of cnvmem, initialized to 1024, for iPSC386;
 * 	this lets pmap know that entire first meg is usable.
 * 	In size_memory, set first_addr to 0 in non-AT386 case,
 * 	instead	of 0x1000, since only BIOS screws up this low memory.
 * 	[90/11/24  11:58:32  jsb]
 * 
 * Revision 2.4  90/11/05  14:27:16  rpd
 * 	Start first_addr at 0x1000 and define first available page 
 * 	after the kernel's end as first_avail.
 * 	Flush EXL.
 * 	[90/09/05            rvb]
 * 
 * Revision 2.3  90/09/23  17:45:10  jsb
 * 	Added support for iPSC386.
 * 	[90/09/21  16:39:41  jsb]
 * 
 * Revision 2.2  90/05/03  15:27:39  dbg
 * 	Alter for pure kernel.
 * 	[90/02/15            dbg]
 * 
 * Revision 1.5.1.4  90/02/01  13:36:37  rvb
 * 	esym must always be defined.  This is as good a place as any.
 * 	[90/01/31            rvb]
 * 
 * Revision 1.5.1.3  89/12/28  12:43:10  rvb
 * 	Fix av_start & esym initialization, esp for MACH_KDB.
 * 	[89/12/26            rvb]
 * 
 * Revision 1.5.1.2  89/12/21  17:59:49  rvb
 * 	enable esym processing.
 * 
 * 
 * Revision 1.5.1.1  89/10/22  11:30:41  rvb
 * 	Setup of rootdevice should not be here.  And it was wrong.
 * 	[89/10/17            rvb]
 * 
 * 	Scary!  We've changed sbss to edata.  AND the coff loader
 * 	following the vuifile spec was actually aligning the bss 
 * 	on 4k boundaries.
 * 	[89/10/16            rvb]
 * 
 * Revision 1.5  89/04/05  12:57:39  rvb
 * 	Move extern out of function scope for gcc.
 * 	[89/03/04            rvb]
 * 
 * Revision 1.4  89/02/26  12:31:25  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *
 */

/*
 *	File:	i386_init.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1986
 *
 *	Basic initialization for I386
 */

#include <cputypes.h>
#include <mach_kdb.h>

#include <mach/i386/vm_param.h>

#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <vm/vm_page.h>

int		loadpt;

vm_size_t	mem_size;
vm_size_t	rawmem_size;
vm_offset_t	first_addr = 0;	/* set by start.s - keep out of bss */
vm_offset_t	last_addr;

vm_offset_t	avail_start, avail_end;
vm_offset_t	virtual_avail, virtual_end;

int boottype = 0;
#ifdef	wheeze
#else	wheeze
char *esym = (char *)0;
char *first_avail = (char *)0;
#endif	wheeze

#if	AT386
/* parameters passed from bootstrap loader */
int cnvmem = 0;		/* must be in .data section */
int extmem = 0;
#endif	AT386

#if	iPSC386
/* parameters passed from NX bootstrap loader */
int ipsc_this_node = 0;	/* must be in .data section */
int ipsc_physnode = 0;
int ipsc_num_nodes = 0;
int ipsc_basemem = 0;
int cnvmem = 1024;		/* no hole at 640K */
#endif	iPSC386

extern char	edata, end;

i386_init()
{
	bzero((char *)&edata,(unsigned)(&end - &edata));

	/*
	 * Initialize the pic prior to any possible call to an spl.
	 */
	picinit();
	size_memory();

	/*
	 *	Initialize kernel physical map, mapping the
	 *	region from loadpt to avail_start.
	 *	Kernel virtual address starts at VM_KERNEL_MIN_ADDRESS.
	 */

	avail_start = first_addr;
	avail_end = last_addr;
	printf("AT386 boot: memory from 0x%x to 0x%x\n", avail_start,
			avail_end);

	pmap_bootstrap(loadpt, &avail_start, &avail_end, &virtual_avail,
				&virtual_end);

	printf("i386_init: virtual_avail = %x, virtual_end = %x\n",
		virtual_avail, virtual_end);

}


size_memory()
{
	register vm_offset_t	look;

#if	AT386
	look = 1024*1024 + extmem*1024;    /* extend memory starts at 1MB */
#endif	AT386
#if	iPSC386
	look = ipsc_basemem;
#endif	iPSC386

	mem_size = look - loadpt;

	vm_set_page_size();

#if	AT386
	first_addr = round_page(0x1000);	/* bios leaves some stuff in low addresses */
#else	AT386
	first_addr = round_page(0);
#endif	AT386
	last_addr = trunc_page(look);
}		 	
