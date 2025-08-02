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
 * $Log:	device_init.c,v $
 * Revision 2.6  91/02/05  17:08:54  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:27:56  mrt]
 * 
 * Revision 2.5  90/12/14  10:59:35  jsb
 * 	Added MACH_CLPORT support.
 * 	[90/12/13  21:06:19  jsb]
 * 
 * Revision 2.4  90/08/27  21:55:11  dbg
 * 	Removed call to cinit.
 * 	[90/07/09            dbg]
 * 
 * Revision 2.3  90/06/02  14:47:29  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:51:02  rpd]
 * 
 * Revision 2.2  89/09/08  11:23:33  dbg
 * 	Created.
 * 	[89/08/02            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/89
 *
 * 	Initialize device service as part of kernel task.
 */
#include <mach_clport.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <kern/task.h>

#include <device/device_types.h>
#include <device/device_port.h>

extern void	ds_init();
extern void	dev_lookup_init();
extern void	net_io_init();
extern void	device_pager_init();

extern void	io_done_thread();
extern void	net_rcv_msg_thread();

ipc_port_t	master_device_port;

void
device_service_create()
{
#if	MACH_CLPORT
	/* already allocated by ipc_clport_init */
#else	MACH_CLPORT
	master_device_port = ipc_port_alloc_kernel();
	if (master_device_port == IP_NULL)
	    panic("can't allocate master device port");
#endif	MACH_CLPORT

	ds_init();
	dev_lookup_init();
	net_io_init();
	device_pager_init();

	(void) kernel_thread(kernel_task, io_done_thread);
	(void) kernel_thread(kernel_task, net_rcv_msg_thread);
}
