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
 * $Log:	model_dep.c,v $
 * Revision 2.6  91/02/05  17:42:56  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:34  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:20  mrt]
 * 
 * Revision 2.5  91/01/09  19:50:15  rpd
 * 	Set pmax_memcheck() for pmaxen.
 * 	[91/01/03  02:12:58  af]
 * 
 * Revision 2.4  90/12/05  23:33:02  af
 * 
 * 
 * Revision 2.2.1.2  90/11/01  03:54:28  af
 * 	Moved hand filling routine where it belonged,
 * 	added isa_pmax() needed by the pm driver: it
 * 	doesn't have any way to distinguish between
 * 	real memory and the framebuffer on a 3max
 * 	where it MUST fail to probe.
 * 
 * Revision 2.2.1.1  90/10/03  11:57:38  af
 * 	Reflected new TC autoconf code.
 * 	[90/10/03            af]
 * 
 * Revision 2.2  90/08/07  22:25:24  rpd
 * 	New 3max-specific memory error routine.
 * 	[90/08/07  15:38:47  af]
 * 
 * Revision 2.1.1.1  90/05/30  15:49:37  af
 * 	Moved over to pure kernel.
 * 
 * 
 * Revision 2.1.1.1  90/05/20  14:25:23  af
 * 	Created.
 * 	[90/04/17            af]
 */
/*
 *	File: model_dep.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	4/90
 *
 *	Cold-configuration functions for specific DEC mips boxes.
 *
 *	This file defines model-dependent changes that must be
 *	applied to various modules before autoconf time.
 *	This includes handling of bus errors, strange SPL setups,
 *	minor variations over standard components (clock), etc.
 */

#include <mach/std_types.h>
#include <mips/mips_box.h>
#include <mips/PMAX/tc.h>

static char pmax_box = 0;
isa_pmax() {return pmax_box;}

kn01_model_dep()
{
	/*
	 *	The pmax is our "generic" DEC box.
	 */
	extern boolean_t kn01_memcheck();

	pmax_box = 1;
	pmax_memcheck = kn01_memcheck;
}

kn02_model_dep()
{
	/*
	 * 3max 
	 */
	extern int      ((*mips_box_buserror[]) ()), wrclr_buserror();
	extern          splimp(), spltty(), splclock();
	extern int      (*memory_sizing_routine) (), size_memory_trusting_prom();
	extern          kn02_tcintr(), mc_intr(), stray_intr(), kn02_errintr();
	extern          (*identify_devices) (), config_3max_routine();
	extern void	kn02_slot_hand_fill();

	mips_box_buserror[0] = wrclr_buserror;

	patch_spl(splimp, 3);
	patch_spl(spltty, 3);
	patch_spl(splclock, 4);

	memory_sizing_routine = size_memory_trusting_prom;

	pmax_intr2 = kn02_tcintr;
	pmax_intr3 = mc_intr;
	pmax_intr4 = stray_intr;
	pmax_intr5 = kn02_errintr;
	pmax_intr6 = stray_intr;

	identify_devices = config_3max_routine;

	set_clock_address(0xbfe80000);
	set_dz_address(0xbfe00000, 1);
	set_se_switch(2);

	/*
	 * Printing on console might require use of
	 * a TURBOchannel (graphic) option.
	 */
	tc_slot_hand_fill = kn02_slot_hand_fill;
	tc_find_all_options();

}

mips_box_model_dep()
{
	int temp = string_to_int(prom_getenv("systype"));

	if (dec_cputype(temp) != 130) {
		dprintf("Unknown System type: 0x%x.  Assuming pmax-like\n",
			temp);
		return kn01_model_dep();
	}

	switch(dec_systype(temp)) {
	case 1:	/* DS3100 Pmax */
		return kn01_model_dep();
	case 2:	/* DS5000 3max */
		return kn02_model_dep();
	case 5:	/* DS5800 Isis */
	case 6:	/* DS5400 MIPSfair */
	default:
		dprintf("Unknown Box 0x%x.  Assuming pmax-like\n",
			dec_systype(temp));
		return kn01_model_dep();
	}
}
