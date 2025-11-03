/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
 * Open Software Foundation, Inc. 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of ("OSF") or Open Software 
 * Foundation not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */
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
 * Bootstrap the various built-in servers.
 */

#include "bootstrap.h"

#include <limits.h>

#include <mach/mach_traps.h>
#include <mach/bootstrap.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/exc_server.h>
#include <mach/bootstrap_server.h>

mach_port_t	bootstrap_master_device_port;	/* local name */
mach_port_t	bootstrap_master_host_port;	/* local name */

char	server_dir_name[PATH_MAX];

char	startup_name[NAME_MAX + 1];

struct server_entry servers[] = {
    {"name_server",	"name_server"			, LOAD_DEFAULT	},
    {"default_pager",	"default_pager"			, LOAD_DEFAULT	},
    {"unix",		(const char *)startup_name	, LOAD_DEFAULT	}
};

static char my_name[] = "(bootstrap):";
char *program_name = (char *)"(bootstrap)";

char	*boot_device = (char *)BOOT_DEVICE_NAME;

boolean_t bootstrap_demux(mach_msg_header_t *, mach_msg_header_t *);
void bootstrap_initialize(mach_port_t);
void start_bootstrap_thread(boolean_t);
boolean_t bootstrap_compat(mach_msg_header_t *, mach_msg_header_t *);

/*
 * Our task port.
 */
task_port_t	bootstrap_self;
mach_port_t	bootstrap_exception_port;

/*
 * Port set for bootstrap task.
 */
mach_port_t	bootstrap_set;

/*
 * We receive bootstrap requests on this port.
 */
mach_port_t	bootstrap_bootstrap_port;

/*
 * Maximum message size for mach_msg_server.
 */
mach_msg_size_t bootstrap_msg_size = 128;

/*
 * Set when we should query for the servers directory.
 */
boolean_t	ask_server_dir = FALSE;

/*
 * See boot_dep.c
 */
unsigned long	unix_mapbase,
		unix_mapend,
		unix_mapsize;		/* Initialized by parse_args */
boolean_t	unix_in_kernel = TRUE;	/* Default is to try kernel-loading */
boolean_t	unix_mapbase_specified = FALSE;  /* Was -k### given? */

/*
 * Bootstrap task.
 * Runs in user space.
 *
 * Called as 'boot -switches'
 *
 */
int
main(int argc, char **argv)
{
	register kern_return_t	kr;
	task_port_t		user_task;
	thread_port_t		user_thread;
	mach_port_t		bootstrap_port;
	int			i;

	/*
	 * Get our bootstrap port
	 */
	kr = task_get_special_port(mach_task_self(),
				       TASK_BOOTSTRAP_PORT,
				       &bootstrap_port);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	/*
	 * Get master host and device ports
	 */
	kr = bootstrap_privileged_ports(bootstrap_port,
					    &bootstrap_master_host_port,
					    &bootstrap_master_device_port);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	/*
	 * Initialize console and reboot services
	 */
	printf_init(bootstrap_master_device_port);
	panic_init(bootstrap_master_host_port);

	strcpy(startup_name, "startup");

	parse_args(bootstrap_master_host_port, argc, argv);

	/*
	 * Check out the server directory.
	 */
	strcpy(server_dir_name, "/dev/");
	strcat(server_dir_name, boot_device);
	strcat(server_dir_name, "/mach_servers");

	while (TRUE) {
	    struct file	f;

	    if (ask_server_dir) {
		char new_server_dir[PATH_MAX];

		printf("Server directory? [ %s ] ",
			server_dir_name);
		safe_gets(new_server_dir, sizeof(new_server_dir));
		if (new_server_dir[0] != '\0')
		    strcpy(server_dir_name, new_server_dir);
	    }

	    bzero((char *)&f, sizeof(f));
	    kr = open_file(bootstrap_master_device_port,
			   server_dir_name, &f);
	    if (kr != 0) {
		printf("Can't open server directory %s: %d\n",
		       server_dir_name, kr);
		ask_server_dir = TRUE;
		continue;
	    }
	    if (file_is_directory(&f)) {
		/*
		 * Found server directory.
		 */
		close_file(&f);
		break;
	    }
	    close_file(&f);
	    printf("%s is not a directory\n", server_dir_name);
	    ask_server_dir = TRUE;
	}

	/*
	 *	This task will become the bootstrap task.
	 */
	bootstrap_self = mach_task_self();

	/*
	 *	Initialize the exception port.
	 */
	kr = mach_port_allocate(bootstrap_self, MACH_PORT_RIGHT_RECEIVE,
				    &bootstrap_exception_port);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	/*
	 *	Initialize the bootstrap port.
	 */
	kr = mach_port_allocate(bootstrap_self, MACH_PORT_RIGHT_RECEIVE,
				    &bootstrap_bootstrap_port);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	kr = mach_port_allocate(bootstrap_self, MACH_PORT_RIGHT_PORT_SET,
				    &bootstrap_set);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	kr = mach_port_move_member(bootstrap_self,
				       bootstrap_exception_port,
				       bootstrap_set);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	kr = mach_port_move_member(bootstrap_self,
				       bootstrap_bootstrap_port,
				       bootstrap_set);
	if (kr != KERN_SUCCESS)
	    panic(my_name);

	/*
	 * Allocate the service server ports and add them to our
	 * port set.
	 */
	service_init();

	/*
	 * task_set_exception_ports and task_set_bootstrap_port
	 * both require a send right.
	 */
	(void) mach_port_insert_right(bootstrap_self, bootstrap_bootstrap_port,
				      bootstrap_bootstrap_port,
				      MACH_MSG_TYPE_MAKE_SEND);
	(void) mach_port_insert_right(bootstrap_self, bootstrap_exception_port,
				      bootstrap_exception_port,
				      MACH_MSG_TYPE_MAKE_SEND);

	/*
	 * Change our exception port.
	 */
	(void) task_set_exception_ports(bootstrap_self,
					EXC_MASK_ALL &
					~(EXC_MASK_SYSCALL |
					  EXC_MASK_MACH_SYSCALL),
					bootstrap_exception_port,
					EXCEPTION_DEFAULT, 0);

	for (i = 0; i < (sizeof(servers) / sizeof(servers[0])); i++) {

	    /*
	     * Check if we really want to load this server
	     */

	    if (!(servers[i].flags & LOAD_SERVER))
		continue;

	    /*
	     * Create a task and thread to run a server
	     */
	    if (strcmp(servers[i].symtab_name, "unix") == 0) {
		unix_in_kernel = unix_in_kernel &&
			is_kernel_loadable(bootstrap_master_device_port,
					   server_dir_name,
					   servers[i].server_name);
		if (unix_in_kernel)
		    kr = kernel_task_create(bootstrap_self, unix_mapbase,
			    unix_mapsize, &user_task);
		else
		    kr = task_create(bootstrap_self, FALSE, &user_task);
	    } else
		kr = task_create(bootstrap_self, FALSE, &user_task);

	    if (kr != KERN_SUCCESS)
		panic("task_create %d", kr);

	    (void) task_set_exception_ports(user_task,
					    EXC_MASK_ALL &
					    ~(EXC_MASK_SYSCALL |
					      EXC_MASK_MACH_SYSCALL),
					    bootstrap_exception_port,
					    EXCEPTION_DEFAULT, 0);
	    (void) task_set_bootstrap_port(user_task,
					   bootstrap_bootstrap_port);

	    kr = thread_create(user_task, &user_thread);
	    if (kr != KERN_SUCCESS)
		panic("thread_create %d", kr);

	    /*
	     * Load the server.
	     */
	    kr = boot_load_program(bootstrap_master_host_port,
				   bootstrap_master_device_port,
				   user_task,
				   user_thread,
				   ((servers[i].flags & LOAD_SYMBOLS) ?
				    servers[i].symtab_name : (char *) 0), 
				   server_dir_name,
				   servers[i].server_name);
	    if (kr != 0)
		panic("boot_load_program %d", kr);

	    /*
	     * Start up the thread
	     */
	    kr = thread_resume(user_thread);
	    if (kr != KERN_SUCCESS)
		panic("thread_resume %d", kr);

	    (void) mach_port_deallocate(bootstrap_self, user_task);
	    (void) mach_port_deallocate(bootstrap_self, user_thread);
	}

	/*
	 *	Deallocate the excess send rights.
	 */
	(void) mach_port_deallocate(bootstrap_self, bootstrap_exception_port);
	(void) mach_port_deallocate(bootstrap_self, bootstrap_bootstrap_port);

	for (;;) {
	    kr = mach_msg_server(bootstrap_demux,
				 bootstrap_msg_size,
				 bootstrap_set,
				 MACH_MSG_OPTION_NONE);
	    panic(my_name, kr);
	}
}

/*
 *	We have three types of requests:
 *
 *		bootstrap requests from our "children"
 *		exceptions from ourself, or our "children"
 *		service requests from our "children"
 */

boolean_t
bootstrap_demux(mach_msg_header_t *in, mach_msg_header_t *out)
{
	if (in->msgh_local_port == bootstrap_bootstrap_port)
		return bootstrap_server(in, out);
	if (in->msgh_local_port == bootstrap_exception_port)
		return exc_server(in, out);
	return service_demux(in, out);
}

/*
 *	Catch exceptions.
 */

kern_return_t
catch_exception_raise(mach_port_t exception_port,
		      mach_port_t thread,
		      mach_port_t task,
		      exception_type_t exception,
		      exception_data_t code,
		      mach_msg_type_number_t codeCnt)
{
	printf("(bootstrap)catch_exception_raise(%d)\n", exception);
	panic(my_name);

	/* mach_msg_server will deallocate thread/task for us */

	return KERN_FAILURE;
}

kern_return_t
catch_exception_raise_state(mach_port_t port,
			    exception_type_t exc_type,
			    exception_data_t ecode,
			    mach_msg_type_number_t code_count,
			    int *flavor,
			    thread_state_t in_state,
			    mach_msg_type_number_t icnt,
			    thread_state_t out_state,
			    mach_msg_type_number_t *ocnt)
{
	printf("(bootstrap)catch_exception_raise_state(%d)\n", exc_type);
	panic(my_name);

	/* mach_msg_server will deallocate thread/task for us */

	return KERN_FAILURE;
}

kern_return_t
catch_exception_raise_state_identity(mach_port_t port,
				     mach_port_t thread_port,
				     mach_port_t task_port,
				     exception_type_t exc_type,
				     exception_data_t code,
				     mach_msg_type_number_t code_count,
				     int *flavor,
				     thread_state_t in_state,
				     mach_msg_type_number_t icnt,
				     thread_state_t out_state,
				     mach_msg_type_number_t *ocnt)
{
	printf("(bootstrap)catch_exception_raise_state_identity(%d)\n",
	       exc_type);
	panic(my_name);

	/* mach_msg_server will deallocate thread/task for us */

	return KERN_FAILURE;
}

/*
 *	Handle bootstrap requests.
 */

kern_return_t
do_bootstrap_privileged_ports(mach_port_t bootstrap,
			      mach_port_t *hostp,
			      mach_port_t *devicep)
{
	*hostp = bootstrap_master_host_port;
	*devicep = bootstrap_master_device_port;
	return KERN_SUCCESS;
}

server_entry_t 
server_lookup(char *name)
{
	register i;

	for (i = 0; i < (sizeof(servers) / sizeof(servers[0])); i++)
		if (!strcmp(servers[i].server_name, name))
			return(&servers[i]);
	printf("(bootstrap): %s: no such server registered\n", name);
	return (server_entry_t) 0;
}
