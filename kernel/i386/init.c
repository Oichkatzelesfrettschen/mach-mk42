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
 * $Log:	init.c,v $
 * Revision 2.9  91/02/05  17:12:18  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:51  mrt]
 * 
 * Revision 2.8  91/01/09  22:41:25  rpd
 * 	Added ktss zone.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.7  91/01/08  15:10:43  rpd
 * 	Initialize the new pcb zone in machine_init.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.6  90/12/04  14:46:04  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:16:27  jsb]
 * 
 * Revision 2.5  90/09/23  17:45:12  jsb
 * 	Added cpu_subtype for iPSC386 (and EXL while I was at it).
 * 	[90/09/21  16:37:06  jsb]
 * 
 * Revision 2.4  90/08/27  21:57:00  dbg
 * 	Fix ldt in start.s
 * 	[90/07/25            dbg]
 * 
 * Revision 2.3  90/06/19  22:57:37  rpd
 * 	In timemmap, don't check for execute permission.
 * 	[90/06/04            rpd]
 * 
 * Revision 2.2  90/05/03  15:27:48  dbg
 * 	Created.
 * 	[90/02/15            dbg]
 * 
 */
#include <mach_kdb.h>

#include <mach/machine.h>
#include <kern/mach_param.h>
#include <kern/time_out.h>
#include <kern/zalloc.h>
#include <i386/thread.h>
#include <i386/tss.h>

#include <sys/time.h>
#if	MACH_KDB
#include <sys/reboot.h>
#endif	MACH_KDB

extern zone_t pcb_zone;
extern zone_t ktss_zone;

extern char	version[];

extern void	setup_main();

void		inittodr();	/* forward */

int		rebootflag = 0;	/* exported to kdintr */

/*
 *	Cpu initialization.  Running virtual, but without MACH VM
 *	set up.  First C routine called.
 */
void machine_startup()
{
	/*
	 * Do basic VM initialization
	 */
	i386_init();

	/*
	 * Initialize the console so we can print.
	 */
	cninit();
#if	MACH_KDB

	/*
	 * Initialize the kernel debugger.
	 */
	kdb_init();

	/*
	 * Cause a breakpoint trap to the debugger before proceeding
	 * any further if the proper option bit was specified in
	 * the boot flags.
	 *
	 * XXX use -a switch to invoke kdb, since there's no
	 *     boot-program switch to turn on RB_HALT!
	 */
	if (boothowto & RB_ASKNAME && boothowto & RB_KDB)
	    Debugger();
#endif	MACH_KDB

	printf(version);

	machine_slot[0].is_cpu = TRUE;
	machine_slot[0].running = TRUE;
	machine_slot[0].cpu_type = CPU_TYPE_I386;
#if	AT386
	machine_slot[0].cpu_subtype = CPU_SUBTYPE_AT386;
#endif	AT386
#if	EXL
	machine_slot[0].cpu_subtype = CPU_SUBTYPE_EXL;
#endif	EXL
#if	iPSC386
	machine_slot[0].cpu_subtype = CPU_SUBTYPE_iPSC386;
#endif	iPSC386

	/*
	 * Start the system.
	 */
	setup_main();
}

/*
 * Find devices.  The system is alive.
 */
void machine_init()
{
	pcb_zone = zinit(sizeof(struct pcb),
			 THREAD_MAX * sizeof(struct pcb),
			 THREAD_CHUNK * sizeof(struct pcb),
			 FALSE, "pcb");
	ktss_zone = zinit(sizeof(struct i386_tss),
			  THREAD_MAX * sizeof(struct i386_tss),
			  THREAD_CHUNK * sizeof(struct i386_tss),
			  FALSE, "ktss");

	/*
	 * Find the devices
	 */
	probeio();

	/*
	 * Find the root device
	 */
	get_root_device();

	/*
	 * Get the time
	 */
	inittodr();
}

/*
 * Halt a cpu.
 */
halt_cpu()
{
}

/*
 * Halt the system or reboot.
 */
halt_all_cpus(reboot)
	boolean_t	reboot;
{
	if (reboot) {
#if	iPSC386
#else	iPSC386
	    kdreboot();
#endif	iPSC386
	}
	rebootflag = 1;
	printf("In tight loop: hit ctl-alt-del to reboot\n");
	(void) spl0();
	for (;;)
	    continue;

}

#include <mach/vm_prot.h>
#include <vm/pmap.h>
#include <mach/time_value.h>

timemmap(dev,off,prot)
	vm_prot_t prot;
{
	extern time_value_t *mtime;

#ifdef	lint
	dev++; off++;
#endif	lint

	if (prot & VM_PROT_WRITE) return (-1);

	return (i386_btop(pmap_extract(pmap_kernel(), (vm_offset_t) mtime)));
}

startrtclock()
{
	clkstart();
}

void
inittodr()
{
	time_value_t	new_time;

	new_time.seconds = 0;
	new_time.microseconds = 0;

	(void) readtodc(&new_time.seconds);

	{
	    int	s = splhigh();
	    time = new_time;
	    splx(s);
	}
}

void
resettodr()
{
	writetodc();
}
