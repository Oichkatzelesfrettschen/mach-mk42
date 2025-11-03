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
 * Revision 2.12  92/07/20  13:32:18  cmaeda
 * 	Added private version of set_ras_address for fast_tas support.
 * 	[92/05/11  14:31:52  cmaeda]
 * 
 * Revision 2.11  92/05/05  10:03:46  danner
 * 	For merge purposes, backed-out the unstable stuff.
 * 	[92/05/04  11:12:01  af]
 * 
 * 	Now we can page an object across partitions.
 * 	Initial rough ideas about automatically extending
 * 	paging space.
 * 	[92/03/11  02:23:58  af]
 * 
 * Revision 2.10  92/03/06  13:58:48  rpd
 * 	Fixed pager_dealloc_page calls in pager_dealloc (from af).
 * 	Removed chatty printfs.
 * 	[92/03/06            rpd]
 * 
 * Revision 2.9  92/03/05  15:58:35  rpd
 * 	Changed PAGEMAP_ENTRIES from 128 to 64.  From af.
 * 	[92/03/05            rpd]
 * 
 * Revision 2.8  92/03/03  12:12:04  rpd
 * 	Changed to catch exception messages and handle bootstrap requests.
 * 	Added partition_init.
 * 	[92/03/03            rpd]
 * 
 * Revision 2.7  92/02/25  11:22:38  elf
 * 	Accept creation of objects bigger than any one partition, in
 * 	anticipation of the code that will page across partitions.
 * 	Since we are at it, also proceed with no paging partitions:
 * 	rely on killing unlucky objects on pageouts.
 * 	[92/02/25            af]
 * 
 * Revision 2.6  92/02/23  23:00:31  elf
 * 	Copyright updated, corrected history.
 * 	[92/02/23            elf]
 * 
 * Revision 2.5  92/02/23  22:25:35  elf
 * 	Improved handling of big objects, fixed a deadlock in
 * 	object relocation, improved printouts.
 * 	Now only crash if out of memory, otherwise use the old
 * 	code that just marked the object as in-error.
 * 	[92/02/23  13:25:49  af]
 * 
 * 	As per jsb instructions, removed all NORMA conditionals.
 * 	Rename port names to odd values, a trivial heuristic that
 * 	makes name conflicts even more unlikely.
 * 	[92/02/22            af]
 * 
 * 	Refined the port name conflict problem.  Instead of renaming
 * 	ports that we send to, just set aside the memory that we cannot
 * 	use.  When objects get deleted put back the memory in the system.
 * 	[92/02/21            af]
 * 
 * 	Added renaming of request and name ports (from af).
 * 	[92/02/21            danner]
 * 
 * 	Many changes. Now supports adding/removing paging files, it does
 * 	not immediately panic if a paging file fills up but relocates the
 * 	object elsewhere, it uses the precious attribute in data_supply
 * 	to reduce paging space usage (under USE_PRECIOUS conditional,
 * 	enabled).
 * 	[92/02/19  17:29:54  af]
 * 
 * 	Two mods: changed bitmap ops to work one int at a time rather
 * 	than one byte at a time.  This helps under load, e.g. when the
 * 	paging file is large and busy. Second mod to use port-to-pointer
 * 	casting in lookups, rather than hash+list searching.  This not
 * 	only helps under load (I see >600 objects on my pmax) but also
 * 	increases parallelism a little.
 * 	Shrunk the code size by one page in the process.
 * 	[92/02/14  01:44:23  af]
 * 
 * Revision 2.4  92/01/23  15:19:41  rpd
 * 	Changed to not include mig server interfaces.
 * 	[92/01/23            rpd]
 * 
 * Revision 2.3  92/01/14  16:43:14  rpd
 * 	Moved mach/default_pager_object.defs to mach/default_pager.defs.
 * 	Revised default_pager_info etc. for their new definitions.
 * 	Removed (now) unnecessary #define's to rename kernel functions.
 * 	[92/01/13            rpd]
 * 	Added page_size to default_pager_info.
 * 	Added default_pager_object_pages.
 * 	[92/01/03            rpd]
 * 
 * 	Updated to handle name ports from memory_object_create.
 * 	Changed to remember the name ports associated with objects.
 * 	Changed default_pager_objects to return the name ports.
 * 	[91/12/28            rpd]
 * 
 * 	Added default_pager_objects.
 * 	[91/12/15            rpd]
 * 
 * Revision 2.2  92/01/03  19:56:21  dbg
 * 	Simplify locking.
 * 	[91/10/02            dbg]
 * 
 * 	Convert to run outside of kernel.
 * 	[91/09/04            dbg]
 * 
 * Revision 2.17  91/08/29  13:44:27  jsb
 * 	A couple quick changes for NORMA_VM. Will be fixed later.
 * 
 * Revision 2.16  91/08/28  16:59:29  jsb
 * 	Fixed the default values of default_pager_internal_count and
 * 	default_pager_external_count.
 * 	[91/08/28            rpd]
 * 
 * Revision 2.15  91/08/28  11:09:32  jsb
 * 	Added seqnos_memory_object_change_completed.
 * 	From dlb: use memory_object_data_supply for pagein when buffer is
 * 	going to be deallocated.
 * 	From me: don't use data_supply under NORMA_VM (will be fixed).
 * 	[91/08/26  14:30:07  jsb]
 * 
 * 	Changed to process requests in parallel when possible.
 * 
 * 	Don't bother keeping track of mscount.
 * 	[91/08/16            rpd]
 * 	Added default_pager_info.
 * 	[91/08/15            rpd]
 * 
 * 	Added sequence numbers to the memory object interface.
 * 	Changed to use no-senders notifications.
 * 	Changed to keep track of port rights and not use mach_port_destroy.
 * 	Added dummy supply-completed and data-return stubs.
 * 	[91/08/13            rpd]
 * 
 * Revision 2.14  91/05/18  14:28:32  rpd
 * 	Don't give privileges to threads handling external objects.
 * 	[91/04/06            rpd]
 * 	Enhanced to use multiple threads, for performance and to avoid
 * 	a deadlock caused by default_pager_object_create.
 * 	Added locking to partitions.
 * 	Added locking to pager_port_hashtable.
 * 	Changed pager_port_hash to something reasonable.
 * 	[91/04/03            rpd]
 * 
 * Revision 2.13  91/05/14  15:21:41  mrt
 * 	Correcting copyright
 * 
 * Revision 2.12  91/03/16  14:41:26  rpd
 * 	Updated for new kmem_alloc interface.
 * 	Fixed memory_object_create to zero the new pager structure.
 * 	[91/03/03            rpd]
 * 	Removed thread_swappable.
 * 	[91/01/18            rpd]
 * 
 * Revision 2.11  91/02/05  17:00:49  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:31  mrt]
 * 
 * Revision 2.10  90/09/09  14:31:01  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.9  90/08/27  21:44:51  dbg
 * 	Add definitions of NBBY, howmany.
 * 	[90/07/16            dbg]
 * 
 * Revision 2.8  90/06/02  14:45:22  rpd
 * 	Changed default_pager_object_create so the out argument
 * 	is a poly send right.
 * 	[90/05/03            rpd]
 * 	Removed references to keep_wired_memory.
 * 	[90/04/29            rpd]
 * 	Converted to new IPC.
 * 	Removed data-request queue.
 * 	[90/03/26  21:30:57  rpd]
 * 
 * Revision 2.7  90/03/14  21:09:58  rwd
 * 	Call default_pager_object_server and add
 * 	default_pager_object_create
 * 	[90/01/22            rwd]
 * 
 * Revision 2.6  90/01/11  11:41:08  dbg
 * 	Use bootstrap-task print routines.
 * 	[89/12/20            dbg]
 * 
 * 	De-lint.
 * 	[89/12/06            dbg]
 * 
 * Revision 2.5  89/12/08  19:52:03  rwd
 * 	Turn off CHECKSUM
 * 	[89/12/06            rwd]
 * 
 * Revision 2.4  89/10/23  12:01:54  dbg
 * 	Change pager_read_offset and pager_write_offset to return block
 * 	number as function result.  default_read()'s caller must now
 * 	deallocate data if not the same as the data buffer passed in.
 * 	Add register declarations and clean up loops a bit.
 * 	[89/10/19            dbg]
 * 
 * 	Oops - nothing like having your debugging code introduce bugs...
 * 	[89/10/17            dbg]
 * 
 * Revision 2.3  89/10/16  15:21:59  rwd
 * 	debugging: checksum pages in each object.
 * 	[89/10/04            dbg]
 * 
 * Revision 2.2  89/09/08  11:22:06  dbg
 * 	Wait for default_partition to be set.
 * 	[89/09/01            dbg]
 * 
 * 	Modified to call outside routines for read and write.
 * 	Removed disk structure.  Added part_create.
 * 	Reorganized code.
 * 	[89/07/11            dbg]
 * 
 */
/* CMU_ENDHIST */
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
/*
 * Default pager.  Pages to paging partition.
 *
 * MUST BE ABLE TO ALLOCATE WIRED-DOWN MEMORY!!!
 */


#include <mach.h>
#include <mach/mach_traps.h>
#include <mach/mach_host.h>
#include <mach/mach_norma.h>
#include <cthreads.h>
#include "wiring.h"

#include <mach/message.h>
#include <mach/bootstrap.h>
#include <mach/notify.h>
#include <mach/vm_region.h>
#include <mach/mig_errors.h>
#include <mach/thread_switch.h>
#include <mach/default_pager_object.h>

#include "queue.h"
#include "assert.h"
#include "file_io.h"
#include "externs.h"

#include <device/device_types.h>
#include <device/device.h>

#define pager_lock_init mutex_init
#define pager_lock mutex_lock
#define pager_unlock mutex_unlock
#define pager_lock_try mutex_try_lock
#define	ptoa(p)	((p)*vm_page_size)
#define	atop(a)	((a)/vm_page_size)
 
#define debug 0
#define STATIC static

int norma_mk;	/* Is the kernel configured with norma */

STATIC char my_name[] = "(default pager):";

/*
 * parallel vs serial switch
 */
#define	PARALLEL 1

#if	0
#define	CHECKSUM	1
#endif

#define	USE_PRECIOUS	1

/*
 * Bitmap allocation.
 */
typedef unsigned int	bm_entry_t;
#define	NB_BM		32
#define	BM_MASK		0xffffffffU

#define	howmany(a,b)	(((a) + (b) - 1)/(b))

/*
 * Value to indicate no block assigned
 */
#define	NO_BLOCK	((vm_offset_t)-1)

/*
 * 'Partition' structure for each paging area.
 * Controls allocation of blocks within paging area.
 */
struct part {
	struct mutex	p_lock;		/* for bitmap/free */
	vm_size_t	total_size;	/* total number of blocks */
	vm_size_t	free;		/* number of blocks free */
	unsigned int	id;		/* named lookup */
	bm_entry_t	*bitmap;	/* allocation map */
	boolean_t	going_away;	/* destroy attempt in progress */
	int		(*p_read)(struct file_direct *, vm_offset_t, vm_size_t,
				  vm_offset_t *, vm_size_t *);
					/* Read block from partition */
	int		(*p_write)(struct file_direct *, vm_offset_t, vm_offset_t,
				   vm_size_t, vm_size_t *);
					/* Write block to partition */
	struct file_direct *p_private;	/* Pointer to private data for
					   read/write routines. */
};
typedef	struct part	*partition_t;

struct {
	struct mutex	lock;
	int		n_partitions;
	partition_t	*partition_list;/* array, for quick mapping */
} all_partitions;			/* list of all such */

typedef unsigned int	p_index_t;

#define	P_INDEX_INVALID	((p_index_t)-1)

#define	no_partition(x)	((x) == P_INDEX_INVALID)
#define	partition_of(x)	all_partitions.partition_list[x]

#define	DEBUG_READER_CONFLICTS	0

#if	DEBUG_READER_CONFLICTS
int	default_pager_read_conflicts = 0;
#endif

union dp_map {

	struct {
		unsigned int	p_offset : 24,
				p_index : 8;
	} block;

	union dp_map		*indirect;
#ifdef CHECKSUM
	int checksum;
#endif
};
typedef union dp_map	*dp_map_t;

/* quick check for part==block==invalid */
#define	no_block(e)		((e).indirect == (dp_map_t)NO_BLOCK)
#define	invalidate_block(e)	((e).indirect = (dp_map_t)NO_BLOCK)

struct dpager {
	struct mutex	lock;		/* lock for extending block map */
					/* XXX should be read-write lock */
#if	DEBUG_READER_CONFLICTS
	int		readers;
	boolean_t	writer;
#endif
	dp_map_t	map;		/* block map */
	vm_size_t	size;		/* size of paging object, in pages */
	p_index_t	cur_partition;
#ifdef	CHECKSUM
 	dp_map_t	checksum;	/* checksum - parallel to block map */
#define	NO_CHECKSUM	((dp_map_t)-1)
#endif	/* CHECKSUM */
};
typedef struct dpager	*dpager_t;

/*
 * A paging object uses either a one- or a two-level map of offsets
 * into a paging partition.
 */

#define	PAGEMAP_ENTRIES		64
				/* number of pages in a second-level map */
#define	PAGEMAP_SIZE(npgs)	((npgs)*sizeof(union dp_map))

#define	INDIRECT_PAGEMAP_ENTRIES(npgs) \
		((((npgs)-1)/PAGEMAP_ENTRIES) + 1)
#define	INDIRECT_PAGEMAP_SIZE(npgs) \
 		(INDIRECT_PAGEMAP_ENTRIES(npgs) * sizeof(union dp_map))
#define	INDIRECT_PAGEMAP(size)	\
		(size > PAGEMAP_ENTRIES)

#define	ROUNDUP_TO_PAGEMAP(npgs) \
		(((npgs) + PAGEMAP_ENTRIES - 1) & ~(PAGEMAP_ENTRIES - 1))

/*
 * Object sizes are rounded up to the next power of 2,
 * unless they are bigger than a given maximum size.
 */
vm_size_t	max_doubled_size = 4 * 1024 * 1024;	/* 4 meg */

/*
 * Mapping between pager port and paging object.
 */
struct dstruct {
	queue_chain_t	links;		/* Link in pager-port list */
	struct mutex	lock;		/* Lock for the structure */
	struct condition
			waiting_seqno,	/* someone waiting on seqno */
			waiting_read,	/* someone waiting on readers */
			waiting_write,	/* someone waiting on writers */
			waiting_refs;	/* someone waiting on refs */
	memory_object_t	pager;		/* Pager port */
	mach_port_seqno_t seqno;	/* Pager port sequence number */
	mach_port_t	pager_request;	/* Request port */
	mach_port_urefs_t request_refs;	/* Request port user-refs */
	mach_port_t	pager_name;	/* Name port */
	mach_port_urefs_t name_refs;	/* Name port user-refs */

	unsigned int	readers;	/* Reads in progress */
	unsigned int	writers;	/* Writes in progress */

	unsigned int	errors;		/* Pageout error count */
	struct dpager	dpager;		/* Actual pager */
};
typedef struct dstruct *	default_pager_t;
#define	DEFAULT_PAGER_NULL	((default_pager_t)0)

/*
 * List of all pagers.  A specific pager is
 * found directly via its port, this list is
 * only used for monitoring purposes by the
 * default_pager_object* calls
 */
struct pager_port {
	queue_head_t	queue;
	struct mutex	lock;
	int		count;	/* saves code */
	queue_head_t	leak_queue;
} all_pagers;

STATIC void partition_init(void);
STATIC void create_paging_partition(const char *,
				    vm_size_t,
				    int (*)(struct file_direct *, vm_offset_t,
					    vm_size_t, vm_offset_t *,
					    vm_size_t *),
				    int (*)(struct file_direct *, vm_offset_t,
					    vm_offset_t, vm_size_t,
					    vm_size_t *),
				    struct file_direct *,
				    boolean_t);
#if 0
STATIC kern_return_t remove_paging_file(const char *);
STATIC kern_return_t destroy_paging_partition(const char *,
					      struct file_direct **);
STATIC boolean_t pager_realloc(dpager_t, p_index_t);
STATIC void pager_move_page(union dp_map, union dp_map *);
#endif
STATIC void default_pager_no_senders(memory_object_t,
				     mach_port_seqno_t,
				     mach_port_mscount_t);
STATIC boolean_t default_pager_notify_server(mach_msg_header_t *,
					     mach_msg_header_t *);
STATIC boolean_t default_pager_demux_object(mach_msg_header_t *,
					    mach_msg_header_t *);
STATIC boolean_t default_pager_demux_default(mach_msg_header_t *,
					     mach_msg_header_t *);
STATIC void default_pager_thread_privileges(void);
STATIC void start_default_pager_thread(boolean_t);
STATIC void default_pager(void);
STATIC void default_pager_initialize(mach_port_t);
STATIC unsigned int part_id(const char *);
STATIC void no_paging_space(boolean_t);
STATIC void overcommitted(boolean_t, vm_size_t);
STATIC p_index_t choose_partition(size_t, p_index_t);
STATIC vm_offset_t pager_alloc_page(p_index_t, boolean_t);
STATIC void pager_dealloc_page(p_index_t, vm_offset_t, boolean_t);
STATIC void pager_alloc(dpager_t, p_index_t, vm_size_t);
STATIC vm_size_t pager_allocated(dpager_t);
STATIC size_t pager_pages(dpager_t, default_pager_page_t *, size_t);
STATIC void pager_extend(dpager_t, vm_size_t);
STATIC void pager_read_offset(dpager_t, vm_offset_t, union dp_map *);
STATIC void pager_release_offset(dpager_t, vm_offset_t);
STATIC void pager_write_offset(dpager_t, vm_offset_t, union dp_map *);
STATIC int default_read(dpager_t, vm_offset_t, vm_size_t, vm_offset_t,
			vm_offset_t *, boolean_t);
STATIC int default_write(dpager_t ds, vm_offset_t, vm_size_t, vm_offset_t);
STATIC default_pager_t pager_port_alloc(vm_size_t);
STATIC void paging_space_info(vm_size_t *, vm_size_t *);
STATIC kern_return_t add_paging_file(mach_port_t, mach_port_t, const char *);
STATIC boolean_t default_pager_setup(mach_port_t, mach_port_t);

#if	PAGING_MEMORY
STATIC kern_return_t add_paging_mem(mach_port_t);
int page_read_mem_direct(struct file_direct *, vm_offset_t, vm_size_t,
				 vm_offset_t *, vm_size_t *);
int page_write_mem_direct(struct file_direct *, vm_offset_t,
				  vm_offset_t, vm_size_t, vm_size_t *);
#endif	/* PAGING_MEMORY */

/*
 * Simple mapping from (file)NAME to id
 * Saves space, filenames can be long.
 */
STATIC unsigned int
part_id(const char *name)
{
	register unsigned int len, id, xorid;

	len = strlen(name);
	id = xorid = 0;
	while (len--) {
		xorid ^= *(unsigned char *)name;
		id += *name++;
	}
	return (id << 8) | xorid;
}

STATIC void
partition_init(void)
{
	pager_lock_init(&all_partitions.lock);
	all_partitions.n_partitions = 0;
}

/*
 * Create a partition descriptor,
 * add it to the list of all such.
 * size is in BYTES.
 */
STATIC void
create_paging_partition(const char *name,
			vm_size_t size,
			int (*p_read)(struct file_direct *, vm_offset_t, vm_size_t,
				      vm_offset_t *, vm_size_t *),
			int (*p_write)(struct file_direct *, vm_offset_t, vm_offset_t,
				       vm_size_t, vm_size_t *),
			struct file_direct *p_private,
			boolean_t isa_file)
{
	register partition_t	part;
	register vm_size_t	bmsize;

	size = atop(size);
	bmsize = howmany(size, NB_BM) * sizeof(bm_entry_t);

	part = (partition_t) kalloc(sizeof(struct part));
	pager_lock_init(&part->p_lock);
	part->total_size = size;
	part->free	= size;
	part->id	= part_id(name);
	part->bitmap	= (bm_entry_t *)kalloc(bmsize);
	part->going_away= FALSE;
	part->p_read	= p_read;
	part->p_write	= p_write;
	part->p_private	= p_private;

	bzero((char *)part->bitmap, bmsize);

	pager_lock(&all_partitions.lock);
	{
		register int i;

		for (i = 0; i < all_partitions.n_partitions; i++)
			if (partition_of(i) == 0) break;

		if (i == all_partitions.n_partitions) {
			register partition_t	*new_list, *old_list;
			register int		n;
			
			n = i ? (i<<1) : 2;
			new_list = (partition_t *)
				kalloc( n * sizeof(partition_t) );
			if (new_list == 0) no_paging_space(TRUE);
			bzero(new_list, n*sizeof(partition_t));
			if (i) {
			    old_list = all_partitions.partition_list;
			    bcopy(old_list, new_list, i*sizeof(partition_t));
			    all_partitions.partition_list = new_list;
			    all_partitions.n_partitions = n;
			    kfree(old_list, i*sizeof(partition_t));
			} else {
			    all_partitions.partition_list = new_list;
			    all_partitions.n_partitions = n;
			}
		}
		partition_of(i) = part;
	}
	pager_unlock(&all_partitions.lock);

	printf("%s Added paging %s %s\n", my_name, (isa_file) ? "file" : "device",  name);
	overcommitted(TRUE, part->free);
}


/*
 * Choose the most appropriate default partition
 * for an object of SIZE bytes.
 * Return the partition locked, unless
 * the object has no CUR_PARTition.
 */
STATIC p_index_t
choose_partition(size_t size, register p_index_t cur_part)
{
	register partition_t	part;
	register boolean_t	found = FALSE;
	register int		i;

	pager_lock(&all_partitions.lock);
	for (i = 0; i < all_partitions.n_partitions; i++) {
		/* the undesireable one ? */
		if (i == cur_part)
			continue;

		/* one that was removed ? */
		if ((part = partition_of(i)) == 0)
			continue;

		/* one that is being removed ? */
		if (part->going_away)
			continue;

		/* is it big enough ? */

		pager_lock(&part->p_lock);
		if (ptoa(part->free) >= size) {
		    if (cur_part != P_INDEX_INVALID) {
			pager_unlock(&all_partitions.lock);
			return (p_index_t)i;
		    } else
			found = TRUE;
		}
		pager_unlock(&part->p_lock);

		if (found) break;
	}

	pager_unlock(&all_partitions.lock);
	return (found) ? (p_index_t)i : P_INDEX_INVALID;
}

/*
 * Allocate a page in a paging partition
 * The partition is returned unlocked.
 */

STATIC vm_offset_t
pager_alloc_page(p_index_t pindex, boolean_t lock_it)
{
	register int	bm_e;
	register int	bit;
	register int	limit;
	register bm_entry_t	*bm;
	partition_t	part;
	STATIC char	here[] = "%spager_alloc_page";

	if (no_partition(pindex))
	    return (NO_BLOCK);
	part = partition_of(pindex);

	/* unlikely, but possible deadlock against destroy_partition */
	if (!part || part->going_away)
	    return (NO_BLOCK);

	if (lock_it)
	    pager_lock(&part->p_lock);

	if (part->free == 0) {
	    /* out of paging space */

	    pager_unlock(&part->p_lock);
	    return (NO_BLOCK);
	}

	limit = howmany(part->total_size, NB_BM);
	bm = part->bitmap;
	for (bm_e = 0; bm_e < limit; bm_e++, bm++)
	    if (*bm != BM_MASK)
		break;

	if (bm_e == limit)
	    panic(here,my_name);

	/*
	 * Find and set the proper bit
	 */
	{
	    register bm_entry_t	b = *bm;

	    for (bit = 0; bit < NB_BM; bit++)
		if ((b & (1<<bit)) == 0)
		    break;
	    if (bit == NB_BM)
		panic(here,my_name);

	    *bm = b | (1<<bit);
	    part->free--;

	}

	pager_unlock(&part->p_lock);
	return (bm_e*NB_BM+bit);
}

/*
 * Deallocate a page in a paging partition
 */
STATIC void
pager_dealloc_page(p_index_t pindex,
		   register vm_offset_t page,
		   boolean_t lock_it)
{
	register partition_t	part;
	register int	bit, bm_e;

	/* be paranoid */
	if (no_partition(pindex))
	    panic("%sdealloc_page",my_name);
	part = partition_of(pindex);

	if (page >= part->total_size)
	    panic("%sdealloc_page",my_name);

	bm_e = page / NB_BM;
	bit  = page % NB_BM;

	if (lock_it)
	    pager_lock(&part->p_lock);

	part->bitmap[bm_e] &= ~(1<<bit);
	part->free++;

	if (lock_it)
	    pager_unlock(&part->p_lock);
}

/*
 * Allocation info for each paging object.
 *
 * Most operations, even pager_write_offset and pager_put_checksum,
 * just need a read lock.  Higher-level considerations prevent
 * conflicting operations on a single page.  The lock really protects
 * the underlying size and block map memory, so pager_extend needs a
 * write lock.
 *
 * An object can now span multiple paging partitions.  The allocation
 * info we keep is a pair (offset,p_index) where the index is in the
 * array of all partition ptrs, and the offset is partition-relative.
 * Size wise we are doing ok fitting the pair into a single integer:
 * the offset really is in pages so we have PAGE_SIZE  bits available
 * for the partition index.
 */

/*
 * Attach a new paging object to a paging partition
 */
STATIC void
pager_alloc(register dpager_t pager,
	    p_index_t part,
	    register vm_size_t size)	/* in BYTES */
{
	register int    i;
	register dp_map_t mapptr, emapptr;

	mutex_init(&pager->lock);

#if	DEBUG_READER_CONFLICTS
	pager->readers = 0;
	pager->writer = FALSE;
#endif
	pager->cur_partition = part;

	/*
	 * Convert byte size to number of pages, then increase to the nearest
	 * power of 2. 
	 */

	size = atop(size);
	if (size <= atop(max_doubled_size)) {
	    i = 1;
	    while (i < size)
		i <<= 1;
	    size = i;
	} else
	    size = ROUNDUP_TO_PAGEMAP(size);


	/*
	 * Allocate and initialize the block map
	 */
	{
		register vm_size_t	alloc_size;
		dp_map_t		init_value;

		if (INDIRECT_PAGEMAP(size)) {
			alloc_size = INDIRECT_PAGEMAP_SIZE(size);
			init_value = (dp_map_t)0;
		} else {
			alloc_size = PAGEMAP_SIZE(size);
			init_value = (dp_map_t)NO_BLOCK;
		}

		mapptr = (dp_map_t) kalloc(alloc_size);
 		for (emapptr = &mapptr[(alloc_size-1) / sizeof(union dp_map)];
		     emapptr >= mapptr;
		     emapptr--)
			emapptr->indirect = init_value;

	}
	pager->map = mapptr;
	pager->size = size;

#ifdef	CHECKSUM
	if (INDIRECT_PAGEMAP(size)) {
 		mapptr = (dp_map_t)
			kalloc(INDIRECT_PAGEMAP_SIZE(size));
		for (i = INDIRECT_PAGEMAP_ENTRIES(size); --i >= 0;)
 			mapptr[i].indirect = (dp_map_t)0;
	} else {
 		mapptr = (dp_map_t) kalloc(PAGEMAP_SIZE(size));
		for (i = 0; i < size; i++)
 			mapptr[i].indirect =  NO_CHECKSUM;
	}
	pager->checksum = mapptr;
#endif	/* CHECKSUM */
}

/*
 * Return size (in bytes) of space actually allocated to this pager.
 * The pager is read-locked.
 */

STATIC vm_size_t
pager_allocated(register dpager_t pager)
{
	vm_size_t       size;
	register dp_map_t map, emap;
	vm_size_t       asize;

	size = pager->size;	/* in pages */
	asize = 0;		/* allocated, in pages */
	map = pager->map;

	if (INDIRECT_PAGEMAP(size)) {
		for (emap = &map[INDIRECT_PAGEMAP_ENTRIES(size)];
		     map < emap; map++) {

			register dp_map_t	map2, emap2;

			if ((map2 = map->indirect) == 0)
				continue;

			for (emap2 = &map2[PAGEMAP_ENTRIES];
			     map2 < emap2; map2++)
				if ( ! no_block(*map2) )
					asize++;

		}
	} else {
		for (emap = &map[size]; map < emap; map++)
			if ( ! no_block(*map) )
				asize++;
	}

	return ptoa(asize);
}

/*
 * Find offsets (in the object) of pages actually allocated to this pager.
 * Returns the number of allocated pages, whether or not they all fit.
 * The pager is read-locked.
 */

STATIC size_t
pager_pages(dpager_t pager,
	    register default_pager_page_t *pages,
	    size_t numpages)
{
	vm_size_t       size;
	dp_map_t        map, emap;
	unsigned int    actual;
	vm_offset_t     offset;

	size = pager->size;	/* in pages */
	map = pager->map;
	actual = 0;
	offset = 0;

	if (INDIRECT_PAGEMAP(size)) {
		for (emap = &map[INDIRECT_PAGEMAP_ENTRIES(size)];
		     map < emap; map++) {

			register dp_map_t	map2, emap2;

			if ((map2 = map->indirect) == 0) {
				offset += vm_page_size * PAGEMAP_ENTRIES;
				continue;
			}
			for (emap2 = &map2[PAGEMAP_ENTRIES];
			     map2 < emap2; map2++)
				if ( ! no_block(*map2) ) {
					if (actual++ < numpages)
						pages++->dpp_offset = offset;
				}
			offset += vm_page_size;
		}
	} else {
		for (emap = &map[size]; map < emap; map++)
			if ( ! no_block(*map) ) {
				if (actual++ < numpages)
					pages++->dpp_offset = offset;
			}
		offset += vm_page_size;
	}
	return actual;
}

/*
 * Extend the map for a paging object.
 *
 * XXX This implementation can allocate an arbitrary large amount
 * of wired memory when extending a big block map.  Because vm-privileged
 * threads call pager_extend, this can crash the system by exhausting
 * system memory.
 */
STATIC void
pager_extend(register dpager_t pager,
	     register vm_size_t new_size)	/* in pages */
{
	register dp_map_t	new_mapptr;
	register dp_map_t	old_mapptr;

	register int		i;
	register vm_size_t	old_size;

	mutex_lock(&pager->lock);		

#if	DEBUG_READER_CONFLICTS
	pager->writer = TRUE;
#endif
	/*
	 * Double current size until we cover new size.
	 * If object is 'too big' just use new size.
	 */
	old_size = pager->size;

	if (new_size <= atop(max_doubled_size)) {
	    i = old_size;
	    while (i < new_size)
		i <<= 1;
	    new_size = i;
	} else
	    new_size = ROUNDUP_TO_PAGEMAP(new_size);

	if (INDIRECT_PAGEMAP(old_size)) {
	    /*
	     * Pager already uses two levels.  Allocate
	     * a larger indirect block.
	     */
	    new_mapptr = (dp_map_t)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    old_mapptr = pager->map;
	    for (i = 0; i < INDIRECT_PAGEMAP_ENTRIES(old_size); i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i].indirect = (dp_map_t)0;
	    kfree((char *)old_mapptr, INDIRECT_PAGEMAP_SIZE(old_size));
	    pager->map = new_mapptr;
	    pager->size = new_size;
#ifdef	CHECKSUM
 	    new_mapptr = (dp_map_t)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    old_mapptr = pager->checksum;
	    for (i = 0; i < INDIRECT_PAGEMAP_ENTRIES(old_size); i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
 		new_mapptr[i].indirect = (dp_map_t)0;
	    kfree((char *)old_mapptr, INDIRECT_PAGEMAP_SIZE(old_size));
	    pager->checksum = new_mapptr;
#endif	/* CHECKSUM */
#if	DEBUG_READER_CONFLICTS
	    pager->writer = FALSE;
#endif

	    mutex_unlock(&pager->lock);
	    return;
	}

	if (INDIRECT_PAGEMAP(new_size)) {
	    /*
	     * Changing from direct map to indirect map.
	     * Allocate both indirect and direct map blocks,
	     * since second-level (direct) block must be
	     * full size (PAGEMAP_SIZE(PAGEMAP_ENTRIES)).
	     */

	    /*
	     * Allocate new second-level map first.
	     */
	    new_mapptr = (dp_map_t) kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
	    old_mapptr = pager->map;
	    for (i = 0; i < old_size; i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < PAGEMAP_ENTRIES; i++)
		invalidate_block(new_mapptr[i]);
	    kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	    old_mapptr = new_mapptr;

	    /*
	     * Now allocate indirect map.
	     */
	    new_mapptr = (dp_map_t)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    new_mapptr[0].indirect = old_mapptr;
	    for (i = 1; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i].indirect = 0;
	    pager->map = new_mapptr;
	    pager->size = new_size;
#ifdef	CHECKSUM
	    /*
	     * Allocate new second-level map first.
	     */
 	    new_mapptr = (dp_map_t)kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
	    old_mapptr = pager->checksum;
	    for (i = 0; i < old_size; i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < PAGEMAP_ENTRIES; i++)
 		new_mapptr[i].indirect =  NO_CHECKSUM;
	    kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	    old_mapptr = new_mapptr;

	    /*
	     * Now allocate indirect map.
	     */
 	    new_mapptr = (dp_map_t)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
 	    new_mapptr[0].indirect = old_mapptr;
	    for (i = 1; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
 		new_mapptr[i].indirect = (dp_map_t)0;
	    pager->checksum = new_mapptr;
#endif	/* CHECKSUM */
#if	DEBUG_READER_CONFLICTS
	    pager->writer = FALSE;
#endif

	    mutex_unlock(&pager->lock);
	    return;
	}

	/*
	 * Enlarging a direct block.
	 */
	new_mapptr = (dp_map_t)	kalloc(PAGEMAP_SIZE(new_size));
	old_mapptr = pager->map;
	for (i = 0; i < old_size; i++)
	    new_mapptr[i] = old_mapptr[i];
	for (; i < new_size; i++)
	    invalidate_block(new_mapptr[i]);
	kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	pager->map = new_mapptr;
	pager->size = new_size;
#ifdef	CHECKSUM
 	new_mapptr = (dp_map_t)
		kalloc(PAGEMAP_SIZE(new_size));
	old_mapptr = pager->checksum;
	for (i = 0; i < old_size; i++)
	    new_mapptr[i] = old_mapptr[i];
	for (; i < new_size; i++)
 	    new_mapptr[i].indirect = NO_CHECKSUM;
	kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	pager->checksum = new_mapptr;
#endif	/* CHECKSUM */
#if	DEBUG_READER_CONFLICTS
	pager->writer = FALSE;
#endif
	mutex_unlock(&pager->lock);
}

/*
 * Given an offset within a paging object, find the
 * corresponding block within the paging partition.
 * Return NO_BLOCK if none allocated.
 */
STATIC void
pager_read_offset(register dpager_t pager,
		  vm_offset_t offset,
		  union dp_map *blockp)
{
	register vm_offset_t	f_page;
	union dp_map		pager_offset;

	f_page = atop(offset);

#if	DEBUG_READER_CONFLICTS
	if (pager->readers > 0)
	    default_pager_read_conflicts++;	/* would have proceeded with
						   read/write lock */
#endif

	mutex_lock(&pager->lock);	

#if	DEBUG_READER_CONFLICTS
	pager->readers++;
#endif

	while (f_page >= pager->size) {
	    /*
	     * Paging object must be extended.
	     * Remember that offset is 0-based, but size is 1-based.
	     */
#if	DEBUG_READER_CONFLICTS
	    pager->readers--;
#endif
	    mutex_unlock(&pager->lock);
	    pager_extend(pager, f_page + 1);
#if	DEBUG_READER_CONFLICTS
	    if (pager->readers > 0)
		default_pager_read_conflicts++;	/* would have proceeded with
						   read/write lock */
#endif
	    mutex_lock(&pager->lock);	
#if	DEBUG_READER_CONFLICTS
	    pager->readers++;
#endif
	}

	if (INDIRECT_PAGEMAP(pager->size)) {
	    register dp_map_t	mapptr;

	    mapptr = pager->map[f_page/PAGEMAP_ENTRIES].indirect;
	    if (mapptr == 0)
		invalidate_block(pager_offset);
	    else
		pager_offset = mapptr[f_page%PAGEMAP_ENTRIES];
	}
	else {
	    pager_offset = pager->map[f_page];
	}

#if	DEBUG_READER_CONFLICTS
	pager->readers--;
#endif

	mutex_unlock(&pager->lock);
	*blockp = pager_offset;
}

#if	USE_PRECIOUS
/*
 * Release a single disk block.
 */
STATIC void
pager_release_offset(register dpager_t pager, vm_offset_t offset)
{
	register union dp_map	entry;

	offset = atop(offset);

	mutex_lock(&pager->lock);

	if (INDIRECT_PAGEMAP(pager->size)) {
		register dp_map_t	mapptr;

		mapptr = pager->map[offset / PAGEMAP_ENTRIES].indirect;
		entry = mapptr[offset % PAGEMAP_ENTRIES];
		invalidate_block(mapptr[offset % PAGEMAP_ENTRIES]);
	} else {
		entry = pager->map[offset];
		invalidate_block(pager->map[offset]);
	}

	mutex_unlock(&pager->lock);
	pager_dealloc_page(entry.block.p_index, entry.block.p_offset, TRUE);
}
#endif	/*USE_PRECIOUS*/


#if 0
/*
 * Move a page from one partition to another
 * New partition is locked, old partition is
 * locked unless LOCK_OLD sez otherwise.
 */
STATIC void
pager_move_page(union dp_map block, union dp_map *new_blockp)
{
	partition_t	old_part, new_part;
	p_index_t	old_pindex, new_pindex;
	union dp_map	ret;
	vm_size_t	size;
	vm_offset_t	raddr, offset, new_offset;
	kern_return_t	rc;
	STATIC char	here[] = "%spager_move_page";

	old_pindex = block.block.p_index;
	invalidate_block(ret);

	/* See if we have room to put it anywhere else */
	new_pindex = choose_partition( ptoa(1), old_pindex);
	if (no_partition(new_pindex)) {
		*new_blockp = ret;
		return;
	}

	/* this unlocks the new partition */
	new_offset = pager_alloc_page(new_pindex, FALSE);
	if (new_offset == NO_BLOCK)
		panic(here,my_name);

	/*
	 * Got the resources, now move the data
	 */
	old_part = partition_of(old_pindex);
	offset = ptoa(block.block.p_offset);
	rc = (*old_part->p_read) (old_part->p_private,
				  offset,
				  vm_page_size,
				  &raddr,
				  &size);
	if (rc != 0)
		panic(here,my_name);

	/* release old */
	pager_dealloc_page(old_pindex, block.block.p_offset, FALSE);

	new_part = partition_of(new_pindex);
	offset = ptoa(new_offset);
	rc = (*new_part->p_write) (new_part->p_private,
				   offset,
				   raddr,
				   size,
				   &size);
	if (rc != 0)
		panic(here,my_name);

	(void) vm_deallocate( mach_task_self(), raddr, size);

	ret.block.p_offset = new_offset;
	ret.block.p_index  = new_pindex;

	*new_blockp = ret;
}
#endif

#ifdef	CHECKSUM
/*
 * Return the checksum for a block.
 */
STATIC int
pager_get_checksum(register dpager_t pager, vm_offset_t offset)
{
	register vm_offset_t	f_page;
	int checksum;

	f_page = atop(offset);

	mutex_lock(&pager->lock);		

	if (f_page >= pager->size)
	    panic("%spager_get_checksum",my_name);

	if (INDIRECT_PAGEMAP(pager->size)) {
 	    register dp_map_t mapptr;

 	    mapptr = pager->checksum[f_page/PAGEMAP_ENTRIES].indirect;
	    if (mapptr == 0)
 		checksum = (int)NO_CHECKSUM;
	    else
 		checksum = mapptr[f_page%PAGEMAP_ENTRIES].checksum;
	}
	else {
 	    checksum = pager->checksum[f_page].checksum;
	}

	mutex_unlock(&pager->lock);
	return (checksum);
}

/*
 * Remember the checksum for a block.
 */
STATIC int
pager_put_checksum(register dpager_t pager, vm_offset_t offset, int checksum)
{
	register vm_offset_t	f_page;
	STATIC char		here[] = "%spager_put_checksum";

	f_page = atop(offset);

	mutex_lock(&pager->lock);		

	if (f_page >= pager->size)
	    panic(here,my_name);

	if (INDIRECT_PAGEMAP(pager->size)) {
 	    register dp_map_t mapptr;

 	    mapptr = pager->checksum[f_page/PAGEMAP_ENTRIES].indirect;
	    if (mapptr == 0)
		panic(here,my_name);

 	    mapptr[f_page%PAGEMAP_ENTRIES].checksum = checksum;
	}
	else {
 	    pager->checksum[f_page].checksum = checksum;
	}
	mutex_unlock(&pager->lock);
}

/*
 * Compute a checksum - XOR each 32-bit word.
 */
STATIC int
compute_checksum(vm_offset_t addr, vm_size_t size)
{
        register int	checksum = (int)NO_CHECKSUM;
	register int	*ptr;
	register int	count;

	ptr = (int *)addr;
	count = size / sizeof(int);

	while (--count >= 0)
	    checksum ^= *ptr++;

	return (checksum);
}
#endif	/* CHECKSUM */

/*
 * Given an offset within a paging object, find the
 * corresponding block within the paging partition.
 * Allocate a new block if necessary.
 *
 * WARNING: paging objects apparently may be extended
 * without notice!
 */
STATIC void
pager_write_offset(register dpager_t pager,
		   vm_offset_t offset,
		   union dp_map *blockp)
{
	register vm_offset_t	f_page;
	register dp_map_t	mapptr;
	register union dp_map	block;

	invalidate_block(block);

	f_page = atop(offset);

#if	DEBUG_READER_CONFLICTS
	if (pager->readers > 0)
	    default_pager_read_conflicts++;	/* would have proceeded with
						   read/write lock */
#endif

	mutex_lock(&pager->lock);		

#if	DEBUG_READER_CONFLICTS
	pager->readers++;
#endif

	/* Catch the case where we had no initial fit partition
	   for this object, but one was added later on */
	if (no_partition(pager->cur_partition)) {
		p_index_t	new_part;
		vm_size_t	size;

		size = (f_page > pager->size) ? f_page : pager->size;
		new_part = choose_partition(ptoa(size), P_INDEX_INVALID);
		if (no_partition(new_part))
			new_part = choose_partition(ptoa(1), P_INDEX_INVALID);
		if (no_partition(new_part))
			/* give up right now to avoid confusion */
			goto out;
		else
			pager->cur_partition = new_part;
	}

	while (f_page >= pager->size) {
	    /*
	     * Paging object must be extended.
	     * Remember that offset is 0-based, but size is 1-based.
	     */
#if	DEBUG_READER_CONFLICTS
	    pager->readers--;
#endif
	    mutex_unlock(&pager->lock);
	    pager_extend(pager, f_page + 1);
#if	DEBUG_READER_CONFLICTS
	    if (pager->readers > 0)
		default_pager_read_conflicts++;	/* would have proceeded with
						   read/write lock */
#endif
	    mutex_lock(&pager->lock);	
#if	DEBUG_READER_CONFLICTS
	    pager->readers++;
#endif
	}

	if (INDIRECT_PAGEMAP(pager->size)) {

	    mapptr = pager->map[f_page/PAGEMAP_ENTRIES].indirect;
	    if (mapptr == 0) {
		/*
		 * Allocate the indirect block
		 */
		register int i;

		mapptr = (dp_map_t) kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		if (mapptr == 0) {
		    /* out of space! */
		    no_paging_space(TRUE);
		    goto out;
		}
		pager->map[f_page/PAGEMAP_ENTRIES].indirect = mapptr;
		for (i = 0; i < PAGEMAP_ENTRIES; i++)
		    invalidate_block(mapptr[i]);
#ifdef	CHECKSUM
		{
 		    register dp_map_t cksumptr;
		    register int j;

 		    cksumptr = (dp_map_t)
				kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		    if (cksumptr == 0) {
			/* out of space! */
			no_paging_space(TRUE);
			goto out;
		    }
 		    pager->checksum[f_page/PAGEMAP_ENTRIES].indirect
 			= cksumptr;
		    for (j = 0; j < PAGEMAP_ENTRIES; j++)
 			cksumptr[j].checksum = (int)NO_CHECKSUM;
		}
#endif	/* CHECKSUM */
	    }
	    f_page %= PAGEMAP_ENTRIES;
	}
	else {
	    mapptr = pager->map;
	}

	block = mapptr[f_page];
	if (no_block(block)) {
	    vm_offset_t	off;

	    /* get room now */
	    off = pager_alloc_page(pager->cur_partition, TRUE);
	    if (off == NO_BLOCK) {
		/*
		 * Before giving up, try all other partitions.
		 */
		p_index_t	new_part;

		/* returns it locked (if any one is non-full) */
		new_part = choose_partition( ptoa(1), pager->cur_partition);
		if ( ! no_partition(new_part) ) {

#if debug
printf("%s partition %x filled,", my_name, pager->cur_partition);
printf("extending object %x (size %x) to %x.\n",
	pager, pager->size, new_part);
#endif

		    /* this one tastes better */
		    pager->cur_partition = new_part;

		    /* this unlocks the partition too */
		    off = pager_alloc_page(pager->cur_partition, FALSE);

		}

		if (off == NO_BLOCK) {
		    /*
		     * Oh well.
		     */
		    no_paging_space(TRUE);
		    goto out;
		}
	    }
	    block.block.p_offset = off;
	    block.block.p_index  = pager->cur_partition;
	    mapptr[f_page] = block;
	}

out:

#if	DEBUG_READER_CONFLICTS
	pager->readers--;
#endif
	mutex_unlock(&pager->lock);
	*blockp = block;
}

/*
 * Deallocate all of the blocks belonging to a paging object.
 * No locking needed because no other operations can be in progress.
 */
STATIC void
pager_dealloc(register dpager_t pager)
{
	register int i, j;
	register dp_map_t	mapptr;
	register union dp_map	block;

	if (INDIRECT_PAGEMAP(pager->size)) {
	    for (i = INDIRECT_PAGEMAP_ENTRIES(pager->size); --i >= 0; ) {
		mapptr = pager->map[i].indirect;

		if (mapptr != 0) {
		    for (j = 0; j < PAGEMAP_ENTRIES; j++) {
			block = mapptr[j];
			if ( ! no_block(block) )
			    pager_dealloc_page(block.block.p_index,
			    			block.block.p_offset, TRUE);
		    }
		    kfree((char *)mapptr, PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		}
	    }
	    kfree((char *)pager->map, INDIRECT_PAGEMAP_SIZE(pager->size));
#ifdef	CHECKSUM
	    for (i = INDIRECT_PAGEMAP_ENTRIES(pager->size); --i >= 0; ) {
 		mapptr = pager->checksum[i].indirect;
		if (mapptr) {
		    kfree((char *)mapptr, PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		}
	    }
	    kfree((char *)pager->checksum,
		  INDIRECT_PAGEMAP_SIZE(pager->size));
#endif	/* CHECKSUM */
	}
	else {
	    mapptr = pager->map;
	    for (i = 0; i < pager->size; i++ ) {
		block = mapptr[i];
		if ( ! no_block(block) )
		    pager_dealloc_page(block.block.p_index,
		    			block.block.p_offset, TRUE);
	    }
	    kfree((char *)pager->map, PAGEMAP_SIZE(pager->size));
#ifdef	CHECKSUM
	    kfree((char *)pager->checksum, PAGEMAP_SIZE(pager->size));
#endif	/* CHECKSUM */
	}
}

#if 0
/*
 * Move all the pages of a PAGER that live in a
 * partition PINDEX somewhere else.
 * Pager should be write-locked, partition too.
 * Returns FALSE if it could not do it, but
 * some pages might have been moved nonetheless.
 */
STATIC boolean_t
pager_realloc(register dpager_t pager, p_index_t pindex)
{
	register dp_map_t	map, emap;
	vm_size_t		size;
	union dp_map		block;

	size = pager->size;	/* in pages */
	map = pager->map;

	if (INDIRECT_PAGEMAP(size)) {
		for (emap = &map[INDIRECT_PAGEMAP_ENTRIES(size)];
		     map < emap; map++) {

			register dp_map_t	map2, emap2;

			if ((map2 = map->indirect) == 0)
				continue;

			for (emap2 = &map2[PAGEMAP_ENTRIES];
			     map2 < emap2; map2++)
				if ( map2->block.p_index == pindex) {

					pager_move_page(*map2, &block);
					if (!no_block(block))
						*map2 = block;
					else
						return FALSE;
				}

		}
		goto ok;
	}

	/* A small one */
	for (emap = &map[size]; map < emap; map++)
		if (map->block.p_index == pindex) {
			pager_move_page(*map, &block);
			if (!no_block(block))
				*map = block;
			else
				return FALSE;
		}
ok:
	pager->cur_partition = choose_partition(0, P_INDEX_INVALID);
	return TRUE;
}
#endif

/*
 * Read/write routines.
 */
#define	PAGER_SUCCESS	0
#define	PAGER_ABSENT	1
#define	PAGER_ERROR	2

/*
 * Read data from a default pager.  Addr is the address of a buffer
 * to fill.  Out_addr returns the buffer that contains the data;
 * if it is different from <addr>, it must be deallocated after use.
 */
STATIC int
default_read(register dpager_t ds,
	     vm_offset_t addr,		/* pointer to block to fill */
	     register vm_size_t size,
	     register vm_offset_t offset,
	     vm_offset_t *out_addr,	/* returns pointer to data */
	     boolean_t deallocate)
{
	union dp_map	block;
	vm_offset_t	raddr;
	vm_size_t	rsize;
	register int	rc;
	boolean_t	first_time;
	register partition_t	part;
#ifdef	CHECKSUM
	vm_size_t	original_size = size;
#endif	/* CHECKSUM */
	vm_offset_t	original_offset = offset;

	/*
	 * Find the block in the paging partition
	 */
	pager_read_offset(ds, offset, &block);
	if ( no_block(block) )
	    return (PAGER_ABSENT);

	/*
	 * Read it, trying for the entire page.
	 */
	offset = ptoa(block.block.p_offset);
	part   = partition_of(block.block.p_index);
	first_time = TRUE;
	*out_addr = addr;

	do {
	    rc = (*part->p_read)(
		part->p_private,
		offset,
		size,
		&raddr,
		&rsize);
	    if (rc != 0)
		return (PAGER_ERROR);

	    /*
	     * If we got the entire page on the first read, return it.
	     */
	    if (first_time && rsize == size) {
		*out_addr = raddr;
		break;
	    }
	    /*
	     * Otherwise, copy the data into the
	     * buffer we were passed, and try for
	     * the next piece.
	     */
	    first_time = FALSE;
	    bcopy((char *)raddr, (char *)addr, rsize);
	    addr += rsize;
	    offset += rsize;
	    size -= rsize;
	} while (size != 0);

#if	USE_PRECIOUS
	if (deallocate)
		pager_release_offset(ds, original_offset);
#endif	/*USE_PRECIOUS*/

#ifdef	CHECKSUM
	{
	    int	write_checksum,
		read_checksum;

	    write_checksum = pager_get_checksum(ds, original_offset);
	    read_checksum = compute_checksum(*out_addr, original_size);
	    if (write_checksum != read_checksum) {
		panic(
  "PAGER CHECKSUM ERROR: offset 0x%x, written 0x%x, read 0x%x",
		    original_offset, write_checksum, read_checksum);
	    }
	}
#endif	/* CHECKSUM */
	return (PAGER_SUCCESS);
}

STATIC int
default_write(register dpager_t	ds,
	      register vm_offset_t addr,
	      register vm_size_t size,
	      register vm_offset_t offset)
{
	union dp_map	block;
	partition_t		part;
	vm_size_t		wsize;
	register int		rc;

	/*
	 * Find block in paging partition
	 */
	pager_write_offset(ds, offset, &block);
	if ( no_block(block) )
	    return (PAGER_ERROR);

#ifdef	CHECKSUM
	/*
	 * Save checksum
	 */
	{
	    int	checksum;

	    checksum = compute_checksum(addr, size);
	    pager_put_checksum(ds, offset, checksum);
	}
#endif	/* CHECKSUM */
	offset = ptoa(block.block.p_offset);
	part   = partition_of(block.block.p_index);

	/*
	 * There are various assumptions made here,we
	 * will not get into the next disk 'block' by
	 * accident. It might well be non-contiguous.
	 */
	do {
	    rc = (*part->p_write)(
		part->p_private,
		offset,
		addr,
		size,
		&wsize);
	    if (rc != 0) {
		printf("*** PAGER ERROR: default_write: ");
		printf("ds=0x%x addr=0x%x size=0x%x offset=0x%x resid=0x%x\n",
			(unsigned)ds, addr, size, offset, wsize);
		return (PAGER_ERROR);
	    }
	    addr += wsize;
	    offset += wsize;
	    size -= wsize;
	} while (size != 0);
	return (PAGER_SUCCESS);
}

STATIC boolean_t
default_has_page(dpager_t ds, vm_offset_t offset)
{
	union dp_map block;

	pager_read_offset(ds, offset, &block);
	return ( ! no_block(block) );
}

#define pager_port_list_init()					\
{								\
	mutex_init(&all_pagers.lock);				\
	queue_init(&all_pagers.queue);				\
	queue_init(&all_pagers.leak_queue);			\
	all_pagers.count = 0;					\
}

STATIC void
pager_port_list_insert(mach_port_t port, default_pager_t ds)
{
        pager_lock(&all_pagers.lock);
	queue_enter(&all_pagers.queue, ds, default_pager_t, links);
	all_pagers.count++;
	pager_unlock(&all_pagers.lock);
}

/*
 * NB:  the port-name conversion defs following are intertwined with
 * with the port name "types" defined by kernel (see ipc/port.h).  We
 * have to ensure that any names we request have their two low-order
 * bits set, so that they "look like" kernel-generated names.
 *
 * These defs assume (and in fact check) that all desired names (i.e.,
 * pointers) are 0 mod 4.
 */
/* given a data structure return a good port-name to associate it to */
#define	pnameof(_x_)	_pnameof(_x_)
/* reverse, assumes no-odd-pointers */
#define	dnameof(_x_)	(((vm_offset_t)(_x_))&~3)

/* The magic typecast */
#define pager_port_lookup(_port_)					\
	((! MACH_PORT_VALID(_port_) ||					\
	 ((default_pager_t)dnameof(_port_))->pager != (_port_)) ?	\
		DEFAULT_PAGER_NULL : (default_pager_t)dnameof(_port_))

STATIC vm_offset_t
_pnameof(void *x)
{
	if ((vm_offset_t)x & 3)
		panic("pnameof arg not 0 mod 4");
	return ((vm_offset_t)x + 3);
}

STATIC void
pager_port_list_delete(default_pager_t ds)
{
        pager_lock(&all_pagers.lock);
	queue_remove(&all_pagers.queue, ds, default_pager_t, links);
	all_pagers.count--;
	pager_unlock(&all_pagers.lock);
}

#if 0
/*
 * Destroy a paging partition.
 * XXX this is not re-entrant XXX
 */
STATIC kern_return_t
destroy_paging_partition(const char *name, struct file_direct **pp_private)
{
	register unsigned int	id = part_id(name);
	register partition_t	part = (partition_t) 0;
	boolean_t		all_ok = TRUE;
	default_pager_t		entry;
	p_index_t		pindex;

	/*
	 * Find and take partition out of list
	 * This prevents choose_partition from
	 * getting in the way.
	 */
	pager_lock(&all_partitions.lock);
	for (pindex = 0; pindex < all_partitions.n_partitions; pindex++) {
		part = partition_of(pindex);
		if (part && (part->id == id)) break;
	}
	if (pindex == all_partitions.n_partitions) {
	        pager_unlock(&all_partitions.lock);
		return KERN_INVALID_ARGUMENT;
	}
	part->going_away = TRUE;
	pager_unlock(&all_partitions.lock);

	/*
	 * This might take a while..
	 */
all_over_again:
#if debug
printf("Partition x%x (id x%x) for %s, all_ok %d\n", part, id, name, all_ok);
#endif
	all_ok = TRUE;

	pager_lock(&part->p_lock);

	pager_lock(&all_pagers.lock);
	queue_iterate(&all_pagers.queue, entry, default_pager_t, links) {

		pager_lock(&entry->lock);

		if (!pager_lock_try(&entry->dpager.lock)) {

			pager_unlock(&entry->lock);
			pager_unlock(&all_pagers.lock);
			pager_unlock(&part->p_lock);

			/* yield the processor */
			(void) thread_switch(MACH_PORT_NULL,
					     SWITCH_OPTION_NONE, 0);
			
			goto all_over_again;

		}

		/*
		 * See if we can relocate all the pages of this object
		 * currently on this partition on some other partition
		 */
		all_ok = pager_realloc(&entry->dpager, pindex);

		pager_unlock(&entry->dpager.lock);
		pager_unlock(&entry->lock);

		if (!all_ok)  break;

	}

	pager_unlock(&all_pagers.lock);

	if (all_ok) {
		/* No need to unlock partition, there are no refs left */

		partition_of(pindex) = 0;
		*pp_private = part->p_private;
		kfree(part->bitmap, howmany(part->total_size, NB_BM) * sizeof(bm_entry_t));
		kfree(part, sizeof(struct part));
		printf("%s Removed paging partition %s\n", my_name, name);
		return KERN_SUCCESS;
	}

	/*
	 * Put partition back in. 
	 */
	part->going_away = FALSE;

	return KERN_FAILURE;
}
#endif

/*
 *	We use the sequence numbers on requests to regulate
 *	our parallelism.  In general, we allow multiple reads and writes
 *	to proceed in parallel, with the exception that reads must
 *	wait for previous writes to finish.  (Because the kernel might
 *	generate a data-request for a page on the heels of a data-write
 *	for the same page, and we must avoid returning stale data.)
 *	terminate requests wait for proceeding reads and writes to finish.
 */

unsigned int default_pager_total = 0;		/* debugging */
unsigned int default_pager_wait_seqno = 0;	/* debugging */
unsigned int default_pager_wait_read = 0;	/* debugging */
unsigned int default_pager_wait_write = 0;	/* debugging */
unsigned int default_pager_wait_refs = 0;	/* debugging */

#if	PARALLEL
/*
 * Waits for correct sequence number.  Leaves pager locked.
 */
STATIC void
pager_port_lock(default_pager_t ds, mach_port_seqno_t seqno)
{
    default_pager_total++;
    pager_lock(&ds->lock);

    while (ds->seqno != seqno)
    {
	default_pager_wait_seqno++;
	condition_wait(&ds->waiting_seqno, &ds->lock);
    }
}

/*
 * Increments sequence number and unlocks pager.
 */
STATIC void
pager_port_unlock(default_pager_t ds)
{
	ds->seqno++;
	pager_unlock(&ds->lock);
	condition_broadcast(&ds->waiting_seqno);
}

/*
 * Start a read - one more reader.  Pager must be locked.
 */
STATIC void
pager_port_start_read(default_pager_t ds)
{
    ds->readers++;
}

/*
 * Wait for readers.  Unlocks and relocks pager if wait needed.
 */
STATIC void
pager_port_wait_for_readers(default_pager_t ds)
{
    while (ds->readers != 0) {
	default_pager_wait_read++;
	condition_wait(&ds->waiting_read, &ds->lock);
    }
}

/*
 * Finish a read.  Pager is unlocked and returns unlocked.
 */
STATIC void
pager_port_finish_read(default_pager_t ds)
{
    pager_lock(&ds->lock);
    if (--ds->readers == 0) {
	pager_unlock(&ds->lock);
	condition_broadcast(&ds->waiting_read);
    }
    else
	pager_unlock(&ds->lock);
}

/*
 * Start a write - one more writer.  Pager must be locked.
 */
STATIC void
pager_port_start_write(default_pager_t ds)
{
    ds->writers++;
}

/*
 * Wait for writers.  Unlocks and relocks pager if wait needed.
 */
STATIC void
pager_port_wait_for_writers(default_pager_t ds)
{
    while (ds->writers != 0) {
	default_pager_wait_write++;
	condition_wait(&ds->waiting_write, &ds->lock);
    }
}

/*
 * Finish a write.  Pager is unlocked and returns unlocked.
 */
STATIC void
pager_port_finish_write(default_pager_t ds)
{
    pager_lock(&ds->lock);
    if (--ds->writers == 0)
    {
	pager_unlock(&ds->lock);
	condition_broadcast(&ds->waiting_write);
    }
    else 
	pager_unlock(&ds->lock);
}

/*
 * Wait for concurrent default_pager_objects.
 * Unlocks and relocks pager if wait needed.
 */
STATIC void
pager_port_wait_for_refs(default_pager_t ds)
{
    while (ds->name_refs == 0) {
	default_pager_wait_refs++;
	condition_wait(&ds->waiting_refs, &ds->lock);
    }
}

/*
 * Finished creating name refs - wake up waiters.
 */
STATIC void
pager_port_finish_refs(default_pager_t ds)
{
    condition_broadcast(&ds->waiting_refs);
}

#else	/* PARALLEL */

#define	pager_port_lock(ds,seqno)
#define	pager_port_unlock(ds)
#define	pager_port_start_read(ds)
#define	pager_port_wait_for_readers(ds)
#define	pager_port_finish_read(ds)
#define	pager_port_start_write(ds)
#define	pager_port_wait_for_writers(ds)
#define	pager_port_finish_write(ds)
#define pager_port_wait_for_refs(ds)
#define pager_port_finish_refs(ds)

#endif	/* PARALLEL */

/*
 *	Default pager.
 */


task_port_t default_pager_self;		/* Our task port. */

mach_port_t default_pager_default_set;	/* Port set for "default" thread. */

mach_port_t default_pager_default_port;	/* Port for memory_object_create. */
thread_port_t default_pager_default_thread;	/* Thread for default_port. */

mach_port_t default_pager_internal_set;	/* Port set for internal objects. */
mach_port_t default_pager_external_set;	/* Port set for external objects. */


typedef struct default_pager_thread {
	cthread_t	dpt_thread;	/* Server thread. */
	vm_offset_t	dpt_buffer;	/* Read buffer. */
	boolean_t	dpt_internal;	/* Do we handle internal objects? */
} default_pager_thread_t;

#if	PARALLEL
	/* determine number of threads at run time */
#define DEFAULT_PAGER_INTERNAL_COUNT	(0)

#else	/* PARALLEL */
#define	DEFAULT_PAGER_INTERNAL_COUNT	(1)
#endif	/* PARALLEL */

/* Memory created by default_pager_object_create should mostly be resident. */
#define DEFAULT_PAGER_EXTERNAL_COUNT	(1)

unsigned int default_pager_internal_count = DEFAULT_PAGER_INTERNAL_COUNT;
					/* Number of "internal" threads. */
unsigned int default_pager_external_count = DEFAULT_PAGER_EXTERNAL_COUNT;
					/* Number of "external" threads. */

STATIC default_pager_t
pager_port_alloc(vm_size_t size)
{
	default_pager_t ds;
	p_index_t	part;

	ds = (default_pager_t) kalloc(sizeof *ds);
	if (ds == DEFAULT_PAGER_NULL)
	    panic("%spager_port_alloc",my_name);
	bzero((char *) ds, sizeof *ds);

	pager_lock_init(&ds->lock);

	/*
	 * Get a suitable partition.  If none big enough
	 * just pick one and overcommit.  If no partitions
	 * at all.. well just fake one so that we will
	 * kill specific objects on pageouts rather than
	 * panicing the system now.
	 */
	part = choose_partition(size, P_INDEX_INVALID);
	if (no_partition(part)) {
		overcommitted(FALSE, atop(size));
		part = choose_partition(0,P_INDEX_INVALID);
#if debug
		if (no_partition(part))
			printf("%s No paging space at all !!\n", my_name);
#endif
	}
	pager_alloc(&ds->dpager, part, size);

	return ds;
}

STATIC mach_port_urefs_t default_pager_max_urefs = 10000;

/*
 * Check user reference count on pager_request port.
 * Pager must be locked.
 * Unlocks and re-locks pager if needs to call kernel.
 */
STATIC void
pager_port_check_request(default_pager_t ds, mach_port_t pager_request)
{
	mach_port_delta_t delta;
	kern_return_t kr;

	assert(ds->pager_request == pager_request);

	if (++ds->request_refs > default_pager_max_urefs) {
		delta = 1 - ds->request_refs;
		ds->request_refs = 1;

		pager_unlock(&ds->lock);

		/*
		 *	Deallocate excess user references.
		 */

		kr = mach_port_mod_refs(default_pager_self, pager_request,
					MACH_PORT_RIGHT_SEND, delta);
		if (kr != KERN_SUCCESS)
			panic("%spager_port_check_request",my_name);

		pager_lock(&ds->lock);
	}
}

STATIC void
default_pager_add(default_pager_t ds, boolean_t internal)
{
	mach_port_t		pager = ds->pager;
	mach_port_t		pset;
	mach_port_mscount_t 	sync;
	mach_port_t		previous;
	kern_return_t		kr;
	STATIC char		here[] = "%sdefault_pager_add";

	/*
	 *	The port currently has a make-send count of zero,
	 *	because either we just created the port or we just
	 *	received the port in a memory_object_create request.
	 */

	if (internal) {
		/* possibly generate an immediate no-senders notification */
		sync = 0;
		pset = default_pager_internal_set;
	} else {
		/* delay notification till send right is created */
		sync = 1;
		pset = default_pager_external_set;
	}

	kr = mach_port_request_notification(default_pager_self, pager,
				MACH_NOTIFY_NO_SENDERS, sync,
				pager, MACH_MSG_TYPE_MAKE_SEND_ONCE,
				&previous);
	if ((kr != KERN_SUCCESS) || (previous != MACH_PORT_NULL))
		panic(here,my_name);

	kr = mach_port_move_member(default_pager_self, pager, pset);
	if (kr != KERN_SUCCESS)
		panic(here,my_name);
}

/*
 *	Routine:	memory_object_create
 *	Purpose:
 *		Handle requests for memory objects from the
 *		kernel.
 *	Notes:
 *		Because we only give out the default memory
 *		manager port to the kernel, we don't have to
 *		be so paranoid about the contents.
 */
kern_return_t
seqnos_memory_object_create(mach_port_t old_pager,
			    mach_port_seqno_t seqno,
			    mach_port_t new_pager,
			    vm_size_t new_size,
			    mach_port_t new_pager_request,
			    mach_port_t new_pager_name,
			    vm_size_t new_page_size)
{
	register default_pager_t	ds;
	kern_return_t			kr;

	assert(old_pager == default_pager_default_port);
	assert(MACH_PORT_VALID(new_pager_request));
	assert(MACH_PORT_VALID(new_pager_name));
	assert(new_page_size == vm_page_size);

	ds = pager_port_alloc(new_size);
rename_it:
	kr = mach_port_rename(	default_pager_self,
				new_pager, (mach_port_t)pnameof((void *)ds));
	if (kr != KERN_SUCCESS) {
		default_pager_t	ds1;

		if (kr != KERN_NAME_EXISTS)
			panic("%s m_o_create", my_name);
		ds1 = (default_pager_t) kalloc(sizeof *ds1);
		*ds1 = *ds;

		pager_lock(&all_pagers.lock);
		queue_enter(&all_pagers.leak_queue, ds, default_pager_t, links);
		pager_unlock(&all_pagers.lock);

		ds = ds1;
		goto rename_it;
	}

	new_pager = (mach_port_t) pnameof((void *)ds);

	/*
	 *	Set up associations between these ports
	 *	and this default_pager structure
	 */

	ds->pager = new_pager;
	ds->pager_request = new_pager_request;
	ds->request_refs = 1;
	ds->pager_name = new_pager_name;
	ds->name_refs = 1;

	/*
	 *	After this, other threads might receive requests
	 *	for this memory object or find it in the port list.
	 */

	pager_port_list_insert(new_pager, ds);
	default_pager_add(ds, TRUE);

	return(KERN_SUCCESS);
}

memory_object_copy_strategy_t default_pager_copy_strategy =
					MEMORY_OBJECT_COPY_DELAY;

kern_return_t
seqnos_memory_object_init(mach_port_t pager,
			  mach_port_seqno_t seqno,
			  mach_port_t pager_request,
			  mach_port_t pager_name,
			  vm_size_t pager_page_size)
{
	register default_pager_t 	ds;
	kern_return_t		 	kr;
	STATIC char		 	here[] = "%sinit";

	assert(MACH_PORT_VALID(pager_request));
	assert(MACH_PORT_VALID(pager_name));
	assert(pager_page_size == vm_page_size);

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
	    panic(here, my_name);
	pager_port_lock(ds, seqno);

	if (ds->pager_request != MACH_PORT_NULL)
	    panic(here, my_name);

	ds->pager_request = pager_request;
	ds->request_refs = 1;
	ds->pager_name = pager_name;
	ds->name_refs = 1;

	/*
	 *	Even if the kernel immediately terminates the object,
	 *	the pager_request port won't be destroyed until
	 *	we process the terminate request, which won't happen
	 *	until we unlock the object.
	 */
	if (norma_mk) {
		memory_object_norma_info_data_t attributes;

		attributes.object_ready = TRUE;
		attributes.may_cache = FALSE;
		attributes.write_completions = TRUE;
		attributes.copy_strategy = default_pager_copy_strategy;
		attributes.cluster_size = vm_page_size;

		kr = memory_object_change_attributes(
				pager_request,
                       		MEMORY_OBJECT_NORMA_INFO,
                       		(memory_object_info_t) &attributes,
                       		MEMORY_OBJECT_NORMA_INFO_COUNT,
                       		MACH_PORT_NULL);
	} else {
		memory_object_attr_info_data_t attributes;

		attributes.object_ready = TRUE;
		attributes.may_cache = FALSE;
		attributes.copy_strategy = default_pager_copy_strategy;

		kr = memory_object_change_attributes(
				pager_request,
                       		MEMORY_OBJECT_ATTRIBUTE_INFO,
                       		(memory_object_info_t) &attributes,
                       		MEMORY_OBJECT_ATTR_INFO_COUNT,
                       		MACH_PORT_NULL);
	}

	if (kr != KERN_SUCCESS)
	    panic(here, my_name);

	pager_port_unlock(ds);

	return(KERN_SUCCESS);
}

kern_return_t
seqnos_memory_object_notify(memory_object_rep_t rep,
			    memory_object_control_t control,
			    mach_port_t host,
			    vm_size_t psize)
{
	return(KERN_FAILURE);
}

kern_return_t
seqnos_memory_object_rejected(memory_object_t object,
			      memory_object_control_t control,
			      kern_return_t reason)
{
	return(KERN_FAILURE);
}

kern_return_t
seqnos_memory_object_synchronize(mach_port_t pager,
				 mach_port_seqno_t seqno,
				 mach_port_t pager_request,
				 vm_offset_t offset,
				 vm_offset_t length,
				 vm_sync_t flags)
{
	default_pager_t ds;

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
		panic("(default_pager)data_return");

	pager_port_lock(ds, seqno);
        pager_port_check_request(ds, pager_request);
	pager_port_unlock(ds);

	memory_object_synchronize_completed(pager_request, offset, length);

	return(KERN_SUCCESS);
}/* seqnos_memory_object_synchronize */

kern_return_t
seqnos_memory_object_terminate(mach_port_t pager,
			       mach_port_seqno_t seqno,
			       mach_port_t pager_request,
			       mach_port_t pager_name)
{
	register default_pager_t	ds;
	mach_port_urefs_t		request_refs, name_refs;
	kern_return_t			kr;
	STATIC char			here[] = "%sterminate";

	/*
	 *	pager_request and pager_name are receive rights,
	 *	not send rights.
	 */

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
		panic(here, my_name);
	pager_port_lock(ds, seqno);

	/*
	 *	Wait for read and write requests to terminate.
	 */

	pager_port_wait_for_readers(ds);
	pager_port_wait_for_writers(ds);

	/*
	 *	After memory_object_terminate both memory_object_init

	 *	and a no-senders notification are possible, so we need
	 *	to clean up the request and name ports but leave
	 *	the pager port.
	 *
	 *	A concurrent default_pager_objects might be allocating
	 *	more references for the name port.  In this case,
	 *	we must first wait for it to finish.
	 */

	pager_port_wait_for_refs(ds);

	ds->pager_request = MACH_PORT_NULL;
	request_refs = ds->request_refs;
	ds->request_refs = 0;
	assert(ds->pager_name == pager_name);
	ds->pager_name = MACH_PORT_NULL;
	name_refs = ds->name_refs;
	ds->name_refs = 0;
	pager_port_unlock(ds);

	/*
	 *	Now we deallocate our various port rights.
	 */

	kr = mach_port_mod_refs(default_pager_self, pager_request,
				MACH_PORT_RIGHT_SEND, -request_refs);
	if (kr != KERN_SUCCESS)
	    panic(here,my_name);


	kr = mach_port_mod_refs(default_pager_self, pager_request,
				MACH_PORT_RIGHT_RECEIVE, -1);
	if (kr != KERN_SUCCESS)
	    panic(here,my_name);

	kr = mach_port_mod_refs(default_pager_self, pager_name,
				MACH_PORT_RIGHT_SEND, -name_refs);
	if (kr != KERN_SUCCESS)
	    panic(here,my_name);

	kr = mach_port_mod_refs(default_pager_self, pager_name,
				MACH_PORT_RIGHT_RECEIVE, -1);
	if (kr != KERN_SUCCESS)
	    panic(here,my_name);

	return (KERN_SUCCESS);
}

STATIC void
default_pager_no_senders(memory_object_t pager,
			 mach_port_seqno_t seqno,
			 mach_port_mscount_t mscount)
{
	register default_pager_t ds;
	kern_return_t		 kr;
	STATIC char		 here[] = "%sno_senders";

	/*
	 *	Because we don't give out multiple send rights
	 *	for a memory object, there can't be a race
	 *	between getting a no-senders notification
	 *	and creating a new send right for the object.
	 *	Hence we don't keep track of mscount.
	 */


	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
		panic(here,my_name);
	pager_port_lock(ds, seqno);

	/*
	 *	We shouldn't get a no-senders notification
	 *	when the kernel has the object cached.
	 */

	if (ds->pager_request != MACH_PORT_NULL)
		panic(here,my_name);

	/*
	 *	Unlock the pager (though there should be no one
	 *	waiting for it).
	 */
	pager_unlock(&ds->lock);

	/*
	 *	Remove the memory object port association, and then
	 *	the destroy the port itself.  We must remove the object
	 *	from the port list before deallocating the pager,
	 *	because of default_pager_objects.
	 */

	pager_port_list_delete(ds);
	pager_dealloc(&ds->dpager);

	kr = mach_port_mod_refs(default_pager_self, pager,
				MACH_PORT_RIGHT_RECEIVE, -1);
	if (kr != KERN_SUCCESS)
		panic(here,my_name);

	/*
	 * Do this *after* deallocating the port name
	 */
	kfree((char *) ds, sizeof(*ds));

	/*
	 * Recover memory that we might have wasted because
	 * of name conflicts
	 */
	pager_lock(&all_pagers.lock);

	while (!queue_empty(&all_pagers.leak_queue)) {

		ds = (default_pager_t) queue_first(&all_pagers.leak_queue);
		queue_remove_first(&all_pagers.leak_queue, ds, default_pager_t, links);
		kfree((char *) ds, sizeof(*ds));
	}

	pager_unlock(&all_pagers.lock);
}

int		default_pager_pagein_count = 0;
int		default_pager_pageout_count = 0;

kern_return_t
seqnos_memory_object_data_request(memory_object_t pager,
				  mach_port_seqno_t seqno,
				  mach_port_t reply_to,
				  vm_offset_t offset,
				  vm_size_t length,
				  vm_prot_t protection_required)
{
	default_pager_thread_t	*dpt;
	default_pager_t		ds;
	vm_offset_t		addr;
	unsigned int 		errors;
	kern_return_t		rc;
	boolean_t precious;

	dpt = (default_pager_thread_t *) cthread_data(cthread_self());

	if (length != vm_page_size)
	    panic("(default_pager)data_request");

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
	    panic("(default_pager)data_request");
	pager_port_lock(ds, seqno);
	pager_port_check_request(ds, reply_to);
	pager_port_wait_for_writers(ds);
	pager_port_start_read(ds);
	pager_port_unlock(ds);

	/*
	 * Get error count while pager locked.
	 */
	errors = ds->errors;

	if (errors) {
	    printf("%s %s\n", my_name,
		   "dropping data_request because of previous paging errors");
	    (void) memory_object_data_error(reply_to,
				offset, vm_page_size,
				KERN_FAILURE);
	    goto done;
	}

#if	USE_PRECIOUS
	precious = TRUE;
#else	/*USE_PRECIOUS*/
	precious = FALSE;
#endif	/*USE_PRECIOUS*/

	rc = default_read(&ds->dpager, dpt->dpt_buffer,
			  vm_page_size, offset,
			  &addr, precious);
	switch (rc) {
	    case PAGER_SUCCESS:
		(void) memory_object_data_supply(
		        reply_to, offset, addr, vm_page_size,
			(addr != dpt->dpt_buffer), VM_PROT_NONE,
			precious, MACH_PORT_NULL);
		break;

	    case PAGER_ABSENT:
		(void) memory_object_data_unavailable(
			reply_to,
			offset,
			vm_page_size);
		break;

	    case PAGER_ERROR:
		(void) memory_object_data_error(
			reply_to,
			offset,
			vm_page_size,
			KERN_FAILURE);
		break;
	}

	default_pager_pagein_count++;

    done:
	pager_port_finish_read(ds);
	return(KERN_SUCCESS);
}

/*
 * memory_object_data_initialize: check whether we already have each page, and
 * write it if we do not.  The implementation is far from optimized, and
 * also assumes that the default_pager is single-threaded.
 */
kern_return_t
seqnos_memory_object_data_initialize(memory_object_t pager,
				     mach_port_seqno_t seqno,
				     mach_port_t pager_request,
				     register vm_offset_t offset,
				     register pointer_t addr,
				     vm_size_t data_cnt)
{
	vm_offset_t	amount_sent;
	default_pager_t	ds;
	STATIC char	here[] = "%sdata_initialize";

#ifdef	lint
	pager_request++;
#endif	/* lint */

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
	    panic(here,my_name);
	pager_port_lock(ds, seqno);
	pager_port_check_request(ds, pager_request);
	pager_port_start_write(ds);
	pager_port_unlock(ds);

	for (amount_sent = 0;
	     amount_sent < data_cnt;
	     amount_sent += vm_page_size) {

	     if (!default_has_page(&ds->dpager, offset + amount_sent)) {
		if (default_write(&ds->dpager,
				  addr + amount_sent,
				  vm_page_size,
				  offset + amount_sent)
			 != PAGER_SUCCESS) {
		    printf("%s%s write error\n", my_name, here);
		    ds->errors++;
		}
	     }
	}

	pager_port_finish_write(ds);
	if (vm_deallocate(default_pager_self, addr, data_cnt) != KERN_SUCCESS)
	    panic(here,my_name);

	if (norma_mk) {
		kern_return_t kr;

		/*
		 * Acknowledge completion of the pageout, allowing the
		 * sending kernel to free up resources.
		 */
		kr = memory_object_data_write_completed(pager_request,
							offset, data_cnt);
		if (kr != KERN_SUCCESS)
		   printf("DEF PAGER DATA_INIT:  COMPLETION FAILURE %x\n", kr);
	}

	return(KERN_SUCCESS);
}

/*ARGSUSED*/
kern_return_t
seqnos_memory_object_copy(memory_object_t old_memory_object,
			  mach_port_seqno_t seqno,
			  memory_object_control_t old_memory_control,
			  vm_offset_t offset,
			  vm_size_t length,
			  memory_object_t new_memory_object)
{
	panic("%scopy", my_name);
	return KERN_FAILURE;
}

kern_return_t
seqnos_memory_object_lock_completed(memory_object_t pager,
				    mach_port_seqno_t seqno,
				    mach_port_t pager_request,
				    vm_offset_t offset,
				    vm_size_t length)
{
#ifdef	lint
	pager++; seqno++; pager_request++; offset++; length++;
#endif	/* lint */

	panic("%slock_completed",my_name);
	return(KERN_FAILURE);
}

kern_return_t
seqnos_memory_object_data_unlock(memory_object_t pager,
				 mach_port_seqno_t seqno,
				 mach_port_t pager_request,
				 vm_offset_t offset,
				 pointer_t addr,
				 vm_size_t data_cnt)
{
	panic("%sdata_unlock",my_name);
	return(KERN_FAILURE);
}

kern_return_t
seqnos_memory_object_supply_completed(memory_object_t pager,
				      mach_port_seqno_t seqno,
				      mach_port_t pager_request,
				      vm_offset_t offset,
				      vm_size_t length,
				      kern_return_t result,
				      vm_offset_t error_offset)
{
	panic("%ssupply_completed",my_name);
	return(KERN_FAILURE);
}

kern_return_t
seqnos_memory_object_data_return(memory_object_t pager,
				 mach_port_seqno_t seqno,
				 mach_port_t pager_request,
				 vm_offset_t offset,
				 pointer_t addr,
				 vm_size_t data_cnt,
				 boolean_t dirty,
				 boolean_t kernel_copy)
{
	register
	vm_size_t	amount_sent;
	default_pager_t	ds;

#ifdef	lint
	pager_request++;
	dirty++;
	kernel_copy++;
#endif	/* lint */

	if ((data_cnt % vm_page_size) != 0)
	    panic("(default_pager)data_return");

	ds = pager_port_lookup(pager);
	if (ds == DEFAULT_PAGER_NULL)
	    panic("(default_pager)data_return");
	pager_port_lock(ds, seqno);
	pager_port_check_request(ds, pager_request);
	pager_port_start_write(ds);
	pager_port_unlock(ds);

	for (amount_sent = 0;
	     amount_sent < data_cnt;
	     amount_sent += vm_page_size) {

	    register int result;

	    result = default_write(&ds->dpager,
			      addr + amount_sent,
			      vm_page_size,
			      offset + amount_sent);
	    if (result != KERN_SUCCESS) {
		printf("*** WRITE ERROR on default_pageout:");
		printf(" pager=%d, offset=0x%x, length=0x%x, result=%d\n",
			pager, offset+amount_sent, vm_page_size, result);
		ds->errors++;
	    }
	    default_pager_pageout_count++;
	}

	pager_port_finish_write(ds);
	if (vm_deallocate(default_pager_self, addr, data_cnt) != KERN_SUCCESS)
		panic("default_data_return: deallocate failed");

	if (norma_mk) {
		kern_return_t kr;

	    /*
	     * Acknowledge completion of the pageout, allowing the
	     * sending kernel to free up resources.
	     */
	    kr = memory_object_data_write_completed(pager_request,
						    offset, data_cnt);
	    if (kr != KERN_SUCCESS)
	        printf("DEF PAGER DATA_WRITE:  COMPLETION FAILURE %x\n", kr);
	}

	return(KERN_SUCCESS);
}

kern_return_t
seqnos_memory_object_change_completed(memory_object_t pager,
				      mach_port_seqno_t seqno,
				      memory_object_control_t memory_control,
				      memory_object_flavor_t flavor)
{
	panic("%schange_completed",my_name);
	return(KERN_FAILURE);
}


STATIC boolean_t
default_pager_notify_server(mach_msg_header_t *in, mach_msg_header_t *out)
{
	register mach_no_senders_notification_t *n =
			(mach_no_senders_notification_t *) in;

	/*
	 *	The only send-once rights we create are for
	 *	receiving no-more-senders notifications.
	 *	Hence, if we receive a message directed to
	 *	a send-once right, we can assume it is
	 *	a genuine no-senders notification from the kernel.
	 */

	if ((n->not_header.msgh_bits !=
			MACH_MSGH_BITS(0, MACH_MSG_TYPE_PORT_SEND_ONCE)) ||
	    (n->not_header.msgh_id != MACH_NOTIFY_NO_SENDERS))
		return FALSE;

        assert(n->not_header.msgh_size == sizeof *n -
                sizeof(mach_msg_format_0_trailer_t));
        assert(n->not_header.msgh_remote_port == MACH_PORT_NULL);

	default_pager_no_senders(n->not_header.msgh_local_port,
				 n->trailer.msgh_seqno, n->not_count);

	out->msgh_remote_port = MACH_PORT_NULL;
	return TRUE;
}

STATIC mach_msg_size_t default_pager_msg_size = 128;

STATIC boolean_t
default_pager_demux_object(mach_msg_header_t *in, mach_msg_header_t *out)
{
	/*
	 *	We receive memory_object_data_initialize messages in
	 *	the memory_object_default interface.
	 */

	return (seqnos_memory_object_server(in, out) ||
		seqnos_memory_object_default_server(in, out) ||
		default_pager_notify_server(in, out));
}

STATIC boolean_t
default_pager_demux_default(mach_msg_header_t *in, mach_msg_header_t *out)
{
	if (in->msgh_local_port == default_pager_default_port) {
		/*
		 *	We receive memory_object_create messages in
		 *	the memory_object_default interface.
		 */

		return (seqnos_memory_object_default_server(in, out) ||
			default_pager_object_server(in, out));
	} else {
		panic(my_name);
		return FALSE;
	}
}

/*
 *	We use multiple threads, for two reasons.
 *
 *	First, memory objects created by default_pager_object_create
 *	are "external", instead of "internal".  This means the kernel
 *	sends data (memory_object_data_return) to the object pageable.
 *	To prevent deadlocks, the external and internal objects must
 *	be managed by different threads.
 *
 *	Second, the default pager uses synchronous IO operations.
 *	Spreading requests across multiple threads should
 *	recover some of the performance loss from synchronous IO.
 *
 *	We have 3+ threads.
 *	One receives memory_object_create and
 *	default_pager_object_create requests.
 *	One or more manage internal objects.
 *	One or more manage external objects.
 */

STATIC void
default_pager_thread_privileges(void)
{
	/*
	 *	Set thread privileges.
	 */

	cthread_wire();		/* attach kernel thread to cthread */
	wire_thread();		/* grab a kernel stack and memory allocation
				   privileges */

}

STATIC void
default_pager_thread(char *arg)
{
	default_pager_thread_t *dpt;
	mach_port_t pset;
	kern_return_t kr;

	dpt = (default_pager_thread_t *) arg;
	cthread_set_data(cthread_self(), (char *) dpt);

	/*
	 *	Threads handling external objects cannot have
	 *	privileges.  Otherwise a burst of data-requests for an
	 *	external object could empty the free-page queue,
	 *	because the fault code only reserves real pages for
	 *	requests sent to internal objects.
	 */

	if (dpt->dpt_internal) {
		default_pager_thread_privileges();
		pset = default_pager_internal_set;
	} else {
		pset = default_pager_external_set;
	}

	for (;;) {
		kr = mach_msg_server(default_pager_demux_object,
				     default_pager_msg_size,
				     pset,
		MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_SEQNO));
		panic(my_name, kr);
	}
}

STATIC void
start_default_pager_thread(boolean_t internal)
{
	default_pager_thread_t *dpt;
	kern_return_t kr;

	dpt = (default_pager_thread_t *) kalloc(sizeof *dpt);
	if (dpt == 0)
		panic(my_name);

	dpt->dpt_internal = internal;

	kr = vm_allocate(default_pager_self, &dpt->dpt_buffer,
			 vm_page_size, TRUE);
	if (kr != KERN_SUCCESS)
		panic(my_name);
	wire_memory(dpt->dpt_buffer, vm_page_size,
		    VM_PROT_READ|VM_PROT_WRITE);

	dpt->dpt_thread = cthread_fork((cthread_fn_t) default_pager_thread,
				       (any_t) dpt);
}

STATIC void
default_pager_initialize(mach_port_t host_port)
{
	kern_return_t		kr;

	/*
	 *	This task will become the default pager.
	 */
	default_pager_self = mach_task_self();
	default_pager_default_thread = mach_thread_self();

	/*
	 *	Initialize the "default pager" port.
	 */
	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_RECEIVE, &default_pager_default_port);

	if (kr != KERN_SUCCESS)
		panic(my_name);

	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_PORT_SET, &default_pager_internal_set);

	if (kr != KERN_SUCCESS)
		panic(my_name);

	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_PORT_SET, &default_pager_external_set);

	if (kr != KERN_SUCCESS)
		panic(my_name);

    {
	memory_object_t DMM = default_pager_default_port;
	kr = vm_set_default_memory_manager(host_port, &DMM);
	if ((kr != KERN_SUCCESS) || (DMM != MACH_PORT_NULL))
		panic(my_name);
    }

	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_PORT_SET, &default_pager_default_set);
	if (kr != KERN_SUCCESS)
		panic(my_name);

	kr = mach_port_move_member(default_pager_self,
				   default_pager_default_port,
				   default_pager_default_set);
	if (kr != KERN_SUCCESS)
		panic(my_name);

	/*
	 * Arrange for wiring privileges.
	 */
	wire_setup(host_port);

	/*
	 * Find out how many CPUs we have, to determine the number
	 * of threads to create.
	 */
	if (default_pager_internal_count == 0) {
		host_basic_info_data_t h_info;
		unsigned int h_info_count;

		h_info_count = HOST_BASIC_INFO_COUNT;
		(void) host_info(host_port, HOST_BASIC_INFO,
				 (host_info_t)&h_info, &h_info_count);

		/*
		 *	Random computation to get more parallelism on
		 *	multiprocessors.
		 */
		default_pager_internal_count =
		    (h_info.avail_cpus > 32 ? 32 : h_info.avail_cpus) / 4 + 3;
	}
}

/*
 * Initialize and Run the default pager
 */
STATIC void
default_pager(void)
{
	kern_return_t kr;
	int i;

	default_pager_thread_privileges();

	/*
	 * Wire down code, data, stack
	 */
	wire_all_memory();

	/*
	 *	Initialize the list of all pagers.
	 */
	pager_port_list_init();

	/*
	 *	Now we create the threads that will actually
	 *	manage objects.
	 */

	for (i = 0; i < default_pager_internal_count; i++)
		start_default_pager_thread(TRUE);

	for (i = 0; i < default_pager_external_count; i++)
		start_default_pager_thread(FALSE);

	for (;;) {
		kr = mach_msg_server(default_pager_demux_default,
				     default_pager_msg_size,
				     default_pager_default_set,
		MACH_RCV_TRAILER_ELEMENTS(MACH_RCV_TRAILER_SEQNO));
		panic(my_name, kr);
	}
}

/*
 * Create an external object.
 */
kern_return_t
default_pager_object_create(mach_port_t pager,
			    mach_port_t *mem_obj,
			    vm_size_t size)
{
	default_pager_t ds;
	mach_port_t port;
	kern_return_t result;

	if (pager != default_pager_default_port)
		return KERN_INVALID_ARGUMENT;

	ds = pager_port_alloc(size);
rename_it:
	port = (mach_port_t) pnameof((void *)ds);
	result = mach_port_allocate_name(default_pager_self,
				    MACH_PORT_RIGHT_RECEIVE, port);
	if (result != KERN_SUCCESS) {
		default_pager_t	ds1;

		if (result != KERN_NAME_EXISTS) return (result);

		ds1 = (default_pager_t) kalloc(sizeof *ds1);
		*ds1 = *ds;
		pager_lock(&all_pagers.lock);
		queue_enter(&all_pagers.leak_queue, ds, default_pager_t, links);
		pager_unlock(&all_pagers.lock);
		ds = ds1;
		goto rename_it;
	}

	/*
	 *	Set up associations between these ports
	 *	and this default_pager structure
	 */

	ds->pager = port;
	pager_port_list_insert(port, ds);
	default_pager_add(ds, FALSE);

	*mem_obj = port;
	return (KERN_SUCCESS);
}

kern_return_t
default_pager_info(mach_port_t pager,
		   default_pager_info_t *infop)
{
	vm_size_t	pages_total, pages_free;

	if (pager != default_pager_default_port)
		return KERN_INVALID_ARGUMENT;

	pager_lock(&all_partitions.lock);
	paging_space_info(&pages_total, &pages_free);
	pager_unlock(&all_partitions.lock);

	infop->dpi_total_space = ptoa(pages_total);
	infop->dpi_free_space = ptoa(pages_free);
	infop->dpi_page_size = vm_page_size;
	return KERN_SUCCESS;
}

kern_return_t
default_pager_objects(mach_port_t pager,
		      default_pager_object_array_t *objectsp,
		      unsigned int *ocountp,
		      mach_port_array_t *portsp,
		      unsigned int *pcountp)
{
	vm_offset_t		oaddr = 0;	/* memory for objects */
	vm_size_t		osize = 0;	/* current size */
	default_pager_object_t	*objects;
	unsigned int		opotential;

	vm_offset_t		paddr = 0;	/* memory for ports */
	vm_size_t		psize = 0;	/* current size */
	mach_port_t		*ports;
	unsigned int		ppotential;

	unsigned int		actual;
	unsigned int		num_pagers;
	kern_return_t		kr;
	default_pager_t		entry;

	if (pager != default_pager_default_port)
		return KERN_INVALID_ARGUMENT;

	/* start with the inline memory */

	num_pagers = 0;

	objects = *objectsp;
	opotential = *ocountp;

	ports = *portsp;
	ppotential = *pcountp;

	pager_lock(&all_pagers.lock);
	/*
	 * We will send no more than this many
	 */
	actual = all_pagers.count;
	pager_unlock(&all_pagers.lock);

	if (opotential < actual) {
		vm_offset_t	newaddr;
		vm_size_t	newsize;

		newsize = 2 * round_page(actual * sizeof *objects);

		kr = vm_allocate(default_pager_self, &newaddr, newsize, TRUE);
		if (kr != KERN_SUCCESS)
			goto nomemory;

		oaddr = newaddr;
		osize = newsize;
		opotential = osize/sizeof *objects;
		objects = (default_pager_object_t *) oaddr;
	}

	if (ppotential < actual) {
		vm_offset_t	newaddr;
		vm_size_t	newsize;

		newsize = 2 * round_page(actual * sizeof *ports);

		kr = vm_allocate(default_pager_self, &newaddr, newsize, TRUE);
		if (kr != KERN_SUCCESS)
			goto nomemory;

		paddr = newaddr;
		psize = newsize;
		ppotential = psize/sizeof *ports;
		ports = (mach_port_t *) paddr;
	}

	/*
	 * Now scan the list.
	 */

	pager_lock(&all_pagers.lock);

	num_pagers = 0;
	queue_iterate(&all_pagers.queue, entry, default_pager_t, links) {

		mach_port_t		port;
		vm_size_t		size;

		if ((num_pagers >= opotential) ||
		    (num_pagers >= ppotential)) {
			/*
			 *	This should be rare.  In any case,
			 *	we will only miss recent objects,
			 *	because they are added at the end.
			 */
			break;
		}

		/*
		 * Avoid interfering with normal operations
		 */
		if (!pager_lock_try(&entry->dpager.lock))
			goto not_this_one;
		size = pager_allocated(&entry->dpager);
		pager_unlock(&entry->dpager.lock);

		pager_lock(&entry->lock);

		port = entry->pager_name;
		if (port == MACH_PORT_NULL) {
			/*
			 *	The object is waiting for no-senders
			 *	or memory_object_init.
			 */
			pager_unlock(&entry->lock);
			goto not_this_one;
		}

		/*
		 *	We need a reference for the reply message.
		 *	While we are unlocked, the bucket queue
		 *	can change and the object might be terminated.
		 *	memory_object_terminate will wait for us,
		 *	preventing deallocation of the entry.
		 */

		if (--entry->name_refs == 0) {
			pager_unlock(&entry->lock);

			/* keep the list locked, wont take long */

			kr = mach_port_mod_refs(default_pager_self,
					port, MACH_PORT_RIGHT_SEND,
					default_pager_max_urefs);
			if (kr != KERN_SUCCESS)
				panic("%sdefault_pager_objects",my_name);

			pager_lock(&entry->lock);

			entry->name_refs += default_pager_max_urefs;
			pager_port_finish_refs(entry);
		}
		pager_unlock(&entry->lock);

		/* the arrays are wired, so no deadlock worries */

		objects[num_pagers].dpo_object = (vm_offset_t) entry;
		objects[num_pagers].dpo_size = size;
		ports  [num_pagers++] = port;
		continue;
not_this_one:
		/*
		 * Do not return garbage
		 */
		objects[num_pagers].dpo_object = (vm_offset_t) 0;
		objects[num_pagers].dpo_size = 0;
		ports  [num_pagers++] = MACH_PORT_NULL;
		
	}

	pager_unlock(&all_pagers.lock);

	/*
	 *	Deallocate and clear unused memory.
	 *	(Returned memory will automagically become pageable.)
	 */

	if (objects == *objectsp) {
		/*
		 *	Our returned information fit inline.
		 *	Nothing to deallocate.
		 */

		*ocountp = num_pagers;
	} else if (actual == 0) {
		(void) vm_deallocate(default_pager_self, oaddr, osize);

		/* return zero items inline */
		*ocountp = 0;
	} else {
		vm_offset_t used;

		used = round_page(actual * sizeof *objects);

		if (used != osize)
			(void) vm_deallocate(default_pager_self,
					     oaddr + used, osize - used);

		*objectsp = objects;
		*ocountp = num_pagers;
	}

	if (ports == *portsp) {
		/*
		 *	Our returned information fit inline.
		 *	Nothing to deallocate.
		 */

		*pcountp = num_pagers;
	} else if (actual == 0) {
		(void) vm_deallocate(default_pager_self, paddr, psize);

		/* return zero items inline */
		*pcountp = 0;
	} else {
		vm_offset_t used;

		used = round_page(actual * sizeof *ports);

		if (used != psize)
			(void) vm_deallocate(default_pager_self,
					     paddr + used, psize - used);

		*portsp = ports;
		*pcountp = num_pagers;
	}

	return KERN_SUCCESS;

    nomemory:

	{
		register int	i;
		for (i = 0; i < num_pagers; i++)
		    (void) mach_port_deallocate(default_pager_self, ports[i]);
	}

	if (objects != *objectsp)
		(void) vm_deallocate(default_pager_self, oaddr, osize);

	if (ports != *portsp)
		(void) vm_deallocate(default_pager_self, paddr, psize);

	return KERN_RESOURCE_SHORTAGE;
}


kern_return_t
default_pager_object_pages(mach_port_t pager,
			   mach_port_t object,
			   default_pager_page_array_t *pagesp,
			   unsigned int *countp)
{
	vm_offset_t			addr;	/* memory for page offsets */
	vm_size_t			size = 0; /* current memory size */
	default_pager_page_t		*pages;
	unsigned int 			potential, actual;
	kern_return_t			kr;

	if (pager != default_pager_default_port)
		return KERN_INVALID_ARGUMENT;

	/* we start with the inline space */

	pages = *pagesp;
	potential = *countp;

	for (;;) {
		default_pager_t		entry;

		pager_lock(&all_pagers.lock);
		queue_iterate(&all_pagers.queue, entry, default_pager_t, links) {
			pager_lock(&entry->lock);
			if (entry->pager_name == object) {
				pager_unlock(&all_pagers.lock);
				goto found_object;
			}
			pager_unlock(&entry->lock);
		}
		pager_unlock(&all_pagers.lock);

		/* did not find the object */

		if (pages != *pagesp)
			(void) vm_deallocate(default_pager_self, addr, size);
		return KERN_INVALID_ARGUMENT;

	    found_object:

		if (!pager_lock_try(&entry->dpager.lock)) {
			/* oh well bad luck */

			pager_unlock(&entry->lock);

			/* yield the processor */
			(void) thread_switch(MACH_PORT_NULL,
					     SWITCH_OPTION_NONE, 0);
			continue;
		}

		actual = pager_pages(&entry->dpager, pages, potential);
		pager_unlock(&entry->dpager.lock);
		pager_unlock(&entry->lock);

		if (actual <= potential)
			break;

		/* allocate more memory */

		if (pages != *pagesp)
			(void) vm_deallocate(default_pager_self, addr, size);
		size = round_page(actual * sizeof *pages);
		kr = vm_allocate(default_pager_self, &addr, size, TRUE);
		if (kr != KERN_SUCCESS)
			return kr;
		pages = (default_pager_page_t *) addr;
		potential = size/sizeof *pages;
	}

	/*
	 *	Deallocate and clear unused memory.
	 *	(Returned memory will automagically become pageable.)
	 */

	if (pages == *pagesp) {
		/*
		 *	Our returned information fit inline.
		 *	Nothing to deallocate.
		 */

		*countp = actual;
	} else if (actual == 0) {
		(void) vm_deallocate(default_pager_self, addr, size);

		/* return zero items inline */
		*countp = 0;
	} else {
		vm_offset_t used;

		used = round_page(actual * sizeof *pages);

		if (used != size)
			(void) vm_deallocate(default_pager_self,
					     addr + used, size - used);

		*pagesp = pages;
		*countp = actual;
	}
	return KERN_SUCCESS;
}

/*
 * When things do not quite workout...
 */
STATIC void
no_paging_space(boolean_t out_of_memory)
{
	STATIC char		here[] = "%s *** NOT ENOUGH PAGING SPACE ***";

	if (out_of_memory)
		printf("*** OUT OF MEMORY *** ");
	panic(here, my_name);
}

STATIC void
overcommitted(boolean_t got_more_space,
	      vm_size_t space)		/* in pages */
{
	vm_size_t	pages_free, pages_total;

	STATIC boolean_t user_warned = FALSE;
	STATIC vm_size_t pages_shortage = 0;

	paging_space_info(&pages_total, &pages_free);

	/*
	 * If user added more space, see if it is enough 
	 */
	if (got_more_space) {
		if (pages_free > pages_shortage) {
			pages_shortage = 0;
			if (user_warned)
				printf("%s paging space ok now.\n", my_name);
		} else
			pages_shortage -= pages_free;

		user_warned = FALSE;
		return;
	}
	/*
	 * We ran out of gas, let user know. 
	 */

	if (pages_free >= space) {
		pages_shortage = 0;
	} else
	  	pages_shortage = space - pages_free;
	if (!user_warned && pages_shortage) {
		user_warned = TRUE;
		printf("%s paging space over-committed.\n", my_name);
	}
#if debug
	user_warned = FALSE;
	printf("%s paging space over-committed [+%d (%d) pages].\n",
			my_name, space, pages_shortage);
#endif
}

STATIC void
paging_space_info(vm_size_t *totp, vm_size_t *freep)
{
	register vm_size_t	pages_total, pages_free;
	register partition_t	part;
	register int		i;

	pages_total = pages_free = 0;
	for (i = 0; i < all_partitions.n_partitions; i++) {

		if ((part = partition_of(i)) == 0) continue;

		/* no need to lock: by the time this data
		   gets back to any remote requestor it
		   will be obsolete anyways */
		pages_total += part->total_size;
		pages_free += part->free;
#if debug
		printf("Partition %d: x%x total, x%x free\n",
		       i, part->total_size, part->free);
#endif
	}
	*totp = pages_total;
	*freep = pages_free;
}


STATIC kern_return_t
add_paging_file(mach_port_t master_host_port,
		mach_port_t master_device_port,
		const char *file_name)
{
	register struct file_direct *fdp;
	register kern_return_t	result;
	struct file     	pfile;
	boolean_t		isa_file;

	bzero((char *) &pfile, sizeof(struct file));

	result = open_file(master_device_port, file_name, &pfile);
	if (result != KERN_SUCCESS)
		return result;

	fdp = (struct file_direct *) kalloc(sizeof *fdp);
	bzero((char *) fdp, sizeof *fdp);

	isa_file = file_is_structured(&pfile);

	result = open_file_direct(&pfile, fdp, isa_file);
	if (result)
		panic("Can't open paging file %s\n", file_name);

	result = add_file_direct(fdp, &pfile);
	if (result)
		panic("Can't read disk addresses: %d\n", result);

	close_file(&pfile);

	/*
	 * Set up the default paging partition 
	 */
	create_paging_partition(file_name,
				fdp->fd_size * fdp->fd_bsize,
				page_read_file_direct,
				page_write_file_direct,
				fdp,
				isa_file);

	return result;
}

#if	PAGING_MEMORY

STATIC kern_return_t
add_paging_mem(mach_port_t host_port)
{
	vm_statistics_data_t	vm_stats;
	unsigned int		count;
	vm_size_t		paging_size;
	vm_offset_t		paging_offset;
	kern_return_t		kr;

	count = sizeof(vm_stats);
	if ((kr = host_statistics(host_port,
				  HOST_VM_INFO,
				  (host_info_t)&vm_stats,
				  &count)) != KERN_SUCCESS)
		panic("host statistics failed: %d", kr);

	paging_size = vm_stats.free_count/2 * vm_page_size;

	if ((kr = vm_allocate(mach_task_self(), &paging_offset,
			 paging_size, TRUE)) != KERN_SUCCESS) {
		printf("vm_allocate %d bytes failed: %d\n", paging_size, kr);
		panic("add_paging_mem: vm_allocate failed");
	}

	wire_memory(paging_offset, paging_size, VM_PROT_READ|VM_PROT_WRITE);

	/*
	 * Set up the default paging partition 
	 */
	create_paging_partition("memory",
				paging_size,
				page_read_mem_direct,
				page_write_mem_direct,
				(struct file_direct *)paging_offset,
				FALSE);

	printf("%s Added %d Kbytes to paging space\n", my_name, paging_size);
	return kr;
}

int page_read_mem_direct(
	struct file_direct	*fp,
	vm_offset_t		off,
        vm_size_t 		size,
	vm_offset_t 		*ret_off,
	vm_size_t 		*ret_size)
{
  	vm_offset_t		paging_offset;
	default_pager_thread_t	*dpt;

	dpt = (default_pager_thread_t *) cthread_data(cthread_self());
	*ret_off = dpt->dpt_buffer;
	paging_offset = (vm_offset_t)fp;
	bcopy((char *)paging_offset+off, (char *)*ret_off, size);
	*ret_size = size;
	return KERN_SUCCESS;
}

int page_write_mem_direct(
	struct file_direct	*fp,
        vm_offset_t		addr,
	vm_offset_t		off,
	vm_size_t		size,
	vm_size_t		*ret_size)
{
  	vm_offset_t		paging_offset;

	paging_offset = (vm_offset_t)fp;
	bcopy((char *)addr, (char *)paging_offset+off, size);
	*ret_size = size;
	return KERN_SUCCESS;
}
#endif	/* PAGING_MEMORY */

#if 0
STATIC kern_return_t
remove_paging_file(const char *file_name)
{
	struct file_direct	*fdp = 0;
	kern_return_t		kr;

	kr = destroy_paging_partition(file_name, &fdp);
	if (kr == KERN_SUCCESS) {
		remove_file_direct(fdp);
		kfree(fdp, sizeof(*fdp));
	}
	return kr;
}
#endif

STATIC boolean_t
default_pager_setup(mach_port_t master_host_port,
		    mach_port_t master_device_port)
{
	register kern_return_t	result;
	char	paging_file_name[1024];

	(void) strcpy(paging_file_name,
		      "/dev/boot_device/mach_servers/paging_file");

	while (TRUE) {
	    result = add_paging_file(master_host_port,
				     master_device_port,
				     paging_file_name);
	    if (result == KERN_SUCCESS)
		break;
	    printf("Can't open paging file %s: %d\n",
		   paging_file_name,  result);

#if	PAGING_MEMORY
	    {
	        struct file     	pfile;
	      
		result = open_file(master_device_port,
				   "/dev/boot_device",
				   &pfile);

		/* 
		 * If no device is reachable, use memory
		 */
		if (result != KERN_SUCCESS) {
	    	    printf("Can't open device %s: %d\n",
			   "/dev/boot_device", result);
		    if (norma_mk) {
			result = add_paging_mem(master_host_port);
			if (result == KERN_SUCCESS)
			    break;
			printf("Can't add paging memory: %d\n", result);
		    }
	        }  
	    }
#endif	/* PAGING_MEMORY */

	    bzero(paging_file_name, sizeof(paging_file_name));
	    printf("Paging file name ? ");
	    safe_gets(paging_file_name, sizeof(paging_file_name));

	    if (paging_file_name[0] == 0) {
		printf("*** WARNING: running without paging area!\n");
		return FALSE;
	    }
	}

	/*
	 * Our caller will become the default pager - later
	 */

	return TRUE;
}


/*
 *	Use 16 Kbyte stacks instead of the default 64K.
 *	Use 4 Kbyte waiting stacks instead of the default 8K.
 */
vm_size_t	cthread_stack_size = 16 * 1024;

extern vm_size_t cthread_wait_stack_size;

int
main(int argc, char **argv)
{
	register kern_return_t	kr;
	task_port_t		my_task = mach_task_self();
	mach_port_t		bootstrap_port;
	mach_port_t		master_device_port;
	mach_port_t		master_host_port;
	int			my_node;	

	/*
	 * Use 4Kbyte cthread wait stacks.
	 */
	cthread_wait_stack_size = 4 * 1024;

	/*
	 * Get our bootstrap port
	 */
	kr = task_get_special_port(my_task,
				   TASK_BOOTSTRAP_PORT,
				   &bootstrap_port);
	if (kr != KERN_SUCCESS) {
	    panic("(default_pager): task_get_special_port: %d\n", kr);
	}

	kr = bootstrap_privileged_ports(bootstrap_port,
					&master_host_port,
					&master_device_port);
	if (kr != KERN_SUCCESS) {
	    panic("(default_pager): bootstrap_privileged_ports: %d\n", kr);
	}

	printf_init(master_device_port);
	panic_init(master_host_port);

	printf("(default_pager): started");

	/*
	 * Does the default pager run on a NORMA kernel ?
	 */
	if (norma_node_self(master_host_port, &my_node) == KERN_SUCCESS) {
		norma_mk = 1;
		printf(" on a NORMA kernel");
	}
	printf("\n");

	/*
	 * Set up the default pager.
	 */
	partition_init();

	default_pager_setup(master_host_port, master_device_port);

	default_pager_initialize(master_host_port);

	{
	    /*
	     * Delete the old stack (containing only the arguments).
	     */
	    vm_offset_t			addr = (vm_offset_t) argv;
	    vm_offset_t			r_addr;
	    vm_size_t			r_size;
	    unsigned int		r_count;
	    memory_object_name_t 	r_object_name;
	    vm_region_basic_info_data_t	r_info;

	    r_addr = addr;
	    r_count = VM_REGION_BASIC_INFO_COUNT;

	    kr = vm_region(my_task,
			&r_addr,
			&r_size,
		  	VM_REGION_BASIC_INFO,
			(vm_region_info_t) &r_info,
			&r_count,
			&r_object_name);
	    if ((kr == KERN_SUCCESS) && MACH_PORT_VALID(r_object_name))
		(void) mach_port_deallocate(my_task, r_object_name);
	    if ((kr == KERN_SUCCESS) &&
		(r_addr <= addr) &&
		((r_info.protection & (VM_PROT_READ|VM_PROT_WRITE)) ==
					(VM_PROT_READ|VM_PROT_WRITE)))
		(void) vm_deallocate(my_task, r_addr, r_size);
	}

	/*
	 * Become the default pager
	 */
	for (;;) {
	    default_pager();
	    /*NOTREACHED*/
	}
}
