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
 * $Log:	scsi.c,v $
 * Revision 2.6  91/02/05  17:44:35  mrt
 * 	Added author notices
 * 	[91/02/04  11:18:01  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:16:40  mrt]
 * 
 * Revision 2.5  90/12/05  23:34:36  af
 * 	Cleanups, still awaits SCSI-2 fixes.
 * 	[90/12/03  23:38:14  af]
 * 
 * Revision 2.3.1.1  90/11/01  03:38:17  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: scsi.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Middle layer of the SCSI driver: chip independent functions
 *	This file contains Controller and Device-independent functions
 */

#include <asc.h>
#include <sii.h>
#define	NSCSI	(NASC+NSII)
#if	NSCSI > 0


#include <mach/std_types.h>
#include <sys/types.h>
#include <device/io_req.h>

#include <mips/busses.h>
#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi2.h>
#include <mips/PMAX/scsi_defs.h>

/*
 *	Overall driver state
 */

target_info_t	scsi_target_data[NSCSI*8];	/* per target state */
scsi_softc_t	scsi_softc_data[NSCSI];		/* per HBA state */
scsi_softc_t	*scsi_softc[NSCSI];		/* quick access&checking */

/*
 * If a specific target should NOT be asked to go synchronous
 * then its bit in this bitmap should be set. Each SCSI controller
 * (Host Bus Adapter) can hold at most 8 targets --> use one
 * byte per controller.  A bit set means NO synchronous.
 * Patch with adb if necessary.
 */

unsigned char	scsi_no_synchronous_xfer[NSCSI];

/*
 * Allocation routines for state structures
 */
scsi_softc_t *
scsi_master_alloc()
{
	static char nmasters = 0;
	scsi_softc_t	*sc;

	if (nmasters < NSCSI) {
		sc = &scsi_softc_data[nmasters];
		scsi_softc[nmasters] = sc;
		sc->masterno = nmasters++;
		return sc;
	}
	return 0;
}

target_info_t *
scsi_slave_alloc(unit, slave, hw)
	char	*hw;
{
	target_info_t	*tgt;

	tgt = &scsi_target_data[(unit<<3) + slave];
	tgt->hw_state = hw;
	tgt->dev_ops = 0;	/* later */
	tgt->target_id = slave;
	tgt->flags = TGT_ALIVE;

	scsi_softc[unit]->target[slave] = tgt;
	return tgt;
}

/*
 * Device-specific operations are switched off this table
 */

extern char
		*scdisk_name(), *sctape_name(), *scprt_name(),
		*sccpu_name(), *scscn_name(), *scmem_name(),
		*scjb_name(), *sccomm_name();
extern scsi_ret_t
		scdisk_optimize(), sctape_optimize(), scprt_optimize(),
		sccpu_optimize(), scworm_optimize(), sccdrom_optimize(),
		scscn_optimize(), scmem_optimize(), scjb_optimize(),
		sccomm_optimize();
extern scsi_ret_t
		scdisk_open(), sctape_open();
extern int
		scdisk_strategy(), sctape_strategy();
extern int
		scdisk_start(), sctape_start();

extern io_return_t
		scdisk_set_status(), scdisk_get_status(),
		sctape_set_status(), sctape_get_status();

scsi_devsw_t	scsi_devsw[] = {
/* SCSI_DISK */		{ scdisk_name, scdisk_optimize,
			  scdisk_open, scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_TAPE */		{ sctape_name, sctape_optimize,
			  sctape_open, sctape_strategy, sctape_start,
			  sctape_get_status, sctape_set_status },
/* SCSI_PRINTER */	{ scprt_name, scprt_optimize, /*XXX*/},
/* SCSI_CPU */		{ sccpu_name, sccpu_optimize, /*XXX*/},
/* SCSI_WORM */		{ scdisk_name, scworm_optimize,
			  scdisk_open, scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
/* SCSI_CDROM */	{ scdisk_name, sccdrom_optimize,
			  scdisk_open, scdisk_strategy, scdisk_start,
			  scdisk_get_status, scdisk_set_status },
#ifdef	SCSI2
/* SCSI_SCANNER */	{ scscn_name, scscn_optimize, /*XXX*/ },
/* SCSI_MEMORY */	{ scmem_name, scmem_optimize, /*XXX*/ },
/* SCSI_J_BOX */	{ scjb_name, scjb_optimize, /*XXX*/ },
/* SCSI_COMM */		{ sccomm_name, sccomm_optimize, /*XXX*/ },
#endif	SCSI2
			0
};

/*
 * Slave routine:
 *	See if the slave description (controller, unit, ..)
 *	matches one of the slaves found during probe
 *
 * Implementation:
 *	Send out an ENQUIRY command to see what sort of device
 *	the slave is.
 * Notes:
 *	At this time the driver is fully functional and works
 *	off interrupts.
 * TODO:
 *	The SCSI2 spec says what exactly must happen: see F.2.3
 */
scsi_slave( ui, reg)
	struct bus_device	*ui;
	unsigned		reg;
{
	scsi_softc_t		*sc = scsi_softc[ui->ctlr];
	target_info_t		*tgt = sc->target[ui->unit];
	scsi_inquiry_data_t	*inq;
	int			ptype, scsi_std, s;

	if (!tgt || !(tgt->flags & TGT_ALIVE))
		return 0;

	/* Might have scanned already */
	if (tgt->dev_ops)
		goto out;

#ifdef	SCSI2
	This is what should happen:
	- for all LUNs 
		ENQUIRY
		scsi_verify_state (see)
		scsi_initialize (see)
#endif	SCSI2

	tgt->unit_no = ui->unit;	/* incorrect, but needed early */

	s = spl0();	/* we need interrupts */

	/*
	 * Ok, it is time to see what type of device this is,
	 * send an INQUIRY cmd and wait till done
	 */
	scsi_inquiry(sc, tgt, SCSI_INQ_STD_DATA);

	inq = (scsi_inquiry_data_t*)tgt->cmd_ptr;
	ptype = (inq->periph_type) & 0x1f; /* scsi2 safe */

	switch (ptype) {
	case SCSI_DISK :
	case SCSI_TAPE :
	case SCSI_PRINTER :
	case SCSI_CPU :
	case SCSI_WORM :
	case SCSI_CDROM :
#ifdef	SCSI2
	case SCSI_SCANNER :
	case SCSI_MEMORY :
	case SCSI_J_BOX :
	case SCSI_COMM :
/*	case SCSI_PREPRESS1 : reserved, really
	case SCSI_PREPRESS2 :	*/
#endif	SCSI2
		tgt->dev_ops = &scsi_devsw[ptype];
		break;
	default:
		printf("%s%d 'x%x' %s %d. ", ui->mi->name, ui->mi->unit,
			inq->periph_type,
		       "Unsupported device type at SCSI id", ui->unit);
		scsi_print_inquiry(inq, SCSI_INQ_STD_DATA, TRUE);
		tgt->flags = 0;
		return 0;
	}

	/*
	 * Tell the user we know this target, then see if we
	 * can be a bit smart about it.
	 */
	scsi_print_inquiry(inq, SCSI_INQ_STD_DATA, tgt->tgt_name);

#ifdef	SCSI2
	/*
	 * The above says if it currently behaves as a scsi2,
	 * however scsi1 might just be the default setting.
	 * The spec say that even if in scsi1 mode the target
	 * should answer to the full scsi2 inquiry spec.
	 */
	scsi_std = (inq->ansi == 2 || inq->response_fmt == 2) ? 2 : 1;
#if nosey
	if (scsi_std == 2) {
		unsigned char	supp_pages[256], i;
		scsi2_impl_opdef_page_t	*impl;

		scsi_inquiry(sc, tgt, SCSI_INQ_SUPP_PAGES);
		impl = (scsi2_impl_opdef_page_t	*)inq;
		npages = impl->page_len - 2;
		bcopy(impl->supp_opdef, supp_pages, npages);

		for (i = 0; i < npages; i++) {
			scsi_inquiry(sc, tgt, supp_pages[i]);
			scsi_print_inquiry(inq, supp_pages[i], 0);
		}
	}
#endif	nosey

	if (scsi_std == 2) {
		scsi2_impl_opdef_page_t	*impl;
		int i;

		scsi_inquiry(sc, tgt, SCSI_INQ_IMPL_OPDEF);
		impl = (scsi2_impl_opdef_page_t	*)inq;
		for (i = 0; i < impl->page_len - 2; i++)
			if (impl->supp_opdef[i] == SCSI2_OPDEF) {
				scsi_change_definition(sc, tgt, SCSI2_OPDEF);
				/* if success .. */
					tgt->flags |= TGT_SCSI_2_MODE;
				break;
			}
	}
#endif	SCSI2

	splx(s);
out:
	return (strcmp(ui->name, (*tgt->dev_ops->driver_name)(TRUE)) == 0);
}

#ifdef	SCSI2
scsi_verify_state(...)
{
verify_state: send test_unit_ready up to 3 times, each time it fails
(with check condition) send a requeste_sense. It is ok to get UNIT ATTENTION 
the first time only, NOT READY the second, only GOOD the last time.
If you get BUSY or RESERVATION CONFLICT retry.
}

scsi_initialize(...)
{

initialize: send start_unit with immed=0 (->disconnect), if fails
with check condition send requeste_sense and if "illegal request"
proceed anyways. Retry on BUSY.
Do a verify_state, then
disks:
	- mode_sense (current) if ANSI2 or needed by vendor (!!!!)
	  and if check-condition&illegal-request goto capacity
	- mode_sense (changeable)
	- if needed do a mode_select (yes, 512)
	- read_capacity
tapes:

}
#endif	SCSI2

/*
 * Attach routine:
 *	Fill in all the relevant per-slave data and make
 *	the slave operational.
 *
 * Implementation:
 *	Get target's status, start the unit and then
 *	switch off to device-specific functions to gather
 *	as much info as possible about the slave.
 */
scsi_attach(ui)
	register struct bus_device *ui;
{
	scsi_softc_t		*sc = scsi_softc[ui->mi->unit];
	target_info_t		*tgt = sc->target[ui->unit];
	int			s;

	printf(" (%s %s)", (*tgt->dev_ops->driver_name)(FALSE),tgt->tgt_name);

	s = spl0();

	/* sense return from inquiry */
	scsi_request_sense(sc, tgt, 0);

	/*
	 * Do this twice, certain targets need it
	 * XXX not for processors XXX
	 */
	scsi_start_unit(sc, tgt, SCSI_CMD_SS_START, 0);
	scsi_start_unit(sc, tgt, SCSI_CMD_SS_START, 0);

	/*
	 * See if it is up and about
	 */
	scsi_test_unit_ready(sc, tgt, 0);

	(*tgt->dev_ops->optimize)(sc, tgt);

	tgt->flags |= TGT_FULLY_PROBED;

	splx(s);

}

#endif	NSCSI > 0
