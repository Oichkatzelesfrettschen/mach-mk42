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
 * $Log:	pm_hdw.c,v $
 * Revision 2.8  91/02/14  14:35:10  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:42:39  af]
 * 
 * Revision 2.7  91/02/05  17:43:11  mrt
 * 	Added author notices
 * 	[91/02/04  11:15:55  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:43  mrt]
 * 
 * Revision 2.6  90/12/05  23:33:16  af
 * 	Cleanups.
 * 	[90/12/03  23:30:26  af]
 * 
 * Revision 2.4.1.1  90/11/01  03:45:08  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	And from code written by Robert V. Baron at CMU.
 * 	[90/09/03            af]
 */
/*
 *	File: pm_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Driver for the VFB01/02 Mono/Color framebuffer (pmax)
 *	Hardware-level operations.
 */

#include <bm.h>
#if	NBM>0

#include <sys/types.h>
#include <mips/busses.h>
#include <mips/mips_cpu.h>
#include <mips/PMAX/kn01.h>

#include <mips/PMAX/screen_defs.h>

#include <mips/PMAX/pm_defs.h>

#define	KN01_CSR_ADDR		PHYS_TO_K1SEG(KN01_SYS_CSR)
#define	KN01_FBUF_ADDR		PHYS_TO_K1SEG(KN01_PHYS_FBUF_START)
#define	KN01_PLM_ADDR		PHYS_TO_K1SEG(KN01_PHYS_COLMASK_START)
#define	KN01_BT478_ADDR		PHYS_TO_K1SEG(KN01_SYS_VDAC)
#define	KN01_DC503_ADDR		PHYS_TO_K1SEG(KN01_SYS_PCC)


/*
 * Definition of the driver for the auto-configuration program.
 */

int	pm_probe(), pm_attach(), pm_intr();

caddr_t	pm_std[] = { 0 };
struct	bus_device *pm_info[1];
struct	bus_driver pm_driver = 
        { pm_probe, 0, pm_attach, 0, pm_std, "pm", pm_info, };


/*
 * Probe/Attach functions
 */

pm_probe( /* reg, ui */)
{
	static probed_once = 0;
	if (!isa_pmax())
		return 0;
	if (check_memory(KN01_FBUF_ADDR, 0))
		return 0;
	if (probed_once++ > 1)
		printf("[mappable] ");
	return 1;
}

pm_attach(ui)
	struct bus_device *ui;
{
	int isa_mono = (*(volatile short *)KN01_CSR_ADDR) & KN01_CSR_MONO;

	printf(": %s%s",
		isa_mono ? "monochrome" : "color",
		" display");
}


/*
 * Interrupt routine
 */

pm_intr(unit,spllevel)
{
	/* this is the vertical retrace one */
	splx(spllevel);
	lk201_led(unit);
}


/*
 * Boot time initialization: must make device
 * usable as console asap.
 */
extern int
	pm_cons_init(), pm_soft_reset(),
	dc503_video_on(), dc503_video_off(),
	pm_char_paint(), dc503_pos_cursor(),
	pm_insert_line(), pm_remove_line(), pm_clear_bitmap(),
	pm_set_status(), pm_get_status();
	dc503_vretrace(), pm_map_page();

static	struct screen_switch pm_sw = {
	screen_noop,		/* graphic_open */
	pm_soft_reset,		/* graphic_close */
	pm_set_status,		/* set_status */
	pm_get_status,		/* set_status */
	pm_char_paint,		/* char_paint */
	dc503_pos_cursor,	/* pos_cursor */
	pm_insert_line,		/* insert_line */
	pm_remove_line,		/* remove_line */
	pm_clear_bitmap,	/* clear_bitmap */
	dc503_video_on,		/* video_on */
	dc503_video_off,	/* video_off */
	dc503_vretrace,		/* enable vert retrace intr */
	pm_map_page		/* map_page */
};

pm_cold_init(unit, up)
	user_info_t	*up;
{
	int isa_mono = (*(volatile short *)KN01_CSR_ADDR) & KN01_CSR_MONO;
	pm_softc_t	*pm;
	screen_softc_t	sc;

	pm_init_screen_params(up);

	sc = screen_up(unit, up);
	bcopy(&pm_sw, &sc->sw, sizeof(sc->sw));
	sc->flags |= isa_mono ? MONO_SCREEN : COLOR_SCREEN;

	pm = pm_alloc(unit, KN01_DC503_ADDR, KN01_FBUF_ADDR, KN01_PLM_ADDR);
	pm->bt478 = (bt478_regmap_t*)KN01_BT478_ADDR;

	screen_default_colors(up);

	dc503_init(pm);

	pm_soft_reset(sc);
}

#endif	NBM>0
