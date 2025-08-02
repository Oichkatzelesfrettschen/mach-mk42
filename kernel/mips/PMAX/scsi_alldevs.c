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
 * $Log:	scsi_alldevs.c,v $
 * Revision 2.4  91/02/05  17:45:27  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:11  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:17:51  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:58  af
 * 	Turned debugging off.
 * 	[90/12/03  23:46:29  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:39:35  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_alldevs.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *	This file contains code for SCSI commands defined for all device types.
 */

#include <mach/std_types.h>
#include <sys/types.h>
#include <kern/sched_prim.h>
#include <device/io_req.h>

#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi2.h>
#include <mips/PMAX/scsi_defs.h>

/*
 * Utilities
 */
void scsi_go_and_wait(sc, tgt, insize, outsize, ior)
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	io_req_t		ior;
{
	tgt->ior = ior;

	(*sc->go)(sc, tgt, insize, outsize, ior==0);

	if (ior)
		iowait(ior);
	else
		while (tgt->done == SCSI_RET_IN_PROGRESS);
}

/*
 * INQUIRY (Almost mandatory)
 */
scsi_inquiry( sc, tgt, pagecode)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	int			pagecode;
{
	scsi_cmd_inquiry_t	*cmd;

retry:
	cmd = (scsi_cmd_inquiry_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_INQUIRY;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba3 = 0;
	cmd->scsi_cmd_xfer_len = 0xff;	/* max len always */
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
#ifdef	SCSI2
	if (pagecode != SCSI_INQ_STD_DATA) {
		cmd->scsi_cmd_lun_and_lba1 |= SCSI_CMD_INQ_EVPD;
		cmd->scsi_cmd_page_code = pagecode;
	} else
#endif	SCSI2
		cmd->scsi_cmd_page_code = 0;

	tgt->cur_cmd = SCSI_CMD_INQUIRY;

	/*
	 * Note: this is sent when we do not know much about the
	 * target, so we do not put an identify message upfront
	 */
	(*sc->go)(sc, tgt, sizeof(*cmd), 0xff, TRUE);

	/*
	 * This spin loop is because we are called at autoconf
	 * time where we cannot thread_block(). Sigh.
	 */
	while (tgt->done == SCSI_RET_IN_PROGRESS) ;
	if (tgt->done == SCSI_RET_RETRY)	/* sync negotiation ? */
		goto retry;
}

scsi_print_inquiry( inq, pagecode, result)
	scsi2_inquiry_data_t	*inq;
	char			*result;
{
	static char *periph_names[10] = {
		"disk", "tape", "printer", "processor", "WORM-disk",
		"CD-ROM", "scanner", "memory", "jukebox", "communication"
	};
	static char *periph_state[4] = {
		"online", "offline", "?", "absent"
	};

	char dev[SCSI_TARGET_NAME_LEN], *devname;
	register int i, j = 0;

	if (pagecode != SCSI_INQ_STD_DATA)
		return;

	devname = result ? result : dev;

	if (!result) {
		printf("\n\t%s %s%s (%s %x)",
			(inq->rmb) ? "" : "non-", "removable SCSI",
			(inq->periph_type > 10) ?
				"?device?" : periph_names[inq->periph_type],
			periph_state[inq->periph_qual & 0x3],
			inq->device_type);
		printf("\n\t%s%s%s",
			inq->iso ? "ISO-compliant, " : "",
			inq->ecma ? "ECMA-compliant, " : "",
			inq->ansi ? "ANSI-compliant, " : "");
		if (inq->ansi)
			printf("%s%d, ", "SCSI-", inq->ansi);
		if (inq->response_fmt == 2)
			printf("%s%s%s%s%s%s%s%s%s%s%s", "Supports: ",
			inq->aenc ? "AENC, " : "",
			inq->trmIOP ? "TrmIOP, " : "",
			inq->RelAdr ? "RelAdr, " : "",
			inq->Wbus32 ? "32 bit xfers, " : "",
			inq->Wbus16 ? "16 bis xfers, " : "",
			inq->Sync ? "Sync xfers, " : "",
			inq->Linked ? "Linked cmds, " : "",
			inq->CmdQue ? "Tagged cmd queues, " : "",
			inq->SftRe ? "Soft" : "Hard", " RESET, ");
	}

	for (i = 0; i < 8; i++)
		if (inq->vendor_id[i] != ' ')
			devname[j++] = inq->vendor_id[i];
	devname[j++] = ' ';
	for (i = 0; i < 16; i++)
		if (inq->product_id[i] != ' ')
			devname[j++] = inq->product_id[i];
	devname[j++] = ' ';
	for (i = 0; i < 4; i++)
		if (inq->product_rev[i] != ' ')
			devname[j++] = inq->product_rev[i];
#if unsafe
	devname[j++] = ' ';
	for (i = 0; i < 8; i++)
		if (inq->vendor_uqe[i] != ' ')
			devname[j++] = inq->vendor_uqe[i];
#endif
	devname[j] = 0;

	if (!result)
		printf("(%s, %s%s)\n", devname, "SCSI ",
			(inq->periph_type > 10) ?
				"?device?" : periph_names[inq->periph_type]);
}

/*
 * REQUESTE SENSE (Mandatory, All)
 */

scsi_request_sense( sc, tgt, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_request_sense_t *cmd;
	unsigned char   code;
	register int	offs;

	cmd = (scsi_cmd_request_sense_t *) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_REQUEST_SENSE;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = 0;
	cmd->scsi_cmd_allocation_length = 0xff;	/* max len always */
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_REQUEST_SENSE;

	scsi_go_and_wait (sc, tgt, sizeof(*cmd), 0xff, ior);

	code = *tgt->cmd_ptr;
	if (code & 0xf) {
		static int      user_warned = 0;
		if (!user_warned++)
			printf("%s%x\n", "REQ_SNS: unk vendor unique code ",
			       code);
		return TRUE;	/* and good luck */
	} else {
		code = tgt->cmd_ptr[2];

		switch (code & 0xf) {
		case SCSI_SNS_NOSENSE:
			return TRUE;
			break;
		case SCSI_SNS_RECOVERED:
			scsi_error(tgt, SCSI_ERR_BAD | SCSI_ERR_SENSE, code);
			return TRUE;
			break;
		case SCSI_SNS_UNIT_ATN:
			scsi_error(tgt, SCSI_ERR_SENSE, code);
			return FALSE;
			break;
		default:
/* e.g.
		case SCSI_SNS_NOTREADY:
		case SCSI_SNS_MEDIUM_ERR:
		case SCSI_SNS_HW_ERR:
		case SCSI_SNS_ILLEGAL_REQ:
		case SCSI_SNS_PROTECT:
*/
			scsi_error(tgt, SCSI_ERR_GRAVE|SCSI_ERR_SENSE, code);
			return FALSE;
			break;
		}
	}
}

/*
 * START STOP UNIT (Optional, disk prin work rom tape[load/unload])
 */
scsi_start_unit( sc, tgt, ss, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_start_t	*cmd;
	register int 		offs;

	cmd = (scsi_cmd_start_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_START_STOP_UNIT;
	cmd->scsi_cmd_lun_and_lba1 = SCSI_CMD_SS_IMMED;/* 0 won't work ? */
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = 0;
	cmd->scsi_cmd_ss_flags = ss;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_START_STOP_UNIT;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), 0, ior);
}

/*
 * TEST UNIT READY (Optional, All)
 * Note: this is where we do the synch negotiation at autoconf
 */
scsi_test_unit_ready( sc, tgt, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_test_unit_ready_t	*cmd;

	cmd = (scsi_cmd_test_unit_ready_t*) (tgt->cmd_ptr + 1);
	*(tgt->cmd_ptr) = SCSI_IDENTIFY;
	cmd->scsi_cmd_code = SCSI_CMD_TEST_UNIT_READY;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = 0;
	cmd->scsi_cmd_ss_flags = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_TEST_UNIT_READY;

	scsi_go_and_wait(sc, tgt, 1 + sizeof(*cmd), 0, ior);

	return tgt->done;
}

/*
 * RECEIVE DIAGNOSTIC RESULTS (Optional, All)
 */
scsi_receive_diag( sc, tgt, result, result_len, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*result;
	io_req_t		ior;
{
	scsi_cmd_receive_diag_t	*cmd;
	int			off = 0;

	if (ior == 0) {
		off = 1;
		*(tgt->cmd_ptr) = SCSI_IDENTIFY;
	}
	cmd = (scsi_cmd_receive_diag_t*) (tgt->cmd_ptr+off);
	cmd->scsi_cmd_code = SCSI_CMD_RECEIVE_DIAG_RESULTS;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = result_len >> 8 & 0xff;
	cmd->scsi_cmd_xfer_len = result_len & 0xff;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_RECEIVE_DIAG_RESULTS;

	scsi_go_and_wait(sc, tgt, off+sizeof(*cmd), result_len, ior);

	bcopy(tgt->cmd_ptr, result, result_len);

	return tgt->done;
}


scsi_mode_sense( sc, tgt, result, len, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*result;
	int			len;
	io_req_t		ior;
{
	scsi_cmd_mode_sense_t	*cmd;

	cmd = (scsi_cmd_mode_sense_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MODE_SENSE;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = 0;
	cmd->scsi_cmd_xfer_len = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_MODE_SENSE;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), len, ior);

	bcopy(tgt->cmd_ptr, result, len);

	return tgt->done;
}

#ifdef	unnecessary

/*
 * COPY (Optional, All)
 */
scsi_copy( sc, tgt, params, params_len, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*params;
	io_req_t		ior;
{
	scsi_cmd_copy_t	*cmd;

	cmd = (scsi_cmd_copy_t*) (tgt->cmd_ptr;
	cmd->scsi_cmd_code = SCSI_CMD_COPY;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 = params_len>>16 & 0xff;
	cmd->scsi_cmd_lba3 = params_len >> 8 & 0xff;
	cmd->scsi_cmd_xfer_len = params_len & 0xff;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	bcopy(params, cmd + 1, params_len);

	tgt->cur_cmd = SCSI_CMD_COPY;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd) + params_len, 0, ior);
}

/*
 * SEND DIAGNOSTIC (Optional, All)
 */
scsi_send_diag( sc, tgt, flags, params, params_len, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*params;
	io_req_t		ior;
{
	scsi_cmd_send_diag_t	*cmd;

	cmd = (scsi_cmd_send_diag_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_SEND_DIAGNOSTICS;
	cmd->scsi_cmd_lun_and_lba1 = flags & 0x7;
	cmd->scsi_cmd_lba2 = 0;
	cmd->scsi_cmd_lba3 = params_len >> 8 & 0xff;
	cmd->scsi_cmd_xfer_len = params_len & 0xff;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	bcopy(params, cmd + 1, params_len);

	tgt->cur_cmd = SCSI_CMD_SEND_DIAGNOSTICS;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), 0, ior);
}

/*
 * COMPARE (Optional, All)
 */
scsi_compare( sc, tgt, params, params_len, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*params;
	io_req_t		ior;
{
	scsi_cmd_compare_t	*cmd;

	cmd = (scsi_cmd_compare_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_COMPARE;
	cmd->scsi_cmd_lun_and_relbit = 0;
	cmd->scsi_cmd_lba1 = 0;
	cmd->scsi_cmd_1_paraml1 = params_len >> 16 & 0xff;
	cmd->scsi_cmd_1_paraml2 = params_len >> 8 & 0xff;
	cmd->scsi_cmd_1_paraml3 = params_len & 0xff;
	cmd->scsi_cmd_xxx = 0;
	cmd->scsi_cmd_xfer_len_1 = 0;
	cmd->scsi_cmd_xfer_len_2 = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	bcopy(params, cmd + 1, params_len);

	tgt->cur_cmd = SCSI_CMD_COMPARE;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), 0, ior);
}

/*
 * COPY AND VERIFY (Optional, All)
 */
scsi_copy_and_verify( sc, tgt, params, params_len, bytchk, ior)
	register scsi_softc_t	*sc;
	register target_info_t	*tgt;
	char			*params;
	io_req_t		ior;
{
	scsi_cmd_compare_t	*cmd;

	cmd = (scsi_cmd_compare_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_COMPARE;
	cmd->scsi_cmd_lun_and_relbit = bytchk ? SCSI_CMD_CPY_BYTCHK : 0;
	cmd->scsi_cmd_lba1 = 0;
	cmd->scsi_cmd_1_paraml1 = params_len >> 16 & 0xff;
	cmd->scsi_cmd_1_paraml2 = params_len >> 8 & 0xff;
	cmd->scsi_cmd_1_paraml3 = params_len & 0xff;
	cmd->scsi_cmd_xxx = 0;
	cmd->scsi_cmd_xfer_len_1 = 0;
	cmd->scsi_cmd_xfer_len_2 = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	bcopy(params, cmd + 1, params_len);

	tgt->cur_cmd = SCSI_CMD_COMPARE;

	scsi_go_and_wait(sc, tgt, sizeof(*cmd), 0, ior);
}

#endif

#ifdef	SCSI2
scsi_change_definition
scsi_log_select
scsi_log_sense
scsi_long_mode_select
scsi_read_buffer
scsi_write_buffer
#endif	SCSI2

/*
 * Warn user of some device error
 */
int	scsi_debug = 0;

void
scsi_error( tgt, code, info)
	target_info_t	*tgt;
	unsigned	code;
	unsigned	info;
{
	char		unit;
	char		*msg, *cmd;
	scsi2_status_byte_t	status;

	if (scsi_debug)
		code |= SCSI_ERR_GRAVE;

	if (tgt)
		unit = tgt->unit_no + '0';
	else
		unit = '?';


	switch (SCSI_ERR_CLASS(code)) {
	case SCSI_ERR_STATUS:
		cmd = "Bad status return";
		status.bits = info;
		switch (status.st.scsi_status_code) {
		case SCSI_ST_GOOD:
		case SCSI_ST_CONDITION_MET:
		case SCSI_ST_INT_GOOD:
		case SCSI_ST_INT_MET:
			return;	/* all is fine */
		case SCSI_ST_CHECK_CONDITION:
			msg = "Check condition"; break;
		case SCSI_ST_RES_CONFLICT:
			msg = "Reservation conflict"; break;
		case SCSI_ST_BUSY:
			msg = "Target busy"; break;
		case SCSI_ST2_QUEUE_FULL:
			msg = "Queue full"; break;
		case SCSI_ST2_CMD_TERMINATED:
			msg = "Command terminated"; break;
		default:
			goto unknown;
		}
		break;
	case SCSI_ERR_SENSE:
		cmd = "Sensed a";
		switch (info & 0xf) {
		case SCSI_SNS_RECOVERED:
			msg = "Recovered"; break;
		case SCSI_SNS_NOTREADY:
			msg = "Unit not ready"; break;
		case SCSI_SNS_MEDIUM_ERR:
			msg = "Medium"; break;
		case SCSI_SNS_HW_ERR:
			msg = "Hardware failure"; break;
		case SCSI_SNS_ILLEGAL_REQ:
			msg = "Illegal request"; break;
		case SCSI_SNS_PROTECT:
			msg = "Protection"; break;
		case SCSI_SNS_UNIT_ATN:
			msg = "Unit Attention Condition"; break;
		default:
unknown:
			msg = "Strange"; break;
		}
		break;
	case SCSI_ERR_MSEL:
		cmd = "Mode select broken"; msg = ""; break;
	default:
		cmd = "Generic"; msg = "";
	}
	if (SCSI_ERR_GRAVITY(code))
		printf("\n%s%c: %s %s %sx%x\n", "rz", unit, cmd, msg,
			" error, further info: ", info);
}
