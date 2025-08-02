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
 * $Log:	bootstrap.c,v $
 * Revision 2.8  91/02/05  17:25:42  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:22  mrt]
 * 
 * Revision 2.7  90/12/14  11:01:58  jsb
 * 	Changes to MACH_CLBOOT support. Use real device port, not a proxy;
 * 	new device forwarding code handles forwarding of requests.
 * 	Have slave not bother starting bootstrap task if there is nothing
 * 	for it to run.
 * 	[90/12/13  21:37:57  jsb]
 * 
 * Revision 2.6  90/09/28  16:55:30  jsb
 * 	Added MACH_CLBOOT support.
 * 	[90/09/28  14:04:43  jsb]
 * 
 * Revision 2.5  90/06/02  14:53:39  rpd
 * 	Load emulator symbols.
 * 	[90/05/11  16:58:37  rpd]
 * 
 * 	Made bootstrap_task available externally.
 * 	[90/04/05            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  22:03:39  rpd]
 * 
 * Revision 2.4  90/01/11  11:43:02  dbg
 * 	Initialize bootstrap print routines.  Remove port number
 * 	printout.
 * 	[89/12/20            dbg]
 * 
 * Revision 2.3  89/11/29  14:09:01  af
 * 	Enlarged the bootstrap task's map to accomodate some unnamed
 * 	greedy RISC box.  Sigh.
 * 	[89/11/07            af]
 * 	Made root_name and startup_name non-preallocated, so that
 * 	they can be changed at boot time on those machines like
 * 	mips and Sun where the boot loader passes command line
 * 	arguments to the kernel.
 * 	[89/10/28            af]
 * 
 * Revision 2.2  89/09/08  11:25:02  dbg
 * 	Pass root partition name to default_pager_setup.
 * 	[89/08/31            dbg]
 * 
 * 	Assume that device service has already been created.
 * 	Create bootstrap task here and give it the host and
 * 	device ports.
 * 	[89/08/01            dbg]
 * 
 * 	Call default_pager_setup.
 * 	[89/07/11            dbg]
 * 
 * 12-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Removed console_port.
 *
 */
/*
 * Bootstrap the various built-in servers.
 */
#include <mach_kdb.h>
#include <mach_clboot.h>

#include <mach/port.h>
#include <mach/message.h>
#include <mach/mach_interface.h>
#include <mach/vm_param.h>
#include <ipc/ipc_port.h>
#include <kern/host.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <device/device_port.h>

mach_port_t	bootstrap_master_device_port;	/* local name */
mach_port_t	bootstrap_master_host_port;	/* local name */

int	boot_load_program();

extern char	*root_name;

char	*startup_name = "/mach_servers/startup";
char	*emulator_name = "/mach_servers/emulator";
#if	MACH_CLBOOT
char	*slave_name = (char *) 0;
#endif	MACH_CLBOOT

extern void	default_pager();
extern void	default_pager_setup();

void		bootstrap();	/* forward */

task_t	bootstrap_task;

void bootstrap_create()
{
	ipc_port_t port;

	/*
	 * Create the bootstrap task.
	 */
#define	BOOTSTRAP_MAP_SIZE	(256 * PAGE_SIZE)

#if	MACH_CLBOOT
	if (node_self() != master_node() && ! slave_name) {
		printf("node %d: not starting bootstrap task.\n", node_self());
		return;
	}
#endif	MACH_CLBOOT
	bootstrap_task = kernel_task_create(TASK_NULL, BOOTSTRAP_MAP_SIZE);

	/*
	 * Insert send rights to the master host and device ports.
	 */

	port = ipc_port_make_send(realhost.host_priv_self);
	bootstrap_master_host_port = 1;
	for (;;) {
		kern_return_t kr;

		kr = mach_port_insert_right(bootstrap_task->itk_space,
					    bootstrap_master_host_port,
					    port, MACH_MSG_TYPE_PORT_SEND);
		if (kr == KERN_SUCCESS)
			break;
		assert(kr == KERN_NAME_EXISTS);

		bootstrap_master_host_port++;
	}

	port = ipc_port_make_send(master_device_port);
	bootstrap_master_device_port = 1;
	for (;;) {
		kern_return_t kr;

		kr = mach_port_insert_right(bootstrap_task->itk_space,
					    bootstrap_master_device_port,
					    port, MACH_MSG_TYPE_PORT_SEND);
		if (kr == KERN_SUCCESS)
			break;
		assert(kr == KERN_NAME_EXISTS);

		bootstrap_master_device_port++;
	}

	/*
	 * Start the bootstrap thread.
	 */
	(void) kernel_thread(bootstrap_task, bootstrap);
}

/*
 * From this point on, the bootstrap task uses the external
 * versions of the kernel interface routines.
 */
#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>
#include <boot_ufs/boot_printf.h>

extern void	boot_printf_init();

void bootstrap()
{
	register kern_return_t	result;
	mach_port_t		user_task;
	mach_port_t		user_thread;
	mach_port_t		bootstrap_port;

	mach_port_t		my_task = mach_task_self();

	char			flag_string[12];

	boot_printf_init();


	/*
	 * Allocate a port to pass initial server ports back and forth.
	 */
	result = mach_port_allocate(my_task, MACH_PORT_RIGHT_RECEIVE,
				    &bootstrap_port);
	assert(result == KERN_SUCCESS);

	result = mach_port_insert_right(my_task, bootstrap_port,
					bootstrap_port,
					MACH_MSG_TYPE_MAKE_SEND);
	assert(result == KERN_SUCCESS);

	result = task_set_bootstrap_port(my_task, bootstrap_port);
	assert(result == KERN_SUCCESS);

	/*
	 * Set up the temporary in-kernel default pager.
	 */
	default_pager_setup(bootstrap_master_device_port, root_name);

	/*
	 * Create the user task and thread to run the startup file.
	 */
	result = task_create(my_task, FALSE, &user_task);
	if (result != KERN_SUCCESS)
	    panic("task_create %d", result);

	result = thread_create(user_task, &user_thread);
	if (result != KERN_SUCCESS)
	    panic("thread_create %d", result);

	/*
	 * Load the startup file.
	 * Pass it a command line of
	 * "startup -boot_flags root_name"
	 */
	get_boot_flags(flag_string);
	result = boot_load_program(bootstrap_master_device_port,
				   user_task,
				   user_thread,
				   root_name,
#if	MACH_CLBOOT
				   (master_node() == node_self()
				    ? startup_name
				    : slave_name),
#else	MACH_CLBOOT
				   startup_name,
#endif	MACH_CLBOOT
				   flag_string,
				   root_name,
				   (char *)0);
	if (result != 0)
	    panic("boot_load_program %d", result);

	/*
	 * Read emulator symbol table.
	 * Startup symbol table was read inside boot_load_program.
	 */
	read_emulator_symbols(bootstrap_master_device_port,
				   root_name,
				   emulator_name);

	/*
	 * Start up the thread
	 */
	result = thread_resume(user_thread);
	if (result != KERN_SUCCESS)
	    panic("thread_resume %d", result);

	{
	    struct imsg {
		mach_msg_header_t	hdr;
		mach_msg_type_t		port_desc_1;
		mach_port_t		port_1;
		mach_msg_type_t		port_desc_2;
		mach_port_t		port_2;
	    } imsg;

	    /*
	     * Wait for the port-request message
	     */
	    while (mach_msg(&imsg.hdr, MACH_RCV_MSG,
			    0, sizeof imsg.hdr, bootstrap_port,
			    MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL)
						!= MACH_MSG_SUCCESS)
		continue;

	    /*
	     * Send back the host and device ports
	     */

	    imsg.hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX |
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(imsg.hdr.msgh_bits), 0);
	    /* msgh_size doesn't need to be initialized */
	    /* leave msgh_remote_port untouched */
	    imsg.hdr.msgh_local_port = MACH_PORT_NULL;
	    /* leave msgh_kind untouched */
	    imsg.hdr.msgh_id += 100;	/* this is a reply msg */

	    imsg.port_desc_1.msgt_name = MACH_MSG_TYPE_COPY_SEND;
	    imsg.port_desc_1.msgt_size = 32;
	    imsg.port_desc_1.msgt_number = 1;
	    imsg.port_desc_1.msgt_inline = TRUE;
	    imsg.port_desc_1.msgt_longform = FALSE;
	    imsg.port_desc_1.msgt_deallocate = FALSE;
	    imsg.port_desc_1.msgt_unused = 0;

	    imsg.port_1	= bootstrap_master_host_port;

	    imsg.port_desc_2 = imsg.port_desc_1;

	    imsg.port_2	= bootstrap_master_device_port;

	    result = mach_msg(&imsg.hdr, MACH_SEND_MSG,
			      sizeof imsg, 0, MACH_PORT_NULL,
			      MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	    if (result != MACH_MSG_SUCCESS)
		panic("mach_msg %x", result);
	}

	/*
	 * Become the default pager
	 */
	default_pager(bootstrap_master_host_port);
	/*NOTREACHED*/
}

/*
 * Parse the boot flags into an argument string.
 * Format as a standard flag argument: '-asd...'
 */
#include <sys/reboot.h>

get_boot_flags(str)
	char	str[];	/* OUT */
{
	register char *cp;
	register int	bflag;
#ifdef	mips
	extern char *machine_get_boot_flags();
	cp = machine_get_boot_flags(str);
#else	mips
	cp = str;
	*cp++ = '-';
#endif	mips

	bflag = boothowto;

	if (bflag & RB_ASKNAME)
	    *cp++ = 'a';
	if (bflag & RB_SINGLE)
	    *cp++ = 's';
#if	MACH_KDB
	if (bflag & RB_KDB)
	    *cp++ = 'd';
#endif	MACH_KDB
	if (bflag & RB_INITNAME)
	    *cp++ = 'n';

	if (cp == &str[1])	/* no flags */
	    *cp++ = 'x';
	*cp = '\0';
}
