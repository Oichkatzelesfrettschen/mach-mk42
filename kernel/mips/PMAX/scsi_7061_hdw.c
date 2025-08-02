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
 * $Log:	scsi_7061_hdw.c,v $
 * Revision 2.7  91/02/14  14:37:18  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  13:18:18  af]
 * 
 * Revision 2.6  91/02/05  17:45:19  mrt
 * 	Added author notices
 * 	[91/02/04  11:18:59  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:17:41  mrt]
 * 
 * Revision 2.5  91/01/08  15:48:33  rpd
 * 	Catched hitachi's problem: spurious interrupt _before_
 * 	dmain/out even got started.
 * 	[90/12/29            af]
 * 	Added continuation argument to thread_block.
 * 	[90/12/27            rpd]
 * 
 * Revision 2.4  90/12/20  17:03:53  jeffreyh
 * 	More hacks to understand rz55, and now hitachis boot.
 * 	There is still one problem for hitachis at large,
 * 	will fix in the next merge.
 * 	[90/12/19  15:17:48  af]
 * 
 * Revision 2.3  90/12/05  23:34:54  af
 * 	Works, but in a crazy-horse-like fashion.  Known problems:
 * 	- phase changes are apparently unreliable, needs sometimes
 * 	  a kick
 * 	- spurious 'bus error' interrupts from chip confuse the issue
 * 	- two big spin loops are still there
 * 	- repeatedly interrupted DMA xfers can loop (happens if you
 * 	  do not "init" the prom).
 * 	Missing features:
 * 	- should disconnect when more than one target on the bus
 * 	- do chaining of select&identify-msg.
 * 	[90/12/03  23:46:05  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:39:29  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	[90/10/17            af]
 */
/*
 *	File: scsi_7061_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Bottom layer of the SCSI driver: chip-dependent functions
 *
 *	This file contains the code that is specific to the DEC DC7061
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

#include <sii.h>
#if	NSII > 0


#include <mach/std_types.h>
#include <sys/types.h>
#include <mips/busses.h>
#include <device/io_req.h>

#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi2.h>
#include <mips/PMAX/scsi_7061.h>
#include <mips/PMAX/scsi_defs.h>

#include <mips/PMAX/kn01.h>
#define	SII_OFFSET_RAM	(KN01_SYS_SII_B_START-KN01_SYS_SII)
#define	SII_RAM_SIZE	((KN01_SYS_SII_B_END-KN01_SYS_SII_B_START)/2)


/*
 * Macros to make certain things a little more readable
 */
#define	SII_COMMAND(regs,cs,ds,cmd)				\
	{							\
		(regs)->sii_command = ((cs) & 0x70) |		\
				      ((ds) & 0x07) | (cmd);	\
		wbflush();					\
	}
#define	SII_ACK(regs,cs,ds,cmd)					\
	{							\
		SII_COMMAND(regs,cs,ds,cmd);			\
		(regs)->sii_conn_csr = (cs);			\
		(regs)->sii_data_csr = (ds);			\
	}

/*
 * Deal with bogus pmax dma buffer
 */

/* 16 bits in 32 bit envelopes */
#define	SII_DMADR_LO(ptr)	((((unsigned)ptr)>>1)&SII_DMA_LOW_MASK)
#define	SII_DMADR_HI(ptr)	((((unsigned)ptr)>>(16+1))&SII_DMA_HIGH_MASK)

static char	decent_buffer[NSII*8][256];

/*
 * A script has a three parts: a pre-condition, an action, and
 * an optional command to the chip.  The first triggers error
 * handling if not satisfied and in our case it is formed by the
 * values of the sii_conn_csr and sii_data_csr register
 * bits.  The action part is just a function pointer, and the
 * command is what the 7061 should be told to do at the end
 * of the action processing.  This command is only issued and the
 * script proceeds if the action routine returns TRUE.
 * See sii_intr() for how and where this is all done.
 */

typedef struct {
	int	condition;	/* expected state at interrupt */
	int	(*action)();	/* extra operations */
	int	command;	/* command to the chip */
} script_t;

#define	SCRIPT_MATCH(cs,ds)	((cs)&0x70|SII_PHASE((ds)))

#define	SII_PHASE_DISC	0x4	/* sort of .. */

/* When no command is needed */
#define	SCRIPT_END	-1

/* forward decls of script actions */
boolean_t
	script_nop(),			/* when nothing needed */
	sii_end(),			/* all come to an end */
	sii_get_status(),		/* get status from target */
	sii_dma_in(),			/* get data from target via dma */
	sii_dma_out(),			/* send data to target via dma */
	sii_identify(),			/* send identify msg */
	sii_dosynch();			/* negotiate synch xfer */
/* forward decls of error handlers */
boolean_t
	sii_err_phase_mismatch(),
	sii_err_try_synch(),		/* errors during inquiry cmds */
	gimmeabreak();			/* drop into the debugger */

/*
 * State descriptor for this layer.  There is one such structure
 * per (enabled) SCSI-7061 interface
 */
struct sii_softc {
	sii_regmap_t	*regs;
	volatile char	*buff;
	script_t	*script;
	int		(*error_handler)();
	volatile char	state;
#define	SII_STATE_BUSY		0x01	/* selecting or currently connected */
#define SII_STATE_WANTED	0x02
#define SII_STATE_TARGET	0x04	/* currently selected as target */
	scsi_softc_t	*sc;
	target_info_t	*target;
	int		in_count;
	int		out_count;
	int		cmd_count;
} sii_softc_data[NSII];

typedef struct sii_softc *sii_softc_t;

sii_softc_t	sii_softc[NSII];

/*
 * Synch xfer parameters, and timing conversions
 */
int	sii_min_period = 63;	/* in 4 ns units */
int	sii_max_offset = 3;	/* pure number */

#define sii_to_scsi_period(a)	(a)
#define scsi_period_to_sii(p)	(((p) < sii_min_period) ? sii_min_period : (p))

/*
 * Definition of the controller for the auto-configuration program.
 */

int	sii_probe(), scsi_slave(), scsi_attach(), sii_go(), sii_intr();

caddr_t	sii_std[NSII] = { 0 };
struct	bus_device *sii_dinfo[NSII*8];
struct	bus_ctlr *sii_minfo[NSII];
struct	bus_driver sii_driver = 
        { sii_probe, scsi_slave, scsi_attach, sii_go, sii_std, "rz", sii_dinfo,
	  "sii", sii_minfo, /*BUS_INTR_B4_PROBE?*/};

/*
 * Scripts
	XXX compact these scripts
 */
script_t
sii_script_early_data_in[] = {	/* started with SEL & DMA */
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_DATAI|SII_CON_CON, sii_dma_in,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_DATAI},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
},

sii_script_early_data_out[] = {	/* started with SEL & DMA */
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_DATAO|SII_CON_CON, sii_dma_out,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_DATAO},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
},

sii_script_early_cmd[] = {
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
},

sii_script_try_synch[] = {
	{ SII_PHASE_MSG_OUT|SII_CON_CON, sii_dosynch, SCRIPT_END}
},

/* After synch negotiation, and XXX with disconnects */

sii_script_nrm_data_in[] = {	/* started with SEL & DMA */
	{ SII_PHASE_MSG_OUT|SII_CON_CON, sii_identify,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_OUT},
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_DATAI|SII_CON_CON, sii_dma_in,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_DATAI},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
},

sii_script_nrm_data_out[] = {	/* started with SEL & DMA */
	{ SII_PHASE_MSG_OUT|SII_CON_CON, sii_identify,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_OUT},
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_DATAO|SII_CON_CON, sii_dma_out,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_DATAO},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
},

sii_script_nrm_cmd[] = {
	{ SII_PHASE_MSG_OUT|SII_CON_CON, sii_identify,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_OUT},
	{ SII_PHASE_CMD|SII_CON_CON, script_nop,
		(SII_CMD_XFER|SII_CMD_DMA)|SII_CON_CON|SII_PHASE_CMD},
	{ SII_PHASE_STATUS|SII_CON_CON, sii_get_status,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_STATUS},
	{ SII_PHASE_MSG_IN|SII_CON_CON, script_nop,
		SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN},
	{ SII_PHASE_DISC, sii_end, SCRIPT_END}
};


/* Before and after we select with ATN things differ */
script_t
*sii_script_data_in[2]    = { sii_script_early_data_in,sii_script_nrm_data_in},
*sii_script_data_out[2]   = { sii_script_early_data_out,sii_script_nrm_data_out},
*sii_script_simple_cmd[2] = { sii_script_early_cmd,sii_script_nrm_cmd};


/*#define	DEBUG*/
#ifdef	DEBUG
struct cnt {
	unsigned int	usage;
	unsigned int	avg;
	unsigned int	min;
	unsigned int	max;
};

static bump(counter, value)
	register struct cnt	*counter;
	register unsigned int	value;
{
	register unsigned int	n = counter->usage + 1;

	counter->usage = n;
	if (n == 0) {
		printf("{Counter at x%x overflowed with avg x%x}",
			counter, counter->avg);
		return;
	} else
	if (n == 1)
		counter->min = 0xffffffff;

	counter->avg = ((counter->avg * (n - 1)) + value) / n;
	if (counter->min > value)
		counter->min = value;
	if (counter->max < value)
		counter->max = value;
}

struct cnt
	go_cnt, go1_cnt, w_cnt, r_cnt, intr_cnt, s_cnt, e_cnt;
#endif	DEBUG


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
sii_probe(reg, ui)
	unsigned	reg;
	struct bus_ctlr	*ui;
{
	int             unit = ui->unit;
	sii_softc_t     sii = &sii_softc_data[unit];
	int		target_id, i;
	scsi_softc_t	*sc;
	register sii_regmap_t	*regs;
	int		s;
	boolean_t	did_banner = FALSE;
	char		*dma_ptr;

	/*
	 * Statically partition the DMA buffer between targets.
	 * This way we will eventually be able to attach/detach
	 * drives on-fly.  And 9k/target is enough.
	 */
#	define PER_TGT_DMA_SIZE		((SII_RAM_SIZE/7) & ~(sizeof(int)-1))

	/*
	 * We are only called if the chip is there,
	 * but make sure anyways..
	 */
	if (check_memory(reg, 0))
		return 0;

	/* Mappable version side */
	SII_probe(reg, ui);

	/*
	 * Initialize hw descriptor
	 */
	sii_softc[unit] = sii;
	sii->regs = (sii_regmap_t *) (reg);
	sii->buff = (volatile char*) (reg + SII_OFFSET_RAM);

	sc = scsi_master_alloc();
	sii->sc = sc;

	sc->go = sii_go;
	sc->max_dma_data = SII_DMA_COUNT_MASK+1;	/* 8k */

	regs = sii->regs;

	/*
	 * Clear out dma buffer
	 */
	bzero(sii->buff, SII_RAM_SIZE);

	/*
	 * Reset chip, fully.
	 */
	s = splbio();
	sii_reset(regs, 0);

	/*
	 * Our SCSI id on the bus.
	 * The user can set this via the prom on pmaxen/3maxen.
	 * If this changes it is easy to fix: make a default that
	 * can be changed as boot arg.
	 */
#ifdef	unneeded
	regs->sii_id = (scsi_initiator_id[unit] & SII_ID_MASK)|SII_ID_IO;
#endif
	sc->initiator_id = regs->sii_id & SII_ID_MASK;
	printf("%s%d: my SCSI id is %d", ui->name, unit, sc->initiator_id);

	/*
	 * For all possible targets, see if there is one and allocate
	 * a descriptor for it if it is there.
	 */
	for (target_id = 0, dma_ptr = (char*)sii->buff;
	     target_id < 8;
	     target_id++, dma_ptr += (PER_TGT_DMA_SIZE*2)) {

		register unsigned csr, dsr;
		register scsi_status_byte_t	status;

		/* except of course ourselves */
		if (target_id == sc->initiator_id)
			continue;

		regs->sii_sel_csr = target_id;
		wbflush();

		/* select */
		regs->sii_command = SII_CMD_SEL;
		wbflush();

		/* wait for a selection timeout delay, and some more */
		delay(251000);

		dsr = regs->sii_data_csr;
		csr = regs->sii_conn_csr;
		if ((csr & SII_CON_CON) == 0) {

			regs->sii_conn_csr = csr;/*wtc bits*/

			/* abort sel in progress */
			if (csr & SII_CON_SIP) {
				regs->sii_command = SII_CMD_DIS;
				wbflush();
				csr = sii_wait(&regs->sii_conn_csr, SII_CON_SCH);
				regs->sii_conn_csr = 0xffff;/*wtc bits */
				regs->sii_data_csr = 0xffff;
				regs->sii_command = 0;
				wbflush();
			}
			continue;
		}

		printf(",%s%d", did_banner++ ? " " : " target(s) at ",
				target_id);

		/* should be command phase here */
		if (SII_PHASE(dsr) != SII_PHASE_CMD)
			panic("sii_probe");

		/* acknowledge state change */
		SII_ACK(regs,csr,dsr,0);

		/* build command in (bogus) dma area */
		{
			unsigned int	*p = (unsigned int*) dma_ptr;

			p[0] = SCSI_CMD_TEST_UNIT_READY | (0 << 8);
			p[1] = 0 | (0 << 8);
			p[2] = 0 | (0 << 8);
		}

		/* set up dma xfer parameters */
		regs->sii_dma_len = 6;
		regs->sii_dma_adr_low = SII_DMADR_LO(dma_ptr);
		regs->sii_dma_adr_hi  = SII_DMADR_HI(dma_ptr);
		wbflush();

		/* issue dma command */
		SII_COMMAND(regs,csr,dsr,SII_CMD_XFER|SII_CMD_DMA);

		/* wait till done */
		dsr = sii_wait(&regs->sii_data_csr, SII_DTR_DONE);
		regs->sii_command &= ~(SII_CMD_XFER|SII_CMD_DMA);
		regs->sii_data_csr = SII_DTR_DONE;/* clear */
		regs->sii_dma_len = 0;

		/* move on to status phase */
		dsr = sii_wait(&regs->sii_data_csr, SII_PHASE_STATUS);
		csr = regs->sii_conn_csr;
		SII_ACK(regs,csr,dsr,0);

		if (SII_PHASE(dsr) != SII_PHASE_STATUS)
			panic("sii_probe");

		/* get status byte */
		dsr = sii_wait(&regs->sii_data_csr, SII_DTR_IBF);
		csr = regs->sii_conn_csr;

		status.bits = regs->sii_data;
		if (status.st.scsi_status_code != SCSI_ST_GOOD)
			scsi_error( 0, SCSI_ERR_STATUS, status.bits);

		/* get cmd_complete message */
		SII_ACK(regs,csr,dsr,0);
		SII_COMMAND(regs,csr,dsr,SII_CMD_XFER);
		dsr = sii_wait(&regs->sii_data_csr, SII_DTR_DONE);

		dsr = sii_wait(&regs->sii_data_csr, SII_PHASE_MSG_IN);
		csr = regs->sii_conn_csr;


		SII_ACK(regs,csr,dsr,0);
		i = regs->sii_data;
		SII_COMMAND(regs,csr,dsr,SII_CMD_XFER);
		
		/* check disconnected, clear all intr bits */
		csr = sii_wait(&regs->sii_conn_csr, SII_CON_SCH);
		if (regs->sii_conn_csr & SII_CON_CON)
			gimmeabreak();

		regs->sii_data_csr = 0xffff;
		regs->sii_conn_csr = 0xffff;

		/*
		 * Found a target
		 */
		{
			register target_info_t	*tgt;
			tgt = scsi_slave_alloc(unit, target_id, sii);
			tgt->sync_period = 0;
			tgt->sync_offset = 0;

			tgt->flags |= TGT_DMA_RESTRICTED;
			tgt->dma_ptr = dma_ptr;
			tgt->cmd_ptr = decent_buffer[unit*8 + target_id];
		}
	}
	printf(".\n");

	splx(s);
	return 1;
}

/*debug*/
sii_state(regs)
	sii_regmap_t	*regs;
{
	unsigned	dmadr;

	if (regs == 0)
		regs = (sii_regmap_t*) 0xba000000;

	dmadr = regs->sii_dma_adr_low | (regs->sii_dma_adr_hi << 16);
	printf("sc %x, dma %x @ x%x, cs %x, ds %x, cmd %x\n",
		(unsigned) regs->sii_sc1,
		(unsigned) regs->sii_dma_len, dmadr,
		(unsigned) regs->sii_conn_csr,
		(unsigned) regs->sii_data_csr,
		(unsigned) regs->sii_command);

}
sii_target_state(tgt)
	target_info_t		*tgt;
{
	if (tgt == 0)
		tgt = sii_softc[0]->target;
	if (tgt)
	printf("fl x%x dma x%x cmd x%x id x%x per x%x off x%x ior x%x ret x%x\n",
		tgt->flags, tgt->dma_ptr, tgt->cmd_ptr,
		tgt->target_id, tgt->sync_period, tgt->sync_offset,
		tgt->ior, tgt->done);
}
sii_script_state()
{
	script_t	*spt = sii_softc[0]->script;
	db_printsym(spt, 1);
	db_printf(": x%x x%x x%x\n", spt->condition, spt->action, spt->command);
	db_printsym(sii_softc[0]->error_handler, 1);
}
/*end debug*/

static sii_wait(preg, until)
	volatile unsigned short	*preg;
{
	int timeo = 1000000;
	while ((*preg & until) != until) {
		delay(1);
		if (!timeo--) {
printf("sii_wait TIMEO with x%x\n", *preg);
			break;
		}
	}
	return *preg;
}

sii_reset(regs, quickly)
	register sii_regmap_t	*regs;
{
	int my_id;

	my_id = regs->sii_id & SII_ID_MASK;

	regs->sii_command = SII_CMD_RESET;
	delay(30);

	/* clear them all random bitsy */
	regs->sii_conn_csr = SII_CON_SWA|SII_CON_SCH|SII_CON_BERR|SII_CON_RST;
	regs->sii_data_csr = SII_DTR_ATN|SII_DTR_DONE;

	regs->sii_id = my_id | SII_ID_IO;

	regs->sii_dma_ctrl = 0;	/* asynch */

	regs->sii_dma_len = 0;
	regs->sii_dma_adr_low = 0;
	regs->sii_dma_adr_hi = 0;

#if 0
	regs->sii_sc2 = SII_CS2_SBE|SII_SC1_ARB|SII_CS2_IGS;
#endif

	regs->sii_csr = SII_CSR_IE|SII_CSR_PCE|SII_CSR_SLE|SII_CSR_HPM;
		/* later: SII_CSR_RSE */

	regs->sii_diag_csr = SII_DIAG_PORT_ENB;
	wbflush();

	if (quickly)
		return;
	/* xxx reset the scsi bus and wait long enough ? */
}

/*
 *	Operational functions
 */

/*
 * Start a SCSI command on a target
 */
sii_go(sc, tgt, out_count, in_count, cmd_only)
	scsi_softc_t		*sc;
	target_info_t		*tgt;
	boolean_t		cmd_only;
{
	sii_softc_t	sii;
	sii_regmap_t	*regs;
	register int	st, s;

#ifdef	DEBUG
	bump(&go_cnt, 1);
#endif	DEBUG

	/* cache some pointers */
	sii = (sii_softc_t)tgt->hw_state;
	regs = sii->regs;

	/*
	 * See if another target is currently selected on
	 * this SCSI bus, e.g. lock the sii structure.
	 * Note that it is the strategy routine's job
	 * to serialize ops on the same target as appropriate.
	 * XXX here and everywhere, locks!
	 */
	s = splbio();
	while (sii->state & SII_STATE_BUSY) {
		sii->state |= SII_STATE_WANTED;
		assert_wait(sii);
#ifdef	DEBUG
		bump(&go1_cnt, 1);
#endif	DEBUG
		thread_block((void (*)()) 0);
	}
	sii->state |= SII_STATE_BUSY;
	splx(s);

	/*
	 * Init the chip and target state
	 */
	st = regs->sii_data;	/* empty silo */

	tgt->done = SCSI_RET_IN_PROGRESS;

	copyout_gap16(tgt->cmd_ptr, tgt->dma_ptr, out_count);

	if (tgt->cur_cmd == SCSI_CMD_WRITE){
		io_req_t	ior = tgt->ior;
		register int	len = ior->io_count;

		if (len > sc->max_dma_data)
			len = sc->max_dma_data;
		copyout_gap16(	ior->io_data,
				tgt->dma_ptr + (out_count<<1),
				len);
		sii->out_count = len;
#ifdef	DEBUG
		bump(&w_cnt, len);
#endif	DEBUG
	} else
		sii->out_count = 1;/* sanity: causes error */
	sii->cmd_count = out_count;

	/* set dma pointer and counter */
	regs->sii_dma_len     = out_count;
	regs->sii_dma_adr_low = SII_DMADR_LO(tgt->dma_ptr);
	regs->sii_dma_adr_hi  = SII_DMADR_HI(tgt->dma_ptr);

	sii->target = tgt;

	regs->sii_sel_csr = tgt->target_id;
	regs->sii_dma_ctrl = tgt->sync_offset;
	regs->sii_csr |= SII_CSR_SLE /* |SII_CSR_RSE */;

	sii->in_count = in_count;

	switch (tgt->cur_cmd) {
	    case SCSI_CMD_INQUIRY:
	    case SCSI_CMD_REQUEST_SENSE:
	    case SCSI_CMD_MODE_SENSE:
	    case SCSI_CMD_RECEIVE_DIAG_RESULTS:
	    case SCSI_CMD_READ_CAPACITY:
	    case SCSI_CMD_READ:
		sii->script = sii_script_data_in[tgt->flags & TGT_DID_SYNCH];
		sii->error_handler = sii_err_phase_mismatch;
		break;
	    case SCSI_CMD_MODE_SELECT:
		sii->cmd_count = sizeof(scsi_cmd_mode_select_t);
		sii->out_count = 0xff;/*??*/
	    case SCSI_CMD_WRITE:
		sii->script = sii_script_data_out[tgt->flags & TGT_DID_SYNCH];
		sii->error_handler = sii_err_phase_mismatch;
		break;
	    case SCSI_CMD_TEST_UNIT_READY:
		/*
		 * Do the synch negotiation here, unless prohibited
		 * or done already
		 */
		if (tgt->flags & TGT_DID_SYNCH ||
		    scsi_no_synchronous_xfer[sc->masterno] & (1 << tgt->target_id)) {
			sii->script = sii_script_simple_cmd[tgt->flags & TGT_DID_SYNCH];
			sii->error_handler = sii_err_phase_mismatch;
		} else {
			sii->script = sii_script_try_synch;
			sii->error_handler = sii_err_try_synch;
			tgt->flags |= TGT_TRY_SYNCH;
		}
		break;
	    default:
		sii->script = sii_script_simple_cmd[tgt->flags & TGT_DID_SYNCH];
		sii->error_handler = sii_err_phase_mismatch;
	}

	if (tgt->flags & TGT_DID_SYNCH)
		regs->sii_command = SII_CMD_SEL | SII_CMD_ATN;
	else if (tgt->flags & TGT_TRY_SYNCH)
		regs->sii_command = SII_CMD_SEL | SII_CMD_ATN;
	else
		regs->sii_command = SII_CMD_SEL;


}

/*
 * Interrupt routine
 *	Take interrupts from the chip
 *
 * Implementation:
 *	Move along the current command's script if
 *	all is well, invoke error handler if not.
 */
int sii_last_ds, sii_last_cs;

sii_intr(unit,spllevel)

{
	register sii_softc_t	sii;
	register script_t	*scpt;
	register int		cs, ds;
	register sii_regmap_t	*regs;
	extern boolean_t	rz_use_mapped_interface;

#ifdef	DEBUG
	bump(&intr_cnt, 1);
#endif	DEBUG
	if (rz_use_mapped_interface)
		return SII_intr(unit,spllevel);

	sii = sii_softc[unit];

	/* collect status information */
	regs = sii->regs;
	cs = regs->sii_conn_csr;
	ds = regs->sii_data_csr;
sii_last_ds = ds; sii_last_cs = cs;

	/* drop spurious calls */
	if (((cs|ds) & (SII_DTR_DI|SII_DTR_CI)) == 0)
		return;

	/* drop priority */
	splx(spllevel);

sii_last_ds = ds; sii_last_cs = cs;

	/* acknowledge transition */
	SII_ACK(regs,cs,ds,(regs->sii_command & SII_CMD_ATN));

	if ((sii->state & SII_STATE_TARGET) ||
	    (cs & SII_CON_DST))
		return sii_target_intr(sii);	/* probably same */

	scpt = sii->script;

	if (SCRIPT_MATCH(cs,ds) != scpt->condition) {
		(*sii->error_handler)(sii, cs, ds);
		cs = regs->sii_conn_csr;
		ds = regs->sii_data_csr;
	}

	if (SCRIPT_MATCH(cs,ds) == scpt->condition) {
		/*
		 * Perform the appropriate operation,
		 * then proceed
		 */
		if ((*scpt->action)(sii, cs, ds)) {
			/* might have been side effected */
			scpt = sii->script;
			sii->script = scpt + 1;
			regs->sii_command = scpt->command;
		}
	}
/*debug*/ else if (scsi_debug) gimmeabreak();
}


sii_target_intr(sii)
	register sii_softc_t	sii;
{
	panic("SII: TARGET MODE !!!\n");
}

/*
 * All the many little things that the interrupt
 * routine might switch to
 */
boolean_t
sii_identify( sii, cs, ds)
	register sii_softc_t	sii;
{
	sii->regs->sii_csr &= ~(SII_CSR_SLE|SII_CSR_RSE);
	sii->regs->sii_data = SCSI_IDENTIFY;/* xxx allow disc */
	return TRUE;
}

boolean_t
sii_end( sii, cs, ds)
	register sii_softc_t	sii;
{
	register target_info_t	*tgt;
	register io_req_t	ior;
	register sii_regmap_t	*regs = sii->regs;

	/* Check we really are disconnected */
	if (regs->sii_conn_csr & SII_CON_CON)
		cs = sii_wait(&regs->sii_conn_csr, SII_CON_SCH);

	regs->sii_data_csr = 0xffff;
	regs->sii_conn_csr = 0xffff;

	if(!(tgt = sii->target))
		return FALSE;	/* spurious */

	sii->target = 0;
	sii->state &= ~SII_STATE_BUSY;
	if (sii->state & SII_STATE_WANTED) {
		sii->state &= ~SII_STATE_WANTED;
		thread_wakeup(sii);
	}

	tgt->done = SCSI_RET_SUCCESS;

/* XXXX should round robin between tgts! */
	if (ior = tgt->ior) {
		if (ior->io_op & IO_READ) {
			register int	len = ior->io_count,
					max = sii->sc->max_dma_data;
			if (len > max)
				len = max;
			copyin_gap16(tgt->dma_ptr, ior->io_data, len);
#ifdef	DEBUG
			bump(&r_cnt, len);
#endif	DEBUG
		}
		(*tgt->dev_ops->restart)( sii->sc, tgt, TRUE);
	} else if (tgt->flags & TGT_FULLY_PROBED)
		thread_wakeup(tgt);

	return FALSE;
}

boolean_t
sii_get_status( sii, cs, ds)
	register sii_softc_t	sii;
{
	register sii_regmap_t	*regs = sii->regs;
	register scsi2_status_byte_t status;
	register target_info_t	*tgt;
	unsigned int		len;

	tgt = sii->target;
	if ((len = sii->in_count) && tgt->cur_cmd != SCSI_CMD_READ) {
		len -= regs->sii_dma_len;
		copyin_gap16(tgt->dma_ptr, tgt->cmd_ptr, len);
		if (len & 0x1) /* odd byte, left in silo */
			tgt->cmd_ptr[len - 1] = regs->sii_data;
	}
	regs->sii_dma_len = 0;

	SII_ACK(regs,cs,ds,0);	/* sometimes needed ???? */

	/* Either MIS or IBF are ok here. What a mess */
	{
		unsigned int timeo = 1000000;
		while ((regs->sii_data_csr & (SII_DTR_MIS|SII_DTR_IBF)) == 0) {
			if (--timeo == 0) {
printf("sii_getstatus TIMEO with x%x\n", regs->sii_data_csr);
				break;
			}
			delay(1);
		}
#ifdef	DEBUG
		bump(&s_cnt, 1000000 - timeo);
#endif	DEBUG
	}

	status.bits = regs->sii_data;

	if (status.st.scsi_status_code != SCSI_ST_GOOD)
		scsi_error(sii->target, SCSI_ERR_STATUS, status.bits);
		/* XXX needs request sense */

	return TRUE;
}

boolean_t
sii_dma_in( sii, cs, ds)
	register sii_softc_t	sii;
{
	register target_info_t	*tgt;
	register sii_regmap_t	*regs = sii->regs;

	tgt = sii->target;
	regs->sii_dma_len     = sii->in_count;
	regs->sii_dma_adr_low = SII_DMADR_LO(tgt->dma_ptr);
	regs->sii_dma_adr_hi  = SII_DMADR_HI(tgt->dma_ptr);
	wbflush();

	return TRUE;
}

boolean_t
sii_dma_out( sii, cs, ds)
	register sii_softc_t	sii;
{
	register sii_regmap_t	*regs = sii->regs;
	register char		*p;

	p = sii->target->dma_ptr + (sii->cmd_count << 1);
	regs->sii_dma_len = sii->out_count;
	regs->sii_dma_adr_low = SII_DMADR_LO(p);
	regs->sii_dma_adr_hi  = SII_DMADR_HI(p);
	wbflush();

	return TRUE;
}

boolean_t
sii_dosynch( sii, cs, ds)
	register sii_softc_t	sii;
{
	/*
	 * Phase is MSG_OUT here, cmd has not been xferred
	 */
	int			*p, len;
	unsigned short		dmalo, dmahi, dmalen;
	register target_info_t	*tgt;
	register sii_regmap_t	*regs = sii->regs;
	register int		dsr, csr;

	tgt = sii->target;

	tgt->flags |= TGT_DID_SYNCH;	/* only one chance */

	p = (int*) (tgt->dma_ptr + (((regs->sii_dma_len<<1) + 2) & ~3));
	p[0] = SCSI_IDENTIFY | (SCSI_EXTENDED_MESSAGE<<8);
	p[1] = 3 | (SCSI_SYNC_XFER_REQUEST<<8);
	p[2] = sii_to_scsi_period(sii_min_period) |(sii_max_offset<<8);

	dmalen = regs->sii_dma_len;
	dmalo  = regs->sii_dma_adr_low;
	dmahi  = regs->sii_dma_adr_hi;
	regs->sii_dma_len = 6;
	regs->sii_dma_adr_low  = SII_DMADR_LO(p);
	regs->sii_dma_adr_hi = SII_DMADR_HI(p);
	wbflush();

	regs->sii_command = SII_CMD_DMA|SII_CMD_XFER|SII_CMD_ATN|
			    SII_CON_CON|SII_PHASE_MSG_OUT;

	dsr =sii_wait(&regs->sii_data_csr, SII_DTR_DONE);
	csr = regs->sii_conn_csr;

	/* ack and release atn */
	regs->sii_conn_csr = csr;
	regs->sii_data_csr = dsr;
	regs->sii_command = SII_CON_CON|SII_PHASE_MSG_OUT;
	
	/* still wants one more msg, after release of atn */
	regs->sii_data = SCSI_NOP;
	regs->sii_command = SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_OUT;
	dsr = sii_wait(&regs->sii_data_csr, SII_PHASE_MSG_IN);

	/* ack */
	regs->sii_data_csr = dsr;
	regs->sii_command = SII_CON_CON|SII_PHASE_MSG_IN;
	wbflush();

	/* set up dma to receive answer */
	regs->sii_dma_adr_low  = SII_DMADR_LO(p);
	regs->sii_dma_adr_hi = SII_DMADR_HI(p);
	regs->sii_dma_len = 0xff;
	wbflush();
	regs->sii_data_csr = regs->sii_data_csr;
	regs->sii_command = SII_CMD_DMA|SII_CMD_XFER|SII_CON_CON|SII_PHASE_MSG_IN;
	wbflush();

	/* wait for the answer, and look at it */
	len = 1000;
	while (len-- > 0 && (regs->sii_data_csr & SII_DTR_MIS) == 0)
		delay(1);
	if (((dsr = regs->sii_data_csr) & SII_DTR_MIS) == 0) {
		regs->sii_command = SII_CMD_DMA|SII_CMD_XFER|
					    SII_CON_CON|SII_PHASE_MSG_IN;
		dsr = sii_wait(&regs->sii_data_csr, SII_DTR_MIS);
	}
	csr = regs->sii_conn_csr;
	len = 0xff - regs->sii_dma_len;
	if (len < 5 || ((p[0] & 0xff) != SCSI_EXTENDED_MESSAGE)) {
		/* did not like it */
printf(" did not like SYNCH xfer ");
		if (SII_PHASE(dsr) != SII_PHASE_DISC)
			gimmeabreak();/* xxx */
		sii_end(sii, csr, dsr);
		return FALSE;
	}

	/* will do synch */
	tgt->sync_period = scsi_period_to_sii((p[1]>>8)&0xff);
	tgt->sync_offset = regs->sii_data;	/* odd xfer, in silo */
	/* sanity */
	if (tgt->sync_offset > sii_max_offset)
		tgt->sync_offset = sii_max_offset;
	regs->sii_dma_ctrl = tgt->sync_offset;

	/* phase should be command now */
	regs->sii_dma_len = dmalen;
	regs->sii_dma_adr_low = dmalo;
	regs->sii_dma_adr_hi = dmahi;

	sii->error_handler = sii_err_phase_mismatch;
	if (SII_PHASE(dsr) == SII_PHASE_CMD ) {
		/* continue with simple command script */
		sii->script = sii_script_simple_cmd[1];
		return TRUE;
	}
	if (SII_PHASE(dsr) == SII_PHASE_STATUS ) {
		sii_get_status(sii, csr, dsr);
		sii->script = &(sii_script_simple_cmd[1][1]);
		return TRUE;
	}
	panic("sii_dosynch");
	return FALSE;
}

/*
 * Error handlers
 */

/*
 * Handle phase mismatch errors due to slow
 * setting of new phase after ACK
 */
sii_err_phase_mismatch(sii, cs, ds)
	register sii_softc_t	sii;
{
	register sii_regmap_t	*regs = sii->regs;
	register int		match;
	unsigned int		timeo;

	match = sii->script->condition;
retry:
	timeo = 10000;
	do {
		cs = regs->sii_conn_csr;
		ds = regs->sii_data_csr;
		if (SCRIPT_MATCH(cs,ds) == match) {
#ifdef	DEBUG
			bump(&e_cnt, 10000 - timeo);
#endif	DEBUG
			return TRUE;
		}
		delay(1);
	} while (--timeo);
#ifdef	DEBUG
	bump(&e_cnt, 10000);
#endif	DEBUG

	/* Hacks, cuz I donno this chip well */

	/* stuck in status */
	if ((match ==0x47) && (SII_PHASE(ds) == 3)) {
if (scsi_debug)
printf("[STATUS %x %x]", sii_last_ds, sii_last_cs);
		regs->sii_command = 0;
		wbflush();
		regs->sii_command = 0x843;
		wbflush();
		if (scsi_debug < 2) goto retry; else return FALSE;;
	} else
	/* dma in interrupted */
	if ((match == 0x43) && (SII_PHASE(ds) == 1)) {
		register int xferred;
		register char *p;

		if (regs->sii_dma_len <= 1) {
if (scsi_debug)
printf("[DMAINZERO %x %x %x]", regs->sii_dma_len, sii_last_ds, sii_last_cs);
			if (regs->sii_dma_len == 0) {
				(void) sii_dma_in( sii, cs, ds);
				regs->sii_command = 0x8841;
			}
			return FALSE;
		}

		xferred = sii->in_count - regs->sii_dma_len;
		p = (char*)( regs->sii_dma_adr_low | ((regs->sii_dma_adr_hi&3)<<16) );
		p += xferred;
if (scsi_debug)
printf("[DMAIN %x %x %x]", xferred, sii_last_ds, sii_last_cs);
		/* odd bytes are not xferred ?? */
		if (((unsigned)p) & 0x1){
			register short	*oddb;
			oddb = (short*)(sii->buff) + ((unsigned)p-1);/*shifts*/
			*oddb = regs->sii_dma_1st_byte;
		}
		regs->sii_dma_adr_low  = ((unsigned)p);
		regs->sii_dma_adr_hi = ((unsigned)p) << 16;
		wbflush();
		regs->sii_command = 0x8841;
		wbflush();
		if (scsi_debug < 2) goto retry; else return FALSE;;
	} else
	/* dma out interrupted */
	if ((match == 0x43) && (SII_PHASE(ds) == 0)) {
		register int xferred;
		register char *p;

		if (regs->sii_dma_len <= 1) {
if (scsi_debug)
printf("[DMAOUTZERO %x %x %x]", regs->sii_dma_len, sii_last_ds, sii_last_cs);
			if (regs->sii_dma_len == 0) {
				(void) sii_dma_out( sii, cs, ds);
				regs->sii_command = 0x8840;
			}
			return FALSE;
		}

		xferred = sii->out_count - regs->sii_dma_len;
if (scsi_debug)
printf("[DMAOUT %x %x %x %x]", regs->sii_dma_len, sii->out_count, sii_last_ds, sii_last_cs);
		sii->out_count -= xferred;
		p = (char*)( regs->sii_dma_adr_low | ((regs->sii_dma_adr_hi&3)<<16) );
		p += xferred;
		regs->sii_dma_adr_low  = ((unsigned)p);
		regs->sii_dma_adr_hi = ((unsigned)p) << 16;
		wbflush();
		regs->sii_command = 0x8840;
		wbflush();
		if (scsi_debug < 2) goto retry; else return FALSE;;
	}
	/* stuck in cmd phase */
	else if ((SII_PHASE(ds) == 2) && ((match == 0x41) || (match == 0x40))) {
if (scsi_debug)
printf("[CMD %x %x %x %x]",  sii->cmd_count, regs->sii_dma_len, sii_last_ds, sii_last_cs);
		if (regs->sii_dma_len != 0) {
			/* ouch, this hurts */
			register int xferred;
			register char *p;

			xferred = sii->cmd_count - regs->sii_dma_len;
			sii->cmd_count -= xferred;
			p = (char*)( regs->sii_dma_adr_low | ((regs->sii_dma_adr_hi&3)<<16) );
			p += xferred;
			regs->sii_dma_adr_low  = ((unsigned)p);
			regs->sii_dma_adr_hi = ((unsigned)p) << 16;
			wbflush();
			regs->sii_command = 0x8842;
			wbflush();
			if (scsi_debug < 2) goto retry; else return FALSE;;

		}
		SII_ACK(regs,cs,ds,0/*match*/);
		wbflush();
		if (scsi_debug < 2) goto retry; else return FALSE;;
	}
else if (scsi_debug) gimmeabreak();
	return FALSE;
}

/*
 * Handle errors during TEST UNIT READY, which is
 * where we try negotiation of synch xfer
 */
sii_err_try_synch(sii, cs, ds)
	register sii_softc_t	sii;
{
	if (scsi_debug) gimmeabreak();
}


#endif	NSII > 0

