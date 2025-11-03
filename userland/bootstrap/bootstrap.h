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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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

/*
 * Bootstrap task definitions
 */
#include "types.h"
#include "fs.h"
#include "disk_inode.h"

#include <mach.h>
#include <device/device_types.h>

#include <stddef.h>		/* for size_t */
#include <mach/boot_info.h>

/*
 * In-core open file.
 */
struct file {
	mach_port_t	f_dev;		/* port to device */
	struct fs *	f_fs;		/* pointer to super-block */
	struct dinode	f_di;		/* copy of on-disk inode */
	int		f_nindir[NIADDR+1];
					/* number of blocks mapped by
					   indirect block at level i */
	vm_offset_t	f_blk[NIADDR];	/* buffer for indirect block at
					   level i */
	vm_size_t	f_blksize[NIADDR];
					/* size of buffer */
	daddr_t		f_blkno[NIADDR];
					/* disk address of block in buffer */
	vm_offset_t	f_buf;		/* buffer for data block */
	vm_size_t	f_buf_size;	/* size of data block */
	daddr_t		f_buf_blkno;	/* block number of data block */
};

/*
 * Error codes for file system errors.
 */
#define	FS_NOT_DIRECTORY	5000	/* not a directory */
#define	FS_NO_ENTRY		5001	/* name not found */
#define	FS_NAME_TOO_LONG	5002	/* name too long */
#define	FS_SYMLINK_LOOP		5003	/* symbolic link loop */
#define	FS_INVALID_FS		5004	/* bad file system */
#define	FS_NOT_IN_FILE		5005	/* offset not in file */
#define	FS_INVALID_PARAMETER	5006	/* bad parameter to a routine */

#define	HEADER_MAX	128

typedef struct objfmt *objfmt_t;
typedef struct objfmt_switch *objfmt_switch_t;

struct objfmt {
    struct loader_info info;
    objfmt_switch_t fmt;
};

struct objfmt_switch {
    const char *name;
    int (*recog)(struct file *, objfmt_t, char **);
    int (*load)(struct file *, objfmt_t, char *);
    void (*symload)(struct file *, mach_port_t, task_port_t, const char *,
		    objfmt_t);
};

extern objfmt_switch_t formats[];

extern boolean_t ask_server_dir;

/*
 * Base kernel address, max legal kernel addresss, and submap size for
 * loading the OSF/1 server into the kernel.  These will be initialized
 * during arg parging to machine-dependendent defaults, which can be
 * overridden with runtime flags.  See boot_dep.c
 */
extern unsigned long	unix_mapbase;
extern unsigned long	unix_mapend;
extern unsigned long	unix_mapsize;

/* Should we try loading server in kernel?  (i.e. -u not given): */
extern boolean_t	unix_in_kernel;

/* Was -k#### given, to specify base loading address for server: */
extern boolean_t	unix_mapbase_specified;

extern int open_file(mach_port_t, const char *, struct file *);
extern void close_file(struct file *);
extern int read_file(struct file *, vm_offset_t, vm_offset_t, vm_size_t);
extern mach_port_t file_dev(struct file *);
extern boolean_t file_is_directory(struct file *);

extern void wire_setup(mach_port_t);
extern void wire_memory(vm_address_t, vm_size_t, vm_prot_t);
extern void wire_thread(void);
extern void wire_all_memory(void);

extern char *boot_device;
#define BOOT_DEVICE_NAME "boot_device"

#define LOAD_SERVER 	0x1
#define LOAD_SYMBOLS	0x2

#define LOAD_DEFAULT (LOAD_SERVER|LOAD_SYMBOLS)

struct server_entry{
	const char 	*symtab_name;
	const char 	*server_name;
	unsigned int	flags;
};

typedef struct server_entry *server_entry_t;

extern server_entry_t server_lookup(char *);

extern int boot_load_program(mach_port_t, mach_port_t,
			     task_port_t, thread_port_t, const char *,
			     const char *, const char *);

extern int atoi(const char *);
extern unsigned long strtoul(const char *, char **, int);
extern void panic(const char *, ...);
extern void parse_args(mach_port_t, int, char **);
extern void printf_init(mach_port_t);
extern void panic_init(mach_port_t);
extern char *strbuild(char *, ...);
extern char *strchr(const char *, int);
extern void printf(const char *, ...);
extern void safe_gets(char *, size_t);
extern void *kalloc(size_t);
extern void kfree(void *, size_t);
extern boolean_t strprefix(const char *, const char *);
extern void ovbcopy(char *, char *, size_t);
extern void *malloc(size_t);
extern void free(void *);
extern void malloc_fork_prepare(void);
extern void malloc_fork_parent(void);
extern void malloc_fork_child(void);

extern char *set_regs(mach_port_t, task_port_t,
		      thread_port_t, struct loader_info *, unsigned long, int);
extern int ex_get_header(struct file *, objfmt_t);
extern kern_return_t host_load_symbol_table(mach_port_t, task_port_t,
					    const char *,
					    vm_offset_t, vm_size_t);

extern boolean_t is_kernel_loadable(mach_port_t, const char *, const char *);
extern boolean_t exc_server(mach_msg_header_t *, mach_msg_header_t *);
extern boolean_t bootstrap_server(mach_msg_header_t *, mach_msg_header_t *);
extern boolean_t service_server(mach_msg_header_t *, mach_msg_header_t *);
extern boolean_t notify_server(mach_msg_header_t *, mach_msg_header_t *);

void service_init(void);
extern boolean_t service_demux(mach_msg_header_t *, mach_msg_header_t *);

extern kern_return_t mach_msg_server(boolean_t (*)(mach_msg_header_t *,
						   mach_msg_header_t *),
				     mach_msg_size_t,
				     mach_port_t,
				     int);
extern void mach_msg_destroy(mach_msg_header_t *);

extern char startup_name[];
extern char *program_name;
extern mach_port_t bootstrap_set;
