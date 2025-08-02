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
 * $Log:	conf.c,v $
 * Revision 2.7  91/02/14  14:42:13  mrt
 * 	Allow com driver and distinguish EtherLinkII from wd8003
 * 	[91/01/28  15:27:02  rvb]
 * 
 * Revision 2.6  91/02/05  17:16:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:42:38  mrt]
 * 
 * Revision 2.5  91/01/08  17:32:42  rpd
 * 	Support for get/set status on hd and fd.
 * 	Also fd has 64 minor devices per unit.
 * 	Switch wd8003 -> ns8390
 * 	[91/01/04  12:17:15  rvb]
 * 
 * Revision 2.4  90/10/01  14:23:02  jeffreyh
 * 	added wd8003 ethernet driver
 * 	[90/09/27  18:23:53  jeffreyh]
 * 
 * Revision 2.3  90/05/21  13:26:53  dbg
 * 	Add mouse, keyboard, IOPL devices.
 * 	[90/05/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:41:34  dbg
 * 	Add 3c501 under name 'et'.
 * 	[90/04/27            dbg]
 * 
 * 	Created.
 * 	[90/02/20            dbg]
 * 
 */

/*
 * Device switch for i386 AT bus.
 */

#include <device/conf.h>

extern int block_io_mmap();

extern int	timeopen(), timeclose(), timemmap();
#define	timename		"time"

#include <hd.h>
#if	NHD > 0
extern int	hdopen(), hdclose(), hdread(), hdwrite();
extern int	hdgetstat(), hdsetstat();
#define	hdname			"hd"
#endif	NHD > 0

#include <fd.h>
#if	NFD > 0
extern int	fdopen(), fdclose(), fdread(), fdwrite();
extern int	fdgetstat(), fdsetstat();
#define	fdname			"fd"
#endif	NFD > 0

#include <wt.h>
#if	NWT > 0
extern int	wtopen(), wtread(), wtwrite(), wtclose();
#define	wtname			"wt"
#endif	NWT > 0

#include <pc586.h>
#if	NPC586 > 0
extern int	pc586open(), pc586output(), pc586getstat(), pc586setstat(),
		pc586setinput();
#define	pc586name		"pc"
#endif NPC586 > 0

#include <ns8390.h>
#if	NNS8390 > 0
extern int	wd8003open(), eliiopen();
extern int	ns8390output(), ns8390getstat(), ns8390setstat(), 
		ns8390setinput();
#define	ns8390wdname		"wd"
#define	ns8390elname		"el"
#endif NNS8390 > 0

#include <at3c501.h>
#if	NAT3C501 > 0
extern int	at3c501open(), at3c501output(),
		at3c501getstat(), at3c501setstat(),
		at3c501setinput();
#define	at3c501name		"et"
#endif NAT3C501 > 0

extern int	kdopen(), kdclose(), kdread(), kdwrite();
extern int	kdgetstat(), kdsetstat(), kdportdeath(), kdmmap();
#define	kdname			"kd"

#include <com.h>
#if	NCOM > 0
extern int	comopen(), comclose(), comread(), comwrite();
extern int	comgetstat(), comsetstat(), comportdeath();
#define	comname			"com"
#endif	NCOM > 0

#include <qd.h>
#if	NQD > 0
extern int	qdopen(), qdclose(), qdread(), qdwrite(), qdioctl();
#define	qdname			"qd"
#endif	NQD > 0

#include <blit.h>
#if NBLIT > 0
extern int	blitopen(), blitclose(), blit_get_stat(), blitmmap();
#define	blitname		"blit"

extern int	mouseinit(), mouseopen(), mouseclose();
extern int	mouseioctl(), mouseselect(), mouseread();
#endif

extern int	kbdopen(), kbdclose(), kbdread();
extern int	kbdgetstat(), kbdsetstat();
#define	kbdname			"kbd"

extern int	mouseopen(), mouseclose(), mouseread();
#define	mousename		"mouse"

extern int	ioplopen(), ioplclose(), ioplmmap();
#define	ioplname		"iopl"


/*
 * List of devices - console must be at slot 0
 */
struct dev_ops	dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev */

	{ kdname,	kdopen,		kdclose,	kdread,
	  kdwrite,	kdgetstat,	kdsetstat,	kdmmap,
	  nodev,	nulldev,	kdportdeath },

	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev },

#if	NHD > 0
	{ hdname,	hdopen,		hdclose,	hdread,
	  hdwrite,	hdgetstat,	hdsetstat,	nulldev,
	  nodev,	nulldev,	nulldev,	16 },
#endif	NHD > 0

#if	NFD > 0
	{ fdname,	fdopen,		fdclose,	fdread,
	  fdwrite,	fdgetstat,	fdsetstat,	nulldev,
	  nodev,	nulldev,	nulldev,	64 },
#endif	NFD > 0

#if	NWT > 0
	{ wtname,	wtopen,		wtclose,	wtread,
	  wtwrite,	nulldev,	nulldev,	nulldev,
	  nodev,	nulldev,	nulldev },
#endif	NWT > 0

#if	NPC586 > 0
	{ pc586name,	pc586open,	nulldev,	nulldev,
	  pc586output,	pc586getstat,	pc586setstat,	nulldev,
	  pc586setinput,nulldev,	nulldev },
#endif

#if	NAT3C501 > 0
	{ at3c501name,	at3c501open,	nulldev,	nulldev,
	  at3c501output,at3c501getstat,	at3c501setstat,	nulldev,
	  at3c501setinput, nulldev,	nulldev },
#endif

#if	NNS8390 > 0
	{ ns8390wdname,	wd8003open,	nulldev,	nulldev,
	  ns8390output, ns8390getstat,	ns8390setstat,	nulldev,
	  ns8390setinput, nulldev,	nulldev },

	{ ns8390elname,	eliiopen,	nulldev,	nulldev,
	  ns8390output, ns8390getstat,	ns8390setstat,	nulldev,
	  ns8390setinput, nulldev,	nulldev },
#endif

#if	NCOM > 0
	{ comname,	comopen,	comclose,	comread,
	  comwrite,	comgetstat,	comsetstat,	nodev,
	  nodev,	nulldev,	comportdeath },
#endif

#if	NBLIT > 0
	{ blitname,	blitopen,	blitclose,	nodev,
	  nodev,	blit_get_stat,	nodev,		blitmmap,
	  nodev,	nodev,		nodev },
#endif

	{ mousename,	mouseopen,	mouseclose,	mouseread,
	  nodev,	nulldev,	nulldev,	nodev,
	  nodev,	nulldev,	nulldev },

	{ kbdname,	kbdopen,	kbdclose,	kbdread,
	  nodev,	kbdgetstat,	kbdsetstat,	nodev,
	  nodev,	nulldev,	nulldev },

	{ ioplname,	ioplopen,	ioplclose,	nodev,
	  nodev,	nodev,		nodev,		ioplmmap,
	  nodev,	nulldev,	nulldev },
};
int	dev_name_count = sizeof(dev_name_list)/sizeof(dev_name_list[0]);

/*
 * Indirect list.
 */
struct dev_indirect dev_indirect_list[] = {

	/* console */
	{ "console",	&dev_name_list[0],	0 }
};
int	dev_indirect_count = sizeof(dev_indirect_list)
				/ sizeof(dev_indirect_list[0]);
