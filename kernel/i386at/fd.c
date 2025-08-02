/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log:	fd.c,v $
 * Revision 2.2  91/02/14  14:42:23  mrt
 * 	This file is the logical contatenation of the previous c765.c,
 * 	m765knl.c and m765drv.c, in that order.  
 * 	[91/01/15            rvb]
 * 
 * Revision 2.5  91/01/08  17:33:32  rpd
 * 	Add some 3.0 get/set stat stuff.
 * 	[91/01/04  12:21:06  rvb]
 * 
 * Revision 2.4  90/11/26  14:50:54  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r1.6.1.6) & XMK35 (r2.4)
 * 	[90/11/15            rvb]
 * 
 * Revision 1.6.1.6  90/11/27  13:44:55  rvb
 * 	Synched 2.5 & 3.0 at I386q (r1.6.1.6) & XMK35 (r2.4)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.3  90/08/27  22:01:22  dbg
 * 	Remove include of device/param.h (unnecessary).  Flush ushort.
 * 	[90/07/17            dbg]
 * 
 * Revision 1.6.1.5  90/08/25  15:44:31  rvb
 * 	Use take_<>_irq() vs direct manipulations of ivect and friends.
 * 	[90/08/20            rvb]
 * 
 * Revision 1.6.1.4  90/07/27  11:26:53  rvb
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/07/27            rvb]
 * 
 * Revision 1.6.1.3  90/07/10  11:45:11  rvb
 * 	New style probe/attach.
 * 	NOTE: the whole probe/slave/attach is a crock.  Someone
 * 	who spends the time to understand the driver should do
 * 	it right.
 * 	[90/06/15            rvb]
 * 
 * Revision 2.2  90/05/03  15:45:37  dbg
 * 	Convert for pure kernel.
 *	Optimized fd_disksort iff dp empty.
 * 	[90/04/19            dbg]
 * 
 * Revision 1.6.1.2  90/01/08  13:30:14  rvb
 * 	Add Intel copyright.
 * 	[90/01/08            rvb]
 * 
 * Revision 1.6.1.1  89/10/22  11:34:51  rvb
 * 	Received from Intel October 5, 1989.
 * 	[89/10/13            rvb]
 * 
 * Revision 1.6  89/09/25  12:27:05  rvb
 * 	vtoc.h -> disk.h
 * 	[89/09/23            rvb]
 * 
 * Revision 1.5  89/09/09  15:23:15  rvb
 * 	Have fd{read,write} use stragegy now that physio maps correctly.
 * 	[89/09/06            rvb]
 * 
 * Revision 1.4  89/03/09  20:07:26  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:40:28  gm0w
 * 	Changes for cleanup.
 *
 */
 
/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*	Copyright (c) 1987, 1988 TOSHIBA Corp.		*/
/*		All Rights Reserved			*/

#ident  "@(#)c765.c	1.15 - 88/03/18"

/*******************************************************************
 *
 *	 Toshiba Floppy Driver for UNIX System V R3
 *
 *					June 21, 1988 
 *
 *	Intended Drive Units:
 *		Worldwide - Model No. ND-356  3.5" unformatted 2MB/1MB
 *		UNIX Media Type Name: 2HD512/2DD512/2D512/1D512.
 *
 *		In Japan Only - Model No. ND-355  3.5" unformatted 1.6MB/1MB
 *		UNIX Media Type Name: 2HC1024/2HC512/2HC256/2DD512/2D512/1D512.
 *
 *		Worldwide - Model No. ND-04DT-A  5.25" unformatted 500 KB
 *		UNIX Media Type Name: 2D512/1D512.
 *
 *		In Japan Only - Model No. ND-08DE  5.25" unformatted 1.6MB/1MB
 *		UNIX Media Type Name: 2HC1024/2HC512/2HC256/2DD512/2D512/1D512.
 *
 *		Use with other devices may require modification.
 *
 *	Notes:
 *		For further detail regarding drive units contact 
 *		Toshiba America,Inc. Disk Products Division,
 *		Irvine, CA (714) 583-3000.
 *
 *******************************************************************/


#include <sys/types.h>
#ifdef	MACH_KERNEL
#include <device/buf.h>
#include <device/errno.h>
#include <sys/ioctl.h>
#else	MACH_KERNEL
#include <sys/param.h>
#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/dir.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#endif	MACH_KERNEL
#include <i386/ipl.h>
#include <i386at/atbus.h>
#include <i386at/fdreg.h>
#include <i386at/disk.h>

/*
 * Floppy Partitions.
 *
 *  start sector number , max sector number 
 *
 *                              use cyl 0 / not use cyl 0 
*/
struct	fdpart F2h18[] =  { 0, 2880, 36, 2844 };
struct	fdpart F2q26[] =  { 0, 4160, 52, 4108 };
struct	fdpart F2q15[] =  { 0, 2400, 30, 2370 };
struct	fdpart F2q8[]  =  { 0, 1232, 16, 1216 };
struct	fdpart F2w9[]  =  { 0, 1440, 18, 1422 };
struct	fdpart F2w8[]  =  { 0, 1280, 16, 1264 };
struct	fdpart F2d9[]  =  { 0, 720,  18, 702  };
struct	fdpart F2d8[]  =  { 0, 640,  16, 624  };
struct	fdpart F1d9[]  =  { 0, 360,   9, 351  };
struct	fdpart F1d8[]  =  { 0, 320,   8, 312  };

/*
 * Floppy Device-Table Definitions (drtabs)
 *
 *      Cyls,Head,Sec,SecSiz,spc,part,Mtype,RWFpl,FGpl
 */
struct	fddrtab m765f0[FORMMAX] = {		/* format search table */
	80, 2,18, 512, 36, F2h18,0x08,0x1b,0x6c, /* [1] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /* [2] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /* [3] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /* [4] */
	80, 2, 9, 512, 18,  F2w9,0x88,0x2a,0x50, /* [5] */
	80, 2, 8, 512, 16,  F2w8,0x88,0x2a,0x50, /* [6] */
	40, 2, 9, 512, 18,  F2d9,0xa8,0x2a,0x50, /* [7] */
	40, 2, 8, 512, 16,  F2d8,0xa8,0x2a,0x50, /* [8] */
	40, 1, 9, 512,  9,  F1d9,0xa8,0x2a,0x50, /* [9] */
	40, 1, 8, 512,  8,  F1d8,0xa8,0x2a,0x50, /*[10] */

	 0, 0, 0,   0,  0,     0,   0,   0,   0, /*[11] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /*[12] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /*[13] */
	 0, 0, 0,   0,  0,     0,   0,   0,   0  /*[14] */
};

struct	fddrtab m765f1[FORMMAX] = {		/* format search table */
	 0, 0, 0,   0,  0,     0,   0,   0,   0, /* [1] */
	80, 2,26, 256, 52, F2q26,0x08,0x0e,0x36, /* [2] */
	80, 2,15, 512, 30, F2q15,0x08,0x1b,0x54, /* [3] */
	77, 2, 8,1024, 16,  F2q8,0x08,0x35,0x74, /* [4] */
	80, 2, 9, 512, 18,  F2w9,0x48,0x2a,0x50, /* [5] */
	80, 2, 8, 512, 16,  F2w8,0x48,0x2a,0x50, /* [6] */
	40, 2, 9, 512, 18,  F2d9,0x68,0x2a,0x50, /* [7] */
	40, 2, 8, 512, 16,  F2d8,0x68,0x2a,0x50, /* [8] */
	40, 1, 9, 512,  9,  F1d9,0x68,0x2a,0x50, /* [9] */
	40, 1, 8, 512,  8,  F1d8,0x68,0x2a,0x50, /*[10] */

	40, 2, 9, 512, 18,  F2d9,0x80,0x2a,0x50, /*[11] */
	40, 2, 8, 512, 16,  F2d8,0x80,0x2a,0x50, /*[12] */
	40, 1, 9, 512,  9,  F1d9,0x80,0x2a,0x50, /*[13] */
	40, 1, 8, 512,  8,  F1d8,0x80,0x2a,0x50  /*[14] */
};

struct	fdcmn m765fdata = { 0xdf,0x02,0x12,0xff };		/* fdc parameter */

struct	fdtree fd0tree[]   = { 
   &m765f0[0], 2,0,18,  &fd0tree[1],  &fd0tree[2], /* [ 0] 2hd ?	  */
   &m765f0[0], 0,0, 0,            0,            0, /* [ 1] 2hd media      */

   &m765f0[4], 2,0, 1,  &fd0tree[3],  &fd0tree[7], /* [ 2] 2dd ?	  */
   &m765f0[4], 2,0, 9,  &fd0tree[4],  &fd0tree[5], /* [ 3] 2dd sec9 ?	  */
   &m765f0[4], 0,0, 0,            0,            0, /* [ 4] 2dd sec9 media */
   &m765f0[5], 2,0, 8,  &fd0tree[6], &fd0tree[17], /* [ 5] 2dd sec8 ?	  */
   &m765f0[5], 0,0, 0,            0,            0, /* [ 6] 2dd sec8 media */

   &m765f0[6], 2,1, 1,  &fd0tree[8], &fd0tree[12], /* [ 7] 2d ?	          */
   &m765f0[6], 2,1, 9,  &fd0tree[9], &fd0tree[10], /* [ 8] 2d sec9 ?	  */
   &m765f0[6], 0,0, 0,            0,            0, /* [ 9] 2d sec9 media  */
   &m765f0[7], 2,1, 8, &fd0tree[11], &fd0tree[17], /* [10] 2d sec8 media  */
   &m765f0[7], 0,0, 0,            0,	        0, /* [11] 2d sec8 media  */

   &m765f0[8], 2,0, 1, &fd0tree[13], &fd0tree[17], /* [12] 1d ?	          */
   &m765f0[8], 2,0, 9, &fd0tree[14], &fd0tree[15], /* [13] 1d sec9 ?	  */
   &m765f0[8], 0,0, 0,            0,            0, /* [14] 1d sec9 media  */
   &m765f0[9], 2,0, 8, &fd0tree[16], &fd0tree[17], /* [15] 1d sec8 media  */
   &m765f0[9], 0,0, 0,            0,	        0, /* [16] 1d sec8 media  */
  	    0, 0,0, 0,            0,            0  /* [17] illegal media  */
};

struct	fdtree fd1tree[]   = { 
   &m765f1[1], 2,0,26,  &fd1tree[1],  &fd1tree[2], /* [ 0] 2hc 256 ?	  */
   &m765f1[1], 0,0, 0,		  0,	        0, /* [ 1] 2hc 256 media  */
   &m765f1[2], 2,0,15,  &fd1tree[3],  &fd1tree[4], /* [ 2] 2hc 512 ?      */
   &m765f1[2], 0,0, 0,		  0,	        0, /* [ 3] 2hc 512 media  */
   &m765f1[3], 2,0, 8,  &fd1tree[5],  &fd1tree[6], /* [ 4] 2hc 1024 ?     */
   &m765f1[3], 0,0, 0,  	  0, 	        0, /* [ 5] 2hc 1024 media */

   &m765f1[4], 2,0, 1,  &fd1tree[7], &fd1tree[11], /* [ 6] 2dd ?	  */
   &m765f1[4], 2,0, 9,  &fd1tree[8],  &fd1tree[9], /* [ 7] 2dd sec9 ?	  */
   &m765f1[4], 0,0, 0,		  0,	        0, /* [ 8] 2dd sec9 media */
   &m765f1[5], 2,0, 8, &fd1tree[10], &fd1tree[21], /* [ 9] 2dd sec8 ?     */
   &m765f1[5], 0,0, 0, 		  0,	        0, /* [10] 2dd sec8 media */

   &m765f1[6], 2,1, 1, &fd1tree[12], &fd1tree[16], /* [11] 2d ?	          */
   &m765f1[6], 2,1, 9, &fd1tree[13], &fd1tree[14], /* [12] 2d sec9 ?	  */
   &m765f1[6], 0,0, 0,		  0,	        0, /* [13] 2d sec9 media  */
   &m765f1[7], 2,1, 8, &fd1tree[15], &fd1tree[21], /* [14] 2d sec8 ?	  */
   &m765f1[7], 0,0, 0, 		  0,	        0, /* [15] 2d sec8 media  */

   &m765f1[8], 2,0, 1, &fd1tree[17], &fd1tree[21], /* [16] 1d ?	          */
   &m765f1[8], 2,0, 9, &fd1tree[18], &fd1tree[19], /* [17] 1d sec9 ?	  */
   &m765f1[8], 0,0, 0,		  0,	        0, /* [18] 1d sec9 media  */
   &m765f1[9], 2,0, 8, &fd1tree[20], &fd1tree[21], /* [19] 1d sec8 ?	  */
   &m765f1[9], 0,0, 0, 		  0,	        0, /* [20] 1d sec8 media  */
	    0, 0,0, 0,		  0,	        0  /* [21] illegal media  */
};

struct	fdtree fd2tree[]   = { 
  &m765f1[10], 2,1, 1,  &fd2tree[1],  &fd2tree[5], /* [ 0] 2d ?	          */
  &m765f1[10], 2,1, 9,  &fd2tree[2],  &fd2tree[3], /* [ 1] 2d sec9 ?	  */
  &m765f1[10], 0,0, 0,		  0,	        0, /* [ 2] 2d sec9 media  */
  &m765f1[11], 2,1, 8,  &fd2tree[4], &fd2tree[10], /* [ 3] 2d sec8 ?	  */
  &m765f1[11], 0,0, 0, 		  0,	        0, /* [ 4] 2d sec8 media  */

  &m765f1[12], 2,0, 1,  &fd2tree[6], &fd2tree[10], /* [ 5] 1d ?	          */
  &m765f1[12], 2,0, 9,  &fd2tree[7],  &fd2tree[8], /* [ 6] 1d sec9 ?	  */
  &m765f1[12], 0,0, 0,		  0,	        0, /* [ 7] 1d sec9 media  */
  &m765f1[13], 2,0, 8,  &fd2tree[9], &fd2tree[10], /* [ 8] 1d sec8 ?	  */
  &m765f1[13], 0,0, 0, 		  0,	        0, /* [ 9] 1d sec8 media  */
	    0, 0,0, 0,		  0,	        0  /* [10] illegal media  */
};

struct fdpart m765part[MAXUNIT*2] = {		/* Floppy partitions data */
	0, 2880, 36, 2844,
	0, 2400, 30, 2370
};
/*
 * The following are static initialization variables
 * which are based on the configuration. These variables
 * MUST NOT CHANGE because the m765 device driver makes
 * most of the calculations based on these variables.
*/
struct fddev  m765dev = {			/* device data table */
	80, 2,18, 512, 0,&m765part[0],0x08,0x1b,0x6c,
	80, 2,15, 512, 0,&m765part[2],0x08,0x1b,0x54,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0
};
struct fdubuf unitbuf[MAXUNIT];			/* unit buffer headers	*/

char *fderr = "FD Error on unit";
char *fdmsg[] = {
	"?",
	"Missing data address mark",
	"Write protected",
	"Sector not found",
	"Data Overrun",				/* Over run error */
	"Uncorrectable data read error",	/* CRC Error */
	"FDC Error",
	"Illegal format type",
	"Drive not ready",
	"diskette not present - please insert",
	"Illegal interrupt type"
};

struct buf	fdrbuf[MAXUNIT];	/* data transfer buffer structures */

struct fdtree  *dchgchk();	/* check unit_change */
struct fdtree  *dchgchk_new();	/* new dchgchk() */	
struct fddrtab *getparm();	/* get fdc parameter table */  
struct fddrtab *getparm_new();	/* new get fdc parameter table */  
unsigned int	residcnt();

int	fdminphys();
int	fdrawio();
int	fdintr(), fdprobe(), fdslave(), fdattach();
int	(*fdintrs[])() = {	fdintr, 0};

struct	isa_driver	fddriver = 
	{fdprobe, fdslave, fdattach, "fd", 0, 0, 0};

int	dmause = 0;
int	m765verify[MAXUNIT] = {1,1};	/* write after read flag */
						/* 0 != verify mode	*/ 
						/* 0 == not verify mode */
char	fddtype = 0;		/* cmos ram data 10h */
unsigned char	kbd_FDsts = 0;	/* fd/prt switch data */
int	first_fdopen_ever = 1;	/* flag for first open */

#ifdef	MACH_KERNEL
extern struct buf *geteblk();
#endif	MACH_KERNEL

fdprobe(ctlr)
struct isa_ctlr *ctlr;
{
	int	spot = (int) ctlr->ctlr_addr + STSREG - CTRLREG;

	if ((inb(spot) & DATAOK) == DATAOK) {
		struct	fdmbuf *mbufh;
		register struct	fdubuf *ubufh,*wubufh;
		register int	cnt0;

		take_ctlr_irq(ctlr);

		mbufh = &(m765dev.d_bufh);
		mbufh->b_unit = 0;			/* set unit_switch */
		mbufh->b_cmd.c_rbmtr = 0;		/* recaribrate/moter flag */
		mbufh->b_cmd.c_intr = CMDRST;		/* interrupt flag */
		wubufh = (struct fdubuf *)mbufh;
		for( cnt0 = 0 ; cnt0 < MAXUNIT ; cnt0++ ){
			ubufh = &unitbuf[cnt0];
			ubufh->b_unit = cnt0;
			ubufh->b_seekaddr = 0;
			ubufh->av_forw = ubufh->av_back = 0;
			mbufh->b_active[cnt0] = FD_IO_IDLE;
			ubufh->b_unitb = wubufh;
			wubufh->b_unitf = ubufh;
			wubufh = ubufh;
		}
		ubufh->b_unitf = mbufh->b_unitf;	/* set unit_buffer_end */
		mbufh->b_rwerr = mbufh->b_seekerr = mbufh->b_rberr = 0;
	
		/* read cmos ram (address 0x10) */
		/* rwrtc(&fddtype, sizeof(fddtype), 0x10, 0); */
		/* fddtype = CMOSread(0x10); */
		fddtype = 0x20;

		if((fddtype&0x0f)==0x04){
			fddtype &= 0xf0;
			fddtype |= fddtype >> 4;
		} else {
			fddtype &= 0x0f;
			fddtype |= fddtype << 4;
		}
		rstout(mbufh,0);	
		specify();

		printf("fdc%d: port = %x, spl = %d, pic = %d.\n",
			ctlr->ctlr_ctlr, ctlr->ctlr_addr, ctlr->ctlr_spl, ctlr->ctlr_pic);
		return(1);
	} else {
		return(0);
	}
}

fdslave(dev)
struct isa_dev		*dev;
{
	int slave = dev->dev_slave;

	return 1;	/* gross hack */
}

fdattach(dev)
struct isa_dev *dev;
{
	int unit = dev->dev_unit;

	printf("fd%d:  port = %x, spl = %d, pic = %d. (controller %d, slave %d)\n",
		unit, dev->dev_addr, dev->dev_spl, dev->dev_pic,
		dev->dev_ctlr, dev->dev_slave);

}

/*****************************************************************************
 *
 * TITLE:	fdopen
 *
 * ABSTRACT:	Open a unit. 
 *
 ****************************************************************************/
fdopen(dev, flag, otyp)
dev_t	dev;
int	flag;			/* not used */
int	otyp;			/* not used */
{
	register struct fddev *dd;
	register struct	fdmbuf *mbufh;
	struct	fdubuf *ubufh;
	register unsigned unit;
#ifdef	MACH_KERNEL
	int error;
#endif	MACH_KERNEL

	dd = &m765dev;
	mbufh = &(dd->d_bufh);
	unit = UNIT(dev);
#ifdef	MACH_KERNEL
	error = D_SUCCESS;
#else	MACH_KERNEL
	u.u_error = OK;
#endif	MACH_KERNEL
	if (unit < MAXUNIT){
		ubufh = mbufh->b_unitf;
		while(ubufh->b_unit != unit)
			ubufh = ubufh->b_unitf;	
#ifdef	MACH_KERNEL
		error = fdchk(dd,mbufh,ubufh,dev);
#else	MACH_KERNEL		
		fdchk(dd,mbufh,ubufh,dev);
#endif	MACH_KERNEL
	} else
#ifdef	MACH_KERNEL
		error = ENXIO;
#else	MACH_KERNEL
		u.u_error = ENXIO;		/* No such unit */
#endif	MACH_KERNEL
#ifdef	MACH_KERNEL
	return (error);
#else	MACH_KERNEL
	return(u.u_error);
#endif	MACH_KERNEL
}
/************************************************************************
 *
 *	floppy disk media type check and dooropen check
 *
 ************************************************************************/
#ifdef	MACH_KERNEL
int
#endif	MACH_KERNEL
fdchk(dd,mbufh,ubufh,dev)
register struct	fddev *dd;
register struct	fdmbuf *mbufh;
register struct	fdubuf *ubufh;
dev_t	dev;
{
	unsigned int	unit,x;
	int	rtn;
	unsigned char	kbd_sts;

	x = SPL();
	openchk(mbufh);
	mbufh->b_cmd.c_devflag |= FDMCHK;
	chkbusy(mbufh);
	splx(x);
	/*
	kbc_ctrl(GETFDSTS,0,&kbd_sts);	** read keyboard_io (0xb4 command) **
	*/
	kbd_sts = 0x1e;
	if(kbd_sts != kbd_FDsts){
		for( x = 0 ; x < MAXUNIT ; x++ )
			dd->d_drtab[x].dr_type &= ~OKTYPE; 
		kbd_FDsts = kbd_sts;
	}


	unit = UNIT(dev);
	mbufh->b_cmd.c_stsflag |= MTRFLAG;
	mtr_on(mbufh,unit);			/* motor on	*/

	rtn = fdopenchk(dd,mbufh,unit);        

	mbufh->b_cmd.c_stsflag &= ~MTRFLAG;
	mtr_on(mbufh,unit);			/* motor on	*/

	x = SPL();
	openfre(mbufh);
	splx(x);

	if((!(rtn))        /* if (rtn == OK) */
		&&((ext2dchk(unit))||(!(dd->d_drtab[unit].dr_type & OKTYPE)))){
		mediatype(dd,dev,mbufh,ubufh);
	}	
#ifdef	MACH_KERNEL
	return (rtn ? ENXIO : D_SUCCESS);
#endif	MACH_KERNEL
}

/**************************************************************************
 *
 *	Check door open routine
 *
 **************************************************************************/
fdopenchk(dd,mbufh,unit)
register struct	fddev	 *dd;
register struct	fdmbuf *mbufh;
register unsigned int	 unit;
{
	struct	fddrtab *dr;
	int	opendata;

	dr = &dd->d_drtab[unit];
	if(!(extchk(unit))){              
		opendata = inb(VFOREG);
		mbufh->b_unit = unit;
		if(extfdtype(mbufh,unit)){   
			if((!(ext2dchk(unit))) 
 	  		  &&((opendata & OPENBIT)||(!(dr->dr_type & OKTYPE)))){
				dr->dr_type &= ~OKTYPE;
				if(!(rbrate( mbufh , 0x08 , unit )))
					fdseek(mbufh , 0x08 , unit , 2);
					if(!(inb(VFOREG) & OPENBIT))
						return(OK);
			} else
				return(OK);
		} 
	}
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	u.u_error = ENXIO;
#endif	MACH_KERNEL
	return(ERROR);
}

/**************************************************************************
 *
 *	Check external floppy disk drive type routine
 *
 **************************************************************************/
extfdtype(mbufh,unit)
register struct	fdmbuf *mbufh;
register unsigned int	 unit;
{
	if(((kbd_FDsts&FDCHG)&&unit)||((!(kbd_FDsts&FDCHG))&&(!unit))){
		if(!fddtype){
			if(rbrate( mbufh , NMSEEK , unit ))
				return(0);
			fdseek( mbufh , NMSEEK , unit , 51 );
			fdseek( mbufh , NMSEEK , unit , 11 );
			fdseek( mbufh , NMSEEK , unit , 10 );
			if(sds(unit) & TRACK0)
				fddtype = EXT2D;
			else
				fddtype = EXT2HD;
		}
	}
	return(1);
}
/**************************************************************************
 *
 *	Media type check routine
 *
 *	set floppy parameter to m765dev.d_drtab[unit].
 *
 **************************************************************************/
mediatype(dd,dev,mbufh,ubufh)
struct	fddev *dd;
dev_t	dev;
register struct	fdmbuf *mbufh;
struct	fdubuf *ubufh;
{
	register struct	fdtree *tree;
	struct	fddrtab *dr;
	register struct	buf	*wbp;

	dr = &dd->d_drtab[UNIT(dev)];
	tree = dchgchk_new(dev);        /* MODIFIED BY KOICHI YAMADA */
	wbp = geteblk(BLKSIZE);		/* get struct buf area */
	while(tree->fd_yes){
		m765sweep(dd, UNIT(dev), tree->fd_parm);
		mbufh->b_cmd.c_rbmtr &= ~(1<<(RBSHIFT+UNIT(dev)));
		++mbufh->b_rwerr;
		clearbp(wbp,dev);
		wbp->b_flags = (B_READ|B_VERIFY);
		wbp->b_bcount  = dr->dr_secsiz;
		wbp->b_pfcent = (tree->cylno*dr->dr_spc)
				+(tree->headno*dr->dr_nsec)+tree->secno-1;
		setqueue(dd,mbufh,ubufh,wbp,UNIT(dev));
		biowait(wbp);
		if(wbp->b_error)
			tree = tree->fd_no;
		else
			tree = tree->fd_yes;
	}
	brelse(wbp);
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	u.u_error = OK;
#endif	MACH_KERNEL
	if(tree->fd_parm){
		m765sweep(dd, UNIT(dev), tree->fd_parm);
		dr->dr_type |= OKTYPE;
	}
	else
		fddtype = 0;
}


/*********************************************************************
 *
 *	set cylno,headno,secno to command table
 *
 *********************************************************************/
clearbp(bp,dev)
register struct buf *bp;
register dev_t	dev;
{
	bp->b_flags &= ~B_DONE;
	bp->b_dev = dev;
#ifdef	MACH_KERNEL
	bp->b_error = 0; bp->b_resid = 0;
#else	MACH_KERNEL
	bp->b_proc = 0; bp->b_error = 0; bp->b_resid = 0;
#endif	MACH_KERNEL
}
/*****************************************************************************
 *
 * TITLE:	fdclose
 *
 * ABSTRACT:	Close a unit.
 *
 *	Called on last close. mark the unit closed and not-ready.
 *
 * 	Unix doesn't actually "open" an inode for rootdev, swapdev or pipedev.
 *	If UNIT(swapdev) != UNIT(rootdev), then must add code in init() to 
 *	"open" swapdev.  These	devices should never be closed.
 *
 *****************************************************************************/
fdclose(dev, flag, otyp, offset)
dev_t	dev;		/* major, minor numbers */
int	flag;		/* not used */
int	otyp;		/* not used */
off_t	offset;		/* not used */
{
	register struct fddev	*dd;
	register struct fdmbuf *mbufh;
	register struct fdubuf *ubufh;
	unsigned int	x;
	unsigned int	unit = UNIT(dev);
	extern		dev_t	rootdev,swapdev;

	dd = &m765dev;
	mbufh = &(dd->d_bufh);
#ifdef	MACH_KERNEL
#else	MACH_KERNEL
	if ((dev == rootdev) || (dev == swapdev)) {
		return(0);		/* never close these */
	}
#endif	MACH_KERNEL
	/* Clear the bit.
	 * If last close of drive insure drtab queue is empty before returning.
	 */
	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	x = SPL();
	while(ubufh->av_forw != NULL) {
		mbufh->b_active[unit] |= FD_IO_WAIT;
		sleep(&mbufh->b_active[unit], PRIBIO);
	}
	splx(x);
#ifdef	MACH_KERNEL
	return (0);
#else	MACH_KERNEL
	close(0);
#endif	MACH_KERNEL
}
/*****************************************************************************
 *
 * TITLE:	fdstrategy
 *
 * ABSTRACT:	Queue an I/O Request, and start it if not busy already.
 *
 *	Reject request if unit is not-ready.
 *
 *	Note:	check for not-ready done here ==> could get requests
 *		queued prior to unit going not-ready.
 *		not-ready status to those requests that are attempted
 *		before a new volume is inserted.  Once a new volume is
 *		inserted, would get good I/O's to wrong volume.
 *
 * CALLS:	m765strchk(),iodone(),setqueue()
 *
 * CALLING ROUTINES:	fdread (indirectly, thru physio)
 *			fdwrite (indirectly, thru physio)
 *
 ****************************************************************************/
fdstrategy(bp)
struct buf *bp;	/* buffer header */
{
	register struct fddev	 *dd;
	register struct	fdubuf *ubufh;
	register struct	fdmbuf *mbufh;
	unsigned int	unit;

	/* initializations */
	dd = &m765dev;
	unit = UNIT(bp->b_dev);
	mbufh = &(dd->d_bufh);
	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	bp->b_error = 0;
	if(m765strchk(dd,bp,bp->b_dev)){	/* check parameter error */
		biodone(bp);
		return(0);
	}
	setqueue(dd,mbufh,ubufh,bp,unit);	/* set queue to buffer */
	return(0);
}
/**************************************************************************
 *
 *	strategy error check routine
 *
 **************************************************************************/
m765strchk(dd,bp,dev)
register struct fddev	 *dd;
register struct buf *bp;
register dev_t	dev;
{
	struct	fddrtab *dr,*sdr;
	struct	fdpart *p;
	unsigned        bytes_left;
	daddr_t		secno;
	int	type;
	daddr_t		psecno;
	unsigned int	unit;
	unit = UNIT(dev);

	dr = &dd->d_drtab[unit];
	/* set b_resid to b_bcount because we haven't done anything yet */
	bp->b_resid = bp->b_bcount;

	if (!(dr->dr_type & OKTYPE)){
		bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
	/*	cmn_err(CE_CONT,"%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]); */
		printf("%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]);
		return(ERROR);
	}

	if(type = MEDIATYPE(bp->b_dev)){
		if(!(sdr = getparm_new(type,unit))){
			bp->b_flags |= B_ERROR;
			bp->b_error = EIO;
		/*	cmn_err(CE_CONT,"%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]); */
			printf("%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]);
			return(ERROR);
		}
		if(cmpparm(dr,sdr)){
			bp->b_flags |= B_ERROR;
			bp->b_error = EIO;
		/*	cmn_err(CE_CONT,"%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]); */
			printf("%s %d : %s\n",fderr,unit,fdmsg[ILLFMT]);
			return(ERROR);
		}
	}
	p = &dr->dr_part[PARTITION(bp->b_dev)];
	/*
	 * Figure "secno" from b_blkno. Adjust sector # for partition.
	 *
	 * If reading just past the end of the device, it's
	 * End of File.  If not reading, or if read starts further in
	 * than the first sector after the partition, it's an error.
	 *
	 * secno is logical blockno / # of logical blocks per sector */
	secno = bp->b_blkno;
	secno = secno * NBPSCTR / dr->dr_secsiz;
	psecno = p->p_nsec;
	if(CYLFLAG(dev)&&(dr->dr_ncyl==80))
		psecno -= (dr->dr_spc * 3);
	if (secno >= psecno) {
		if (!((bp->b_flags & B_READ) && (secno == psecno))){
			/* off the deep end */
			bp->b_flags |= B_ERROR;
			bp->b_error = ENXIO;
		}
		return(ERROR);
	}
/* At this point, it is no longer possible to directly return from strategy.
   We now set b_resid to the number of bytes we cannot transfer because
   they lie beyond the end of the request's partition.  This value is 0
   if the entire request is within the partition. */
	bytes_left = (p->p_nsec - secno) * dr->dr_secsiz;
	bp->b_resid = ((bp->b_bcount <= bytes_left)
			? 0 : (bp->b_bcount - bytes_left));

	secno += p->p_fsec;
	bp->b_pfcent = secno;
	return(OK);
}
/***************************************************************************
 *
 *	set queue to buffer
 *
 ***************************************************************************/
setqueue(dd,mbufh,ubufh,bp,unit)
register struct	fddev	*dd;
register struct	fdmbuf *mbufh;
register struct	fdubuf *ubufh;
struct buf *bp;
unsigned int	unit;
{
	unsigned int	x;
	int	count;

	x = SPL();
	openchk(mbufh);					/* openning check */
	mbufh->b_cmd.c_devflag |= STRCHK;
	fd_disksort(ubufh,bp,dd->d_drtab[unit].dr_nsec); /* queue the request */
	/*
	 * If no requests are in progress, start this one up.  Else
	 * leave it on the queue, and fdintr will call m765io later.
	 */
	for ( count = MAXUNIT ; count > 0 ; count--){
		if(mbufh->b_active[count-1] != FD_IO_IDLE)
			break;
	}
	mbufh->b_active[unit] |= FD_IO_BUSY;
	if(!(count)){
		/*
		 * Set b_active flag to indicate controller is busy.  No one
		 * should make requests to the controller while this is set.
	 	 * It gets cleared right above here, usually when fdintr
		 * calls m765io to process next request and the queue is empty.
		 */
		dmaget();
		m765io(dd,ubufh,unit);
	}
	splx(x);
}

/***************************************************************************
 *
 *	check io_busy routine
 *
 ***************************************************************************/
chkbusy(mbufh)
register struct	fdmbuf *mbufh;
{
	while(mbufh->b_cmd.c_devflag & STRCHK ){
		mbufh->b_cmd.c_devflag |= STRWAIT;
		sleep(&(mbufh->b_cmd.c_devflag),PZERO);
	} 
}
/***************************************************************************
 *
 *	free open routine
 *
 ***************************************************************************/
openstr(mbufh)
register struct	fdmbuf *mbufh;
{
	mbufh->b_cmd.c_devflag &= ~STRCHK;
	if(mbufh->b_cmd.c_devflag & STRWAIT){
		mbufh->b_cmd.c_devflag &= ~STRWAIT;
		wakeup(&(mbufh->b_cmd.c_devflag));
	}
}

/***************************************************************************
 *
 *	check fdopen() routine
 *
 ***************************************************************************/
openchk(mbufh)
register struct	fdmbuf *mbufh;
{
	while(mbufh->b_cmd.c_devflag & FDMCHK ){
		mbufh->b_cmd.c_devflag |= FDWAIT;
		sleep(&(mbufh->b_cmd.c_devflag),PZERO);
	} 
}

/***************************************************************************
 *
 *	free fdopen() routine
 *
 ***************************************************************************/
openfre(mbufh)
register struct	fdmbuf *mbufh;
{
	mbufh->b_cmd.c_devflag &= ~FDMCHK;
	if(mbufh->b_cmd.c_devflag & FDWAIT){
		mbufh->b_cmd.c_devflag &= ~FDWAIT;
		wakeup(&(mbufh->b_cmd.c_devflag));
	}
}
/*****************************************************************************
 *
 * TITLE:	m765io
 *
 * ABSTRACT:	Start handling an I/O request.
 *
 ****************************************************************************/
m765io(dd,ubufh,unit)
struct fddev *dd;
struct	fdubuf *ubufh;
register unsigned int	unit;
{
	extern  int(m765iosub)();
	register struct fdmbuf *mbufh;
	register struct buf *bp;

	mbufh = &(dd->d_bufh);
	bp = ubufh->av_forw;		/* move bp to mbufh->b_buf */
	mbufh->b_buf = bp;
	mbufh->b_unit = unit;

	mbufh->b_xferaddr  = bp->b_un.b_addr;
	mbufh->b_xfercount = bp->b_bcount - bp->b_resid;
	mbufh->b_sector    = bp->b_pfcent;

	mbufh->b_cmd.c_stsflag |= MTRFLAG;
	if(!(mtr_start(mbufh,unit)))
		timeout(m765iosub,dd,HZ);
	else
		m765iosub(dd);
}
/****************************************************************************
 *
 *	m765io subroutine
 *
 ****************************************************************************/
m765iosub(dd)
struct fddev  *dd;
{
	register struct fdmbuf *mbufh;
	register struct fdubuf *ubufh;
	register unsigned int	unit;
	struct fddrtab *dr;
	int	startsec,lastsec;

	mbufh = &(dd->d_bufh);
	unit = mbufh->b_unit;

	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	dr = &dd->d_drtab[unit];

	rwcmdset(dr,mbufh,unit,mbufh->b_sector);
	if(mbufh->b_buf->b_flags&B_FORMAT)
		goto skipchk;
	startsec = (mbufh->b_cmd.c_rwdata[3] * dr->dr_nsec) 
			+ mbufh->b_cmd.c_rwdata[4];
	lastsec = startsec+(mbufh->b_xfercount/dr->dr_secsiz)-1;
	if(lastsec > dr->dr_spc){
		mbufh->b_xferdma = 
			(dr->dr_spc-startsec+1) * dr->dr_secsiz;
	} else
skipchk:	mbufh->b_xferdma = mbufh->b_xfercount;
	if(!(mbufh->b_cmd.c_rbmtr & (1<<(RBSHIFT+unit))))
       		mbufh->b_status=rbirate(mbufh,unit,dr->dr_type);
	else {
		if(ubufh->b_seekaddr != mbufh->b_cmd.c_saddr)
			mbufh->b_status =
				fdiseek(dd,unit,mbufh->b_cmd.c_saddr);
		else {
			mbufh->b_status = outicmd(dd,mbufh,unit);
		}
	}
	if(mbufh->b_status)
		intrerr0(dd,mbufh,unit);
	return;
}
/***************************************************************************
 *
 *	read / write / format / verify command set to command table
 *
 ***************************************************************************/
rwcmdset(dr,mbufh,unit,sector)
register struct fddrtab *dr;
register struct fdmbuf *mbufh;
unsigned unit;
daddr_t	sector;
{
	short	resid;
	register struct fdcmd *cmd;

	cmd = &mbufh->b_cmd;
	switch(mbufh->b_buf->b_flags&(B_FORMAT|B_VERIFY|B_READ|B_WRITE)){
	case B_VERIFY|B_WRITE:	/* VERIFY after WRITE */
		cmd->c_rwdata[0] = RDMV;
		break;

	case B_FORMAT:
		cmd->c_dcount = FMTCNT; 
		cmd->c_rwdata[0] = FMTM;
		cmd->c_saddr = sector / dr->dr_spc;
		resid = sector % dr->dr_spc;
		cmd->c_rwdata[1] = unit|((resid/dr->dr_nsec)<<2);
		cmd->c_rwdata[2] = 
			((struct fmttbl *)mbufh->b_buf->b_un.b_addr)->s_type;
		cmd->c_rwdata[3] = dr->dr_nsec;
		cmd->c_rwdata[4] = dr->dr_fgpl;
		cmd->c_rwdata[5] = FMTDATA;
		break;

	case B_WRITE:
	case B_READ:
	case B_READ|B_VERIFY:
		cmd->c_dcount = RWCNT;
		if(mbufh->b_buf->b_flags&B_READ)
			if(mbufh->b_buf->b_flags&B_VERIFY)
				cmd->c_rwdata[0] = RDMV;
			else
				cmd->c_rwdata[0] = RDM;
		else {
			cmd->c_rwdata[0] = WTM;	/* format or write */
		}
		resid = sector % dr->dr_spc;
		cmd->c_rwdata[3] = resid / dr->dr_nsec;
		cmd->c_rwdata[1] = unit|(cmd->c_rwdata[3]<<2);
		cmd->c_saddr = 
		cmd->c_rwdata[2] = sector / dr->dr_spc;
		cmd->c_rwdata[4] = (resid % dr->dr_nsec) + 1;
		if((cmd->c_rwdata[5] = dr->dr_secsiz>>8) == 4)
			cmd->c_rwdata[5] = 3;
		cmd->c_rwdata[6] = dr->dr_nsec;
		cmd->c_rwdata[7] = dr->dr_rwgpl;
		cmd->c_rwdata[8] = m765fdata.f_dtl;
		break;
	}
}

/*****************************************************************************
 *
 * TITLE:	m765dmause
 *
 * ABSTRACT:	Use dmause to "break" up the request into
 *		physically contiguous chunks.
 *
 ****************************************************************************/
m765dmause(bp)
register struct buf *bp;	/* buffer header */
{
	/* physdmause(fdstrategy, bp, 1, BLKSIZE); */
	/* dma_breakup(fdstrategy, bp); */
}

/*****************************************************************************
 *
 * TITLE:	fdread
 *
 * ABSTRACT:	"Raw" read.  Use physio().
 *
 * CALLS:	m765breakup (indirectly, thru physio)
 *
 ****************************************************************************/
fdread(dev, uio)
register dev_t	dev;
struct uio *uio;
{ 
#ifdef	MACH_KERNEL
	/* no need for page-size restriction */
	return (block_io(fdstrategy, minphys, uio));
#else	MACH_KERNEL
	/* physio(m765dmause, NULL, dev, B_READ); */
	return(physio(fdstrategy, &fdrbuf[UNIT(dev)], dev, B_READ, fdminphys, uio));
#endif	MACH_KERNEL
}

/*****************************************************************************
 *
 * TITLE:	fdwrite
 *
 * ABSTRACT:	"Raw" write.  Use physio().
 *
 * CALLS:	m765breakup (indirectly, thru physio)
 *
 ****************************************************************************/
fdwrite(dev, uio)
register dev_t	dev;
struct uio *uio;
{
#ifdef	MACH_KERNEL
	/* no need for page-size restriction */
	return (block_io(fdstrategy, minphys, uio));
#else	MACH_KERNEL
	/* physio(m765dmause, NULL, dev, B_WRITE); */
	return(physio(fdstrategy, &fdrbuf[UNIT(dev)], dev, B_WRITE, fdminphys, uio));
#endif	MACH_KERNEL
}
/*****************************************************************************
 *
 * TITLE:	fdminphys
 *
 * ABSTRACT:	Trim buffer length if buffer-size is bigger than page size
 *
 * CALLS:	physio
 *
 ****************************************************************************/
fdminphys(bp)
struct buf	*bp;
{
	if (bp->b_bcount > PAGESIZ)
		bp->b_bcount = PAGESIZ;
}

#ifdef	MACH_KERNEL
		/* IOC_OUT only and not IOC_INOUT */
io_return_t fdgetstat(dev, flavor, data, count)
	dev_t		dev;
	int		flavor;
	int *		data;		/* pointer to OUT array */
	unsigned int	*count;		/* OUT */
{
	io_return_t	result;
	register struct fddev	*dd = &m765dev;
	register struct fdmbuf	*mbufh = &(dd->d_bufh);
	register unsigned 	unit = UNIT(dev);

	switch (flavor) {
	case V_GETPARMS:
		if (*count < sizeof (struct disk_parms)/sizeof (int))
		    return (D_INVALID_OPERATION);
		*count = sizeof (struct disk_parms)/sizeof(int);
return (fd_getparms(dd, dev, data));
#if	0
	    {
		struct disk_parms	*dp;
		int			unit = UNIT(dev);
		int			part = PARTITION(dev);

		if (*count < sizeof (struct disk_parms))
		    return (D_INVALID_OPERATION);

		dp = (struct disk_parms *) data;
		*count = sizeof (struct disk_parms)/sizeof(int);

		dp->dp_type = DPT_WINI;
		dp->dp_heads = hdparams[unit].nheads;
		dp->dp_cyls = hdparams[unit].ncylinders;
		dp->dp_sectors  = hdparams[unit].nsecpertrack;
  		dp->dp_dosheads = rlparams[unit].nheads;
		dp->dp_doscyls = rlparams[unit].ncylinders;
		dp->dp_dossectors  = rlparams[unit].nsecpertrack;
		dp->dp_secsiz = SECSIZE;
		dp->dp_ptag = partition_struct[unit][part].p_tag;
		dp->dp_pflag =partition_struct[unit][part].p_flag;
		dp->dp_pstartsec = partition_struct[unit][part].p_start;
		dp->dp_pnumsec =partition_struct[unit][part].p_size;
		result = D_SUCCESS;
		break;
	    }
#endif	0
	}
	return (result);
}

		/* IOC_VOID or IOC_IN or IOC_INOUT */
/*ARGSUSED*/
io_return_t fdsetstat(dev, flavor, data, count)
	dev_t		dev;
	int		flavor;
	int *		data;
	unsigned int	count;
{
/*	io_return_t		result = D_SUCCESS;*/
	io_return_t		errcode = D_SUCCESS;
	int			unit = UNIT(dev);
	register struct fddev	*dd = &m765dev;
	register struct fdmbuf	*mbufh = &(dd->d_bufh);

	switch (flavor) {
	case V_SETPARMS:    /* Caller wants reset_parameters */
		return(fd_setparms(dd,mbufh,unit,*(int *)data));

	case V_FORMAT:
		return(fd_format(dd,dev,data));

	case V_VERIFY:	/* cmdarg : 0 == not verify mode */
			/*	    0 != verify mode     */
		m765verify[unit] = *(int *)data;
		return(0);
	}
	return (errcode);
}
#else	MACH_KERNEL
/*****************************************************************************
 *
 * TITLE:	fdioctl
 *
 * ABSTRACT:	m765 driver special functions.
 *
 * CALLING ROUTINES:	kernel
 *
 ****************************************************************************/
int 
fdioctl(dev, cmd, cmdarg, flag)
dev_t	dev;		/* major, minor numbers */
int	cmd;		/* command code */
int	*cmdarg;	/* user structure with parameters */
int	flag;		/* not used */
{
	register struct fddev	*dd = &m765dev;
	register struct fdmbuf	*mbufh = &(dd->d_bufh);
	register unsigned 	unit = UNIT(dev);

	switch (cmd) {
		case V_SETPARMS:    /* Caller wants reset_parameters */
			return(fd_setparms(dd,mbufh,unit,*cmdarg));
		case V_GETPARMS:    /* Caller wants device parameters */
			return(fd_getparms(dd,dev,cmdarg));
		case V_FORMAT:
			return(fd_format(dd,dev,cmdarg));
		case V_VERIFY:	/* cmdarg : 0 == not verify mode */
				/*	    0 != verify mode     */
			m765verify[unit] = *cmdarg;
			return(0);
		default:
			/*u.u_error = EINVAL;*/	
			return(EINVAL);
	}
}
#endif	MACH_KERNEL
/****************************************************************************
 *
 *	set fd parameters 
 *
 ****************************************************************************/
int
fd_setparms(dd,mbufh,unit,cmdarg)
register struct fddev *dd;
struct fdmbuf *mbufh;
register unsigned int unit;
long cmdarg;
{
	register struct fddrtab *dr;
	int	type;
	struct fddrtab *fdparm;
	unsigned int x;

	dr = &dd->d_drtab[unit];
	mbufh->b_cmd.c_rbmtr &= ~(1<<(RBSHIFT+unit));
	extfdtype(&dd->d_bufh,unit);
	if(type = MEDIATYPE(cmdarg)){
		if((fdparm = getparm_new(type,unit))
				== (struct fddrtab *)ERROR){
			return(EINVAL);
		}
	} else {
	   	if(dunitchk(unit)){
			if(ext2dchk(unit))
				fdparm = &m765f1[10];
			else
				fdparm = &m765f1[2];
		} else
			fdparm = &m765f0[0];
	}
	x = SPL();
	openchk(mbufh);
	mbufh->b_cmd.c_devflag |= FDMCHK;
	chkbusy(mbufh);
	m765sweep(dd, unit, fdparm);
	dr->dr_type |= OKTYPE;
	openfre(mbufh);
	splx(x);
	return(0);
}
/****************************************************************************
 *
 *	get fd parameters 
 *
 ****************************************************************************/
int
fd_getparms(dd,dev,cmdarg)
struct fddev *dd;
dev_t	dev;		/* major, minor numbers */
int	*cmdarg;
{
	register struct fddrtab *dr;
	struct disk_parms *diskp;
	register struct fdpart *p;

	diskp = (struct disk_parms *)cmdarg;

	dr = &dd->d_drtab[UNIT(dev)];
	p = &dr->dr_part[PARTITION(dev)];
	if(dr->dr_type & OKTYPE){
		diskp->dp_type = DPT_FLOPPY;
		diskp->dp_heads = dr->dr_nrhead;
		diskp->dp_sectors = dr->dr_nsec;
		diskp->dp_secsiz = dr->dr_secsiz;
		diskp->dp_pstartsec = p->p_fsec;
		if(CYLFLAG(dev)&&(dr->dr_ncyl==80)){
			diskp->dp_cyls = CYL77;
			diskp->dp_pnumsec = 
				p->p_nsec - (dr->dr_spc * 3);
		} else {
			diskp->dp_cyls = dr->dr_ncyl;
			diskp->dp_pnumsec = p->p_nsec;
		}
		/* Put parameters into user segment */
		/*
		if(copyout((caddr_t)&diskp, cmdarg, (unsigned)sizeof(struct disk_parms))){
			u.u_error = EFAULT;
		}
		*/
	} else {
		/* u.u_error = ENXIO; */
		return(ENXIO);
	}
	return(0);
}
/****************************************************************************
 *
 *	format command
 *
 ****************************************************************************/
fd_format(dd,dev,cmdarg)
struct fddev *dd;
dev_t	dev;		/* major, minor numbers */
int	*cmdarg;
{
	register struct fddrtab *dr;
	register struct buf *bp;
	register daddr_t track;
	union  io_arg  *varg;
	u_short	num_trks;

	dr = &dd->d_drtab[UNIT(dev)];
	if(!(dr->dr_type & OKTYPE)) {
		/* u.u_error = EINVAL; */
		return(EINVAL);	
	}
	/*
	if(copyin(cmdarg,(caddr_t)&varg,(unsigned)sizeof(union io_arg))){
		u.u_error = EFAULT; 
	}
	*/
	varg = (union io_arg *)cmdarg;

	num_trks = varg->ia_fmt.num_trks;
	track = (daddr_t)(varg->ia_fmt.start_trk*dr->dr_nsec);
	varg->ia_fmt.start_trk += dr->dr_part[PARTITION(dev)].p_fsec/dr->dr_nsec;
	if((track + (num_trks*dr->dr_nsec))>dr->dr_part[PARTITION(dev)].p_nsec){
		/* u.u_error = EINVAL; */
		return(EINVAL);
	}
	bp = geteblk(BLKSIZE);		/* get struct buf area */
	for( ; num_trks > 0 ; num_trks-- , track += dr->dr_nsec ){
		clearbp(bp,dev);
		bp->b_flags = B_FORMAT;	
		bp->b_bcount  = dr->dr_nsec * FMTID;
		bp->b_blkno = (daddr_t)(track * dr->dr_secsiz / NBPSCTR);
		if(makeidtbl(bp->b_un.b_addr,dr,
				varg->ia_fmt.start_trk++,varg->ia_fmt.intlv)) {
			/*u.u_error = EINVAL;*/
			return(EINVAL);
		}
		fdstrategy(bp);
		biowait(bp);
		if(bp->b_error){
			if(( bp->b_error == (char)EBBHARD )
				||( bp->b_error == (char)EBBSOFT ))
				/* u.u_error = EIO;*/
				return(EIO);
			else
				/* u.u_error = bp->b_error; */
				return(bp->b_error);
			break;
		}
	}
fmterr:	brelse(bp);
	return(0);	
}
/****************************************************************************
 *
 *	make id table for format
 *
 ****************************************************************************/
makeidtbl(tblpt,dr,track,intlv)
struct fmttbl *tblpt;
struct fddrtab *dr;
unsigned short track;
unsigned short intlv;
{
	register int	i,j,secno;
	char	cyl,head,s_type;

/*	if(!((0 < intlv)&&(intlv < dr->dr_nsec)))	*/
	if(!(intlv < dr->dr_nsec))
		return(1);
	for( i = 0 ; i < dr->dr_nsec ; i++ )
		tblpt[i].sector = 0;
	cyl = track / dr->dr_nrhead;
	head = track % dr->dr_nrhead;
	if((s_type = dr->dr_secsiz>>8) == 4)
		s_type = 3;
	for(i = 0 , j = 0 , secno = 1 ; i < dr->dr_nsec ; i++){
		tblpt[j].cyl = cyl;
		tblpt[j].head = head;
		tblpt[j].sector = secno++;
		tblpt[j].s_type = s_type;
		j += intlv;
		if(j < dr->dr_nsec ){
			continue;
		}
		j -= dr->dr_nsec;
		for( ; j < dr->dr_nsec ; j++ ){
			if(tblpt[j].sector == 0)
				break;
		}
	}
	return(0);
}
/*****************************************************************************
 *
 * TITLE:	fdintr
 *
 * ABSTRACT:	Handle interrupt.
 *
 *	Interrupt procedure for m765 driver.  Gets status of last
 *	operation and performs service function according to the
 *	type of interrupt.  If it was an operation complete interrupt,
 *	switches on the current driver state and either declares the
 *	operation done, or starts the next operation
 *
 ****************************************************************************/
fdintr(level)
int	level;			/* interrupt level (Not used)*/
{
	extern  int(m765intrsub)();
	register struct	fddev		*dd;
	register struct	fdmbuf	*mbufh;
	register struct fdubuf	*ubufh;
	struct fddrtab *dr;
	unsigned int	unit;

#if	0
	if (first_fdopen_ever) {
		printf("fdintr: fdinit() hasn't been called yet.\n");
		fdinit();
		first_fdopen_ever = 0;
	}
#endif
	dd = &m765dev;
	mbufh = &(dd->d_bufh);
	unit = mbufh->b_unit;
	dr = &dd->d_drtab[unit];
	if((level != RWLEVEL)&&(mbufh->b_cmd.c_stsflag & INTROUT)){
		untimeout(fdintr, RWLEVEL);
#if	0
		untimeout(mbufh->b_cmd.c_timeid);
#endif
	}
	mbufh->b_cmd.c_stsflag &= ~INTROUT;	
	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	switch(mbufh->b_cmd.c_intr){
		case RWFLAG:
			rwintr(dd,mbufh,ubufh,unit,dr);
			break;	

		case SKFLAG:
		case SKEFLAG|SKFLAG:
		case RBFLAG:
			timeout(m765intrsub,dd,SEEKWAIT);
			break;

		case WUPFLAG:
			mbufh->b_cmd.c_intr &= ~WUPFLAG;
			wakeup(mbufh);
			break;
		default:
		/*	cmn_err(CE_WARN,"%s %d : %s",fderr,unit,fdmsg[ILLINT]); */
		/*	printf("%s %d : %s",fderr,unit,fdmsg[ILLINT]);
			printf("interrupt type is %d\n",mbufh->b_cmd.c_intr); */
			break;
	}
	return(0);
}

/*****************************************************************************
 *
 *	interrup subroutine (seek recalibrate)
 *
 *****************************************************************************/
m765intrsub(dd)
register struct	fddev		*dd;
{
register struct	fdmbuf	*mbufh;
register struct fdubuf	*ubufh;
unsigned int	unit;

	mbufh = &(dd->d_bufh);
	unit = mbufh->b_unit;
	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	mbufh->b_status = sis();
	if(mbufh->b_status !=  ST0OK){
		switch(mbufh->b_cmd.c_intr){
			case SKFLAG:
				seekintr(dd,mbufh,ubufh,unit);
				break;

			case SKEFLAG|SKFLAG:
				seekintre(dd,mbufh,ubufh,unit);
				break;

			case RBFLAG:
				rbintr(dd,mbufh,ubufh,unit);
				break;
		}
	}
}
/*****************************************************************************
 *
 *	read / write / format / verify interrupt routine
 *
 *****************************************************************************/
rwintr(dd,mbufh,ubufh,unit,dr)
register struct	fddev	*dd;
register struct	fdmbuf	*mbufh;
register struct fdubuf	*ubufh;
unsigned	unit;
struct fddrtab *dr;
{
	mbufh->b_cmd.c_intr &= ~RWFLAG;
	if(openrtry(dd,mbufh,unit))
		return;
	if(mbufh->b_status = rwstschk())
		rwierr(dd,mbufh,unit,dr);
	else {
		/* write command */
		if((mbufh->b_buf->b_flags&(B_FORMAT|B_READ|B_WRITE))==B_WRITE){
			if(m765verify[unit])
				if(wverify(dd,dr,mbufh,unit))
					return;
		}
		/* clear retry count */
		mbufh->b_rwerr = mbufh->b_seekerr = mbufh->b_rberr = 0;
		mbufh->b_xfercount -= mbufh->b_xferdma;
		mbufh->b_xferaddr += mbufh->b_xferdma;
		mbufh->b_sector =
			mbufh->b_sector+(mbufh->b_xferdma/dr->dr_secsiz);
		/* next address ( cyl,head,sec ) */
		if((int)mbufh->b_xfercount>0){
			m765iosub(dd);
		} else {
			quechk(dd,ubufh,unit);
		}
	}
}
/****************************************************************************
 *
 *	door open timeout
 *
 ****************************************************************************/
openrtry(dd,mbufh,unit)
register struct	fddev	*dd;
register struct	fdmbuf	*mbufh;
int	unit;
{
	register char	seekpoint;

	if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))!=(B_READ|B_VERIFY)){
		if(inb(VFOREG)&OPENBIT){
			if(mbufh->b_buf->b_flags&B_FORMAT){
				mbufh->b_status = TIMEOUT;
				intrerr0(dd,mbufh,unit);
			} else {
				if((inb(STSREG)&ST0OK)!=ST0OK)
				/*	cmn_err(CE_CONT,"%s %d : %s\n",fderr,mbufh->b_unit,fdmsg[DOORERR]); */
					printf("%s %d : %s\n",fderr,mbufh->b_unit,fdmsg[DOORERR]);
				rstout(mbufh,unit);	
				specify();
				mbufh->b_cmd.c_rbmtr &= RBRST;
				mbufh->b_cmd.c_intr |= SKEFLAG;
				if(mbufh->b_cmd.c_saddr > 2)
					seekpoint = mbufh->b_cmd.c_saddr-2;
				else
					seekpoint = mbufh->b_cmd.c_saddr+2;
				fdiseek(dd,unit,seekpoint);
			}
			return(1);
		}
	}
	return(0);
}

/****************************************************************************
 *
 *	write verify routine
 *
 ****************************************************************************/
wverify(dd,dr,mbufh,unit)
register struct fddev *dd;
struct	fddrtab *dr;
register struct fdmbuf *mbufh;
register unsigned int	 unit;
{
	if(!(mbufh->b_buf->b_flags & B_VERIFY)){
		mbufh->b_buf->b_flags |= B_VERIFY;
		rwcmdset(dr,mbufh,unit,mbufh->b_sector);
		if(mbufh->b_status = outicmd(dd,mbufh,unit))
			intrerr0(dd,mbufh,unit);
		return(ERROR);
	} else 
		mbufh->b_buf->b_flags &= ~B_VERIFY;
	return(OK);
}
/*****************************************************************************
 *
 *	read / write / format / verify error routine
 *
 *****************************************************************************/
rwierr(dd,mbufh,unit,dr)
register struct	fddev		*dd;
register struct	fdmbuf	*mbufh;
unsigned int	unit;
register struct fddrtab *dr;
{
	char	seekpoint;
	short	status;

	if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))==(B_READ|B_VERIFY)){
		if((mbufh->b_rwerr&SRMASK)<MEDIARD)
			goto rwrtry;
		if((mbufh->b_rwerr&MRMASK)<MEDIASEEK)
			goto rwseek;
		else
			goto rwexit;
	} else {
		if(mbufh->b_buf->b_flags&B_VERIFY){
			mbufh->b_buf->b_flags &= ~B_VERIFY;
			rwcmdset(dr,mbufh,unit,mbufh->b_sector);
		}
	}
rwrtry:	status = mbufh->b_status;
	if((++mbufh->b_rwerr&SRMASK)<SRETRY){
#ifdef	DEBUG
		if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))
			!= (B_READ|B_VERIFY)){
		/*	cmn_err(CE_CONT,"Warning : "); */
			printf("Warning : ");
			checkerr(dr,mbufh,unit);
		}
#endif
		mbufh->b_status = outicmd(dd,mbufh,unit);
	} else {
rwseek:		mbufh->b_rwerr = (mbufh->b_rwerr&RMRMASK)+MINC;
		if((mbufh->b_rwerr&MRMASK)<MRETRY){
			mbufh->b_cmd.c_intr |= SKEFLAG;
			if(mbufh->b_cmd.c_saddr > 2)
				seekpoint = mbufh->b_cmd.c_saddr-2;
			else
				seekpoint = mbufh->b_cmd.c_saddr+2;
			mbufh->b_status=fdiseek(dd,unit,seekpoint);
		} else {
			mbufh->b_rwerr = (mbufh->b_rwerr&LRMASK)+LINC;
			if((mbufh->b_rwerr&LRMASK)<LRETRY)
       				mbufh->b_status=rbirate(mbufh,unit,dr->dr_type);
		}
	}
	if(mbufh->b_status){
		mbufh->b_status = status;
rwexit:		intrerr0(dd,mbufh,unit);
	}
}
/*****************************************************************************
 *
 *	recalibrate interrupt routine
 *
 *****************************************************************************/
rbintr(dd,mbufh,ubufh,unit)
struct	fddev  *dd;
register struct	fdmbuf *mbufh;
register struct	fdubuf *ubufh;
register unsigned int	 unit;
{
	mbufh->b_cmd.c_intr &= ~RBFLAG;
	if(mbufh->b_status){
		if(++mbufh->b_rberr<SRETRY)
			mbufh->b_status =
				rbirate(mbufh,unit,dd->d_drtab[unit].dr_type);
	} else {
		mbufh->b_cmd.c_rbmtr |= 1<<(RBSHIFT+unit);
		ubufh->b_seekaddr = 0;
		mbufh->b_rberr = 0;
		mbufh->b_status=fdiseek(dd,unit,mbufh->b_cmd.c_saddr);
	}
	if(mbufh->b_status)
		intrerr0(dd,mbufh,unit);
}

/******************************************************************************
 *
 *	seek interrupt routine
 *
 ******************************************************************************/
seekintr(dd,mbufh,ubufh,unit)
register struct	fddev *dd;
register struct	fdmbuf *mbufh;
register struct	fdubuf *ubufh;
unsigned int	 unit;
{
	mbufh->b_cmd.c_intr &= ~SKFLAG;
	if(mbufh->b_status){
			seekierr(dd,mbufh,unit,mbufh->b_cmd.c_saddr);
	} else {
		ubufh->b_seekaddr = mbufh->b_cmd.c_saddr;
		mbufh->b_status = outicmd(dd,mbufh,unit);
	}
	if(mbufh->b_status)
		intrerr0(dd,mbufh,unit);
	else
		mbufh->b_seekerr = 0;
}
/*****************************************************************************
 *
 *	seek error retry interrupt routine
 *
 ******************************************************************************/
seekintre(dd,mbufh,ubufh,unit)
struct	fddev *dd;
register struct	fdmbuf *mbufh;
register struct	fdubuf *ubufh;
unsigned int	 unit;
{
	register char		seekpoint;

	mbufh->b_cmd.c_intr &= ~(SKEFLAG|SKFLAG);
	if(mbufh->b_cmd.c_saddr > 2)
		seekpoint = mbufh->b_cmd.c_saddr-2;
	else
		seekpoint = mbufh->b_cmd.c_saddr+2;
	if(mbufh->b_status)
		seekierr(dd,mbufh,unit,seekpoint);
	else {
		ubufh->b_seekaddr = seekpoint;
		mbufh->b_status=fdiseek(dd,unit,mbufh->b_cmd.c_saddr);
	}
	if(mbufh->b_status)
		intrerr0(dd,mbufh,unit);
	else
		mbufh->b_seekerr = 0;
}

/*****************************************************************************
 *
 *	seek error routine
 *
 ******************************************************************************/
seekierr(dd,mbufh,unit,seekpoint)
struct fddev  *dd;
register struct fdmbuf *mbufh;
register unsigned int	unit;
register char		seekpoint;
{
	if((++mbufh->b_seekerr&SRMASK)<SRETRY){
		mbufh->b_status=fdiseek(dd,unit,seekpoint);
	} else {
		mbufh->b_seekerr = (mbufh->b_seekerr&MRMASK) + MINC;
		if((mbufh->b_seekerr&MRMASK)<MRETRY)
			mbufh->b_status=rbirate(mbufh,unit,dd->d_drtab[unit].dr_type);
	}
	if(mbufh->b_status){
		intrerr0(dd,mbufh,unit);
	}
}
/*****************************************************************************
 *
 * TITLE:	m765sweep
 *
 * ABSTRACT:	Perform an initialization sweep.  
 *
 **************************************************************************/
m765sweep(dd, unit, cdr)
struct fddev *dd;			/* device parameters */
register unsigned int	unit;
register struct fddrtab  *cdr;	/* device initialization data */
{
	register struct fddrtab *dr;

	dr = &dd->d_drtab[unit];
	dr->dr_ncyl	      = cdr->dr_ncyl;
	dr->dr_nrhead	      = cdr->dr_nrhead;
	dr->dr_nsec	      = cdr->dr_nsec;
	dr->dr_secsiz	      = cdr->dr_secsiz;	
	dr->dr_spc	      = cdr->dr_spc;
	dr->dr_part[0].p_fsec = cdr->dr_part[0].p_fsec;
	dr->dr_part[0].p_nsec = cdr->dr_part[0].p_nsec;
	dr->dr_part[1].p_fsec = cdr->dr_part[1].p_fsec;
	dr->dr_part[1].p_nsec = cdr->dr_part[1].p_nsec;
	dr->dr_type	      = cdr->dr_type;
	dr->dr_rwgpl	      = cdr->dr_rwgpl;
	dr->dr_fgpl	      = cdr->dr_fgpl;
}

/******************************************************************************
 *
 *	fdc parameter cmp routine
 *
 ******************************************************************************/
cmpparm(sdr,ddr)
register struct fddrtab  *sdr;	/* device initialization data */
register struct fddrtab  *ddr;	/* device initialization data */
{
	if(sdr->dr_ncyl   != ddr->dr_ncyl)
		return(ERROR);
	if(sdr->dr_nrhead != ddr->dr_nrhead)
		return(ERROR);
	if(sdr->dr_nsec   != ddr->dr_nsec)
		return(ERROR);
	if(sdr->dr_secsiz != ddr->dr_secsiz)	
		return(ERROR);
	if((sdr->dr_type&~OKTYPE) != ddr->dr_type)
		return(ERROR);
	if(sdr->dr_rwgpl  != ddr->dr_rwgpl)
		return(ERROR);
	if(sdr->dr_fgpl   != ddr->dr_fgpl)
		return(ERROR);
	return(OK);
}
/*****************************************************************************
 *
 *  TITLE:  m765disksort
 *
 *****************************************************************************/
fd_disksort(dp, bp, nsec)
struct fdubuf *dp;		/*  Pointer to head of active queue	*/
register struct buf *bp;	/*  Pointer to buffer to be inserted	*/
char	nsec;			/*  Sectors/track */
{
	register struct buf *bp2; /*  Pointer to next buffer in queue	*/
	register struct buf *bp1; /*  Pointer where to insert buffer	*/

	bp1 = dp->av_forw;
	if (bp1 == 0) {
	    /* No other buffers to compare against */
	    dp->av_forw = bp;
	    bp->av_forw = 0;
	    return;
	}
	bp2 = bp1->av_forw;

	while(bp2){
		if(relative(bp1->b_pfcent,bp->b_pfcent,nsec)
				<relative(bp1->b_pfcent,bp2->b_pfcent,nsec))
			break;
		bp1 = bp2;
		bp2 = bp1->av_forw;
	}
	/* bp->b_start = lbolt; */
	bp1->av_forw = bp;
	bp->av_forw = bp2;
}

relative(sector1,sector2,nsec)
register daddr_t sector1;
register daddr_t sector2;
register char	 nsec;
{
	if(sector1>sector2)
		return((int)((sector1/nsec) - (sector2/nsec)));
	else
		return((int)((sector2/nsec) - (sector1/nsec)));
}
/*****************************************************************************
 *
 *	Set Interrupt error and FDC reset
 *
 *****************************************************************************/
intrerr0(dd,mbufh,unit)
struct	 fddev *dd;
register struct	fdmbuf *mbufh;
register unsigned int	 unit;
{
	register struct	fddrtab *dr;
	struct buf *bp;		/*  Pointer to next buffer in queue	*/
	struct	fdubuf *ubufh;

	dr = &dd->d_drtab[unit];
	ubufh = mbufh->b_unitf;
	while(ubufh->b_unit != unit)
		ubufh = ubufh->b_unitf;	
	checkerr(dr,mbufh,unit);
	mbufh->b_rwerr = mbufh->b_seekerr = mbufh->b_rberr = 0;
	mbufh->b_cmd.c_intr = CMDRST;
	if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))!=(B_READ|B_VERIFY)){
		if(((kbd_FDsts&FDCHG)&&unit)||((!(kbd_FDsts&FDCHG))&&(!unit))){
			fddtype = 0;
			dr->dr_type &= ~OKTYPE; 
		}
	}
	bp = mbufh->b_buf;
	bp->b_flags |= B_ERROR;
	switch(mbufh->b_status&BYTEMASK){
		case ADDRERR:
			bp->b_error = EIO;
			break;
		case OVERRUN:
			bp->b_error = EIO;
			break;
		case FDCERR:
			bp->b_error = EIO;
			break;
		case TIMEOUT:
			bp->b_error = EIO;
			break;				
		case WTPRT:
#ifdef	MACH_KERNEL
			bp->b_error = ENXIO;	/* ??? */
#else	MACH_KERNEL
			bp->b_error = ENODEV;
#endif	MACH_KERNEL
			break;
		case NOREC:
			bp->b_error = EBBHARD;
			break;				
		case CRCERR:
			bp->b_error = EBBSOFT;
			break;				
	}
	rstout(mbufh,unit);
	specify();
	mbufh->b_cmd.c_rbmtr &= RBRST;
	quechk(dd,ubufh,unit);
}
/*****************************************************************************
 *
 *	Next queue check routine
 *
 *****************************************************************************/
quechk(dd,ubufh,unit)
struct	fddev *dd;
struct	fdubuf *ubufh;
unsigned int	unit;
{
	register struct	fdmbuf *mbufh;
	register struct	buf *bp;
	register int	count;
	int	wunit;
	unsigned int	x;
	
	mbufh = &(dd->d_bufh);

	x = SPL();
	/* clear retry count */
	mbufh->b_rwerr = mbufh->b_seekerr = mbufh->b_rberr = 0;
	bp = ubufh->av_forw;
	bp->b_resid = bp->b_resid + mbufh->b_xfercount;
	ubufh->av_forw = bp->av_forw; 

	biodone(bp);
	for ( wunit = unit , count = MAXUNIT ; count > 0 ; count--){
		ubufh = ubufh->b_unitf;	
		wunit = (wunit+1)%MAXUNIT;
		if(ubufh->av_forw != NULL)
			break;
		else {	
			if(mbufh->b_active[wunit] & FD_IO_WAIT)
				wakeup((caddr_t)&mbufh->b_active[wunit]);
			mbufh->b_active[wunit] = FD_IO_IDLE;
		}
	}
	if(count){
		m765io(dd,ubufh,wunit);
	} else {
		dmadone();
		mbufh->b_cmd.c_stsflag &= ~MTRFLAG;
		mtr_on(mbufh,unit);
		openstr(mbufh);
	}
	splx(x);
}
/*****************************************************************************
 *
 *	interrupt error output routine
 *
 *****************************************************************************/
checkerr(dr,mbufh,unit)
register struct	fddrtab *dr;
register struct fdmbuf *mbufh;
register unsigned int	unit;
{
	int	resid;

	if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))!=(B_READ|B_VERIFY)){
		resid = mbufh->b_xfercount = mbufh->b_xferdma - 1 - residcnt();
		resid = (mbufh->b_sector + ( resid / dr->dr_secsiz ))
			% dr->dr_spc;
	/*	cmn_err(CE_CONT,"%s %d : %s\n",
			fderr,unit,fdmsg[mbufh->b_status&BYTEMASK]); */
		printf("%s %d : %s\n",
			fderr,unit,fdmsg[mbufh->b_status&BYTEMASK]);
	/*	cmn_err(CE_CONT,"cylinder = %d  ",mbufh->b_cmd.c_saddr); */
		printf("cylinder = %d  ",mbufh->b_cmd.c_saddr);
	/*	cmn_err(CE_CONT,"head = %d  sector = %d  byte/sec = %d\n",
		resid / dr->dr_nsec , (resid % dr->dr_nsec)+1 , dr->dr_secsiz); */
		printf("head = %d  sector = %d  byte/sec = %d\n",
		resid / dr->dr_nsec , (resid % dr->dr_nsec)+1 , dr->dr_secsiz);
	}
}
/*****************************************************************************
 *
 *	get dma cntroler
 *
 *****************************************************************************/
dmaget()
{
	register unsigned int	x;

	x = splhi();
	while(dmause==DMABUSY)
		sleep(&dmause,PZERO);
	dmause = DMABUSY;
	splx(x);
}

/*****************************************************************************
 *
 *	release dma cntroler
 *
 *****************************************************************************/
dmadone()
{
	register unsigned int	x;

	x = splhi();
	dmause = DMADONE;
	wakeup(&dmause);
	splx(x);
}

fdprint(dev,str)
dev_t	dev;
char	*str;
{
	printf("floppy disk driver: %s on bad dev %d, partition %d\n",
			str, UNIT(dev), PARTITION(dev)	);
}

fdsize()
{
	printf("fdsize()	-- not implemented\n");
}

fddump()
{
	printf("fddump()	-- not implemented\n");
}

/*****************************************************************************
 *
 *	external fd check routine
 *
 *****************************************************************************/
extchk(unit)
register unsigned int unit;
{
	if(kbd_FDsts&INFDD){			/* 2 internal FDD ? */
		if(!(kbd_FDsts&FDEXT)){		/* external conect ? */
			if(kbd_FDsts&FDCHG){
				if(unit)
					return(ERROR);
			} else {
				if(!(unit))
					return(ERROR);
			}
		} 
	}
	return(OK);
}

/*****************************************************************************
 *
 *	drive check ( 0 : 2HD  1 : 2HC ) routine
 *
 *****************************************************************************/
dunitchk(unit)
register unsigned int	unit;
{
	if(kbd_FDsts & INFDD){
		if( kbd_FDsts & FDCHG ){
			if( unit ||((!(unit))&&(kbd_FDsts&HDDRV)))
				return(ERROR);
		} else {
			if(!(unit&&(!(kbd_FDsts&HDDRV))))
				return(ERROR);
		}
	} else {
		if(kbd_FDsts&HDDRV)
			return(ERROR);
	}
	return(OK);
}
/*****************************************************************************
 *
 *	2D_external drive check ( ERROR : 2D  OK : not 2D ) routine
 *
 *****************************************************************************/
ext2dchk(unit)
register unsigned int	unit;
{
	int	chgsts;

	chgsts = kbd_FDsts & FDCHG;
	if((chgsts&&(!unit))||((!chgsts)&&unit))
		goto not2d;
	if(fddtype == EXT2D)
		return(ERROR);
not2d:	return(OK);
}

/*****************************************************************************
 *
 *	drive checge check routine
 *
 *****************************************************************************/
struct fdtree *dchgchk(unit)
register unsigned int	unit;
{
	if(dunitchk(unit)){
		if(ext2dchk(unit))
			return(&fd2tree[0]);
		return(&fd1tree[0]);
	} else
		return(&fd0tree[0]);
}

/*******************************************************************************
 *
 * 	new drive checge check routine
 *
 ******************************************************************************/
struct fdtree *dchgchk_new(dev)
register unsigned int 	dev;
{
	register unsigned int mtype;

	if ((mtype = MEDIATYPE(dev)) == 1)
		return(&fd0tree[0]);  /* if the media is F2h18 */
	mtype = (mtype == 0) ? 2 : mtype - 1;
	return(&fd1tree[mtype]);
}

/*****************************************************************************
 *
 *	get fdc parameter table routine
 *
 *****************************************************************************/
struct fddrtab *getparm(type,unit)
register int	type;
register unsigned int	unit;
{
	register struct fddrtab *rtn;

	type--;
	if(ext2dchk(unit))
		type += 4;
	rtn = (struct fddrtab *)ERROR;
	if(type < FORMMAX){
		if(dunitchk(unit))
			rtn = &m765f1[type];
		else
			rtn = &m765f0[type];
		if(rtn->dr_ncyl == 0)
			rtn = (struct fddrtab *)ERROR;
	}
	return(rtn);
}
/*****************************************************************************
 *
 *	new get fdc parameter table routine
 *
 *****************************************************************************/
struct fddrtab *getparm_new(type,unit)
register int	type;
register unsigned int	unit;
{
	register struct fddrtab *rtn;

	type--;
	rtn = (struct fddrtab *)ERROR;
	if(type < FORMMAX){
		if (type) 
			rtn = &m765f1[type];
		else
			rtn = &m765f0[type];
		if(rtn->dr_ncyl == 0)
			rtn = (struct fddrtab *)ERROR;
	}
	return(rtn);
}

/*****************************************************************************
 *
 *	fdc reset routine
 *
 *****************************************************************************/
rstout(mbufh,unit)
register struct	fdmbuf *mbufh;
register unsigned int unit;
{
	register int	outd;
	int	mtrnum;	

	mtrnum = (mbufh->b_cmd.c_rbmtr&MTRMASK);	
	outd = (mtrnum<<MTR_ON)|unit;
	outb( CTRLREG , outd );
	waitx(MSEC);
	outd |= FDC_RST;
	outb( CTRLREG , outd );
	waitx(MSEC);
	outd |= DMAREQ;
	outb( CTRLREG , outd );
	waitx(MSEC);
}

/*****************************************************************************
 *
 *	specify command routine
 *
 ******************************************************************************/
specify()
{
	if(fdc_sts(FD_OSTS))		/* status check */
		goto sperr;
	outb( DATAREG , SPCCMD );		/* Specify command */
	if(fdc_sts(FD_OSTS))		/* status check */
		goto sperr;
	outb( DATAREG , m765fdata.f_srthut ); 	/* Step rate,Head unload time */
	if(fdc_sts(FD_OSTS))		/* status check */
		goto sperr;
	outb( DATAREG , m765fdata.f_hltnd );  	/* Head load time,Non DMA Mode*/
sperr:	return;
}
/*****************************************************************************
 *
 *	transfer rate set routine
 *
 *****************************************************************************/
trfrate( type )		/* set transfer rate */
register unsigned char	type;
{
	outb( VFOREG , ((type & RATEMASK)>>6) );
}

/*****************************************************************************
 *
 *	recalibrate and seek transfer rate set routine
 *
 *****************************************************************************/
rbskrate( type )		/* set transfer rate */
register char	type;
{
	if(type & RAPID)
		trfrate(RPSEEK);		/* set transfer rate */
	else
		trfrate(NMSEEK);		/* set transfer rate */
}
/****************************************************************************
 *
 *	recalibrate command routine
 *
 ****************************************************************************/
rbrate(mbufh,mtype,unit)
register struct	fdmbuf *mbufh;
char	 mtype;
unsigned unit;
{
	register int	rtn = 1;
	register int	rty_flg = 2;
	unsigned int	x;

	rbskrate(mtype);			/* set transfer rate */
	while((rty_flg--)&&rtn){
		if(rtn = fdc_sts(FD_OSTS))	/* status check */
			break;
		outb( DATAREG , RBCMD );	/* recalibrate command*/
		if(rtn = fdc_sts(FD_OSTS))	/* status check */
			break;
		mbufh->b_cmd.c_intr |= WUPFLAG;

		x = SPL();
		outb( DATAREG , unit );
		rtn = ERROR;
		while(rtn) {
			sleep(mbufh,PZERO);
			if((rtn = sis()) == ST0OK)
				mbufh->b_cmd.c_intr |= WUPFLAG;
			else
				break;
		}
		splx(x);
		waitx(m765fdata.f_hst*MSEC);
	}
	return(rtn);
}
/*****************************************************************************
 *
 *	seek command routine
 *
 ****************************************************************************/
fdseek( mbufh , mtype , unit , cylno )
register struct	fdmbuf *mbufh;
register char	mtype;
unsigned unit;
register int	cylno;
{
	unsigned int	x;
	int	rtn;

	rbskrate(mtype);
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	outb( DATAREG , SEEKCMD );	/* seek command */
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	outb( DATAREG , unit );		/* drive number */
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	x = SPL();
	mbufh->b_cmd.c_intr |= WUPFLAG;
	outb( DATAREG , cylno );	/* seek count */
	rtn = ERROR;
	while(rtn){	
		sleep(mbufh,PZERO);
		if((rtn = sis()) == ST0OK)
			mbufh->b_cmd.c_intr |= WUPFLAG;
		else
			break;
	}
	splx(x);
	waitx(m765fdata.f_hst*MSEC);
	return(rtn);
}
/****************************************************************************
 *
 *	dma parameter set routine
 *
 ****************************************************************************/
dmaset(mbufh)
register struct fdmbuf *mbufh;
{
	register long	address;
	register int	data;
	register int	dmalen;

	outb(DMACMD1,DMADATA0);	/* DMA #1 command register 	*/
	outb(DMAMSK1,DMADATA1);	/* DMA #1 all mask register	*/
	outb(DMACMD2,DMADATA2);
	outb(DMASMSK,DMADATA3);
	outb(DMAMOD2,DMADATA4);
	outb(DMAMSK2,DMADATA5);
	switch(mbufh->b_cmd.c_rwdata[0]){
		case RDM:
			data = DMARD;
			break;
		case WTM:
		case FMTM:
			data = DMAWT;
			break;
		case RDMV:
			data = DMAVRF;
			break;
	}
	outb(DMABPFF,data);
	outb(DMAMODE,data);

	/* get work buffer physical address */
	address = kvtophys(mbufh->b_xferaddr);
	dmalen = i386_trunc_page(address) + I386_PGBYTES - address;
	dmalen = dmalen <= mbufh->b_xferdma ? dmalen : mbufh->b_xferdma;
	mbufh->b_xferdma = dmalen;

	/* set buffer address */
	outb(DMAADDR,(int)address&BYTEMASK);		
	outb(DMAADDR,(((int)address>>8)&BYTEMASK));
	outb(DMAPAGE,(((int)address>>16)&BYTEMASK));

	/* set transfer count */
	dmalen--;
	outb(DMACNT,dmalen&BYTEMASK);	
	outb(DMACNT,((dmalen>>8)&BYTEMASK));
	outb(DMAMSK,CHANEL2);
}
/*****************************************************************************
 *
 *	seek commnd routine(use interrupt)
 *
 *****************************************************************************/
fdiseek( dd , unit , cylno )
struct	fddev *dd;
unsigned unit;
int	cylno;
{
	register struct	fdmbuf *mbufh;
	register struct fdubuf *ubufh;
	register int	rtn;
	int	seektype;

	mbufh = &(dd->d_bufh);
	ubufh = mbufh->b_unitf;
	if(unit)
		ubufh = ubufh->b_unitf;
	rbskrate( dd->d_drtab[unit].dr_type );	/* set transfer rate */
	if(rtn = fdc_sts(FD_OSTS))		/* status check */
		goto fdiend;
	outb( DATAREG , SEEKCMD );		/* seek command */
	if(rtn = fdc_sts(FD_OSTS))		/* status check */
		goto fdiend;
	outb( DATAREG , unit );			/* drive number */
	if(rtn = fdc_sts(FD_OSTS))		/* status check */
		goto fdiend;
	ubufh->b_seekaddr = cylno;
	if(dd->d_drtab[unit].dr_type&DOUBLE)
		cylno = cylno * 2;
	mbufh->b_cmd.c_intr |= SKFLAG;
	outb( DATAREG , cylno );		/* seek count */
fdiend:	
	if(rtn)
		rtn |= SEEKCMD<<8;
	return(rtn);
}
/*****************************************************************************
 *
 *	recalibrate command routine(use interrupt)
 *
 *****************************************************************************/
rbirate(mbufh,unit,rbtype)
register struct	fdmbuf *mbufh;
unsigned unit;
register int	rbtype;
{
	register int	rtn;

	rbskrate(rbtype);		/* set transfer rate */
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		goto rbiend;
	outb( DATAREG , RBCMD );	/* recalibrate command */
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		goto rbiend;
	mbufh->b_cmd.c_intr |= RBFLAG;
	outb( DATAREG , unit );
rbiend:	
	if(rtn)
		rtn |= RBCMD<<8;
	return(rtn);
}
/*****************************************************************************
 *
 *	read / write / format / verify command out routine(use interrupt)
 *
 *****************************************************************************/
outicmd(dd,mbufh,unit)
struct	fddev *dd;
struct	fdmbuf *mbufh;
unsigned int unit;
{
	extern  int(fdintr)();
	int	rtn;
	unsigned int	x;
	register struct	fdcmd *cmd;
	register int	*data;
	register int	cnt0;

	dmaset(mbufh);
	trfrate(dd->d_drtab[unit].dr_type);	/* set transfer rate */
	cmd = &mbufh->b_cmd;
	data = &cmd->c_rwdata[0];
	x = SPL();
	for( cnt0 = 0 ; cnt0 < cmd->c_dcount ; cnt0++ , data++){
		if(rtn = fdc_sts(FD_OSTS))	/*statu check*/
			break;
		outb( DATAREG , *data );
	}
	if(!(rtn)){
		cmd->c_intr |= RWFLAG;
		cmd->c_stsflag |= INTROUT;
		if((mbufh->b_buf->b_flags&(B_READ|B_VERIFY))==(B_READ|B_VERIFY))
			cnt0 = TOUT;
		else
			cnt0 = ITOUT;
#ifdef	MACH_KERNEL
		timeout(fdintr,RWLEVEL,cnt0);
#else	MACH_KERNEL
		cmd->c_timeid = timeout(fdintr,RWLEVEL,cnt0);
#endif	MACH_KERNEL
	}
	splx(x);
	return(rtn);
}

/*****************************************************************************
 *
 *	sense device status routine
 *
 *****************************************************************************/
sds(unit)
unsigned int unit;
{
	register int	rtn;

	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	outb( DATAREG , SDSCMD );
	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	outb( DATAREG , unit );
	if(rtn = fdc_sts(FD_ISTS))	/* status check */
		return(rtn);
	return((int)inb( DATAREG ));	/* get st3 */
}

/*****************************************************************************
 *
 *	sense interrupt status routine
 *
 *****************************************************************************/
sis()
{
	register int	rtn;
	register int	st0;

	if(rtn = fdc_sts(FD_OSTS))	/* status check */
		return(rtn);
	outb( DATAREG , SISCMD );
	if(rtn = fdc_sts(FD_ISTS))	/* status check */
		return(rtn);
	st0 = inb( DATAREG ) & ST0OK;	/* get st0 */
	if(rtn = fdc_sts(FD_ISTS))	/* status check */
		return(rtn);
	inb( DATAREG );			/* get pcn */
	switch(st0){
		case ST0AT:
		case ST0IC:
			st0 = FDCERR;
	}
	return(st0);
}

/*****************************************************************************
 *
 *	fdc status get routine
 *
 *****************************************************************************/
fdc_sts(mode)
register int	mode;
{
	register int	ind,cnt0;

	cnt0 = STSCHKCNT;
	while(cnt0--){
		ind = inb(STSREG);
		if( ind & DATAOK )
			if((ind & DTOCPU) == mode)
				return(0);
	}
	return(TIMEOUT);
}

/*****************************************************************************
 *
 *	read / write / format / verify status get routine
 *
 *****************************************************************************/
rwstschk()
{
	int	rsult[7];
	register int	count;
	register int	rtn;

	for( count = 0 ; count < 7 ; count++ ){
		if(rtn = fdc_sts(FD_ISTS))	/* status check */
			goto rwend;
		rsult[count] = inb( DATAREG );
	}
	rtn = 0;
	if(rsult[0]&0xc0){
		rtn = m765dev.d_bufh.b_cmd.c_rwdata[0]<<8;
		if(rsult[0]&0x80){ rtn |= FDCERR;   goto rwend; }
		if(rsult[1]&0x80){ rtn |= NOREC;    goto rwend; }
		if(rsult[1]&0x20){ rtn |= CRCERR;   goto rwend; }
		if(rsult[1]&0x10){ rtn |= OVERRUN;  goto rwend; }
		if(rsult[1]&0x04){ rtn |= NOREC;    goto rwend; }
		if(rsult[1]&0x02){ rtn |= WTPRT;    goto rwend; }
		if(rsult[1]&0x01){ rtn |= ADDRERR;  goto rwend; }
		rtn |= FDCERR;
rwend:		outb( 0x0a , 0x06 );
	}
	return(rtn);
}
/*****************************************************************************
 *
 *	resid count get routine
 *
 ******************************************************************************/
unsigned int residcnt()
{
	register unsigned int	count;

	count = inb(DMACNT);
	count += (inb(DMACNT)<<8);
	return(count);
}

/*****************************************************************************
 *
 *	motor on routine
 *
 *****************************************************************************/
mtr_on(mbufh,unit)
register struct fdmbuf *mbufh;
register unsigned unit;
{
	register int	status;
	
	status = mtr_start(mbufh,unit);
	mtr_wait(mbufh,unit,status);
}
/*****************************************************************************
 *
 *	motor start routine
 *
 *****************************************************************************/
mtr_start(mbufh,unit)
register struct fdmbuf *mbufh;
register unsigned unit;
{
	int	status;
	register int	outd;
	unsigned int	x;
	int	(mtr_off)();

	x = splhi();
	if(mbufh->b_cmd.c_stsflag & MTROFF){
		untimeout(mtr_off, unit);
#if	0
	/*	untimeout(mbufh->b_cmd.c_mtrid); */
#endif
		mbufh->b_cmd.c_stsflag &= ~MTROFF;
	}
	status = mbufh->b_cmd.c_rbmtr&(1<<unit);
	mbufh->b_cmd.c_rbmtr |= (1<<unit);
	outd = (mbufh->b_cmd.c_rbmtr&MTRMASK)<<MTR_ON;
	outd |= FDC_RST|unit|DMAREQ;
	outb( CTRLREG , outd );
	splx(x);
	return(status);
}

/*****************************************************************************
 *
 *	motor on wait routine
 *
 *****************************************************************************/
mtr_wait(mbufh,unit,status)
register struct fdmbuf *mbufh;
register unsigned unit;
int	status;
{
	extern  int(mtr_off)();
	extern	int(mtr_wake)();
	register unsigned int	x;

	x = splhi();
	if(!(status)){
		timeout(mtr_wake,&mbufh->b_cmd.c_stsflag,HZ);
		sleep(&mbufh->b_cmd.c_stsflag,PZERO);
	}
	mbufh->b_cmd.c_stsflag |= MTROFF;
#ifdef	MACH_KERNEL
	timeout(mtr_off,unit,MTRSTOP);
#else	MACH_KERNEL
	mbufh->b_cmd.c_mtrid = timeout(mtr_off,unit,MTRSTOP);
#endif	MACH_KERNEL
	splx(x);
}

/*****************************************************************************
 *
 *	motor wakeup routine
 *
 *****************************************************************************/
mtr_wake(c_stsflag)
register int	*c_stsflag;
{
	wakeup(c_stsflag);
}

/*****************************************************************************
 *
 *	motor off routine
 *
 *****************************************************************************/
mtr_off(unit)
register unsigned unit;
{
	register struct	fdmbuf *mbufh;
	register int	outd;
	unsigned int	x;

	x = splhi();
	mbufh = &(m765dev.d_bufh);
	mbufh->b_cmd.c_stsflag &= ~MTROFF;
	if(!(mbufh->b_cmd.c_stsflag&MTRFLAG)){
		mbufh->b_cmd.c_rbmtr &= MTRRST;
		outd = FDC_RST | DMAREQ;
		outb( CTRLREG , outd );
	} 
	splx(x);
}

/*****************************************************************************
 *
 *	wait loop 
 *
 *****************************************************************************/
waitx(count0)
register unsigned int	count0;
{
	if(count0){
		while(count0--){
			;
		}
	}
}
