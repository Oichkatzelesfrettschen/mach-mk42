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
 * $Log:	dz_hdw.c,v $
 * Revision 2.6  91/02/14  14:33:44  mrt
 * 	In interrupt routine, drop priority as now required.
 * 	[91/02/12  12:45:12  af]
 * 
 * 	Modified dz_param never to use 7bits per char.
 * 	Now we can use the serial lines even in non-raw mode,
 * 	which means we can login, for instance.
 * 	[90/12/31            af]
 * 
 * Revision 2.5  91/02/05  17:40:35  mrt
 * 	Added author notices
 * 	[91/02/04  11:13:03  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:10:54  mrt]
 * 
 * Revision 2.4  91/01/08  16:18:17  rpd
 * 	Modified dz_param never to use 7bits per char.
 * 	Now we can use the serial lines even in non-raw mode,
 * 	which means we can login, for instance.
 * 	[90/12/31            af]
 * 
 * Revision 2.3  90/12/05  23:31:02  af
 * 	Extensive additions for modem support, pmaxen.
 * 	Still to be tested: autoanswer modems.
 * 	[90/12/03  23:16:10  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:37:41  af
 * 	Created, from the DEC specs:
 * 	"DECstation 3100 Desktop Workstation Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 28, 1990.
 * 	and
 * 	"DECstation 5000/200 KN02 System Module Functional Specification"
 * 	Workstation Systems Engineering, Palo Alto, CA. Aug 27, 1990.
 * 	[90/09/03            af]
 */
/*
 *	File: dz_hdw.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Hardware-level operations for the DZ Serial Line Driver
 */

#include <dz.h>
#if	NDZ > 0

#include <mach_kdb.h>

#include <mips/PMAX/dz_defs.h>
#include <mips/mips_cpu.h>
#include <mips/PMAX/kn01.h>
#include <mips/PMAX/screen_defs.h>


/* this is ok both for rcv (char) and xmt (csr) */
#define	LINEOF(x)	(((x) >> 8) & 0x3)

/*
 * Driver status
 */
struct dz_softc {
	dz_regmap_t	*regs;
	unsigned short	breaks;
	unsigned short	fake;	/* missing rs232 bits */
	int		polling_mode;
	unsigned short	prev_msr;
	char		softCAR;
} dz_softc_data[NDZ];

typedef struct dz_softc *dz_softc_t;

dz_softc_t	dz_softc[NDZ];

dz_softCAR(unit, line, on)
{
	if (on)
		dz_softc[unit]->softCAR |= 1<<line;
	else
		dz_softc[unit]->softCAR &= ~(1 << line);
}

struct pseudo_dma dz_pdma[NDZ*NDZLINE];

short	dz_speeds[] =
	{ 0, DZ_LPAR_50, DZ_LPAR_75, DZ_LPAR_110, DZ_LPAR_134_5, DZ_LPAR_150,
	  0, DZ_LPAR_300, DZ_LPAR_600, DZ_LPAR_1200, DZ_LPAR_1800, DZ_LPAR_2400,
	  DZ_LPAR_4800, DZ_LPAR_9600, DZ_LPAR_MAX_SPEED, 0 };


/*
 * Definition of the driver for the auto-configuration program.
 */

int	dz_probe(), dz_attach(), dz_intr();

caddr_t	dz_std[] = { 0 };
struct	bus_device *dz_info[1];
struct	bus_driver dz_driver = 
        { dz_probe, 0, dz_attach, 0, dz_std, "dz", dz_info,};

/*
 * Adapt/Probe/Attach functions
 */
static dz_regmap_t	*dz_regs = (dz_regmap_t*)PHYS_TO_K1SEG(KN01_SYS_DZ);
static boolean_t 	dz_full_modem = FALSE;

set_dz_address( regs, has_modem)
	dz_regmap_t	*regs;
	boolean_t	has_modem;
{
	dz_regs = regs;
	dz_full_modem = has_modem;
}

dz_probe( xxx, ui)
	struct bus_device *ui;
{
	int             unit = ui->unit;
	dz_softc_t      sc;
	register struct pseudo_dma *pdp;
	register struct tty **tp;
	register int cntr;
	extern dz_tint();
	static int probed_once = 0;

	/*
	 * If this is not there we are toast 
	 */
	if (check_memory(dz_regs, 0)) {
		dprintf("%s", "dz_probe!\n");
		prom_restart();
	}
	if (probed_once++)
		return 1;

	sc = &dz_softc_data[unit];
	dz_softc[unit] = sc;
	sc->regs = dz_regs;

	pdp = &dz_pdma[unit*NDZLINE];
	tp = &dz_tty[unit*NDZLINE];
	for (cntr = 0; cntr < NDZLINE; cntr++) {
		pdp->p_addr = dz_regs;
		pdp->p_arg = (int)*tp;
		pdp->p_fcn = dz_tint;
		pdp++, tp++;
	}

	/* pmaxen lack many modem bits */
	if (!dz_full_modem)
		sc->fake = DZ_MSR_CTS3|DZ_MSR_DSR3|DZ_MSR_CD3|
			   DZ_MSR_CTS2|DZ_MSR_CD2;

	dz_regs->dz_tcr = 0;/* disable all lines, drop RTS,DTR */
	return 1;
}

boolean_t dz_timer_started = FALSE;

dz_attach(ui)
	register struct bus_device *ui;
{
	int unit = ui->unit;
	extern dz_scan();
	extern int tty_inq_size;
	int i;

	/* We only have 4 ttys, but always at 9600
	 * Give em a lot of room
	 */
	tty_inq_size = 2048;
	for (i = 0; i < (NDZ*NDZLINE); i++)
		ttychars(dz_tty[i]);

	if (!dz_timer_started) {
		dz_timer_started = TRUE;
		dz_scan();
	}

	if (SCREEN_ISA_CONSOLE()) {
		printf("\nsl0: "); lk201_attach(unit);
		printf("\nsl1: "); mouse_attach(unit);
		printf("\nsl2: \nsl3: ");
		if (rcline == 3) printf("( rconsole )");
	}
}

/*
 * Polled I/O (debugger)
 */
dz_pollc(unit, on)
	boolean_t		on;
{
	dz_softc_t		sc = dz_softc[unit];
	if (on) {
		sc->polling_mode++;
		screen_on_off(unit, TRUE);
	} else
		sc->polling_mode--;
}

/*
 * Interrupt routine
 */
dz_intr(unit,spllevel)
{
	dz_softc_t		sc = dz_softc[unit];
	register dz_regmap_t	*regs = sc->regs;
	register short		csr;

	csr = regs->dz_csr;

	if (csr & DZ_CSR_TRDY) {
		int             c = dz_tint(unit, LINEOF(csr));

		if (c == -1) {
			/* no more data for this line */
			regs->dz_tcr &= ~(1 << LINEOF(csr));
		} else {
			regs->dz_tbuf = (c & 0xff) | sc->breaks;
			/* and leave it enabled */
		}
	}
	if (sc->polling_mode)
		return;

	while (regs->dz_csr & DZ_CSR_RDONE) {
		short           c = regs->dz_rbuf;
		int		oldspl;

		oldspl = splx(spllevel);
		dz_rint(unit, LINEOF(c), c);
		splx(oldspl);
	}
}

/*
 * Get a char from a specific DZ line
 */
dz_getc( unit, line, wait, raw )
	boolean_t	wait;
	boolean_t	raw;
{
	dz_softc_t      sc = dz_softc[unit];
	int             s = spltty();
	register dz_regmap_t *regs = sc->regs;
	unsigned short  c;
	int             rl;

again:
	/*
	 * wait till something in silo 
	 */
	while ((regs->dz_csr & DZ_CSR_RDONE) == 0 && wait)
		delay(10);
	c = regs->dz_rbuf;

	/*
	 * check if right line. For keyboard, rconsole is ok too 
	 */
	rl = LINEOF(c);
	if (wait && (line != rl) &&
	    !((line == 0) && rcline == rl))
		goto again;
	/*
	 * bad chars not ok 
	 */
	if ((c & (DZ_RBUF_PERR | DZ_RBUF_OERR | DZ_RBUF_FERR)) && wait)
		goto again;

	splx(s);

	/*
	 * if nothing found return -1 
	 */
	if ( ! (c & DZ_RBUF_VALID))
		return -1;

	if ((rl == DZ_LINE_KEYBOARD) && !raw && SCREEN_ISA_CONSOLE())
		return lk201_rint(SCREEN_CONS_UNIT(), c, wait, sc->polling_mode);
	else
		return c & DZ_RBUF_CHAR;
}

/*
 * Put a char on a specific DZ line
 */
void
dz_putc( unit, line, c )
{
	dz_softc_t      sc = dz_softc[unit];
	register dz_regmap_t *regs = sc->regs;
	int             s = spltty();

	/*
	 * do not change the break status of other lines 
	 */
	c = (c & 0xff) | sc->breaks;

	/*
	 * Xmit line info only valid if TRDY,
	 * but never TRDY if no xmit enabled
	 */
	if ((regs->dz_tcr & DZ_TCR_LNENB) == 0)
		goto select_it;

	while ((regs->dz_csr & DZ_CSR_TRDY) == 0)
		delay(100);

	/*
	 * see if by any chance we are already on the right line 
	 */
	if (LINEOF(regs->dz_csr) == line)
		regs->dz_tbuf = c;
	else {
		unsigned short tcr;
select_it:
		tcr = regs->dz_tcr;
		regs->dz_tcr = (1 << line) | (tcr & 0xff00);
		wbflush();

		do
			delay(2);
		while ((regs->dz_csr & DZ_CSR_TRDY) == 0 ||
		       (LINEOF(regs->dz_csr) != line));

		regs->dz_tbuf = c;
		wbflush();

		/* restore previous settings */
		regs->dz_tcr = tcr;
	}

	splx(s);
}

/*
 * and now for a more canonical interface
 */
int cnunit = 0;		/* which DZ owns the 'console' */
int cnline = 0;		/* which line of the DZ */
int rcline = 3;		/* alternate, "remote console" line */

void rcputc(c)
{
	if (rcline)
		dz_putc( cnunit, rcline, c);
}


void cnputc(c)
{
	if (SCREEN_ISA_CONSOLE()) {
		/* this does its own rcputc */
		screen_blitc(SCREEN_CONS_UNIT(), c);
	} else {
		rcputc(c);
		dz_putc( cnunit, cnline, c);/* insist on a console still */
	}
	if (c == '\n')
		cnputc('\r');
}

cngetc()
{
	return dz_getc( cnunit, cnline, TRUE, FALSE);
}

#include <mach_kdb.h>
#if	MACH_KDB
cnmaygetc()
{
	return dz_getc( cnunit, cnline, FALSE, FALSE);
}
#endif	MACH_KDB

 
dz_param(unit)
	register int unit;
{
	register struct tty *tp;
	register dz_regmap_t *dz_addr;
	register int lpr;
 
	tp = dz_tty[unit];

	/*
	 * Do not let user fool around with kbd&mouse
	 */
	if (SCREEN_ISA_CONSOLE() && unit <= 1) {
		tp->t_ispeed = tp->t_ospeed = B4800;
		tp->t_flags |= TF_LITOUT;
	}

	dz_addr = dz_pdma[unit].p_addr;
	dz_addr->dz_csr = DZ_CSR_MSE|DZ_CSR_RIE|DZ_CSR_TIE;
	if (tp->t_ispeed == 0) {
		(void) dz_mctl(unit, TM_HUP, DMSET);	/* hang up line */
		return;
	}
/* 19200/38400 here */
	lpr = dz_speeds[tp->t_ispeed] | (unit&DZ_LPAR_LINE) | DZ_LPAR_ENABLE;
	lpr |= DZ_LPAR_8BITS;

	if ((tp->t_flags & (TF_ODDP|TF_EVENP)) == TF_ODDP)
		lpr |= DZ_LPAR_ODD_PAR;

	if (tp->t_ispeed == B110)
		lpr |= DZ_LPAR_STOP;
	dz_addr->dz_lpr = lpr;
}
 
/*
 * This is a total mess: not only are bits spread out in
 * various registers, but we have to fake some for pmaxen.
 */
unsigned
dz_mctl(dev, bits, how)
	dev_t dev;
	int bits, how;
{
	register dz_regmap_t *regs;
	register int unit;
	register int tcr, msr, brk, n_tcr, n_brk;
	int b, s;
	dz_softc_t      sc;

	unit = minor(dev);

	/* no modem support on lines 0 & 1 */
/* XXX break on 0&1 */
	if ((unit & 2) == 0)
		return TM_LE|TM_DTR|TM_CTS|TM_CAR|TM_DSR;

	b = 1 ^ (unit & 1);	/* line 2 ? */
	
	sc = dz_softc[unit>>2];
	regs = sc->regs;
	s = spltty();

	tcr = ((regs->dz_tcr | (sc->fake>>4)) & 0xf00) >> (8 + b*2);
	brk = (sc->breaks >> (8 + (unit&3))) & 1;	/* THE break bit */

	n_tcr = (bits & (TM_RTS|TM_DTR)) >> 1;
	n_brk = (bits & TM_BRK) >> 9;

	/* break transitions, must 'send' a char out */
	bits = (brk ^ n_brk) & 1;

	switch (how) {
	case DMSET:
		tcr = n_tcr;
		brk = n_brk;
		break;

	case DMBIS:
		tcr |= n_tcr;
		brk |= n_brk;
		break;

	case DMBIC:
		tcr &= ~n_tcr;
		brk = 0;
		break;

	case DMGET:
		msr = ((regs->dz_msr|sc->fake) & 0xf0f) >> (b*8);
		(void) splx(s);
		return  (tcr<<1)|/* DTR, RTS */
			((msr&1)<<5)|/* CTS */
			((msr&2)<<7)|/* DSR */
			((msr&0xc)<<4)|/* CD, RNG */
			(brk << 9)|/* BRK */
			TM_LE;
	}
	n_tcr =	(regs->dz_tcr & ~(3 << (8 + b*2))) |
		(tcr << (8 + b*2));

	regs->dz_tcr = n_tcr;
	sc->fake = (sc->fake & 0xf0f) | (n_tcr<<4&0xf000);

	sc->breaks = (sc->breaks & ~(1 << (8 + (unit&3)))) |
		    (brk << (8 + (unit&3)));
	if(bits) dz_putc( unit>>2, unit&3, 0);/* force break, now */
	(void) splx(s);
	return 0;/* useless to compute it */
}

/*
 * Periodically look at the CD signals:
 * they do not generate interrupts.
 */
dz_scan()
{
	register i;
	register dz_regmap_t *regs;
	register msr;
	register struct tty *tp;
 
	for (i = 0; i < NDZ; i++) {
		dz_softc_t      sc = dz_softc[i];
		register int	temp;

		if (sc == 0)
			continue;
		regs = sc->regs;

		tp = dz_tty[i * NDZLINE];

		msr = regs->dz_msr | (sc->fake & 0xf0f);
		if (temp = sc->softCAR) {
			if (temp & 0x4)
				msr |= DZ_MSR_CD2;
			if (temp & 0x8)
				msr |= DZ_MSR_CD3;
		}

		/* Lines 0 and 1 have carrier on by definition */
		/* [horrid casts cuz compiler stupid] */
		check_car((char*)tp + 0*sizeof(struct tty), 1);
		check_car((char*)tp + 1*sizeof(struct tty), 1);
		check_car((char*)tp + 2*sizeof(struct tty), msr & DZ_MSR_CD2);
		check_car((char*)tp + 3*sizeof(struct tty), msr & DZ_MSR_CD3);

		/* nothing else to do if no msr transitions */
		if ((temp = sc->prev_msr) == msr)
			continue;
		else
			sc->prev_msr = msr;

		/* see if we have an incoming call */
#define	RING	(DZ_MSR_RI2|DZ_MSR_RI3)
		if ((msr & RING) != (temp & RING)) {
printf("%s %x->%x\n", "Phone RI", temp & RING, msr & RING);
			check_ring((char*)tp + 2*sizeof(struct tty),
				msr & DZ_MSR_RI2, temp & DZ_MSR_RI2);
			check_ring((char*)tp + 3*sizeof(struct tty),
				msr & DZ_MSR_RI3, temp & DZ_MSR_RI3);
		}
	}
	timeout(dz_scan, (caddr_t)0, 2*hz);
}

static dz_hup(tp)
	register struct tty *tp;
{
printf("%s %x\n", "Phone HUP", tp->t_dev);
	dz_mctl(tp->t_dev, TM_DTR, DMBIC);
}

static check_car(tp, car)
	register struct tty *tp;
{
	if (car) {
		/* cancel modem timeout if need to */
		if (car & (DZ_MSR_CD2|DZ_MSR_CD3))
			untimeout(dz_hup, (caddr_t)tp);

		/* I think this belongs in the MI code */
		if (tp->t_state & TS_WOPEN)
			tp->t_state |= TS_ISOPEN;
		/* carrier present */
		if ((tp->t_state & TS_CARR_ON) == 0)
			(void)ttymodem(tp, 1);
	} else if ((tp->t_state&TS_CARR_ON) && ttymodem(tp, 0) == 0)
		dz_mctl( tp->t_dev, TM_DTR, DMBIC);
}

int	dz_ring_timeout	= 60;	/* seconds, patchable */

static check_ring(tp, ring, oring)
	register struct tty *tp;
{
	if (ring == oring)
		return;
	if (ring) {
		dz_mctl( tp->t_dev, TM_DTR, DMBIS);
		/* give it ample time to find the right carrier */
		timeout(dz_hup, (caddr_t)tp, dz_ring_timeout*hz);
	}
}
#endif	NDZ > 0
