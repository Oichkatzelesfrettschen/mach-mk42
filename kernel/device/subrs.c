/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * $Log:	subrs.c,v $
 * Revision 2.6  91/02/05  17:10:17  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:30:39  mrt]
 * 
 * Revision 2.5  91/01/08  15:10:02  rpd
 * 	Added continuation argument to thread_block.
 * 	[90/12/08            rpd]
 * 
 * Revision 2.4  90/05/03  15:19:27  dbg
 * 	Add compatibility routines for BSD-compatible device drivers:
 * 	sleep, wakeup, geteblk, brelse.
 * 	[90/03/14            dbg]
 * 
 * Revision 2.3  90/01/11  11:42:31  dbg
 * 	De-linted.
 * 	[89/12/15            dbg]
 * 
 * 	Add lock initialization in if_init_queues.
 * 	[89/11/30            dbg]
 * 
 * Revision 2.2  89/11/29  14:08:58  af
 * 	RCS-ed, added strncmp (needed for scsi label comparisons).
 * 	[89/10/28            af]
 * 
 */
/*
 * Random device subroutines and stubs.
 */

#include <vm/vm_kern.h>
#include <device/buf.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>

/*
 * Print out disk name and block number for hard disk errors.
 */
harderr(bp, cp)
	struct buf *bp;
	char *	cp;
{
	printf("%s%d%c: hard error sn%d ",
	       cp,
	       minor(bp->b_dev) >> 3,
	       'a' + (minor(bp->b_dev) & 0x7),
	       bp->b_blkno);
}

/* strcmp should be in a library somewhere */
/*
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

strcmp(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++=='\0')
			return(0);
	return(*s1 - *--s2);
}
/* strcmp should be in a library somewhere */
/* strncmp should be in a library somewhere */
/*
 * Compare bounded strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

strncmp(s1, s2, len)
register char *s1, *s2;
register int len;
{
	while ((--len >= 0) && (*s1 == *s2++))
		if (*s1++=='\0')
			return(0);
	return( (len < 0) ? 0 : *s1 - *--s2);
}
/* strncmp should be in a library somewhere */

/* strncpy should be in a library somewhere */
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strncpy.c	5.2 (Berkeley) 3/9/86";
#endif LIBC_SCCS and not lint

/*
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */

char *
strncpy(s1, s2, n)
register char *s1, *s2;
{
	register i;
	register char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}
/* strncpy should be in a library somewhere */


/*
 * Ethernet support routines.
 */
u_char	etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/*
 * Convert Ethernet address to printable (loggable) representation.
 */
char *
ether_sprintf(ap)
	register u_char *ap;
{
	register i;
	static char etherbuf[18];
	register char *cp = etherbuf;
	static char digits[] = "0123456789abcdef";

	for (i = 0; i < 6; i++) {
		*cp++ = digits[*ap >> 4];
		*cp++ = digits[*ap++ & 0xf];
		*cp++ = ':';
	}
	*--cp = 0;
	return (etherbuf);
}

/*
 * Initialize send and receive queues on an interface.
 */
if_init_queues(ifp)
	register struct ifnet *ifp;
{
	IFQ_INIT(&ifp->if_snd);
	queue_init(&ifp->if_rcv_port_list);
	simple_lock_init(&ifp->if_rcv_port_list_lock);
}


/*
 * Compatibility with BSD device drivers.
 */
sleep(channel, priority)
	char *	channel;
	int	priority;
{
	assert_wait((int)channel, FALSE);	/* not interruptible XXX */
	thread_block((void (*)()) 0);
}

wakeup(channel)
	char *	channel;
{
	thread_wakeup((int)channel);
}

struct buf *
geteblk(size)
	int	size;
{
	register io_req_t	ior;

	io_req_alloc(ior, 0);
	ior->io_device = (device_t)0;
	ior->io_unit = 0;
	ior->io_op = 0;
	ior->io_mode = 0;
	ior->io_recnum = 0;
	ior->io_count = size;
	ior->io_residual = 0;
	ior->io_error = 0;

	size = round_page(size);
	ior->io_data = (io_buf_ptr_t)
		kmem_alloc_wired(kernel_map, size);
	ior->io_alloc_size = size;

	return (ior);
}

brelse(bp)
	struct buf *bp;
{
	register io_req_t	ior = bp;

	(void) vm_deallocate(kernel_map,
			(vm_offset_t) ior->io_data,
			(vm_size_t) ior->io_alloc_size);
	io_req_free(ior);
}
