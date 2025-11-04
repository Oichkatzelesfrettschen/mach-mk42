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
 * Copyright (c) 1991,1990,1989, 1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include "bootstrap.h"
#include <mach/mach_host.h>

#include <mach/i386/vm_param.h>
#include <errno.h>

#define server_delimiter ':'

/*
 * The min and max kernel address and submap size for loading the OSF/1 server
 * into the kernel.  The kernel addresses are exported by Mach.  The size
 * is big enough for an OSF/1 server that is using a 7.2MB buffer cache,
 * such as on a 36MB system.  The min address can be overridden by use of
 * -k#### and submap size can be set with -S###.
 */
#define UNIX_MAPBASE_DEFAULT	VM_MIN_KERNEL_LOADED_ADDRESS /* XXX */
#define UNIX_MAPEND_DEFAULT	VM_MAX_KERNEL_LOADED_ADDRESS /* XXX */
#define UNIX_MAPSIZE_DEFAULT	(48 * 1024 * 1024)	/* XXX */

extern boolean_t ask_server_dir;

kernel_boot_info_t kernel_boot_info;

void nab_minusk(char **);
void nab_minusS(char **);

/*
 * Is c part of a word?  (Newline is a delimiter, to cope with
 * bootmagic strings appended to normal boot_info by NORMA
 * kernel.)
 */
#define is_wordchar(c)	((c) != '\0' && (c) != '\n' && (c) != ' ')

/*
 * For the AT386, we ask the kernel for the complete boot string
 */

void
parse_args(mach_port_t master_host_port, int argc, char **argv)
{
	char *boot_info;
	kern_return_t kr;

	kr = host_get_boot_info(master_host_port, kernel_boot_info);
	if (kr != KERN_SUCCESS) {
	    printf("(bootstrap): host_get_boot_info() failed: %d\n", kr);
	    return;
	}

	/*
	 * The general syntax of kernel_boot_info is:
	 *
	 * rootdev_name [ flags ] [ kernel[:server] ]
	 */
	boot_info = kernel_boot_info;
	printf("(bootstrap): boot_info %s\n", boot_info);

	/* Initialize mapbase and mapsize to their defaults: */
	unix_mapbase = UNIX_MAPBASE_DEFAULT;
	unix_mapend = UNIX_MAPEND_DEFAULT;
	unix_mapsize = UNIX_MAPSIZE_DEFAULT;

	/*
	 * Skip over the rootdev name
	 */
	while (is_wordchar(*boot_info))
	    boot_info++;
	while (*boot_info == ' ')
	    boot_info++;

	/*
	 * Process rest of command line -- at start of each
	 * iteration, we expect to be at the start of an argument,
	 * or at the end of line.
	 */
	while (is_wordchar(*boot_info)) {
		if (*boot_info == '-') {
			/*
			 * Process a flag.
			 */
			while (is_wordchar(*boot_info)) {
				if (*boot_info == 'q') {
					ask_server_dir = TRUE;
					boot_info++;
				}
				if (*boot_info == 'k') {
					nab_minusk(&boot_info);
				}
				else if (*boot_info == 'S') {
					nab_minusS(&boot_info);
				}
				else if (*boot_info == 'u') {
					/* Don't try to load it in kernel: */
					unix_in_kernel = FALSE;
					boot_info++;
				}
				else
					boot_info++;
			}
		}
		else {
			/*
			 * Check for the optional kernel:server arg.
			 */
			while (is_wordchar(*boot_info)) {
				if (*boot_info++ != server_delimiter)
					continue;
				if (is_wordchar(*boot_info)) {
					char *s = startup_name;

					while (is_wordchar(*boot_info))
						*s++ = *boot_info++;
					*s = 0;
				}
			}
		}
		while (*boot_info == ' ')
			boot_info++;
	}
}

void
nab_minusk(
	char **pboot_info)
{
	char *binfo = *pboot_info;
	char *end;
	unsigned long mapbase;

	for (++binfo; *binfo == ' '; )
		++binfo;
	errno = 0;
	mapbase = strtoul(binfo, &end, 0);
	if (!errno && end != binfo) {
		unix_in_kernel = TRUE;
		unix_mapbase = mapbase;
		unix_mapbase_specified = TRUE;
	}
	*pboot_info = end;
}

void
nab_minusS(
	char **pboot_info)
{
	char *binfo = *pboot_info;
	char *end;
	unsigned long mapsize;

	for (++binfo; *binfo == ' '; )
		++binfo;
	errno = 0;
	mapsize = strtoul(binfo, &end, 0);
	if (!errno && end != binfo) {
		unix_in_kernel = TRUE;
		unix_mapsize = mapsize;
	}
	*pboot_info = end;
}
