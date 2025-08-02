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
 * $Log:	scsi_tape.c,v $
 * Revision 2.6  91/02/05  17:46:24  mrt
 * 	Added author notices
 * 	[91/02/04  11:20:18  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:19:07  mrt]
 * 
 * Revision 2.5  90/12/05  23:35:49  af
 * 	Mild attempt to get it working.  Actually wrote and read
 * 	back a tape.
 * 	[90/12/03  23:49:49  af]
 * 
 * Revision 2.3.1.1  90/11/01  03:40:30  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_tape.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for SEQUENTIAL ACCESS devices.
 */

#include <mach/std_types.h>
#include <device/io_req.h>
#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi_defs.h>

char *sctape_name(internal)
	boolean_t	internal;
{
	return internal ? "tz" : "tape";
}

sctape_optimize(sc, tgt)
	scsi_softc_t		*sc;
	target_info_t		*tgt;
{
	register int 	i;
	char		result[6];

	/* Some (DEC) tapes want to send you the self-test results */
	for (i = 0; i < 10; i++) {
		if (scsi_receive_diag( sc, tgt, result, sizeof(result), 0)
		    == SCSI_RET_SUCCESS)
			break;
	}
printf("[tape_rcvdiag: after %d, x%x x%x x%x x%x x%x x%x]\n", i+1,
result[0], result[1], result[2], result[3], result[4], result[5]);

}

/*
 * SCSI commands specific to sequential access devices
 */
sctape_mode_select( sc, tgt, ttype, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	register int 		ttype;
	io_req_t		ior;
{
	scsi_cmd_mode_select_t	*cmd;
	scsi_mode_select_param_t	*parm;
	register int			offs;
	unsigned char			*vuqe;

#define	VUQE_SIZE	2
	if (ttype)
		gimmeabreak();	/* XXXX non tk50 tape XXXX */

	bzero(tgt->cmd_ptr, sizeof(*cmd) + 2 * sizeof(*parm));
	cmd = (scsi_cmd_mode_select_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MODE_SELECT;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_xfer_len = sizeof(scsi_mode_select_param_t) + VUQE_SIZE;

	parm = (scsi_mode_select_param_t*) (cmd + 1);
	parm->buffer_mode = 1;
	parm->desc_len = 8;

	/* vuqe for tk50 */
	vuqe = (unsigned char*)(parm+1);
	vuqe[0] = 1;
	vuqe[1] = 0x47;	/* nof=7, notimo=1 */


	tgt->cur_cmd = SCSI_CMD_MODE_SELECT;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd) + sizeof(*parm) + VUQE_SIZE, 0, ior);

	return tgt->done;
#undef	VUQE_SIZE
}

sctape_read( sc, tgt, ior)
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
	cmd->scsi_cmd_lun_and_lba1 = 0; /**/
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_READ;

	(*sc->go)(sc, tgt, sizeof(*cmd), len, FALSE);
}

sctape_write( sc, tgt, ior)
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
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_WRITE;

	(*sc->go)(sc, tgt, sizeof(*cmd) + len, 0, FALSE);
}


#if 0
scsi_rewind
scsi_read_block_limits
sctape_read
sctape_write
scsi_track_select
scsi_read_reverse
scsi_write_filemarks
scsi_space
sctape_verify
scsi_recover_buffered_data
scsi_erase
#endif

#ifdef	SCSI2
scsi_locate
scsi_read_position
#endif	SCSI2
