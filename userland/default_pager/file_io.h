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
/* CMU_HIST */
/*
 * Revision 2.4  92/02/23  22:25:47  elf
 * 	Added macros for raw (unstructured) devices.
 * 	[92/02/22  18:53:33  af]
 * 
 * 	Added defs for remove_file_direct(), exported.
 * 	[92/02/19  17:31:35  af]
 * 
 * Revision 2.3  92/01/14  16:43:23  rpd
 * 	Changed <mach/mach.h> to <mach.h>.
 * 	[92/01/07  13:43:43  rpd]
 * 
 * Revision 2.2  92/01/03  19:57:19  dbg
 * 	Make file_direct self-contained: add the few fields needed from
 * 	the superblock.
 * 	[91/10/17            dbg]
 * 
 * 	Add close_file to free storage.
 * 	[91/09/25            dbg]
 * 
 * 	Move outside of kernel.
 * 	[91/09/04            dbg]
 * 
 * Revision 2.6  91/08/28  11:09:46  jsb
 * 	Added struct file_direct and associated functions.
 * 	[91/08/19            rpd]
 * 
 * Revision 2.5  91/05/18  14:28:52  rpd
 * 	Added f_lock.
 * 	[91/04/03            rpd]
 * 
 * Revision 2.4  91/05/14  15:23:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:01:33  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:57  mrt]
 * 
 * Revision 2.2  90/08/27  21:45:45  dbg
 * 	Re-create as boot_ufs/file_io.h.
 * 	[90/07/18            dbg]
 * 
 * 	Add table containing number of blocks mapped by each level of
 * 	indirect block.
 * 	[90/07/17            dbg]
 * 
 * 	Declare error codes.
 * 	[90/07/16            dbg]
 * 
 * Revision 2.3  90/06/02  14:45:38  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:31:55  rpd]
 * 
 * Revision 2.2  89/09/08  11:22:21  dbg
 * 	Put device_port and superblock pointer into inode.
 * 	Rename structure to 'struct file'.
 * 	[89/08/24            dbg]
 * 
 * 	Version that reads the disk instead of mapping it.
 * 	[89/07/17            dbg]
 * 
 * 26-Oct-88  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 */
/* CMU_ENDHIST */
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
 */

#ifndef	_BOOT_UFS_FILE_IO_H_
#define	_BOOT_UFS_FILE_IO_H_

/*
 * Read-only file IO.
 */

#include <mach.h>
#include <cthreads.h>

#include <device/device_types.h>

#include "defs.h"
#include "fs.h"
#include "disk_inode.h"

/*
 * In-core open file.
 */
struct file {
	struct mutex	f_lock;		/* lock */
	
	mach_port_t	f_dev;		/* port to device */
	struct fs *	f_fs;		/* pointer to super-block */
	long		f_scsize;	/* disk sector size */
	struct icommon	i_ic;		/* copy of on-disk inode */
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

#define file_is_structured(_fp_)	((_fp_)->f_fs != 0)

/*
 * In-core open file, with in-core block map.
 */
struct file_direct {
	mach_port_t	fd_dev;		/* port to device */
	daddr_t *	fd_blocks;	/* array of disk block addresses */
	long		fd_size;	/* number of blocks in the array */
	long		fd_bsize;	/* disk block size */
	long		fd_bshift;	/* log2(fd_bsize) */
	long		fd_scsize;	/* disk sector size */
	long		fd_scshift;	/* log2 (fd_scsize) */
	long		fd_fsbtodb;	/* log2(fd_bsize / fd_scsize) */
};

#define	file_is_device(_fd_)		((_fd_)->fd_blocks == 0)

/*
 * Exported routines.
 */
extern int open_file(mach_port_t, const char *, struct file *);
extern void close_file(struct file *);
extern int read_file(struct file *, vm_offset_t, vm_offset_t,
		     vm_size_t, vm_size_t *);

extern int open_file_direct(struct file *, struct file_direct *, boolean_t);
extern int add_file_direct(struct file_direct *, struct file *);
extern void remove_file_direct(struct file_direct *);
extern int file_wire_direct(mach_port_t, struct file_direct *);
extern int page_read_file_direct(struct file_direct *, vm_offset_t, vm_size_t,
				 vm_offset_t *, vm_size_t *);
extern int page_write_file_direct(struct file_direct *, vm_offset_t,
				  vm_offset_t, vm_size_t, vm_size_t *);

/*
 * Error codes for file system errors.
 */

#define	FS_NOT_DIRECTORY	5000		/* not a directory */
#define	FS_NO_ENTRY		5001		/* name not found */
#define	FS_NAME_TOO_LONG	5002		/* name too long */
#define	FS_SYMLINK_LOOP		5003		/* symbolic link loop */
#define	FS_INVALID_FS		5004		/* bad file system */
#define	FS_NOT_IN_FILE		5005		/* offset not in file */
#define	FS_INVALID_PARAMETER	5006		/* bad parameter to
						   a routine */
#define FS_BAD_RECSIZE          5007            /* SBOFF not a multiple of
                                                   record size */

#endif	/* _FILE_IO_H_ */
