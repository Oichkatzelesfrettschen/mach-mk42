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
 * $Log:	scsi_53C94_hdw.c,v $
 * Revision 2.6  91/02/14  14:35:31  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	Also, sanity check for spurious interrupts.
 * 	Some more debugging tools.
 * 	[91/02/12  12:46:38  af]
 * 
 * Revision 2.5  91/02/05  17:45:07  mrt
 * 	Added author notices
 * 	[91/02/04  11:18:43  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:17:20  mrt]
 * 
 * Revision 2.4  91/01/08  15:48:24  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/27            rpd]
 * 
 * Revision 2.3  90/12/05  23:34:48  af
 * 	Recovered from pmax merge.. and from the destruction of a disk.
 * 	[90/12/03  23:40:40  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:39:09  af
 * 	Created, from the DEC specs:
 * 	"PMAZ-AA TURBOchannel SCSI Module Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 27, 1990.
 * 	And from the NCR data sheets
 * 	"NCR 53C94, 53C95, 53C96 Advances SCSI Controller"
 * 	[90/09/03            af]
 */
/*
 *	File: scsi_53C94_hdw.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Bottom layer of the SCSI driver: chip-dependent functions
 *
 *	This file contains the code that is specific to the NCR 53C94
 *	SCSI chip (Host Bus Adapter in SCSI parlance): probing, start
 *	operation, and interrupt routine.
 */

/*
 * This layer works based on small simple 'scripts' that are installed
 * at the start of the command and drive the chip to completion.
 * The idea comes from the specs of the NCR 53C700 'script' processor.
 *
 * There are various reasons for this, mainly
 * - Performance: identify the common (successful) path, and follow it;
 *   at interrupt time no code is needed to find the current status
 * - Code size: it should be easy to compact common operations
 * - Adaptability: the code skeleton should adapt to different chips without
 *   terrible complications.
 * - Error handling: and it is easy to modify the actions performed
 *   by the scripts to cope with strange but well identified sequences
 *
 */

#include <asc.h>
#if	NASC > 0

#include <mach/std_types.h>
#include <sys/types.h>
#include <mips/busses.h>
#include <device/io_req.h>
#include <device/param.h>

#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi2.h>

#include <mips/PMAX/pmaz_aa.h>
#include <mips/PMAX/scsi_53C94.h>
#include <mips/PMAX/scsi_defs.h>

/*
 * A script has a three parts: a pre-condition, an action, and
 * an optional command to the chip.  The first triggers error
 * handling if not satisfied and in our case it is formed by the
 * values of the interrupt register and status register, this
 * basically captures the phase of the bus and the TC and BS
 * bits.  The action part is just a function pointer, and the
 * command is what the 53c90 should be told to do at the end
 * of the action processing.  This command is only issued and the
 * script proceeds if the action routine returns TRUE.
 * See asc_intr() for how and where this is all done.
 */

typedef struct {
	int		condition;	/* expected state at interrupt */
	boolean_t	(*action)();	/* extra operations */
	int		command;	/* command to the chip */
} script_t;

/* Matching on the condition value */
#define	SCRIPT_MATCH(csr,ir)		(ASC_PHASE(csr)|((ir)&0xf8))

/* When no command is needed */
#define	SCRIPT_END	-1

/* forward decls of script actions */
boolean_t
	script_nop(),			/* when nothing needed */
	asc_end(),			/* all come to an end */
	asc_get_status(),		/* get status from target */
	asc_dma_in();			/* get data from target via dma */
	asc_dma_out();			/* send data to target via dma */
	asc_dosynch();			/* negotiate synch xfer */
/* forward decls of error handlers */
boolean_t
	asc_err_try_synch();		/* errors during inquiry cmds */
	gimmeabreak();			/* drop into the debugger */

/*
 * State descriptor for this layer.  There is one such structure
 * per (enabled) SCSI-53c90 interface
 */
struct asc_softc {
	asc_regmap_t	*regs;
	volatile int	*dmar;
	volatile char	*buff;
	script_t	*script;
	int		(*error_handler)();
	volatile char	state;
#define	ASC_STATE_BUSY		0x01	/* selecting or currently connected */
#define ASC_STATE_WANTED	0x02
#define ASC_STATE_TARGET	0x04	/* currently selected as target */
	scsi_softc_t	*sc;
	target_info_t	*target;	/* the current one */
	int		in_count;
} asc_softc_data[NASC];

typedef struct asc_softc *asc_softc_t;

asc_softc_t	asc_softc[NASC];

/*
 * Synch xfer parameters, and timing conversions
 */
int	asc_min_period = 5;	/* in CLKS/BYTE, 1 CLK = 40nsecs */
int	asc_max_offset = 15;	/* pure number */

#define asc_to_scsi_period(a)	(a * 10)

int scsi_period_to_asc(p)
{
	register int ret = p / 10;
	if (p < asc_min_period)
		return asc_min_period;
	if ((ret * 10) < p)
		return ret + 1;
	return ret;
}

/*
 * Definition of the controller for the auto-configuration program.
 */

int	asc_probe(), scsi_slave(), scsi_attach(), asc_go(), asc_intr();

caddr_t	asc_std[NASC] = { 0 };
struct	bus_device *asc_dinfo[NASC*8];
struct	bus_ctlr *asc_minfo[NASC];
struct	bus_driver asc_driver = 
        { asc_probe, scsi_slave, scsi_attach, asc_go, asc_std, "rz", asc_dinfo,
	  "asc", asc_minfo, BUS_INTR_B4_PROBE};

/*
 * Scripts
	XXX compact these scripts
 */
script_t
asc_script_early_data_in[] = {	/* started with SEL & DMA */
	{(ASC_INT_BS|ASC_INT_FC)|ASC_PHASE_DATAI, asc_dma_in,ASC_CMD_XFER_INFO|ASC_CMD_DMA},
	{ASC_INT_BS|ASC_PHASE_STATUS, script_nop, ASC_CMD_I_COMPLETE},
	{ASC_INT_FC|ASC_PHASE_MSG_IN, asc_get_status, ASC_CMD_MSG_ACPT},
	{ASC_INT_DISC, asc_end, SCRIPT_END}
},

asc_script_early_data_out[] = {	/* started with SEL & DMA */
	{(ASC_INT_BS|ASC_INT_FC)|ASC_PHASE_DATAO, asc_dma_out, ASC_CMD_XFER_INFO|ASC_CMD_DMA},
	{ASC_INT_BS|ASC_PHASE_STATUS, script_nop, ASC_CMD_I_COMPLETE},
	{ASC_INT_FC|ASC_PHASE_MSG_IN, asc_get_status, ASC_CMD_MSG_ACPT},
	{ASC_INT_DISC, asc_end, SCRIPT_END}
},

asc_script_try_synch[] = {
	{(ASC_INT_BS|ASC_INT_FC)|ASC_PHASE_MSG_OUT, asc_dosynch, ASC_CMD_I_COMPLETE},
	{ASC_INT_FC|ASC_PHASE_MSG_IN, asc_get_status, ASC_CMD_MSG_ACPT},
	{ASC_INT_DISC, asc_end, SCRIPT_END}/* allow disconnects */
},

asc_simple_cmd_script[] = {
	{(ASC_INT_FC|ASC_INT_BS)|ASC_PHASE_STATUS, script_nop, ASC_CMD_I_COMPLETE},
	{ASC_INT_FC|ASC_PHASE_MSG_IN, asc_get_status, ASC_CMD_MSG_ACPT},
	{ASC_INT_DISC, asc_end, SCRIPT_END}
};


/*
 *	Probe/Slave/Attach functions
 */

/*
 * Probe routine:
 *	Should find out (a) if the controller is
 *	present and (b) which/where slaves are present.
 *
 * Implementation:
 *	Send an identify msg to each possible target on the bus
 *	except of course ourselves.
 */
asc_probe(reg, ui)
	unsigned	reg;
	struct bus_ctlr	*ui;
{
	int             unit = ui->unit;
	asc_softc_t     asc = &asc_softc_data[unit];
	int		target_id;
	scsi_softc_t	*sc;
	register asc_regmap_t	*regs;
	int		s;
	boolean_t	did_banner = FALSE;
	char		*dma_ptr;

	/*
	 * Statically partition the DMA buffer between targets.
	 * This way we will eventually be able to attach/detach
	 * drives on-fly.  And 18k/target is plenty for normal use.
	 */
#	define PER_TGT_DMA_SIZE		((ASC_RAM_SIZE/7) & ~(sizeof(int)-1))

	/*
	 * We are only called if the right board is there,
	 * but make sure anyways..
	 */
	if (check_memory(reg + ASC_OFFSET_53C94, 0))
		return 0;

	/* Mappable version side */
	ASC_probe(reg, ui);

	/*
	 * Initialize hw descriptor, cache some pointers
	 */
	asc_softc[unit] = asc;
	asc->regs = (asc_regmap_t *) (reg + ASC_OFFSET_53C94);
	asc->dmar = (volatile int *) (reg + ASC_OFFSET_DMAR);
	asc->buff = (volatile char*) (reg + ASC_OFFSET_RAM);

	sc = scsi_master_alloc();
	asc->sc = sc;

	sc->go = asc_go;
	/*
	 * Note that our buffer partitioning is
	 * a stricter limit than the HW one
	 */
	sc->max_dma_data = /*ASC_TC_MAX*/ dbtob(btodb(PER_TGT_DMA_SIZE));

	regs = asc->regs;

	/*
	 * Clear out dma buffer
	 */
	bzero(asc->buff, ASC_RAM_SIZE);

	/*
	 * Reset chip, fully.  Note that interrupts are already enabled.
	 */
	s = splbio();
	asc_reset(regs, 0);

	/*
	 * Our SCSI id on the bus.
	 * The user can set this via the prom on 3maxen/pmaxen.
	 * If this changes it is easy to fix: make a default that
	 * can be changed as boot arg.
	 */
#ifdef	unneeded
	regs->asc_cnfg1 = (regs->asc_cnfg1 & ~ASC_CNFG1_MY_BUS_ID) |
			      (scsi_initiator_id[unit] & 0x7);
#endif
	sc->initiator_id = regs->asc_cnfg1 & ASC_CNFG1_MY_BUS_ID;
	printf("%s%d: SCSI id %d", ui->name, unit, sc->initiator_id);

	/*
	 * For all possible targets, see if there is one and allocate
	 * a descriptor for it if it is there.
	 */
	for (target_id = 0, dma_ptr = (char*)asc->buff;
	     target_id < 8;
	     target_id++, dma_ptr += PER_TGT_DMA_SIZE) {
		register unsigned char	csr, ss, ir, ff;
		register scsi_status_byte_t	status;

		/* except of course ourselves */
		if (target_id == sc->initiator_id)
			continue;

		regs->asc_cmd = ASC_CMD_FLUSH;	/* empty fifo */
		delay(1);/*debug*/

		regs->asc_dbus_id = target_id; delay(1);
		regs->asc_sel_timo = ASC_TIMEOUT_250; delay(1);

		/*
		 * See if the unit is ready.
		 * XXX SHOULD inquiry LUN 0 instead !!!
		 */
		regs->asc_fifo = SCSI_CMD_TEST_UNIT_READY;
delay(1);	regs->asc_fifo = 0;
delay(1);	regs->asc_fifo = 0;
delay(1);	regs->asc_fifo = 0;
delay(1);	regs->asc_fifo = 0;
delay(1);	regs->asc_fifo = 0;

		/* select and send it */
		regs->asc_cmd = ASC_CMD_SEL;

		/* wait for the chip to complete, or timeout */
		csr = asc_wait(regs, ASC_CSR_INT);
		ss = regs->asc_ss;
		ir = regs->asc_intr;

		/* empty fifo, there is garbage in it if timeout */
		regs->asc_cmd = ASC_CMD_FLUSH;

		/*
		 * Check if the select timed out
		 */
		if ((ASC_SS(ss) == 0) && (ir == ASC_INT_DISC))
			/* noone out there */
			continue;

		if (ASC_PHASE(csr) != ASC_PHASE_STATUS) {
			printf( " %s%d%s", "ignoring target at ", target_id,
				" cuz it acts weirdo");
			continue;
		}

		printf(",%s%d", did_banner++ ? " " : " target(s) at ",
				target_id);

		regs->asc_cmd = ASC_CMD_I_COMPLETE;
		delay(1);
		csr = asc_wait(regs, ASC_CSR_INT);
		ir = regs->asc_intr; /* ack intr */

		status.bits = regs->asc_fifo; /* empty fifo */
		ff = regs->asc_fifo;

		if (status.st.scsi_status_code != SCSI_ST_GOOD)
			scsi_error( 0, SCSI_ERR_STATUS, status.bits);

		regs->asc_cmd = ASC_CMD_MSG_ACPT;
		csr = asc_wait(regs, ASC_CSR_INT);
		ir = regs->asc_intr; /* ack intr */

		/*
		 * Found a target
		 */
		{
			register target_info_t	*tgt;
			tgt = scsi_slave_alloc(unit, target_id, asc);
			tgt->sync_period = 0;
			tgt->sync_offset = 0;

			tgt->flags |= TGT_DMA_RESTRICTED;
			tgt->dma_ptr = dma_ptr;
			tgt->cmd_ptr = dma_ptr;
		}
	}
	printf(".\n");

	splx(s);
	return 1;
}

#define DEBUG
#ifdef	DEBUG
asc_state(regs)
	asc_regmap_t	*regs;
{
	register unsigned char ss,ff,csr,ir;

	if(regs == 0)
		regs = (asc_regmap_t*)0xbf400000;
	ss = regs->asc_ss;
	ff = regs->asc_flags;
	csr = regs->asc_csr;
	ir = regs->asc_intr;
	printf("ss %x ff %x csr %x ir %x\n", ss, ff, csr, ir);
}

asc_target_state(tgt)
	target_info_t	*tgt;
{
	if (tgt == 0)
		tgt = asc_softc[0]->target;
	if (tgt)
	printf("fl %x dma %x cmd %x id %x per %x off %x ior %x ret %x\n",
		tgt->flags, tgt->dma_ptr, tgt->cmd_ptr,
		tgt->target_id, tgt->sync_period, tgt->sync_offset,
		tgt->ior, tgt->done);
}

asc_script_state(unit)
{
	script_t	*spt = asc_softc[unit]->script;

	db_printsym(spt,1);
	db_printf(": %x %x %x\n", spt->condition, spt->action, spt->command);
	db_printsym(asc_softc[unit]->error_handler, 1);
}
#endif	DEBUG



static asc_wait(regs, until)
	asc_regmap_t	*regs;
{
	int timeo = 1000000;
	while ((regs->asc_csr & until) == 0) {
		delay(1);
		if (!timeo--) {
printf("asc_wait TIMEO with x%x\n", regs->asc_csr);
			break;
		}
	}
	return regs->asc_csr;
}

asc_reset(regs, quick)
	asc_regmap_t	*regs;
{
	char	my_id;

	/* preserve our ID for now */
	my_id = (regs->asc_cnfg1 & ASC_CNFG1_MY_BUS_ID);

	/*
	 * Reset chip and wait till done
	 */
	regs->asc_cmd = ASC_CMD_RESET;
	wbflush(); delay(25);

	/* spec says this is needed after reset */
	regs->asc_cmd = ASC_CMD_NOP;
	wbflush(); delay(25);

	/*
	 * Set up various chip parameters
	 */
	regs->asc_ccf = ASC_CCF_25MHz;	/* 25 MHz clock */
	wbflush();
	delay(25);
	regs->asc_sel_timo = ASC_TIMEOUT_250;
	/* restore our ID */
	regs->asc_cnfg1 = my_id | ASC_CNFG1_P_CHECK;
	regs->asc_cnfg2 = 0/*ASC_CNFG2_SCSI2*/;
	regs->asc_cnfg3 = 0;
	/* zero anything else */
	ASC_TC_PUT(regs, 0);
	regs->asc_syn_p = asc_min_period;
	regs->asc_syn_o = 0;	/* asynch for now */
	delay(1);

	if (quick) return;

	/* xxx send out a bus reset and wait long enough */
	/* but I am not so sure that I really want it */
}

/*
 *	Operational functions
 */

/*
 * Start a SCSI command on a target
 */
asc_go(sc, tgt, out_count, in_count, cmd_only)
	scsi_softc_t		*sc;
	target_info_t		*tgt;
	boolean_t		cmd_only;
{
	asc_softc_t	asc;
	asc_regmap_t	*regs;
	register int	st, s;

	/* cache some pointers */
	asc = (asc_softc_t)tgt->hw_state;
	regs = asc->regs;

	/*
	 * See if another target is currently selected on
	 * this SCSI bus, e.g. lock the asc structure.
	 * Note that it is the strategy routine's job
	 * to serialize ops on the same target as appropriate.
	 * XXX here and everywhere, locks!
	 */
	s = splbio();
	while (asc->state & ASC_STATE_BUSY) {
		asc->state |= ASC_STATE_WANTED;
		assert_wait(asc);
		thread_block((void (*)()) 0);
	}
	asc->state |= ASC_STATE_BUSY;
	splx(s);

	/*
	 * Init the chip and target state
	 */
	regs->asc_cmd = ASC_CMD_FLUSH;

	tgt->done = SCSI_RET_IN_PROGRESS;

	/*
	 * We cannot do real DMA.
	 */
	if (tgt->cur_cmd == SCSI_CMD_WRITE){
		io_req_t	ior = tgt->ior;
		register int	len = ior->io_count;

		if (len > sc->max_dma_data)
			len = sc->max_dma_data;
		bcopy(ior->io_data, tgt->cmd_ptr + out_count , len);
		out_count += len;
	}

	if (!cmd_only) {
		regs->asc_fifo = SCSI_IDENTIFY;/* xxx allow disc */
		out_count++;
	}
	*(asc->dmar) = ASC_DMAR_WRITE | ASC_DMA_ADDR(tgt->cmd_ptr);
delay(1);
	ASC_TC_PUT(regs, out_count);
	asc->target = tgt;

delay(1);	regs->asc_dbus_id = tgt->target_id;
delay(1);	regs->asc_syn_p = tgt->sync_period;
delay(1);	regs->asc_syn_o = tgt->sync_offset;
delay(1);	regs->asc_sel_timo = ASC_TIMEOUT_250;

	asc->in_count = in_count;
	delay(1);

	switch (tgt->cur_cmd) {
	    case SCSI_CMD_INQUIRY:
	    case SCSI_CMD_REQUEST_SENSE:
	    case SCSI_CMD_MODE_SENSE:
	    case SCSI_CMD_RECEIVE_DIAG_RESULTS:
case SCSI_CMD_READ_CAPACITY:
case SCSI_CMD_READ:
		asc->script = asc_script_early_data_in;
		asc->error_handler = gimmeabreak;
		break;
case SCSI_CMD_MODE_SELECT:
case SCSI_CMD_WRITE:
		asc->script = asc_script_early_data_out;
		asc->error_handler = gimmeabreak;
		break;
	    case SCSI_CMD_TEST_UNIT_READY:
		/*
		 * Do the synch negotiation here, unless prohibited
		 * or done already
		 */
		if (tgt->flags & TGT_DID_SYNCH ||
		    scsi_no_synchronous_xfer[sc->masterno] & (1 << tgt->target_id)) {
			asc->script = asc_simple_cmd_script;
			asc->error_handler = gimmeabreak;
		} else {
			asc->script = asc_script_try_synch;
			asc->error_handler = asc_err_try_synch;
			tgt->flags |= TGT_TRY_SYNCH;
		}
		break;
	    default:
		asc->script = asc_simple_cmd_script;
		asc->error_handler = gimmeabreak;
	}

	if (tgt->flags & TGT_DID_SYNCH)
		regs->asc_cmd = ASC_CMD_SEL_ATN | ASC_CMD_DMA;
	else if (tgt->flags & TGT_TRY_SYNCH)
		regs->asc_cmd = ASC_CMD_SEL_ATN_STOP | ASC_CMD_DMA;
	else
		regs->asc_cmd = ASC_CMD_SEL | ASC_CMD_DMA;
}

/*
 * Interrupt routine
 *	Take interrupts from the chip
 *
 * Implementation:
 *	Move along the current command's script if
 *	all is well, invoke error handler if not.
 */
asc_intr(unit, spllevel)
	int	spllevel;
{
	register asc_softc_t	asc;
	register script_t	*scpt;
	register int		ss, ir, csr;
	register asc_regmap_t	*regs;
	extern boolean_t	rz_use_mapped_interface;

	if (rz_use_mapped_interface)
		return ASC_intr(unit,spllevel);

	asc = asc_softc[unit];

	/* collect ephemeral information */
	regs = asc->regs;
	csr  = regs->asc_csr;
	/* drop spurious interrupts */
	if ((csr & ASC_CSR_INT) == 0)
		return;
	ss   = regs->asc_ss;
	ir   = regs->asc_intr;	/* this resets the previous two */

	splx(spllevel);	/* drop priority */

	/* XXX */
	if ((asc->state & ASC_STATE_TARGET) ||
	    (ir & (ASC_INT_SEL|ASC_INT_SEL_ATN|ASC_INT_RESEL)))
		return asc_target_intr(asc);	/* probably same */

	scpt = asc->script;
	if (SCRIPT_MATCH(csr,ir) == scpt->condition) {/* xxx */
		/*
		 * Perform the appropriate operation,
		 * then proceed
		 */
		if ((*scpt->action)(asc, csr, ss, ir)) {
			asc->script = scpt + 1;
			regs->asc_cmd = scpt->command;
		}
	} else
		return (*asc->error_handler)(asc, csr, ss, ir);
}

asc_target_intr()
{
	panic("ASC: TARGET MODE !!!\n");
}

/*
 * All the many little things that the interrupt
 * routine might switch to
 */
boolean_t
script_nop(asc, csr, ss, ir)
	register asc_softc_t	asc;

{
	return TRUE;
}

boolean_t
asc_end(asc, csr, ss, ir)
	register asc_softc_t	asc;
{
	register target_info_t	*tgt;
	register io_req_t	ior;

	tgt = asc->target;
	asc->target = 0;
	asc->state &= ~ASC_STATE_BUSY;
	if (asc->state & ASC_STATE_WANTED) {
		asc->state &= ~ASC_STATE_WANTED;
		thread_wakeup(asc);
	}

	tgt->done = SCSI_RET_SUCCESS;

	if (ior = tgt->ior) {
		if (ior->io_op & IO_READ) {
			register int	len = ior->io_count,
					max = asc->sc->max_dma_data;
			if (len > max)
				len = max;
			bcopy(tgt->dma_ptr, ior->io_data, len);
		}
		(*tgt->dev_ops->restart)( asc->sc, tgt, TRUE);
	} else if (tgt->flags & TGT_FULLY_PROBED)
		thread_wakeup(tgt);

	return FALSE;
}

boolean_t
asc_get_status(asc, csr, ss, ir)
	register asc_softc_t	asc;
{
	register asc_regmap_t	*regs = asc->regs;
	register scsi2_status_byte_t status;

	/*
	 * Get the last two bytes in FIFO
	 */
	while ((regs->asc_flags & ASC_FLAGS_FIFO_CNT) > 2)
		status.bits = regs->asc_fifo;

	status.bits = regs->asc_fifo;
	if (status.st.scsi_status_code != SCSI_ST_GOOD)
		scsi_error(asc->target, SCSI_ERR_STATUS, status.bits);
		/* XXX needs request sense */

	status.bits = regs->asc_fifo;	/* just pop the command_complete */

regs->asc_cmd = ASC_CMD_FLUSH; /* debug */
delay(1);
	return TRUE;
}

boolean_t
asc_dma_in(asc, csr, ss, ir)
	register asc_softc_t	asc;
{
	register target_info_t	*tgt;
	register asc_regmap_t	*regs = asc->regs;
	register int		count;

	tgt = asc->target;
	count = asc->in_count;
	ASC_TC_PUT(regs, count);
	wbflush();
	(*asc->dmar) = ASC_DMA_ADDR(tgt->dma_ptr);
	wbflush();
	return TRUE;
}

boolean_t
asc_dma_out(asc, csr, ss, ir)
	register asc_softc_t	asc;
{
	register asc_regmap_t	*regs = asc->regs;
	register int		reload_count;

	ASC_TC_GET(regs, reload_count);
	reload_count += regs->asc_flags & ASC_FLAGS_FIFO_CNT;
	ASC_TC_PUT(regs, reload_count);
	wbflush();
delay(1);
	return TRUE;
}

boolean_t
asc_dosynch(asc, csr, ss, ir)
	register asc_softc_t	asc;
	register unsigned char	csr, ir;
{
	register asc_regmap_t	*regs = asc->regs;
	register unsigned char	c;
	int i, per, offs;
	register target_info_t	*tgt;

	/*
	 * Phase is MSG_OUT here.
	 * Try synch negotiation, unless prohibited
	 */
	regs->asc_cmd = ASC_CMD_FLUSH;
delay(1);
/* XXX make a script of all this mess */

	per = asc_min_period;
	offs = 0;
	tgt = asc->target;

	regs->asc_fifo = SCSI_EXTENDED_MESSAGE;
delay(1);	regs->asc_fifo = 3;
delay(1);	regs->asc_fifo = SCSI_SYNC_XFER_REQUEST;
delay(1);	regs->asc_fifo = asc_to_scsi_period(asc_min_period);
delay(1);	regs->asc_fifo = asc_max_offset;
	delay(1);
	regs->asc_cmd = ASC_CMD_XFER_INFO;
	csr = asc_wait(regs, ASC_CSR_INT);
	ir = regs->asc_intr;

	if (ASC_PHASE(csr) != ASC_PHASE_MSG_IN)
		gimmeabreak();

	regs->asc_cmd = ASC_CMD_XFER_INFO;
	csr = asc_wait(regs, ASC_CSR_INT);
	ir = regs->asc_intr;

	while ((regs->asc_flags & ASC_FLAGS_FIFO_CNT) > 0)
		c = regs->asc_fifo;	/* see what it says */

	if (c == SCSI_MESSAGE_REJECT) {
printf(" did not like SYNCH xfer ");
		tgt->flags |= TGT_DID_SYNCH;
		regs->asc_cmd = ASC_CMD_MSG_ACPT;
		csr = asc_wait(regs, ASC_CSR_INT);
		ir = regs->asc_intr;
		/* some just break out here, some dont */
		if (ASC_PHASE(csr) == ASC_PHASE_MSG_OUT) {
			regs->asc_fifo = SCSI_ABORT;
	delay(1);
			regs->asc_cmd = ASC_CMD_XFER_INFO;
			csr = asc_wait(regs, ASC_CSR_INT);
			ir = regs->asc_intr;
		}
		if (ir & ASC_INT_DISC) {
			asc_end(asc,csr,0,ir);
			return FALSE;
		}
		goto status;
	}

	regs->asc_cmd = ASC_CMD_MSG_ACPT;
	asc_wait(regs, ASC_CSR_INT);
	ir = regs->asc_intr;

	if (c != SCSI_EXTENDED_MESSAGE)
		gimmeabreak();

	regs->asc_cmd = ASC_CMD_XFER_INFO;
	asc_wait(regs, ASC_CSR_INT);
	c = regs->asc_intr;
	if (regs->asc_fifo != 3)
		panic("asc_dosynch");

	for (i = 0; i < 3; i++) {
		regs->asc_cmd = ASC_CMD_MSG_ACPT;
		asc_wait(regs, ASC_CSR_INT);
		c = regs->asc_intr;

		regs->asc_cmd = ASC_CMD_XFER_INFO;
		asc_wait(regs, ASC_CSR_INT);
		c = regs->asc_intr;/*ack*/
		c = regs->asc_fifo;

		if (i == 1) tgt->sync_period = scsi_period_to_asc(c);
		if (i == 2) tgt->sync_offset = c;
	}

	regs->asc_cmd = ASC_CMD_MSG_ACPT;
	csr = asc_wait(regs, ASC_CSR_INT);
	c = regs->asc_intr;

	/* might jump to status ?? */
	if (ASC_PHASE(csr) == ASC_PHASE_COMMAND) {
		/* test unit ready or what ? */
		regs->asc_fifo = 0;
delay(1);		regs->asc_fifo = 0;
delay(1);		regs->asc_fifo = 0;
delay(1);		regs->asc_fifo = 0;
delay(1);		regs->asc_fifo = 0;
delay(1);		regs->asc_fifo = 0;
delay(1);		ASC_TC_PUT(regs,0xff);
delay(1);		regs->asc_cmd = ASC_CMD_XFER_PAD; /*0x98*/
		csr = asc_wait(regs, ASC_CSR_INT);
		ir = regs->asc_intr;/*ack*/
	}

status:
	if (ASC_PHASE(csr) != ASC_PHASE_STATUS)
		gimmeabreak();

	tgt->flags |= TGT_DID_SYNCH;
	return TRUE;
}


/*
 * Error handlers
 */

/*
 * Handle errors during TEST UNIT READY, which is
 * where we try negotiation of synch xfer
 */
asc_err_try_synch(asc, csr, ss, ir)
	register asc_softc_t	asc;
{
	gimmeabreak();
}

#endif	NASC > 0

