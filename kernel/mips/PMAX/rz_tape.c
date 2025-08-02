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
 * $Log:	rz_tape.c,v $
 * Revision 2.4  91/02/05  17:44:01  mrt
 * 	Added author notices
 * 	[91/02/04  11:17:11  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:49  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:04  af
 * 	Mild attempt to get it working.  Needs lots of work still.
 * 	[90/12/03  23:34:27  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:44:11  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz_tape.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to TAPE-like devices.
 */

#include <mach/std_types.h>
#include <device/io_req.h>

#include <sys/ioctl.h>
#include <device/tape_status.h>

#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi_defs.h>
#include <mips/PMAX/rz.h>

sctape_open(sc, tgt)
{
	io_return_t	ret;
	io_req_t	ior;
	int		i;
	char		result[16];

	/*
	 * Dummy ior for proper sync purposes
	 */
	ior = (io_req_t) kalloc(sizeof(*ior));
	bzero(ior, sizeof(*ior));

	/* Exabyte wants a mode sense first */
	ret = scsi_mode_sense(sc, tgt, result, 16, ior);
{int *p = (int*)result;
printf("[modsns(%x): x%x x%x x%x x%x]", ret, p[0], p[1], p[2], p[3]);
}

	/* Try hard to do a mode select */
	for (i = 0; i < 5; i++) {
		ior->io_op = 0;
		ret = sctape_mode_select(sc, tgt, 0/*XXX*/, ior);
		if (ret == SCSI_RET_SUCCESS)
			break;
	}

	kfree(ior, sizeof(*ior));
	return ret;
}

sctape_strategy(ior)
	register io_req_t	ior;
{
	target_info_t  *tgt;
	register scsi_softc_t	*sc;
	scsi_ret_t      ret;
	register int    i = ior->io_unit;
	io_req_t	head, tail;

	sc = scsi_softc[controller(i)];
	tgt = sc->target[slave(i)];

	ior->io_next = 0;
	ior->io_prev = 0;

	i = splbio();
	if (head = tgt->ior) {
		/* Queue it up at the end of the list */
		if (tail = head->io_prev)
			tail->io_next = ior;
		else
			head->io_next = ior;
		head->io_prev = ior;	/* tail pointer */
	} else {
		/* Was empty, start operation */
		tgt->ior = ior;
		sctape_start( sc, tgt, FALSE);
	}
	splx(i);

	return D_SUCCESS;
}

sctape_start( sc, tgt, done)
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	boolean_t	done;
{
	io_req_t		head, ior = tgt->ior;
	scsi_ret_t		ret;

	if (done) {
		/* dequeue next one */
		head = ior;
		if (head->io_next) {
			ior = head->io_next;
			ior->io_prev = head->io_prev;
		} else
			ior = 0;
		tgt->ior = ior;

		iodone(head);

		if (ior == 0)
			return;
	}
	if (ior->io_op & IO_READ)
		ret = sctape_read( sc, tgt, ior );
	else
		ret = sctape_write( sc, tgt, ior );
}

sctape_get_status(sc, tgt, flavor, status, status_count)
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	switch (flavor) {
	case TAPE_STATUS: {
		struct tape_status *ts = (struct tape_status *) status;

		ts->mt_type		= MT_ISSCSI;
		ts->mt_hw_status	= 0;	/* XXX */
		ts->mt_error_status	= 0;	/* XXX */
		*status_count = TAPE_STATUS_COUNT;

		break;
	}
	/* U*x compat */
	case MTIOCGET: {
		struct mtget *g = (struct mtget *) status;

		bzero(g, sizeof(struct mtget));
		g->mt_type = 0x7;	/* Ultrix compat */
		*status_count = sizeof(struct mtget)/sizeof(int);
	}
	default:
		return D_INVALID_OPERATION;
	}
	return D_SUCCESS;
}

sctape_set_status(sc, tgt, flavor, status, status_count)
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	switch (flavor) {
	case TAPE_STATUS:
		break;
	/* U*x compat */
	case MTIOCTOP: {
		struct tape_params *mt = (struct tape_params *) status;

printf("[sctape_sstatus: %x %x %x]\n", flavor, mt->mt_operation, mt->mt_repeat_count);
#if 0
		switch (mt->mt_operation) {
		case MTWEOF:	/* write an end-of-file record */
			write_filemarks;
			break;
		case MTFSF:	/* forward space file */
			?space F?
			break;
		case MTBSF:	/* backward space file */
			?space F?
			break;
		case MTFSR:	/* forward space record */
			?space R?
			break;
		case MTBSR:	/* backward space record */
			?space R?
			break;
		case MTREW:	/* rewind */
			rewind;
			break;
		case MTOFFL:	/* rewind and put the drive offline */
			unload;
			break;
		case MTNOP:	/* no operation, sets status only */
			break;
		case MTCACHE:	/* enable controller cache */
			break;
		case MTNOCACHE:	/* disable controller cache */
			break;
		default:
			return D_INVALID_OPERATION;
		}
#endif
		break;
	}
	case MTIOCIEOT:
	case MTIOCEEOT:
	default:
		return D_INVALID_OPERATION;
	}
	return D_SUCCESS;
}

