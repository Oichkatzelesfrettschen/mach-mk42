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
 * $Log:	com.c,v $
 * Revision 2.4  91/02/14  14:42:06  mrt
 * 	Merge of dbg's latest working com.c onto the old com.c
 * 	with the new autoconf and other major changes.
 * 	[91/01/28  15:26:13  rvb]
 * 
 * Revision 2.3  91/02/05  17:16:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:42:21  mrt]
 * 
 * Revision 2.2  90/11/26  14:49:26  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Apparently first version is r2.2
 * 	[90/11/25  10:44:41  rvb]
 * 
 * 	Synched 2.5 & 3.0 at I386q (r2.3.1.6) & XMK35 (r2.2)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.3.1.5  90/08/25  15:43:06  rvb
 * 	I believe that nothing of the early Olivetti code remains.
 * 	Copyright gone.
 * 	[90/08/21            rvb]
 * 
 * 	Use take_<>_irq() vs direct manipulations of ivect and friends.
 * 	[90/08/20            rvb]
 * 
 * 		Moved and rewrote much of the code to improve performance.
 * 	     Still suffers from overruns.
 * 	[90/08/14            mg32]
 * 
 * Revision 2.3.1.4  90/07/10  11:43:07  rvb
 * 	Rewrote several functions to look more like vax-BSD dh.c.
 * 	[90/06/25            mg32]
 * 
 * 	New style probe/attach.
 * 	Also com_struct has been radically reworked, ...
 * 	[90/06/15            rvb]
 * 
 * Revision 2.3.1.3  90/02/28  15:49:12  rvb
 * 	Fix numerous typo's in Olivetti disclaimer.
 * 	[90/02/28            rvb]
 * 
 * Revision 2.3.1.2  90/01/08  13:32:00  rvb
 * 	Add Olivetti copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 2.3.1.1  89/12/21  18:01:29  rvb
 * 	Changes from Ali Ezzet.
 * 
 * Revision 2.2.0.0  89/07/17  10:39:30  rvb
 * 	New from Olivetti.
 * 
 */

#include <com.h>
#if NCOM > 0

#ifdef	MACH_KERNEL
#include <sys/types.h>
#include <sys/time.h>
#include <device/conf.h>
#include <device/errno.h>
#include <device/tty.h>
#include <device/io_req.h>
#else	MACH_KERNEL
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/tty.h>
#endif	MACH_KERNEL

#include <i386/ipl.h>
#include <i386/pio.h>
#include <i386at/atbus.h>
#include <i386at/comreg.h>

extern void timeout(), ttrstrt();

int comprobe(), comattach(), comintr(), comstart();
void comparam();
#ifdef	MACH_KERNEL
int comstop(), comgetstat(), comsetstat();
#endif	MACH_KERNEL

int (*comintrs[])() = {
	comintr, 0};
struct isa_driver comdriver = {
	comprobe, 0, comattach, "com", 0, 0, 0};

struct tty com_tty[NCOM];
struct isa_dev *cominfo[NCOM];
int commodem[NCOM];
int comcarrier[NCOM] = {0, 0,};

#ifndef	PORTSELECTOR
#define ISPEED	B9600
#define IFLAGS	(EVENP|ODDP|ECHO|CRMOD)
#else
#define ISPEED	B4800
#define IFLAGS	(EVENP|ODDP)
#endif

u_short divisorreg[] = {
	0,	2304,	1536,	1047,		/*     0,    50,    75,   110*/
	857,	 768,	 576,	 384,	 192,	/*   134.5, 150,   200,   300,   600*/
	 96,	  64,		  48,		/*  1200,  1800,  2000,  2400 */
      		  24,	  	  12,		/*  3600,  4800,  7200,  9600 */
	  6,	   3,	   2};			/* 19200, 38400, 56000 */

comprobe(dev)
struct isa_dev *dev;
{
	caddr_t	addr = dev->dev_addr;
	int	unit = dev->dev_unit;

	if ((unit < 0) || (unit > NCOM)) {
		printf("com %d out of range\n", unit);
		return(0);
	}
	outb(INTR_ENAB(addr), 0);
	return(!inb(INTR_ENAB(addr)));
}

comattach(dev)
struct isa_dev *dev;
{
	u_char	unit = dev->dev_unit;
	caddr_t	addr = dev->dev_addr;

	take_dev_irq(dev);
	printf("com%d: port = %x, spl = %d, pic = %d. (DOS COM%d)\n",
		unit, dev->dev_addr, dev->dev_spl, dev->dev_pic, unit+1);

	cominfo[unit] = dev;
/*	comcarrier[unit] = addr->dev_flags;*/
	commodem[unit] = 0;

	outb(INTR_ENAB(addr), 0);
	outb(MODEM_CTL(addr), 0);
	while (!(inb(INTR_ID(addr))&1)) {
		inb(LINE_STAT(addr)); 
		inb(TXRX(addr)); 
		inb(MODEM_STAT(addr)); 
	}
}

comopen(dev, flag, ior)
int dev;
int flag;
#ifdef	MACH_KERNEL
io_req_t ior;
#else	MACH_KERNEL
#endif	MACH_KERNEL
{
	int		unit = minor(dev);
	struct isa_dev	*isai;
	struct tty	*tp;
	int		s;

	if (unit >= NCOM || (isai = cominfo[unit]) == 0 || isai->dev_alive == 0)
		return(ENXIO);
	tp = &com_tty[unit];
#ifndef	MACH_KERNEL
	if (tp->t_state & TS_XCLUDE && u.u_uid != 0)
		return(EBUSY);
#endif	MACH_KERNEL

	if ((tp->t_state & (TS_ISOPEN|TS_WOPEN)) == 0) {
		ttychars(tp);
		tp->t_addr = isai->dev_addr;
		tp->t_dev = dev;
		tp->t_oproc = comstart;
#ifdef	MACH_KERNEL
		tp->t_stop = comstop;
		tp->t_getstat = comgetstat;
		tp->t_setstat = comsetstat;
#endif	MACH_KERNEL
#ifndef	PORTSELECTOR
		if (tp->t_ispeed == 0) {
#else
			tp->t_state |= TS_HUPCLS;
#endif	PORTSELECTOR
			tp->t_ispeed = ISPEED;
			tp->t_ospeed = ISPEED;
			tp->t_flags = IFLAGS;
			tp->t_state &= ~TS_BUSY;
#ifndef	PORTSELECTOR
		}
#endif	PORTSELECTOR
	}
/*rvb	tp->t_state |= TS_WOPEN; */
	if ((tp->t_state & TS_ISOPEN) == 0)
		comparam(unit);
#ifdef	MACH_KERNEL
	s = spltty();
	if (!comcarrier[unit])	/* not originating */
		tp->t_state |= TS_CARR_ON;
	else {
		int modem_stat = inb(MODEM_STAT(tp->t_addr));
		if (modem_stat & iRLSD)
			tp->t_state |= TS_CARR_ON;
		else
			tp->t_state &= ~TS_CARR_ON;
		fix_modem_state(unit, modem_stat);
	} 
	splx(s);
	return (char_open(dev, tp, flag, ior));
#else	MACH_KERNEL
	tp->t_state |= TS_CARR_ON;
	return ((*linesw[tp->t_line].l_open)(dev, tp));
#endif	MACH_KERNEL
}

comclose(dev, flag)
int dev;
int flag;
{
	struct tty	*tp = &com_tty[minor(dev)];
	caddr_t		addr = tp->t_addr;

#ifndef	MACH_KERNEL
	(*linesw[tp->t_line].l_close)(tp);
#endif	MACH_KERNEL
	ttyclose(tp);
	if (tp->t_state&TS_HUPCLS || (tp->t_state&TS_ISOPEN)==0) { 
		outb(INTR_ENAB(addr), 0);
		outb(MODEM_CTL(addr), 0);
		tp->t_state &= ~TS_BUSY;
#ifdef	MACH_KERNEL
		commodem[minor(dev)] = 0;
#else	MACH_KERNEL
		tp->t_ispeed = 0;
#endif	MACH_KERNEL
	}
	return 0;
}

#ifdef	MACH_KERNEL
comread(dev, ior)
int	dev;
io_req_t ior;
{
	return char_read(&com_tty[minor(dev)], ior);
}

comwrite(dev, ior)
int	dev;
io_req_t ior;
{
	return char_write(&com_tty[minor(dev)], ior);
}

comportdeath(dev, port)
dev_t		dev;
mach_port_t	port;
{
	return (tty_portdeath(&com_tty[minor(dev)], port));
}

io_return_t
comgetstat(dev, flavor, data, count)
dev_t		dev;
int		flavor;
int		*data;		/* pointer to OUT array */
unsigned int	*count;		/* out */
{
	io_return_t	result = D_SUCCESS;
	int		unit = minor(dev);

	switch (flavor) {
	case TTY_MODEM:
		fix_modem_state(unit, inb(MODEM_STAT(cominfo[unit]->dev_addr)));
		*data = commodem[unit];
		*count = 1;
		break;
	default:
		result = tty_get_status(&com_tty[unit], flavor, data, count);
		break;
	}
	return (result);
}

io_return_t
comsetstat(dev, flavor, data, count)
dev_t		dev;
int		flavor;
int *		data;
unsigned int	count;
{
	io_return_t	result = D_SUCCESS;
	int 		unit = minor(dev);
	caddr_t		dev_addr = cominfo[unit]->dev_addr;
	int		s;

	switch (flavor) {
	case TTY_SET_BREAK:
		s = spltty();
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) | iSETBREAK);
		splx(s);
		break;
	case TTY_CLEAR_BREAK:
		s = spltty();
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) & ~iSETBREAK);
		splx(s);
		break;
	case TTY_MODEM:
		{
			int	old_modem, new_modem;

			s = spltty();
			old_modem = commodem[unit];
			new_modem = *data;
			if ((old_modem & TM_DTR) == 0 && (new_modem & TM_DTR)) {
				/* set dtr (and RTS) */
				outb(MODEM_CTL(dev_addr), iOUT2|iDTR|iRTS);
				new_modem |= TM_RTS;
			} else if ((old_modem & TM_DTR) && (new_modem & TM_DTR) == 0) {
					/* clear dtr */
				outb(MODEM_CTL(dev_addr), iOUT2|iRTS);
			}
			commodem[unit] = new_modem;
			splx(s);
			break;
		}
	default:
		result = tty_set_status(&com_tty[unit], flavor, data, count);
		if (result == D_SUCCESS && flavor == TTY_STATUS)
			comparam(unit);
		return (result);
	}
	return (D_SUCCESS);
}
#else	MACH_KERNEL
comread(dev, uio)
int dev;
struct uio *uio;
{
	struct tty *tp= &com_tty[minor(dev)];
	return ((*linesw[tp->t_line].l_read)(tp, uio));
}

comwrite(dev, uio)
int dev;
struct uio *uio;
{
	struct tty *tp= &com_tty[minor(dev)];
	return ((*linesw[tp->t_line].l_write)(tp, uio));
}

comioctl(dev, cmd, addr, mode)
int dev;
int cmd;
caddr_t addr;
int mode;
{
	int			error;
	int			unit = minor(dev);
	struct tty		*tp = &com_tty[unit];
	caddr_t 		dev_addr = cominfo[unit]->dev_addr;

	error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, addr,mode);
	if (error >= 0)
		return(error);
	error = ttioctl(tp, cmd, addr,mode);
	if (error >= 0) {
		if (cmd == TIOCSETP || cmd == TIOCSETN || cmd == TIOCLBIS ||
		    cmd == TIOCLBIC || cmd == TIOCLSET)
			comparam(unit);
		return (error);
	}
	switch (cmd) {
	case TIOCSBRK:
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) | iSETBREAK); 
		break;
	case TIOCCBRK:
		outb(LINE_CTL(dev_addr), inb(LINE_CTL(dev_addr)) & ~iSETBREAK); 
		break;
	case TIOCSDTR:
		outb(MODEM_CTL(dev_addr), iOUT2|iDTR|iRTS); 
		break;
	case TIOCCDTR:
		outb(MODEM_CTL(dev_addr), iOUT2|iRTS); 
		break;
	case TIOCMSET:
	case TIOCMBIS:
	case TIOCMBIC:
	case TIOCMGET:
		uprintf("modem control not yet implemented\n");
	default:
		return(ENOTTY);
	}
	return(0);
}
#endif	MACH_KERNEL

comintr(unit)
int unit;
{
	register struct tty	*tp = &com_tty[unit];
	caddr_t			addr = cominfo[unit]->dev_addr;
	static char 		comoverrun = 0;
	char			c, line, intr_id;
	int			modem_stat, line_stat;

	while (!((intr_id=inb(INTR_ID(addr))) & 1))
	    switch (intr_id) { 
		case MODi: 
		    /* modem change */
			modem_stat = inb(MODEM_STAT(addr));
#ifdef	MACH_KERNEL
			if (!(commodem[unit] & TM_CAR) && (modem_stat & iRLSD)) {
				(void) ttymodem(tp, 1);
			} else if ((commodem[unit] & TM_CAR) && !(modem_stat & iRLSD)) {
				if (ttymodem(tp, 0) == 0) {
					/* turn off */
				}
			}
			fix_modem_state(unit, modem_stat);
#else	MACH_KERNEL
			printf("com%d: modem %x\n", unit, modem_stat);
#endif	MACH_KERNEL
			break;

		case TRAi:
			tp->t_state &= ~(TS_BUSY|TS_BUSY);
#ifdef	MACH_KERNEL
			tt_write_wakeup(tp);
#else	MACH_KERNEL
			if (tp->t_line)
				(*linesw[tp->t_line].l_start)(tp);
			else
#endif	MACH_KERNEL
				comstart(tp);
			break;
		case RECi:
			if (tp->t_state&TS_ISOPEN) {
				c = inb(TXRX(addr));
#ifdef	MACH_KERNEL
				ttyinput(c, tp);
#else	MACH_KERNEL
				line = inb(LINE_STAT(addr));
				if (!(line&iPE && tp->t_flags&(EVENP|ODDP))) {
					if (line&iOR && !comoverrun) {
						printf("com%d: overrun\n", unit);
						comoverrun = 1;
					} 
					if (line&(iFE|iBRKINTR))
					/*
					 * At framing error (break) generate
					 * a null (in raw mode, for getty), or a
					 * interrupt (in cooked/cbreak mode).
					 */
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
					c = (tp->t_flags&RAW?0:tp->t_intrc);
#endif	MACH_KERNEL
					(*linesw[tp->t_line].l_rint)(c, tp);
				}
#endif	MACH_KERNEL
			} else
#ifdef	MACH_KERNEL
			tt_open_wakeup(tp);
#else	MACH_KERNEL
			wakeup((caddr_t)&tp->t_rawq);
#endif	MACH_KERNEL
			break;
		case LINi: 
			line_stat = inb(LINE_STAT(addr));
#ifdef	MACH_KERNEL
			if ((line_stat & iPE) != 0 &&
			    (tp->t_flags & (EVENP | ODDP))) {
				/* parity error */
			} else if (line_stat & (iFE | iBRKINTR)) {
				/* framing error or break */
				ttyinput(tp->t_breakc, tp);
			}
#else	MACH_KERNEL
			printf("com%d: line %x\n", unit, line_stat);
#endif	MACH_KERNEL
			break;
		}
}

void
comparam(unit)
register int unit;
{
	struct tty	*tp = &com_tty[unit];
	caddr_t		addr = tp->t_addr;
	int		s = spltty();
	int		mode;

        if (tp->t_ispeed == B0) {
		tp->t_state |= TS_HUPCLS;
		outb(MODEM_CTL(addr), iOUT2);
		commodem[unit] = 0;
		splx(s);
		return;
	}

	outb(LINE_CTL(addr), iDLAB);
	outb(BAUD_LSB(addr), divisorreg[tp->t_ispeed] & 0xff);
	outb(BAUD_MSB(addr), divisorreg[tp->t_ispeed] >>8);

	if (tp->t_flags & (RAW|LITOUT|PASS8))
		mode = i8BITS;
	else
		mode = i7BITS | iPEN;
	if (tp->t_flags & EVENP)
		mode |= iEPS;
	if (tp->t_ispeed == B110)
		/*
		 * 110 baud uses two stop bits -
		 * all other speeds use one
		 */
		mode |= iSTB;

	outb(LINE_CTL(addr), mode);

	outb(INTR_ENAB(addr), iTX_ENAB|iRX_ENAB|iMODEM_ENAB|iERROR_ENAB);
	outb(MODEM_CTL(addr), iDTR|iRTS|iOUT2);
	commodem[unit] |= (TM_DTR|TM_RTS);
        splx(s);
}

comstart(tp)
struct tty *tp;
{
	char nch;

	/*
	 * The line below is a hack.  It seems that under SLIP, we sometimes lose
	 * interrupts, sigh.  Someday, this should be fixed.  It also may be that
	 * only certain machines manifest this problem.
	 */
	comintr(minor(tp->t_dev));

	if (tp->t_state & (TS_TIMEOUT|TS_TTSTOP|TS_BUSY)) {
		return(0);
	}
	if (tp->t_outq.c_cc <= TTLOWAT(tp)) {
#ifdef	MACH_KERNEL
		tt_write_wakeup(tp);
#else	MACH_KERNEL
		if (tp->t_state & TS_ASLEEP) {
			tp->t_state &= ~TS_ASLEEP;
			wakeup ((caddr_t)&tp->t_outq);
		}
		if (tp->t_wsel) {
			selwakeup(tp->t_wsel, tp->t_state & TS_WCOLL);
			tp->t_wsel = 0;
			tp->t_state &= ~TS_WCOLL;
		}
#endif	MACH_KERNEL
	}
	if (!tp->t_outq.c_cc) {
		return(0);
	}
#ifdef	MACH_KERNEL
	nch = getc(&tp->t_outq);
	if ((tp->t_flags & LITOUT) == 0 && (nch & 0200)) {
	    timeout(ttrstrt, (char *)tp, (nch & 0x7f) + 6);
	    tp->t_state |= TS_TIMEOUT;
	    return;
	}
	outb(TXRX(tp->t_addr), nch);
	tp->t_state |= TS_BUSY;
#else	MACH_KERNEL
	if (tp->t_flags & (RAW|LITOUT))
		nch = ndqb(&tp->t_outq,0);
	else {
		nch = ndqb(&tp->t_outq, 0200);
		if (!nch) {
			nch = getc(&tp->t_outq);
			timeout(ttrstrt,(caddr_t)tp,(nch&0x7f)+6);
			tp->t_state |= TS_TIMEOUT;
			return(0);
		}
	}
	if (nch) {
		outb(TXRX(tp->t_addr), getc(&tp->t_outq));
		tp->t_state |= TS_BUSY;
	} else
		tp->t_state &= ~TS_BUSY;
#endif	MACH_KERNEL
	return(0);
}

/*
 * Set receive modem state from modem status register.
 */
fix_modem_state(unit, modem_stat)
int	unit, modem_stat;
{
	int	stat = 0;

	if (modem_stat & iCTS)
	    stat |= TM_CTS;	/* clear to send */
	if (modem_stat & iDSR)
	    stat |= TM_DSR;	/* data set ready */
	if (modem_stat & iRI)
	    stat |= TM_RNG;	/* ring indicator */
	if (modem_stat & iRLSD)
	    stat |= TM_CAR;	/* carrier? */

	commodem[unit] = (commodem[unit] & ~(TM_CTS|TM_DSR|TM_RNG|TM_CAR))
				| stat;
}

#ifdef	MACH_KERNEL
comstop(tp, flags)
register struct tty *tp;
int	flags;
{
	if ((tp->t_state & TS_BUSY) && (tp->t_state & TS_TTSTOP) == 0)
	    tp->t_state |= TS_FLUSH;
}
#else	MACH_KERNEL
comstop(tp, flag)
struct tty *tp;
{
	int s = spltty();

	if ((tp->t_state&TS_BUSY) && (!(tp->t_state&TS_TTSTOP)))
		tp->t_state |= TS_FLUSH;
	splx(s);
}
#endif	MACH_KERNEL

compr(unit)
{
	compr_addr(cominfo[unit]->dev_addr);
	return(0);
}

compr_addr(addr)
{
	printf("TXRX(%x) %x, INTR_ENAB(%x) %x, INTR_ID(%x) %x, LINE_CTL(%x) %x,\n\
MODEM_CTL(%x) %x, LINE_STAT(%x) %x, MODEM_STAT(%x) %x\n",
	TXRX(addr), inb(TXRX(addr)),
	INTR_ENAB(addr), inb(INTR_ENAB(addr)),
	INTR_ID(addr), inb(INTR_ID(addr)),
	LINE_CTL(addr), inb(LINE_CTL(addr)),
	MODEM_CTL(addr), inb(MODEM_CTL(addr)),
	LINE_STAT(addr), inb(LINE_STAT(addr)),
	MODEM_STAT(addr), inb(MODEM_STAT(addr)));
}
#endif NCOM
