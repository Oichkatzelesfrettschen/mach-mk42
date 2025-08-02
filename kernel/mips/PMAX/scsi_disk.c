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
 * $Log:	scsi_disk.c,v $
 * Revision 2.7  91/02/05  17:45:49  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:35  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:21  mrt]
 * 
 * Revision 2.6  90/12/05  23:35:14  af
 * 	Use and prefer BSD/OSF labels.
 * 	[90/12/03  23:48:09  af]
 * 
 * Revision 2.4.1.1  90/11/01  03:39:58  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_disk.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for DISK devices.
 */

#include <mach/std_types.h>
#include <device/io_req.h>
#include <device/param.h>
#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi_defs.h>


char *scdisk_name(internal)
	boolean_t	internal;
{
	return internal ? "rz" : "disk";
}

int scdisk_optimize(sc, tgt)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
{
}

/*
 * SCSI commands partially specific to disks
 */
scdisk_read( sc, tgt, secno, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_read_t		*cmd;
	register unsigned	len;

	len = ior->io_count;
	if (len > sc->max_dma_data)
		len = sc->max_dma_data;

	cmd = (scsi_cmd_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ;
	cmd->scsi_cmd_lun_and_lba1 = (secno>>16)&SCSI_LBA_MASK;
	cmd->scsi_cmd_lba2 	   = (secno>> 8)&0xff;
	cmd->scsi_cmd_lba3 	   = (secno    )&0xff;
	cmd->scsi_cmd_xfer_len     = btodb(len);
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_READ;

	(*sc->go)(sc, tgt, sizeof(*cmd), len, FALSE);
}

scdisk_write( sc, tgt, secno, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_write_t	*cmd;
	unsigned		len;	/* in bytes */

	len = ior->io_count;
	if (len > sc->max_dma_data)
		len = sc->max_dma_data;

	cmd = (scsi_cmd_write_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_WRITE;
	cmd->scsi_cmd_lun_and_lba1 = (secno>>16)&SCSI_LBA_MASK;
	cmd->scsi_cmd_lba2 	   = (secno>> 8)&0xff;
	cmd->scsi_cmd_lba3 	   = (secno    )&0xff;
	cmd->scsi_cmd_xfer_len     = btodb(len);
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_WRITE;

	(*sc->go)(sc, tgt, sizeof(*cmd), 0, FALSE);
}


scdisk_mode_select( sc, tgt, lbn, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	register int 		lbn;
	io_req_t		ior;
{
	scsi_cmd_mode_select_t	*cmd;
	scsi_mode_select_param_t	*parm;
	register int			offs;

	bzero(tgt->cmd_ptr, sizeof(*cmd) + sizeof(*parm));
	cmd = (scsi_cmd_mode_select_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MODE_SELECT;
	cmd->scsi_cmd_lun_and_lba1 = SCSI_CMD_MSL_PF;	/* XXX only if... */
	cmd->scsi_cmd_xfer_len = sizeof(scsi_mode_select_param_t);/* no vuq */

	parm = (scsi_mode_select_param_t*) (cmd + 1);
	parm->desc_len = 8;
	/* this really is the LBN */
	parm->descs[0].reclen1 = (lbn>>16)&0xff;
	parm->descs[0].reclen2 = (lbn>> 8)&0xff;
	parm->descs[0].reclen3 = (lbn    )&0xff;

	tgt->cur_cmd = SCSI_CMD_MODE_SELECT;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd) + sizeof(*parm), 0, ior);

	return tgt->done;
}

/*
 * SCSI commands fully specific to disks
 */
scsi_read_capacity( sc, tgt, lbn, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	int			lbn;
	io_req_t		ior;
{
	scsi_cmd_read_capacity_t	*cmd;

	bzero(tgt->cmd_ptr, sizeof(*cmd));
	cmd = (scsi_cmd_read_capacity_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_CAPACITY;
	/* all zeroes, unless... */
	if (lbn) {
		cmd->scsi_cmd_rcap_flags = SCSI_CMD_RCAP_PMI;
		cmd->scsi_cmd_lba1 = (lbn>>24);
		cmd->scsi_cmd_lba2 = (lbn>>16)&0xff;
		cmd->scsi_cmd_lba3 = (lbn>> 8)&0xff;
		cmd->scsi_cmd_lba4 = (lbn    )&0xff;
	}
	
	tgt->cur_cmd = SCSI_CMD_READ_CAPACITY;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), sizeof(scsi_rcap_data_t),ior);

	return tgt->done;
}

#ifdef	notyet
scsi_rezero_unit		group 0
scsi_format_unit
scsi_reassign_blocks
scsi_seek
scsi_reserve
scsi_release
scsi_medium_removal

scsi_long_read			group 1
scsi_long_write
scsi_long_seek
scsi_write_verify
scsi_verify
scsi_search high equal low
scsi_set_limits
#endif

#ifdef	SCSI2
scsi_lock_cache
scsi_prefetch
scsi_read_defect_data
scsi_sync_cache
scsi_write_same
#endif	SCSI2
