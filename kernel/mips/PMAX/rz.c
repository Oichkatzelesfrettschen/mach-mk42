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
 * $Log:	rz.c,v $
 * Revision 2.5  91/02/05  17:43:37  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:27  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:22  mrt]
 * 
 * Revision 2.4  90/12/20  16:59:59  jeffreyh
 * 	Do not use minphys(), we do not need to trim requests.
 * 	[90/12/11            af]
 * 
 * Revision 2.3  90/12/05  23:33:46  af
 * 	Cut&retry for requests that exceed the HBA's dma capacity.
 * 	[90/12/03  23:32:38  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:31  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI side.
 */

/*
 * This file contains the code that is common to all scsi devices,
 * operations and/or behaviours specific to certain devices live
 * in the corresponding rz_mumble files.
 */

#include <scsi.h>

#include <mach/std_types.h>
#include <device/device_types.h>

#include <kern/time_out.h>
#include <device/io_req.h>
#define	await(event)	sleep(event,0)
extern	wakeup();

#include <mips/PMAX/scsi_defs.h>
#include <mips/PMAX/rz.h>

static boolean_t
rz_check(dev, p_sc, p_tgt)
	scsi_softc_t	**p_sc;
	target_info_t	**p_tgt;
{
	if (controller(dev) >= NSCSI ||
	    (*p_sc = scsi_softc[controller(dev)]) == 0)
		return FALSE;

	*p_tgt = (*p_sc)->target[slave(dev)];

	if (!*p_tgt ||
	    !((*p_tgt)->flags&TGT_ALIVE))
		return FALSE;
	return TRUE;
}

/*
 * Open routine
 *
 * On tapes and other devices might have to wait a bit for
 * the unit to come alive. The following patchable variable
 * takes this into account
 */
int rz_open_timeout = 20;/* seconds */

rz_open(dev, mode, ior)
	int		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	scsi_ret_t	ret;
	register int	i;

	if (!rz_check(dev, &sc, &tgt))
		return D_NO_SUCH_DEVICE;
/* XXX probe it again: might have installed a new device */

	if (scsi_debug)
		printf("opening %s%d..", (*tgt->dev_ops->driver_name)(TRUE), dev&0xff);

	/* XXX initialize anything that needs to */

	/*
	 * Bring the unit online, retrying if necessary.
	 * If the target is spinning up we wait for it.
	 */
	if ( ! (tgt->flags & TGT_ONLINE)) {
		io_req_t	ior = (io_req_t) kalloc(sizeof(*ior));
		
		bzero(ior, sizeof(*ior));
		for (i = 0; i < rz_open_timeout; i++) {

			ret = scsi_test_unit_ready(sc, tgt, ior);

			if (ret == SCSI_RET_SUCCESS)
				break;

			if (i == 5) printf("%s%d: %s\n", 
					   (*tgt->dev_ops->driver_name)(TRUE),
					   dev&0xff,
					   "Waiting to come online..");
			timeout(wakeup, tgt, hz);
			await(tgt);
		}
		kfree(ior, sizeof(*ior));
		if (i == rz_open_timeout)
			return D_DEVICE_DOWN;
	}
	/*
	 * Perform anything open-time special on the device
	 */
	if (tgt->dev_ops->open) {
		ret = (*tgt->dev_ops->open)(sc, tgt);
		if (ret != SCSI_RET_SUCCESS) {
			 printf("%s%d: open failed x%x\n",
			 (*tgt->dev_ops->driver_name)(TRUE), dev&0xff, ret);
			return ret;
		}
	}
	tgt->flags |= TGT_ONLINE;
	return 0;	/* XXX */
}

rz_close(dev)
	int		dev;
{
	/* nothing here today */
}

/* used as a null minphys cuz we have no limits */
static rz_nop(ior)
	io_req_t	ior;
{
}

rz_read(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	tgt = scsi_softc[controller(dev)]->target[slave(dev)];

	return block_io(tgt->dev_ops->strategy, rz_nop, ior);
}

rz_write(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	tgt = scsi_softc[controller(dev)]->target[slave(dev)];

	return block_io(tgt->dev_ops->strategy, rz_nop, ior);
}

rz_get_status(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;

	sc  = scsi_softc[controller(dev)];
	tgt = sc->target[slave(dev)];

	printf("rz_get_status: x%x x%x x%x x%x\n",
		dev, flavor, status, *status_count);
	return (*tgt->dev_ops->get_status)(sc, tgt, flavor, status, status_count);
}

rz_set_status(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;

	sc  = scsi_softc[controller(dev)];
	tgt = sc->target[slave(dev)];

	printf("rz_set_status: x%x x%x x%x x%x\n",
		dev, flavor, status, status_count);
	return (*tgt->dev_ops->set_status)(sc, tgt, flavor, status, status_count);
}

#if	0
rz_size(dev)
	int	dev;
{	
	return -1;
}

rz_reset(adaptor)
	int	adaptor;
{}
#endif

