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
 * $Log:	def_pager_setup.c,v $
 * Revision 2.7  91/02/05  17:00:34  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:27  mrt]
 * 
 * Revision 2.6  90/12/20  16:35:09  jeffreyh
 * 	Added init for  default_partition_lock
 * 	[90/12/11            jeffreyh]
 * 
 * Revision 2.5  90/08/27  21:44:42  dbg
 * 	Use 'new' file_io package.
 * 	[90/07/18            dbg]
 * 
 * Revision 2.4  90/06/02  14:45:17  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:28:45  rpd]
 * 
 * Revision 2.3  90/01/11  11:41:03  dbg
 * 	Use bootstrap_task print routines.
 * 	[89/12/20            dbg]
 * 
 * Revision 2.2  89/09/08  11:22:01  dbg
 * 	Created.
 * 	[89/09/01  17:12:55  dbg]
 * 
 * 31-Aug-89  David Golub (dbg) at Carnegie-Mellon University
 *	Use new open_file routine.
 *
 */
#include <boot_ufs/file_io.h>
#include <mach/mach_interface.h>
#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>
#include <boot_ufs/boot_printf.h>


#include <kern/kalloc.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <vm/vm_map.h>

/*
 * Set up default pager
 */
char	paging_file_name[128] = "\0";

extern void	create_default_partition();
decl_simple_lock_data(extern ,default_partition_lock);

void
default_pager_setup(master_device_port, rootname)
	mach_port_t master_device_port;
	char	*rootname;
{
	register struct file	*fp;
	register int	result;

	int		(*p_read)(), (*p_write)();
	extern int	page_read_file(), page_write_file();
	vm_size_t	size;

	extern char *	strbuild();
	simple_lock_init(&default_partition_lock);
	fp = (struct file *)kalloc(sizeof(struct file));
	bzero((char *)fp, sizeof(struct file));

	(void) strbuild(paging_file_name,
			"/dev/",
			rootname,
			"/mach_servers/paging_file",
			(char *)0);

	while (TRUE) {
	    result = open_file(master_device_port,
			       paging_file_name,
			       fp);
	    if (result == 0)
		break;
	    printf("Can't open paging file %s: %d\n",
		   paging_file_name,
		   result);

	    bzero(paging_file_name, sizeof(paging_file_name));
	    printf("Paging file name ? ");
	    gets(paging_file_name, sizeof(paging_file_name));

	    if (paging_file_name[0] == 0) {
		printf("*** WARNING: running without paging area!\n");
		return;
	    }
	}

	printf("Paging file %s found\n", paging_file_name);

	/*
	 * Wire the buffers, since they will be used by the default pager.
	 * Set wiring privileges first.
	 */
	current_task()->map->wiring_allowed = TRUE;	/* XXX */
	result = file_wire(fp);
	if (result) {
	    panic("Can't wire down file buffers: %d", result);
	}

	size = fp->i_size;
	p_read = page_read_file;
	p_write = page_write_file;

	/*
	 * Set up the default paging partition
	 */
	create_default_partition(size, p_read, p_write, (char *)fp);

	/*
	 * Our caller will become the default pager - later
	 */
}

#ifdef notyet
/* do this in separate task - synchronize with messages */
 
void
default_pager_create(master_host_port, master_device_port)
	port_t	master_host_port;
	port_t	master_device_port;
{
	pager_task = kernel_task_create(task_self(), PAGING_TASK_MAP_SIZE);
	(void) kernel_thread(pager_task, default_pager);

	/*
	 * Wait for the request for the ports
	 */
	{
	    struct imsg {
		msg_header_t	hdr;
		msg_type_t	port_desc_1;
		port_t		port_1;
		msg_type_t	port_desc_2;
		port_t		port_2;
	    } imsg;

	    bzero((char *)&imsg, sizeof(imsg));
	    imsg.hdr.msg_size = sizeof(imsg_hdr);
	    (void) task_get_bootstrap_port(task_self, &bootstrap_port);
	    imsg.hdr.msg_local_port = bootstrap_port;
	    (void) msg_receive(&imsg.hdr, MSG_OPTION_NONE, 0);

	    /*
	     * Send back the host and device ports
	     */
	    imsg.hdr.msg_simple = FALSE;
	    imsg.hdr.msg_size	= sizeof(imsg);
	    imsg.hdr.msg_type	= MSG_TYPE_NORMAL;
	    /* local port is our bootstrap port */
	    /* remote port is the reply port */
	    imsg.hdr.msg_id += 100;	/* reply msg */

	    imsg.port_desc_1.msg_type_name = MSG_TYPE_PORT;
	    imsg.port_desc_1.msg_type_size = sizeof(port_t) * 8;
	    imsg.port_desc_1.msg_type_number = 1;
	    imsg.port_desc_1.msg_type_inline = TRUE;
	    imsg.port_desc_1.msg_type_longform = FALSE;
	    imsg.port_desc_1.msg_type_deallocate = FALSE;

	    imsg.port_1 = master_host_port;

	    imsg.port_desc_2 = imsg.port_desc_1;

	    imsg.port_2 = master_device_port;

	    result = msg_send(&imsg.hdr, MSG_OPTION_NONE, 0);
	    if (result)
		panic("msg_send %d", result);

	    /*
	     * Wait for the reply message after startup
	     */
	    bzero((char *)&imsg, sizeof(imsg));
	    imsg.hdr.msg_size = sizeof(imsg_hdr);
	    (void) task_get_bootstrap_port(task_self, &bootstrap_port);
	    imsg.hdr.msg_local_port = bootstrap_port;
	    (void) msg_receive(&imsg.hdr, MSG_OPTION_NONE, 0);

	}

	/*
	 * Done
	 */
}

void
device_pager()
{
	(void) task_get_bootstrap_port(task_self(), &bootstrap_port);
	(void) port_allocate(task_self(), &reply_port);

	/*
	 * Get the host and device ports
	 */
	{
	    struct imsg {
		msg_header_t	hdr;
		msg_type_t	port_desc_1;
		port_t		port_1;
		msg_type_t	port_desc_2;
		port_t		port_2;
	    } imsg;

	    imsg.hdr.msg_simple = TRUE;
	    imsg.hdr.msg_size = sizeof(imsg.hdr);
	    imsg.hdr.msg_type = MSG_TYPE_NORMAL;
	    imsg.hdr.msg_local_port = reply_port;
	    imsg.hdr.msg_remote_port = bootstrap_port;
	    imsg.hdr.msg_id = 9999;

	    result = msg_send(&imsg.hdr, MSG_OPTION_NONE, 0);
	    if (result)
		panic("msg_send: %d", result);

	    bzero((char *)&imsg, sizeof(imsg));
	    imsg.hdr.msg_size = sizeof(imsg);
	    imsg.hdr.msg_local_port = reply_port;
	    result = msg_receive(&imsg.hdr, MSG_OPTION_NONE, 0);
	    if (result)
		panic("msg_receive: %d", result);

	    host_port = imsg.port_1;
	    device_port = imsg.port_2;
	}

	...

	/*
	 * Send back synchronization message to resume
	 * the bootstrap
	 */
	{
	    msg_header_t	imsg;

	    imsg.msg_simple = TRUE;
	    imsg.msg_size = sizeof(imsg.hdr);
	    imsg.msg_type = MSG_TYPE_NORMAL;
	    imsg.msg_local_port = reply_port;
	    imsg.msg_remote_port = bootstrap_port;
	    imsg.msg_id = 99999;

	    result = msg_send(&imsg, MSG_OPTION_NONE, 0);
	    if (result)
		panic("msg_send: %d", result);

	}

	device_pager_loop();
}
#endif	notyet
