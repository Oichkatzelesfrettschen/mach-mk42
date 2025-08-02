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
 * $Log:	pm_misc.c,v $
 * Revision 2.4  91/02/05  17:43:19  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:01  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:14:55  mrt]
 * 
 * Revision 2.3  90/12/05  23:33:18  af
 * 	Cleaned up and tested.
 * 	[90/12/03  23:31:05  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:45:19  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: pm_misc.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Driver for the VFB01/02 Mono/Color framebuffer (pmax)
 *	Hardware-independent operations, mostly shared with
 *	the CFB driver (see each individual function header).
 */


#include <cfb.h>
#include <fb.h>
#define	NPM	(NCFB+NFB)
#if	(NPM > 0)

#include <mach/vm_param.h>		/* PAGE_SIZE */

#include <mips/PMAX/screen_defs.h>
#include <mips/PMAX/pm_defs.h>

#include <device/device_types.h>

#include <mips/mips_cpu.h>

/* Hardware state */
pm_softc_t	pm_softc_data[NPM];

pm_softc_t*
pm_alloc(unit, cur, fb, pl)
	char		*cur;
	unsigned char	*fb;
	unsigned char	*pl;
{
	pm_softc_t     *pm = &pm_softc_data[unit];

	pm->cursor_registers	= cur;
	pm->framebuffer 	= fb;
	pm->plane_mask 		= pl;

	screen_attach(unit, (char *) pm);

	return pm;
}


/*
 * Routine to paint a char on a simple framebuffer.
 * This is common to the pm and cfb drivers.
 */
pm_char_paint(sc, c, row, col)
	screen_softc_t	sc;
{
	register int	incr;
	int		line_size;
	register unsigned char	*font, *bmap;
	pm_softc_t	*pm = (pm_softc_t*)sc->hw_state;

	/*
	 * Here are the magic numbers that drive the loops below:
	 *	incr		bytes between scanlines of the glyph
	 *	line_size	bytes in a row, using the system font
	 * Note a scanline is worth two screen lines
	 * This code has been optimized to avoid multiplications,
	 * and is therefore much less obvious than it could be.
	 */
	if (sc->flags & COLOR_SCREEN) {
		/*
		 * Color screen: 4 planes
		 *	incr --> 4 * (BytesPerLine * 2)
		 *	line_size = CharHeight * incr
		 */
		incr = ScanCols;
		col <<= 3;
		line_size = CharHeight << 10; /* x1024 */
	} else {
		/*
		 * B&W screen: 1 plane
		 *	incr --> 1 * (BytesPerLine * 2)
		 *	line_size = CharHeight * incr
		 */
		incr = ScanCols >> 2;
		line_size = CharHeight << 8; /* x256 */
	}
	bmap = pm->framebuffer + col + (row * line_size);
	font = &kfont_7x14[ (int)(c - ' ') * 15];
	if (sc->flags & MONO_SCREEN) {
		/*
		 * Unroll simple loops, take note of common cases
		 */
		if (sc->standout) {
#			define mv()	*bmap = ~*font++; bmap += incr;
			mv();mv();mv();mv();mv();mv();mv();mv();
			mv();mv();mv();mv();mv();mv();mv();
#			undef	mv
		} else if (c == ' ') {
#			define mv()	*bmap = 0; bmap += incr;
			mv();mv();mv();mv();mv();mv();mv();mv();
			mv();mv();mv();mv();mv();mv();mv();
#			undef	mv
		} else {
#			define mv()	*bmap = *font++; bmap += incr;
			mv();mv();mv();mv();mv();mv();mv();mv();
			mv();mv();mv();mv();mv();mv();mv();
#			undef	mv
		}
	} else {
		/*
		 * 4 color planes --> 2 words per font byte (scanlines!)
		 * In order to spread out the 8 bits of a glyph line over
		 * the 64 bits of a scanline use a simple vector multiply,
		 * taking 4 bits at a time to get the two resulting words
		 */
		static unsigned int spread[16] = {
			0x00000000, 0x00000001, 0x00000100, 0x00000101,
			0x00010000, 0x00010001, 0x00010100, 0x00010101,
			0x01000000, 0x01000001, 0x01000100, 0x01000101,
			0x01010000, 0x01010001, 0x01010100, 0x01010101,
		};
		register int	rev_video = sc->standout;
		register int	j;
		for (j = 0; j < CharHeight; j++) {
			register unsigned char c = *font++;
			if (rev_video) c = ~c;
			((int*)bmap)[0] = spread[ c & 0xf ];
			((int*)bmap)[1] = spread[ (c>>4) & 0xf ];
			bmap += incr;
		}
	}
}

/*
 * Delete the line at the given row.
 * This is common to the pm and cfb drivers.
 */
pm_remove_line(sc, row)
	screen_softc_t	sc;
	short row;
{
	int line;
	register unsigned char *framebuffer;
	register int *dest, *src;
	register int *end;
	register int temp0,temp1,temp2,temp3;
	register int i, scaninc, blockcnt;
	pm_softc_t	*pm = (pm_softc_t*)sc->hw_state;

	framebuffer = pm->framebuffer;

	/* Inner loop works 4 words at a time (writebuffer deep) */
#	define BlockSizeShift	4

	/*  To copy one (MONO) line, we need to iterate this many times */
#	define Blocks    (CharCols>>BlockSizeShift)

	/*  Skip this many bytes to get to the next line */
#	define Slop      (line/CharHeight - (blockcnt<<BlockSizeShift))

	if (sc->flags & MONO_SCREEN) {
		/* only need to copy first half of a scanline */
		blockcnt = Blocks;
		/* See comments in pm_char_paint() */
		line = CharHeight << 8; /* x256 */
	} else {
		/* 4 full scanlines */
		blockcnt = Blocks << 3;
		/* See comments in pm_char_paint() */
		line = CharHeight << 10; /* x1024 */
	}
	scaninc = Slop>>2;		/* pointers are int* */

	dest = (int *)(framebuffer + row * line);
	src  = (int *)((char*)dest + line);
	end  = (int *)(framebuffer + CharRows * line);
	while (src < end) {
		i = 0;
		do {
			temp0 = src[0];
			temp1 = src[1];
			temp2 = src[2];
			temp3 = src[3];
			dest[0] = temp0;
			dest[1] = temp1;
			dest[2] = temp2;
			dest[3] = temp3;
			dest += 4;
			src += 4;
			i++;
		} while (i < blockcnt);
		src += scaninc;
		dest += scaninc;
	}

	/* Now zero out the last line */
	bzero(framebuffer + (CharRows - 1) * line,  line);

	ascii_screen_rem_update(sc, row);
}


/*
 * Open a new blank line at the given row.
 * This is common to the pm and cfb drivers.
 */
pm_insert_line(sc,row)
	screen_softc_t	sc;
	short	row;
{
	int line;
	register unsigned char *framebuffer;
	register int *dest, *src;
	register int *end;
	register int temp0,temp1,temp2,temp3;
	register int i, scaninc, blockcnt;
	pm_softc_t	*pm = (pm_softc_t*)sc->hw_state;

	framebuffer = pm->framebuffer;

	/* See above for comments */
	if (sc->flags & MONO_SCREEN) {
		blockcnt = Blocks;
		line = CharHeight << 8; /* x256 */
	} else {
		blockcnt = Blocks << 3;
		line = CharHeight << 10; /* x1024 */
	}
	scaninc = Slop + ((2 * blockcnt) << BlockSizeShift);
	scaninc >>= 2;			/* pointers are int* */
	dest = (int *)(framebuffer + (CharRows - 1) * line);
	src  = (int *)((char*)dest - line);
	end  = (int *)(framebuffer + row * line);
	while (src >= end) {
		i = 0;
		do {
			temp0 = src[0];
			temp1 = src[1];
			temp2 = src[2];
			temp3 = src[3];
			dest[0] = temp0;
			dest[1] = temp1;
			dest[2] = temp2;
			dest[3] = temp3;
			dest += 4;
			src += 4;
			i++;
		} while (i < blockcnt);
		src -= scaninc;
		dest -= scaninc;
	}

	/* Now zero out the line being opened */
	bzero(framebuffer + row * line, line);

	ascii_screen_ins_update(sc, row);
}

#undef	Slop


/*
 * Initialize screen parameters in the
 * user-mapped descriptor.
 * This is common to various drivers.
 */
pm_init_screen_params(up)
	user_info_t	*up;
{
	up->max_row		= CharRows;
	up->max_col		= CharCols;
	up->max_x		= ScanCols;
	up->max_y		= ScanRows;
	up->max_cur_x		= ScanCols - 1;
	up->max_cur_y		= ScanRows - 1;
	up->min_cur_x		= -15;
	up->min_cur_y		= -15;

	up->version		= 11;

	up->mouse_threshold	= 4;	
	up->mouse_scale		= 2;

	up->dev_dep_2.pm.tablet_scale_x	= ((ScanCols - 1) * 1000) / 2200;
	up->dev_dep_2.pm.tablet_scale_y	= ((ScanRows - 1) * 1000) / 2200;
}

/*
 * Clear the screen
 * Used by both pm and cfb
 */
pm_clear_bitmap(sc)
	screen_softc_t	sc;
{
	pm_softc_t     *pm = (pm_softc_t *) sc->hw_state;

	bzero(	pm->framebuffer,
		1024 * ((sc->flags & MONO_SCREEN) ? 256 : 1024));

	/* clear ascii screenmap */
	ascii_screen_fill(sc, ' ');
}


/*
 * Size of the user-mapped structure
 * Used by both pm and cfb
 */
pm_mem_need()
{
	return USER_INFO_SIZE;
}

/*
 * Device-specific get status.
 * Used by cfb also.
 */
pm_get_status(sc, flavor, status, status_count)
	screen_softc_t	sc;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	if (flavor == SCREEN_GET_OFFSETS) {
		unsigned	*offs = (unsigned *) status;

		offs[0] = PM_SIZE(sc);		/* virtual size */
		offs[1] = 0;			/* offset of user_info_t */
		*status_count = 2;
		return D_SUCCESS;
	} else
		return D_INVALID_OPERATION;
}

/*
 * Driver-specific set status
 * Only partially used by cfb.
 */
pm_set_status(sc, flavor, status, status_count)
	screen_softc_t	sc;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
/* XXX spl */
	switch (flavor) {
	case SCREEN_ADJ_MAPPED_INFO: {
		unsigned	user_addr = *(unsigned *) status;
		user_info_t	*up = sc->up;

		/* Make it point to the event_queue, in user virtual */
		up->evque.events = (screen_event_t *)(user_addr +
			((char*)up->event_queue - (char*)up));

		/* Make it point to the point_track, in user virtual */
		up->evque.track = (screen_timed_point_t *)(user_addr +
			((char*)up->point_track - (char*)up));

		up->dev_dep_1.pm.planemask = (unsigned char *)(user_addr + USER_INFO_SIZE);

		up->dev_dep_1.pm.bitmap = up->dev_dep_1.pm.planemask + PMASK_SIZE;

		break;
	}

	case SCREEN_LOAD_CURSOR:
		dc503_load_cursor(sc->hw_state, (unsigned short*)status);
	    	break;

	case SCREEN_SET_CURSOR_COLOR: {
		pm_softc_t		*pm = (pm_softc_t*) sc->hw_state;

		bt478_cursor_color (pm->bt478, (cursor_color_t*) status);
		break;
	}
	     
	case SCREEN_SET_CMAP_ENTRY: {
		pm_softc_t		*pm = (pm_softc_t*) sc->hw_state;
		color_map_entry_t	*e = (color_map_entry_t*) status;

		if (e->index < 256)
			bt478_load_colormap_entry( pm->bt478, e->index, &e->value);
		break;
	}

	default:
		return D_INVALID_OPERATION;
	}
	return D_SUCCESS;
}


/*
 *-----------------------------------------------------------
 *	The rest of this file is stricly pmax-specific
 *-----------------------------------------------------------
 */

/*
 * Do what's needed when the X server exits
 */
pm_soft_reset(sc)
	screen_softc_t	sc;
{
	pm_softc_t	*pm = (pm_softc_t*) sc->hw_state;
	user_info_t	*up = sc->up;

	/*
	 * Restore params in mapped structure
	 */
	pm_init_screen_params(up);
	up->row = CharRows - 1;

	dc503_init(pm);

	if (sc->flags & MONO_SCREEN)
		bt478_init_bw_map(pm->bt478, pm->plane_mask);
	else
		bt478_init_color_map(pm->bt478, pm->plane_mask);
}

pm_init_screen(){}


/*
 * Map pages to user space
 */
pm_map_page(sc, off, prot)
	screen_softc_t	sc;
	unsigned	off;
	int		prot;
{
	int		bitmapsize;
	int		addr;
	pm_softc_t	*pm = (pm_softc_t *)sc->hw_state;

	bitmapsize = BITMAP_SIZE(sc);

#define	OFF0	USER_INFO_SIZE	 		/* user mapped info */
#define	OFF1	OFF0+PMASK_SIZE			/* plane mask register */
#define	OFF2	OFF1+bitmapsize			/* frame buffer mem */

	if (off < OFF0)
		addr = (int)K0SEG_TO_PHYS(sc->up);
	else
	if (off < OFF1) {
		addr = (int)pm->plane_mask;
		off -= OFF0;
	} else
	if (off < OFF2) {
		addr = (int)pm->framebuffer;
		off -= OFF1;
	} else 
		return D_INVALID_SIZE;	/* ??? */

	addr = mips_btop(addr + off);

	return (addr);
}

#endif	(NPM > 0)

