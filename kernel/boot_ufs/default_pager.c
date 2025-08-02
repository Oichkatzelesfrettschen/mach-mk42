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
 * $Log:	default_pager.c,v $
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
/*
 * Default pager.  Pages to paging partition.
 *
 * MUST BE ABLE TO ALLOCATE WIRED-DOWN MEMORY!!!
 */

#include <mach/std_types.h>

#include <kern/queue.h>
#include <kern/zalloc.h>
#include <kern/kalloc.h>

#include <kern/task.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>

#include <mach/memory_object.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <mach/vm_inherit.h>
#include <vm/vm_kern.h>

#include <mach/message.h>

#include <device/device_types.h>
#include <device/device.h>

#include <boot_ufs/boot_printf.h>

/*#define	CHECKSUM	1*/

boolean_t	default_pager_debug = FALSE;
/*

 */
/*
 * 'Partition' structure for each paging area.
 * Controls allocation of blocks within paging area.
 */
struct part {
	vm_size_t	total_size;	/* total number of blocks */
	vm_size_t	free;		/* number of blocks free */
	unsigned char	*bitmap;	/* allocation map */
	int		(*p_read)();	/* Read block from partition */
	int		(*p_write)();	/* Write block to partition */
	char		*p_private;	/* Pointer to private data for
					   read/write routines. */
};
typedef	struct part	*partition_t;

/*
 * Bitmap allocation.
 */
#define	NBBY		8
#define	BYTEMASK	0xff

#define	howmany(a,b)	(((a) + (b) - 1)/(b))

/*
 * Value to indicate no block assigned
 */
#define	NO_BLOCK	((vm_offset_t)-1)

/*
 * Create a partition descriptor.
 * size is in BYTES.
 */
partition_t
part_create(size, p_read, p_write, p_private)
	vm_offset_t	size;		/* size in pages */
	int		(*p_read)();	/* read routine */
	int		(*p_write)();	/* write routine */
	char *		p_private;	/* structure used by read/write */
{
	register partition_t	part;

	size = atop(size);

	part = (partition_t) kalloc(sizeof(struct part));
	part->total_size = size;
	part->free	= size;
	part->bitmap	= (unsigned char *)kalloc(howmany(size, NBBY));
	part->p_read	= p_read;
	part->p_write	= p_write;
	part->p_private	= p_private;

	bzero((char *)part->bitmap, howmany(size, NBBY));

	return (part);
}

/*
 * Allocate a page in a paging file
 */
vm_offset_t
pager_alloc_page(part)
	register partition_t	part;
{
	register int	byte;
	register int	bit;
	register int	limit;

	if (part->free == 0)
		return (NO_BLOCK);	/* out of paging space */

	limit = howmany(part->total_size, NBBY);
	for (byte = 0; byte < limit; byte++)
	    if (part->bitmap[byte] != BYTEMASK)
		break;

	if (byte == limit)
	    panic("pager_alloc_page: free != 0 but no free blocks");

	for (bit = 0; bit < NBBY; bit++)
	    if ((part->bitmap[byte] & (1<<bit)) == 0)
		break;
	if (bit == NBBY)
	    panic("pager_alloc_page: no bits!");

	part->bitmap[byte] |= (1<<bit);
	if (part->free-- == 0)
	    printf("Default pager is full\n");

	return (byte*NBBY+bit);
}

/*
 * Deallocate a page in a paging file
 */
void
pager_dealloc_page(part, page)
	register partition_t	part;
	register vm_offset_t	page;
{
	register int	bit, byte;

	if (page >= part->total_size)
	    panic("dealloc_page");

	byte = page / NBBY;
	bit  = page % NBBY;
	part->bitmap[byte] &= ~(1<<bit);
	part->free++;
}

void
no_paging_space()
{
	panic("*** PAGING SPACE EXHAUSTED\n");
}

/*

 */
/*
 * Allocation info for each paging object.
 */
struct dpager {
	partition_t	partition;	/* allocation area for pager */
	vm_offset_t	*map;		/* block map */
	vm_size_t	size;		/* size of paging object, in pages */
#ifdef	CHECKSUM
	vm_offset_t	*checksum;	/* checksum - parallel to block map */
#define	NO_CHECKSUM	((vm_offset_t)-1)
#endif	CHECKSUM
};
typedef struct dpager	*dpager_t;

/*
 * A paging object uses either a one- or a two-level map of offsets
 * into the associated paging partition.
 */
#define	PAGEMAP_ENTRIES		128
				/* number of pages in a second-level map */
#define	PAGEMAP_SIZE(npgs)	((npgs)*sizeof(vm_offset_t))

#define	INDIRECT_PAGEMAP_ENTRIES(npgs) \
		((((npgs)-1)/PAGEMAP_ENTRIES) + 1)
#define	INDIRECT_PAGEMAP_SIZE(npgs) \
		(INDIRECT_PAGEMAP_ENTRIES(npgs) * sizeof(vm_offset_t *))
#define	INDIRECT_PAGEMAP(size)	\
		(size > PAGEMAP_ENTRIES)

/*
 * Attach a new paging object to a paging partition
 */
void
pager_alloc(pager, part, size)
	register dpager_t	pager;
	partition_t		part;
	register vm_size_t	size;
{
	register int		i;
	register vm_offset_t *	mapptr;

	pager->partition = part;

	/*
	 * Convert byte size to number of pages,
	 * then increase to the nearest power of 2.
	 */
	size = atop(size);
	i = 1;
	while (i < size)
	    i <<= 1;
	size = i;

	if (INDIRECT_PAGEMAP(size)) {
	    mapptr = (vm_offset_t *)
				kalloc(INDIRECT_PAGEMAP_SIZE(size));
	    for (i = INDIRECT_PAGEMAP_ENTRIES(size); --i >= 0; )
		mapptr[i] = 0;
	}
	else {
	    mapptr = (vm_offset_t *)kalloc(PAGEMAP_SIZE(size));
	    for (i = 0; i < size; i++)
		mapptr[i] = NO_BLOCK;
	}
	pager->map = mapptr;
	pager->size = size;
#ifdef	CHECKSUM
	if (INDIRECT_PAGEMAP(size)) {
	    mapptr = (vm_offset_t *)
				kalloc(INDIRECT_PAGEMAP_SIZE(size));
	    for (i = INDIRECT_PAGEMAP_ENTRIES(size); --i >= 0; )
		mapptr[i] = 0;
	}
	else {
	    mapptr = (vm_offset_t *)kalloc(PAGEMAP_SIZE(size));
	    for (i = 0; i < size; i++)
		mapptr[i] = NO_CHECKSUM;
	}
	pager->checksum = mapptr;
#endif	CHECKSUM
}

/*
 * Extend the map for a paging object.
 */
void
pager_extend(pager, new_size)
	register dpager_t	pager;
	register vm_size_t	new_size;	/* in pages */
{
	register vm_offset_t *	new_mapptr;
	register vm_offset_t *	old_mapptr;
	register int		i;
	register vm_size_t	old_size;

	/*
	 * Double current size until we cover new size.
	 */
	old_size = pager->size;

	i = old_size;
	while (i < new_size)
	    i <<= 1;
	new_size = i;

	if (INDIRECT_PAGEMAP(old_size)) {
	    /*
	     * Pager already uses two levels.  Allocate
	     * a larger indirect block.
	     */
	    new_mapptr = (vm_offset_t *)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    old_mapptr = pager->map;
	    for (i = 0; i < INDIRECT_PAGEMAP_ENTRIES(old_size); i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i] = 0;
	    kfree((char *)old_mapptr, INDIRECT_PAGEMAP_SIZE(old_size));
	    pager->map = new_mapptr;
	    pager->size = new_size;
#ifdef	CHECKSUM
	    new_mapptr = (vm_offset_t *)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    old_mapptr = pager->checksum;
	    for (i = 0; i < INDIRECT_PAGEMAP_ENTRIES(old_size); i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i] = 0;
	    kfree((char *)old_mapptr, INDIRECT_PAGEMAP_SIZE(old_size));
	    pager->checksum = new_mapptr;
#endif	CHECKSUM
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
	    new_mapptr = (vm_offset_t *)kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
	    old_mapptr = pager->map;
	    for (i = 0; i < old_size; i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < PAGEMAP_ENTRIES; i++)
		new_mapptr[i] = NO_BLOCK;
	    kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	    old_mapptr = new_mapptr;

	    /*
	     * Now allocate indirect map.
	     */
	    new_mapptr = (vm_offset_t *)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    new_mapptr[0] = (vm_offset_t) old_mapptr;
	    for (i = 1; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i] = 0;
	    pager->map = new_mapptr;
	    pager->size = new_size;
#ifdef	CHECKSUM
	    /*
	     * Allocate new second-level map first.
	     */
	    new_mapptr = (vm_offset_t *)kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
	    old_mapptr = pager->checksum;
	    for (i = 0; i < old_size; i++)
		new_mapptr[i] = old_mapptr[i];
	    for (; i < PAGEMAP_ENTRIES; i++)
		new_mapptr[i] = NO_CHECKSUM;
	    kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	    old_mapptr = new_mapptr;

	    /*
	     * Now allocate indirect map.
	     */
	    new_mapptr = (vm_offset_t *)
			kalloc(INDIRECT_PAGEMAP_SIZE(new_size));
	    new_mapptr[0] = (vm_offset_t) old_mapptr;
	    for (i = 1; i < INDIRECT_PAGEMAP_ENTRIES(new_size); i++)
		new_mapptr[i] = 0;
	    pager->checksum = new_mapptr;
#endif	CHECKSUM
	    return;
	}
	/*
	 * Enlarging a direct block.
	 */
	new_mapptr = (vm_offset_t *)
		kalloc(PAGEMAP_SIZE(new_size));
	old_mapptr = pager->map;
	for (i = 0; i < old_size; i++)
	    new_mapptr[i] = old_mapptr[i];
	for (; i < new_size; i++)
	    new_mapptr[i] = NO_BLOCK;
	kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	pager->map = new_mapptr;
	pager->size = new_size;
#ifdef	CHECKSUM
	new_mapptr = (vm_offset_t *)
		kalloc(PAGEMAP_SIZE(new_size));
	old_mapptr = pager->checksum;
	for (i = 0; i < old_size; i++)
	    new_mapptr[i] = old_mapptr[i];
	for (; i < new_size; i++)
	    new_mapptr[i] = NO_CHECKSUM;
	kfree((char *)old_mapptr, PAGEMAP_SIZE(old_size));
	pager->checksum = new_mapptr;
#endif	CHECKSUM
}

/*
 * Given an offset within a paging object, find the
 * corresponding block within the paging partition.
 * Return NO_BLOCK if none allocated.
 */
vm_offset_t
pager_read_offset(pager, offset)
	register dpager_t	pager;
	vm_offset_t		offset;
{
	register vm_offset_t	f_page;

	f_page = atop(offset);

	if (f_page >= pager->size) {
	    printf("*** PAGER WARNING: offset 0x%x out of range [0..0x%x)\n",
			f_page, pager->size);
	    return (NO_BLOCK);
	}

	if (INDIRECT_PAGEMAP(pager->size)) {
	    register vm_offset_t *mapptr;

	    mapptr = (vm_offset_t *)pager->map[f_page/PAGEMAP_ENTRIES];
	    if (mapptr == 0)
		return (NO_BLOCK);

	    return (mapptr[f_page%PAGEMAP_ENTRIES]);
	}
	else {
	    return (pager->map[f_page]);
	}
}

#ifdef	CHECKSUM
/*
 * Return the checksum for a block.
 */
int
pager_get_checksum(pager, offset)
	register dpager_t	pager;
	vm_offset_t		offset;
{
	register vm_offset_t	f_page;

	f_page = atop(offset);

	if (f_page >= pager->size) {
	    printf("*** PAGER WARNING: offset 0x%x out of range [0..0x%x)\n",
			f_page, pager->size);
	    return (NO_CHECKSUM);
	}

	if (INDIRECT_PAGEMAP(pager->size)) {
	    register vm_offset_t *mapptr;

	    mapptr = (vm_offset_t *)pager->checksum[f_page/PAGEMAP_ENTRIES];
	    if (mapptr == 0)
		return (NO_CHECKSUM);

	    return (mapptr[f_page%PAGEMAP_ENTRIES]);
	}
	else {
	    return (pager->checksum[f_page]);
	}
}

/*
 * Remember the checksum for a block.
 */
int
pager_put_checksum(pager, offset, checksum)
	register dpager_t	pager;
	vm_offset_t		offset;
	int			checksum;
{
	register vm_offset_t	f_page;

	f_page = atop(offset);

	if (f_page >= pager->size) {
	    printf("*** PAGER WARNING: offset 0x%x out of range [0..0x%x)\n",
			f_page, pager->size);
	    panic("pager_put_checksum");
	}

	if (INDIRECT_PAGEMAP(pager->size)) {
	    register vm_offset_t *mapptr;

	    mapptr = (vm_offset_t *)pager->checksum[f_page/PAGEMAP_ENTRIES];
	    if (mapptr == 0)
		panic("pager_put_checksum");

	    mapptr[f_page%PAGEMAP_ENTRIES] = checksum;
	}
	else {
	    pager->checksum[f_page] = checksum;
	}
}

/*
 * Compute a checksum - XOR each 32-bit word.
 */
int
compute_checksum(addr, size)
	vm_offset_t	addr;
	vm_size_t	size;
{
	register int	checksum = NO_CHECKSUM;
	register int	*ptr;
	register int	count;

	ptr = (int *)addr;
	count = size / sizeof(int);

	while (--count >= 0)
	    checksum ^= *ptr++;

	return (checksum);
}
#endif	CHECKSUM

/*
 * Given an offset within a paging object, find the
 * corresponding block within the paging partition.
 * Allocate a new block if necessary.
 *
 * WARNING: paging objects apparently may be extended
 * without notice!
 */
vm_offset_t
pager_write_offset(pager, offset)
	register dpager_t	pager;
	vm_offset_t		offset;
{
	register vm_offset_t	block, f_page;
	register vm_offset_t	*mapptr;

	f_page = atop(offset);

	if (f_page >= pager->size) {
	    /*
	     * Paging object must be extended.
	     * Remember that offset is 0-based, but size is 1-based.
	     */
	    pager_extend(pager, f_page + 1);
	}

	if (INDIRECT_PAGEMAP(pager->size)) {

	    mapptr = (vm_offset_t *)pager->map[f_page/PAGEMAP_ENTRIES];
	    if (mapptr == 0) {
		/*
		 * Allocate the indirect block
		 */
		register int i;

		mapptr = (vm_offset_t *)kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		if (mapptr == 0) {
		    /* out of space! */
		    printf("*** PAGER ERROR: out of memory\n");
		    no_paging_space();
		    return (NO_BLOCK);
		}
		pager->map[f_page/PAGEMAP_ENTRIES] = (vm_offset_t)mapptr;
		for (i = 0; i < PAGEMAP_ENTRIES; i++)
		    mapptr[i] = NO_BLOCK;
#ifdef	CHECKSUM
		{
		    register vm_offset_t *cksumptr;
		    register int j;

		    cksumptr = (vm_offset_t *)
				kalloc(PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		    if (cksumptr == 0) {
			/* out of space! */
			printf("*** PAGER ERROR: out of memory\n");
			no_paging_space();
			return (FALSE);
		    }
		    pager->checksum[f_page/PAGEMAP_ENTRIES]
			= (vm_offset_t)cksumptr;
		    for (j = 0; j < PAGEMAP_ENTRIES; j++)
			cksumptr[j] = NO_CHECKSUM;
		}
#endif	CHECKSUM
	    }
	    f_page %= PAGEMAP_ENTRIES;
	}
	else {
	    mapptr = pager->map;
	}

	block = mapptr[f_page];
	if (block == NO_BLOCK) {
	    block = pager_alloc_page(pager->partition);
	    if (block == NO_BLOCK) {
		no_paging_space();
		return (NO_BLOCK);	/* out of space */
	    }
	    mapptr[f_page] = block;
	}

	return (block);
}

/*
 * Deallocate all of the blocks belonging to a paging object.
 */
void
pager_dealloc(pager)
	register dpager_t	pager;
{
	register int i, j;
	register vm_offset_t	block;
	register vm_offset_t	*mapptr;

	if (INDIRECT_PAGEMAP(pager->size)) {
	    for (i = INDIRECT_PAGEMAP_ENTRIES(pager->size); --i >= 0; ) {
		mapptr = (vm_offset_t *)pager->map[i];
		if (mapptr) {
		    for (j = 0; j < PAGEMAP_ENTRIES; j++) {
			block = mapptr[j];
			if (block != NO_BLOCK)
			    pager_dealloc_page(pager->partition, block);
		    }
		    kfree((char *)mapptr, PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		}
	    }
	    kfree((char *)pager->map, INDIRECT_PAGEMAP_SIZE(pager->size));
#ifdef	CHECKSUM
	    for (i = INDIRECT_PAGEMAP_ENTRIES(pager->size); --i >= 0; ) {
		mapptr = (vm_offset_t *)pager->checksum[i];
		if (mapptr) {
		    kfree((char *)mapptr, PAGEMAP_SIZE(PAGEMAP_ENTRIES));
		}
	    }
	    kfree((char *)pager->checksum,
		  INDIRECT_PAGEMAP_SIZE(pager->size));
#endif	CHECKSUM
	}
	else {
	    mapptr = pager->map;
	    for (i = 0; i < pager->size; i++ ) {
		block = mapptr[i];
		if (block != NO_BLOCK)
		    pager_dealloc_page(pager->partition, block);
	    }
	    kfree((char *)pager->map, PAGEMAP_SIZE(pager->size));
#ifdef	CHECKSUM
	    kfree((char *)pager->checksum, PAGEMAP_SIZE(pager->size));
#endif	CHECKSUM
	}
}
/*

 */

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
int
default_read(ds, addr, size, offset, out_addr)
	register dpager_t	ds;
	vm_offset_t		addr;	/* pointer to block to fill */
	register vm_size_t	size;
	register vm_offset_t	offset;
	vm_offset_t		*out_addr;
				/* returns pointer to data */
{
	register vm_offset_t	block;
	vm_offset_t	raddr;
	vm_size_t	rsize;
	register int	rc;
	boolean_t	first_time;
	register partition_t	part = ds->partition;
#ifdef	CHECKSUM
	vm_size_t	original_size = size;
	vm_offset_t	original_offset = offset;
#endif	CHECKSUM
	/*
	 * Find the block in the paging partition
	 */
	block = pager_read_offset(ds, offset);
	if (block == NO_BLOCK)
	    return (PAGER_ABSENT);

	/*
	 * Read it, trying for the entire page.
	 */
	offset = ptoa(block);
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
	    if (default_pager_debug)
		printf("    checksum 0x%x ok\n", read_checksum);
	}
#endif	CHECKSUM
	return (PAGER_SUCCESS);
}

int
default_write(ds, addr, size, offset)
	register dpager_t	ds;
	register vm_offset_t	addr;
	register vm_size_t	size;
	register vm_offset_t	offset;
{
	vm_offset_t	block;
	vm_size_t	wsize;
	register int	rc;

	/*
	 * Find block in paging partition
	 */
	block = pager_write_offset(ds, offset);
	if (block == NO_BLOCK)
	    return (PAGER_ERROR);

#ifdef	CHECKSUM
	/*
	 * Save checksum
	 */
	{
	    int	checksum;

	    checksum = compute_checksum(addr, size);
	    pager_put_checksum(ds, offset, checksum);
	    if (default_pager_debug)
		printf("    saving checksum 0x%x\n", checksum);
	}
#endif	CHECKSUM
	offset = ptoa(block);

	do {
	    rc = (*ds->partition->p_write)(
		ds->partition->p_private,
		offset,
		addr,
		size,
		&wsize);
	    if (rc != 0) {
		printf("*** PAGER ERROR: default_write: ");
		printf("ds=0x%x addr=0x%x size=0x%x offset=0x%x resid=0x%x\n",
			ds, addr, size, offset, wsize);
		return (PAGER_ERROR);
	    }
	    addr += wsize;
	    offset += wsize;
	    size -= wsize;
	} while (size != 0);
	return (PAGER_SUCCESS);
}

boolean_t
default_has_page(ds, offset)
	dpager_t	ds;
	vm_offset_t	offset;
{
	return (pager_read_offset(ds, offset) != NO_BLOCK);
}

/*

 */

/*
 * Mapping between pager port and paging object.
 */
struct dstruct {
	queue_chain_t	links;		/* Link in pager-port hash list */
	memory_object_t	pager;		/* Pager port */
	mach_port_t	pager_request;	/* Known request port */
	mach_port_t	pager_name;	/* Known name port */
	int		errors;		/* Pageout error count */
	struct dpager	dpager;		/* Actual pager */
};
typedef struct dstruct *	default_pager_t;
#define	DEFAULT_PAGER_NULL	((default_pager_t)0)

zone_t	dstruct_zone;

#define	PAGER_PORT_HASH_COUNT	127

queue_head_t	pager_port_hashtable[PAGER_PORT_HASH_COUNT];

#define	pager_port_hash(name_port) \
		(((int)(name_port) & 0xffffff) % PAGER_PORT_HASH_COUNT)

void pager_port_hash_init()
{
	register int	i;

	for (i = 0; i < PAGER_PORT_HASH_COUNT; i++)
	    queue_init(&pager_port_hashtable[i]);
}

void pager_port_hash_insert(name_port, ds)
	mach_port_t	name_port;
	default_pager_t	ds;
{
	ds->pager = name_port;
	queue_enter(&pager_port_hashtable[pager_port_hash(name_port)],
			ds, default_pager_t, links);
}

default_pager_t pager_port_lookup(name_port)
	register mach_port_t	name_port;
{
	register queue_t	bucket;
	register default_pager_t entry;

	bucket = &pager_port_hashtable[pager_port_hash(name_port)];

	for (entry = (default_pager_t)queue_first(bucket);
	     !queue_end(bucket, (queue_entry_t)entry);
	     entry = (default_pager_t)queue_next(&entry->links)) {
		if (entry->pager == name_port)
		    return (entry);
	}
	return (DEFAULT_PAGER_NULL);
}

void pager_port_hash_delete(name_port)
	register mach_port_t	name_port;
{
	register queue_t	bucket;
	register default_pager_t entry;

	bucket = &pager_port_hashtable[pager_port_hash(name_port)];

	for (entry = (default_pager_t)queue_first(bucket);
	     !queue_end(bucket, (queue_entry_t)entry);
	     entry = (default_pager_t)queue_next(&entry->links)) {
		if (entry->pager == name_port) {
		    queue_remove(bucket, entry, default_pager_t, links);
		    return;
		}
	}
}

/*
 *	Default pager.
 *	Wired into kernel.
 */
partition_t	default_partition = 0;	/* Where to allocate new pagers */
decl_simple_lock_data(,
		default_partition_lock);
					/* lock to set it */

mach_port_t		default_pager_self;
mach_port_t		default_pager_enabled_set;
mach_port_t		default_pager_default;

vm_offset_t		default_pager_input_buffer;

/*
 *	Make all calls involving the kernel interface go through IPC.
 */

#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>

/*
 *	Rename all of the functions in the pager interface,
 *	to avoid confusing them with the kernel interface
 */

#define	memory_object_init		default_pager_init_pager
#define	memory_object_terminate		default_pager_terminate_object
#define	memory_object_data_request	default_pagein
#define	memory_object_data_unlock	default_pagein
#define	memory_object_data_write	default_data_write
#define	memory_object_data_initialize	default_data_initialize
#define	memory_object_create		default_pager_create_pager
#define	memory_object_copy		default_pager_copy
#define	memory_object_lock_completed	default_pager_lock_completed


int		default_pager_pagein_count = 0;
int		default_pager_pageout_count = 0;

kern_return_t memory_object_data_request(pager, reply_to, offset,
					 length, protection_required)
	memory_object_t	pager;
	mach_port_t	reply_to;
	vm_offset_t	offset;
	vm_size_t	length;
	vm_prot_t	protection_required;
{
	default_pager_t	ds;
	vm_offset_t addr;
	kern_return_t rc;

	if (default_pager_debug)
	    printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
		   "memory_object_data_request(default_pager)",
		   pager, offset, length);

	if (length != PAGE_SIZE)
	    panic("default_pagein: bad length");

	ds = pager_port_lookup(pager);
	assert(ds != DEFAULT_PAGER_NULL);

	if (ds->errors) {
	    printf("(default_pager)data_request: dropping request because of");
	    printf(" previous paging errors\n");
	    (void) memory_object_data_error(reply_to,
				offset, PAGE_SIZE,
				KERN_FAILURE);
	    return;
	}

	rc = default_read(&ds->dpager, default_pager_input_buffer,
			  PAGE_SIZE, offset,
			  &addr);
	switch (rc) {
	    case PAGER_SUCCESS:
		(void) memory_object_data_provided(
			reply_to, offset,
			addr, PAGE_SIZE,
			VM_PROT_NONE);
		if (addr != default_pager_input_buffer)
		    (void) vm_deallocate(default_pager_self, addr, PAGE_SIZE);
		break;

	    case PAGER_ABSENT:
		(void) memory_object_data_unavailable(
			reply_to,
			offset,
			PAGE_SIZE);
		break;

	    case PAGER_ERROR:
		(void) memory_object_data_error(
			reply_to,
			offset,
			PAGE_SIZE,
			KERN_FAILURE);
		break;
	}

	default_pager_pagein_count++;

	return(KERN_SUCCESS);
}

/*
 * memory_object_data_initialize: check whether we already have each page, and
 * write it if we do not.  The implementation is far from optimized, and
 * also assumes that the default_pager is single-threaded.
 */
kern_return_t memory_object_data_initialize(pager, pager_request,
					    offset, addr, data_cnt)
	memory_object_t	pager;
	mach_port_t	pager_request;
	register
	vm_offset_t	offset;
	register
	pointer_t	addr;
	vm_size_t	data_cnt;
{
	vm_offset_t	amount_sent;
	default_pager_t	ds;

#ifdef	lint
	pager_request++;
#endif	lint

	if (default_pager_debug)
		printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			"memory_object_data_initialize(default_pager)",
			pager, offset, data_cnt);

	ds = pager_port_lookup(pager);

	for (amount_sent = 0;
	     amount_sent < data_cnt;
	     amount_sent += PAGE_SIZE) {

	     if (!default_has_page(&ds->dpager, offset + amount_sent)) {
		if (default_write(&ds->dpager,
				  addr + amount_sent,
				  PAGE_SIZE,
				  offset + amount_sent)
			 != PAGER_SUCCESS) {
		    printf("initialize: write error\n");
		    ds->errors++;
		}
	     }
	}
	if (vm_deallocate(default_pager_self, addr, data_cnt) != KERN_SUCCESS)
	    panic("default_initialize: deallocate failed");

	return(KERN_SUCCESS);
}

/*
 * memory_object_data_write: split up the stuff coming in from
 * a memory_object_data_write call
 * into individual pages and pass them off to default_write.
 */
kern_return_t memory_object_data_write(pager, pager_request,
				       offset, addr, data_cnt)
	memory_object_t	pager;
	mach_port_t	pager_request;
	register
	vm_offset_t	offset;
	register
	pointer_t	addr;
	vm_size_t	data_cnt;
{
	register
	vm_size_t	amount_sent;
	default_pager_t	ds;

#ifdef	lint
	pager_request++;
#endif	lint

	if (default_pager_debug)
		printf("%s: pager=%d, offset=0x%x, length=0x%x\n",
			"pager_data_write(default_pager)",
			pager, offset, data_cnt);

	if ((data_cnt % PAGE_SIZE) != 0) {
		printf("(default_pager)memory_object_data_write: not a multiple of a page");
		data_cnt = round_page(data_cnt);
	}

	ds = pager_port_lookup(pager);

	for (amount_sent = 0;
	     amount_sent < data_cnt;
	     amount_sent += PAGE_SIZE) {

	    register int result;

	    result = default_write(&ds->dpager,
			      addr + amount_sent,
			      PAGE_SIZE,
			      offset + amount_sent);
	    if (result != KERN_SUCCESS) {
		printf("*** WRITE ERROR on default_pageout:");
		printf(" pager=%d, offset=0x%x, length=0x%x, result=%d\n",
			pager, offset+amount_sent, PAGE_SIZE, result);
		ds->errors++;
	    }
	    default_pager_pageout_count++;
	}

	if (vm_deallocate(default_pager_self, addr, data_cnt) != KERN_SUCCESS)
		panic("default_data_write: deallocate failed");

	return(KERN_SUCCESS);
}

/*ARGSUSED*/
kern_return_t memory_object_copy(old_memory_object, old_memory_control,
				 offset, length, new_memory_object)
	memory_object_t	old_memory_object;
	memory_object_control_t
			old_memory_control;
	vm_offset_t	offset;
	vm_size_t	length;
	memory_object_t	new_memory_object;
{
	panic("(default_pager)memory_object_copy: called");
	return KERN_FAILURE;
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
kern_return_t memory_object_create(old_pager, new_pager, new_size,
				   new_pager_request, new_pager_name,
				   new_page_size)
	mach_port_t	old_pager;
	mach_port_t	new_pager;
	vm_size_t	new_size;
	mach_port_t	new_pager_request;
	mach_port_t	new_pager_name;
	vm_size_t	new_page_size;
{
	register default_pager_t	ds;

	if (default_pager_debug)
		printf("%s: new_pager=%d, new_request=%d, new_name=%d\n",
			"memory_object_data_create(default_pager)",
			new_pager, new_pager_request, new_pager_name);

	if (old_pager != default_pager_default) {
	    printf("memory_object_create(default_pager): non-kernel caller!\n");
	    /*
	     * XXX Should throw away spurious port rights -- 
	     * use port_status to avoid giving away important ports
	     */
	    return(KERN_FAILURE);
	}

	if (new_page_size != PAGE_SIZE) {
	    printf("memory_object_create(default_pager): wrong page size\n");
	    return(KERN_INVALID_ARGUMENT);
	}

	ds = (default_pager_t) zalloc(dstruct_zone);
	if (ds == DEFAULT_PAGER_NULL) {
	    printf("memory_object_create(default_pager): unable to allocate");
	    printf(" a default pager structure [REBOOT SUGGESTED]\n");
	    return(KERN_RESOURCE_SHORTAGE);
	}

	pager_alloc(&ds->dpager, default_partition, new_size);

	/*
	 *	Set up associations between these ports
	 *	and this default_pager structure
	 */

	ds->pager = new_pager;
	ds->pager_request = new_pager_request;
	ds->pager_name = new_pager_name;
	pager_port_hash_insert(new_pager, ds);

	if (mach_port_move_member(default_pager_self,
				  new_pager,
				  default_pager_enabled_set) != KERN_SUCCESS)
	    panic("memory_object_create: couldn't enable");

	return(KERN_SUCCESS);
}

memory_object_copy_strategy_t default_copy_strategy = MEMORY_OBJECT_COPY_DELAY;

kern_return_t memory_object_init(pager, pager_request, pager_name,
				 pager_page_size)
	mach_port_t	pager;
	mach_port_t	pager_request;
	mach_port_t	pager_name;
	vm_size_t	pager_page_size;
{
	register default_pager_t	ds;

	if (default_pager_debug)
		printf("%s: pager=%d, request=%d, name=%d\n",
			"memory_object_data_init(default_pager)",
			pager, pager_request, pager_name);

	if (pager_page_size != PAGE_SIZE) {
	    printf("memory_object_init: wrong page size");
	    return(KERN_FAILURE);
	}

	if ((ds = pager_port_lookup(pager)) == DEFAULT_PAGER_NULL) {
	    printf("memory_object_init: bogus pager");
	    return(KERN_FAILURE);
	}

	if (ds->pager_request != MACH_PORT_NULL &&
	    ds->pager_request != pager_request) {
		printf("memory_object_init: multiple users");
		return(KERN_FAILURE);
	}

	ds->pager_request = pager_request;
	ds->pager_name = pager_name;

	memory_object_set_attributes(pager_request,
				     TRUE,
				     FALSE,	/* do not cache */
				     default_copy_strategy);

	return(KERN_SUCCESS);
}

kern_return_t memory_object_terminate(pager, pager_request, pager_name)
	mach_port_t	pager;
	mach_port_t	pager_request;
	mach_port_t	pager_name;
{
	default_pager_t	ds = pager_port_lookup(pager);

	if (default_pager_debug)
		printf("%s: pager=%d, request=%d, name=%d\n",
			"memory_object_terminate(default_pager)",
			pager, pager_request, pager_name);

	/*
	 *	Throw away the port rights, regardless whether this
	 *	request made any sense at all.  In order for the
	 *	message to be accepted, they must have been port_all_t's.
	 *	Therefore, they can't be any of the ports we already owned.
	 */

	if (pager_name != MACH_PORT_NULL)
		mach_port_destroy(default_pager_self, pager_name);
	if (pager_request != MACH_PORT_NULL)
		mach_port_destroy(default_pager_self, pager_request);

	if (ds == DEFAULT_PAGER_NULL)
		return (KERN_SUCCESS);

	/*
	 *	Remove the memory object port association, and then
	 *	the destroy the port itself.
	 */

	pager_port_hash_delete(ds->pager);
	(void) mach_port_destroy(default_pager_self, ds->pager);

	pager_dealloc(&ds->dpager);

	zfree(dstruct_zone, (vm_offset_t) ds);

	return (KERN_SUCCESS);
}

kern_return_t memory_object_lock_completed(memory_object, pager_request_port,
					   offset, length)
	memory_object_t	memory_object;
	mach_port_t	pager_request_port;
	vm_offset_t	offset;
	vm_size_t	length;
{
#ifdef	lint
	memory_object++; pager_request_port++; offset++; length++;
#endif	lint

	printf("memory_object_lock_completed(inode_pager): called\n");
	return(KERN_FAILURE);
}

/*
 *	Include the server loop
 */

#define	SERVER_LOOP		default_pager_server_loop
#define	SERVER_NAME		"default_pager"
#define	SERVER_DISPATCH(in,out)	\
		(default_pager_server(in, out) || \
		 default_pager_default_server(in, out) || \
		 default_pager_object_server(in, out))

#include <kern/server_loop.c>

#define	memory_object_server		default_pager_server
#include <mach/memory_object_server.c>
#define	memory_object_default_server	default_pager_default_server
#include <mach/memory_object_default_server.c>
#include <mach/default_pager_object_server.c>

int	dstruct_max	= 1000;		/* sb same as number of vm_objects */

void
default_pager(host_port)
	mach_port_t	host_port;
{
	memory_object_t		DMM;
	register thread_t	my_thread = current_thread();
	kern_return_t		kr;

	default_pager_self = mach_task_self();

	/*
	 *	Set thread privileges.
	 */
	thread_swappable(my_thread, FALSE);
	my_thread->vm_privilege = TRUE;

	/*
	 *	Initialize the name port hashing stuff.
	 */
	pager_port_hash_init();

	dstruct_zone = zinit(sizeof(struct dstruct),
			(vm_size_t) sizeof(struct dstruct) * dstruct_max,
			PAGE_SIZE,
			FALSE,
			"default pager structures");

	/*
	 *	We are the default pager.
	 *	Initialize the "default pager" port.
	 */

	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_RECEIVE,
				&default_pager_default);
	if (kr != KERN_SUCCESS)
		panic("default pager: can't allocate default port");

	kr = mach_port_allocate(default_pager_self, MACH_PORT_RIGHT_PORT_SET,
				&default_pager_enabled_set);
	if (kr != KERN_SUCCESS)
		panic("default_pager: can't create enabled port set");

	kr = mach_port_move_member(default_pager_self,
				   default_pager_default,
				   default_pager_enabled_set);
	if (kr != KERN_SUCCESS)
		panic("default_pager: can't enable default port");

	/*
	 * Wait for at least the default paging partition to be
	 * set up.
	 */
	simple_lock(&default_partition_lock);
	while (default_partition == (partition_t)0) {
	    thread_sleep((int)&default_partition,
			 simple_lock_addr(default_partition_lock),
			 FALSE);
	    simple_lock(&default_partition_lock);
	}
	simple_unlock(&default_partition_lock);

	DMM = default_pager_default;
	kr = vm_set_default_memory_manager(host_port, &DMM);
	if ((kr != KERN_SUCCESS) || (DMM != MACH_PORT_NULL))
		panic("default_pager: can't set default memory manager");

	/*
	 *	Allocate the input buffer.
	 */
	default_pager_input_buffer = kmem_alloc(kernel_map, PAGE_SIZE);
	if (default_pager_input_buffer == 0)
		panic("default pager: can't allocate input buffer");

	SERVER_LOOP(default_pager_enabled_set);
}

/*
 * Set up the default paging partition.
 */
void
create_default_partition(size, p_read, p_write, p_private)
	vm_size_t	size;
	int		(*p_read)();
	int		(*p_write)();
	char		*p_private;
{
	default_partition = part_create(size, p_read, p_write, p_private);
	simple_lock(&default_partition_lock);
	thread_wakeup((int)&default_partition);
	simple_unlock(&default_partition_lock);
}

kern_return_t default_pager_object_create(pager, mem_obj, type, size)
	mach_port_t pager;
	mach_port_t *mem_obj;
	mach_msg_type_name_t *type;
	vm_size_t size;
{
	mach_port_t port;
	kern_return_t result;

	result = mach_port_allocate(default_pager_self,
				    MACH_PORT_RIGHT_RECEIVE, &port);
	if (result != KERN_SUCCESS) return (result);

	result = memory_object_create(pager, port, size,
				      MACH_PORT_NULL, MACH_PORT_NULL,
				      PAGE_SIZE);
	if (result != KERN_SUCCESS) return (result);

	*mem_obj = port;
	*type = MACH_MSG_TYPE_MAKE_SEND;
	return (KERN_SUCCESS);
}
