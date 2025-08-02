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
 * $Log:	conf.c,v $
 * Revision 2.8  91/02/05  17:47:29  mrt
 * 	Added author notices
 * 	[91/02/04  11:21:26  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:24:58  mrt]
 * 
 * Revision 2.7  90/12/05  23:37:09  af
 * 	Mods for new, copyright free PMAX drivers.
 * 	Added mapped SCSI.  
 * 	SCSI tape "tz" just has a different name now
 * 	(for backward compat), but it really is the
 * 	same as the disk "rz" one.
 * 	[90/12/03  23:02:10  af]
 * 
 * Revision 2.5  90/09/10  15:00:34  rpd
 * 	Made se_name be "se" and SE_name be "SE".
 * 	[90/09/10            rpd]
 * 
 * Revision 2.4  90/09/09  23:20:57  rpd
 * 	Added mapped ether device, kept the old one for now.
 * 	[90/08/30            af]
 * 
 * Revision 2.3  89/12/08  19:47:38  rwd
 * 	Bitmap screen is mapped via the mouse, with the console's
 * 	major. Yeech.
 * 	[89/12/05  02:22:14  af]
 * 
 * Revision 2.2  89/11/29  14:12:48  af
 * 	Got ether to work, with some name changes.
 * 	[89/11/26  10:31:55  af]
 * 
 * 	Created.
 * 	[89/10/05            af]
 * 
 */
/*
 *	File: conf.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/89
 *
 *	Device switch for MIPS.
 *
 */

#include <device/conf.h>

extern int block_io_mmap();

#include <ts.h>
#if NTS > 0
int	tsopen(),tsclose(),/*tsstrategy()*/,tsread(),tswrite();
int	tsioctl(),tssetstat(),/*tsdump()*/,tsreset();
#define tsname "ts"
#endif


#include <tthu.h>
#if NTTHU > 0
int	tthopen(),tthclose(),/*tthstrategy(),*/tthread(),tthwrite();
int	tthioctl(), tthsetstat();
#define tthname "tt"
#endif

#include <rd.h>
#if NRD > 0
int	rdopen(),/*rdstrategy(),*/rdread(),rdwrite(),
	/*rddump(),*/rdioctl(),/*rdsize(),*/rdsetstat();
#define rdname "rd"
#endif

#include <dkip.h>
#if NDKIP > 0
int	dkipopen()/*,dkipstrategy()*/,dkipread(),dkipwrite()/*,dkipdump()*/;
int	dkipioctl()/*,dkipsize()*/;
#define dkname "ip"
#endif

/* SCSI (always here since driver is dynamic) */
int     rz_open(), rz_close(), rz_read(), rz_write();
int     rz_get_status(), rz_set_status();
#define rzname "rz"

/* Mapped SCSI (ditto) */
int     RZ_open(), RZ_close(), RZ_mmap(), RZ_portdeath(),
	RZ_set_status(), RZ_get_status();
#define RZname "RZ"

#define tzname "tz"

/* Time device (mappable device) */

extern int timeopen(),timeclose(),timemmap();
#define timename	"time"

/* Char devices */

int	dz_open(),dz_close(),dz_read(),dz_write(),
	dz_get_status(), dz_set_status(), dz_portdeath();
int	screen_mmap();
#define	cons_name		"sl"

#include <cp.h>
#if NCP > 0
int	cpopen(),cpclose(),cpread(),cpwrite(),cpioctl(),
	/*cpstop(),*/ cpsetstat(), cpreset(), cpportdeath();
/*extern struct	tty cp_tty[];*/
#define cpname "cp"
#endif

int	se_open(), se_output(), se_get_status(), se_set_status(),
	se_setinput(), se_restart();
#define se_name "SE"

int	SE_open(), SE_close(), SE_mmap(), SE_portdeath(),
	SE_get_status(), SE_set_status();
#define SE_name "se"

/*
 * List of devices - console must be at slot 0
 */
struct dev_ops	dev_name_list[] =
{
	/*name,		open,		close,		read,
	  write,	getstat,	setstat,	mmap,
	  async_in,	reset,		port_death,	subdev */

	{ cons_name,	dz_open,	dz_close,	dz_read,
	  dz_write,	dz_get_status,	dz_set_status,	screen_mmap,
	  nodev,	nulldev,	dz_portdeath, },

	{ timename,	timeopen,	timeclose,	nulldev,
	  nulldev,	nulldev,	nulldev,	timemmap,
	  nodev,	nulldev,	nulldev },

#if	NTS > 0
	{ tsname,	tsopen,		tsclose,	tsread,
	  tswrite,	tsioctl,	tssetstat,	nodev,
	  nodev,	tsreset,	nulldev,	1 },
#endif

#if	NTTHU > 0
	{ tthname,	tthopen,	tthclose,	tthread,
	  tthwrite,	tthioctl,	tthsetstat,	nodev,
	  nodev,	nulldev,	nulldev,	8 },
#endif

#if	NRD > 0
	{ rdname,	rdopen,		nulldev,	rdread,
	  rdwrite,	rdioctl,	rdsetstat,	nodev,
	  nodev,	nulldev,	nulldev,	0 },
#endif

#if	NDKIP > 0
	{ dkipname,	dkipopen,	nulldev,	dkipread,
	  dkipwrite,	dkipioctl,	nulldev,	block_io_mmap,
	  nodev,	nulldev,	nulldev,	8 },
#endif

	{ rzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	8 },

	{ RZname,	RZ_open,	RZ_close,	nulldev,
	  nulldev,	RZ_get_status,	RZ_set_status,	RZ_mmap,
	  nodev,	nulldev,	RZ_portdeath, },

	{ tzname,	rz_open,	rz_close,	rz_read,
	  rz_write,	rz_get_status,	rz_set_status,	nodev,
	  nodev,	nulldev,	nulldev,	8 },

#if	NCP > 0
	{ cpname,	cpopen,		cpclose,	cpread,
	  cpwrite,	cpioctl,	cpsetstat,	nodev,
	  nodev,	cpreset,	cpportdeath,	16 },
#endif

	{ SE_name,	SE_open,	SE_close,	nulldev,
	  nulldev,	SE_get_status,	SE_set_status,	SE_mmap,
	  nodev,	nulldev,	SE_portdeath, },

	{ se_name,	se_open,	nulldev,	nodev,
	  se_output,	se_get_status,	se_set_status,	nodev,
	  se_setinput,	se_restart,	nulldev },

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
				/sizeof(dev_indirect_list[0]);
