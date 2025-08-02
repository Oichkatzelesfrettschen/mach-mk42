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
 * $Log:	iopl.c,v $
 * Revision 2.6  91/02/05  17:18:43  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:44:59  mrt]
 * 
 * Revision 2.5  90/11/26  14:50:15  rvb
 * 	jsb bet me to XMK34, sigh ...
 * 	[90/11/26            rvb]
 * 	Synched 2.5 & 3.0 at I386q (r2.1.1.3) & XMK35 (r2.5)
 * 	[90/11/15            rvb]
 * 
 * Revision 2.1.1.2  90/03/28  08:30:58  rvb
 * 	Add ioplmmap to map 0x0a0000 - 0x100000
 * 	[90/03/25            rvb]
 * 
 * Revision 2.1.1.1  90/02/01  13:36:55  rvb
 * 	Allow io privileges to the process.
 * 	[90/02/01            rvb]
 * 
 */
 
#ifdef	MACH_KERNEL
#include <mach/vm_prot.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>
#include <device/io_req.h>
#include <ipc/ipc_port.h>
#else	MACH_KERNEL
#include <sys/types.h>
#include <sys/user.h>
#include <sys/proc.h>
#endif	MACH_KERNEL

#ifdef	MACH_KERNEL
/*
 * IOPL device.
 */
ipc_port_t	iopl_device_port = IP_NULL;

int
ioplopen(dev, flag, ior)
	int	dev;
	int	flag;
	io_req_t ior;
{
	iopl_device_port = ior->io_device->port;
	return (0);
}
#else	MACH_KERNEL
/*ARGSUSED*/
ioplopen(dev, flags)
	dev_t dev;
	int flags;
{
	set_iopl(1);
	return 0;
}
#endif	MACH_KERNEL


/*ARGSUSED*/
ioplclose(dev, flags)
#ifdef	MACH_KERNEL
	int	dev;
#else	MACH_KERNEL
	dev_t dev;
#endif	MACH_KERNEL
	int flags;
{
#ifdef	MACH_KERNEL
	iopl_device_port = IP_NULL;
#else	MACH_KERNEL
	set_iopl(0);
#endif	MACH_KERNEL
	return 0;
}

/*ARGSUSED*/
int
ioplmmap(dev, off, prot)
#ifdef	MACH_KERNEL
int		dev;
vm_offset_t	off;
vm_prot_t	prot;
#else	MACH_KERNEL
dev_t dev;
off_t off;
int prot;
#endif	MACH_KERNEL
{
	if (off > 0x60000)
		return(-1);

	/* Get page frame number for the page to be mapped. */

	return(i386_btop(0xa0000 + off));
}
