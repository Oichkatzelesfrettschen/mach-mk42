/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log:	rz_disk.c,v $
 * Revision 2.6  91/02/14  14:35:22  mrt
 * 	Added (optional and disabled) code for checksumming.
 * 	[91/02/12  12:58:27  af]
 * 
 * 	Bug in strategy routine: for xfers larger than
 * 	max_dma was not setting the io_count right at
 * 	the end of the transfer.  Now new fsck works.
 * 	[90/12/10  17:26:47  af]
 * 
 * Revision 2.5  91/02/05  17:43:47  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:50  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:37  mrt]
 * 
 * Revision 2.4  90/12/20  17:01:09  jeffreyh
 * 	Bug in strategy routine: for xfers larger than
 * 	max_dma was not setting the io_count right at
 * 	the end of the transfer.  Now new fsck works.
 * 	[90/12/10  17:26:47  af]
 * 
 * Revision 2.3  90/12/05  23:33:57  af
 * 	Use and prefer BSD/OSF labels.
 * 	[90/12/03  23:33:43  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:47  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz_disk.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to disk-like devices.
 */

#include <mach/std_types.h>
#include <device/io_req.h>
#include <device/param.h>

#include <mips/PMAX/scsi.h>
#include <mips/PMAX/scsi_defs.h>
#include <mips/PMAX/rz_labels.h>
#include <mips/PMAX/rz.h>

/*
 * Label used if the disk does not have one
 */
scsi_bsd_label_t	scsi_default_label =
{
	BSD_PARTITION_MAGIC, LAB_DTYPE_SCSI, 0,
	"SCSI", "",
	DEV_BSIZE, 1, 1, 1, 1, 1, 0, 0, 0,
	3600, 1, 1, 1, 0, 0, 0,
	{0,}, {0,},
	BSD_PARTITION_MAGIC, 0,
	8, sizeof(scsi_dec_boot0_t), 8192,
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 },
	{ -1, 0, 1024, LAB_FS_BSDFFS, 8, 3 }
};

/*
 * Specialized side of the open routine for disks
 */
scdisk_open(sc, tgt)
	scsi_softc_t		*sc;
	target_info_t		*tgt;
{
	register int 	i;
	register scsi_ret_t	ret = SCSI_RET_SUCCESS;
	unsigned int		disk_size, secs_per_cyl, sector_size;
	scsi_rcap_data_t	*cap;
	scsi_bsd_label_t	*label;
	io_req_t		ior;
	char			buff[DEV_BSIZE];

	if (tgt->flags & TGT_ONLINE)
		return SCSI_RET_SUCCESS;

	/*
	 * Dummy ior for proper sync purposes
	 */
	ior = (io_req_t) kalloc(sizeof(*ior));
	bzero(ior, sizeof(*ior));

	/*
	 * Set the LBN to DEV_BSIZE with a MODE SELECT.
	 * xxx do a MODE SENSE instead ?
	 */
	for (i = 0; i < 5; i++) {
		ior->io_op = 0;
		ret = scdisk_mode_select(sc, tgt, DEV_BSIZE, ior);
		if (ret == SCSI_RET_SUCCESS)
			break;
	}
	if (ret != SCSI_RET_SUCCESS) {
		scsi_error( tgt, SCSI_ERR_MSEL, ret);
		ret = D_INVALID_SIZE;
		goto done;
	}

	/*
	 * Do a READ CAPACITY to get max size. Check LBN too.
	 */
	for (i = 0; i < 5; i++) {
		ior->io_op = 0;
		ret = scsi_read_capacity(sc, tgt, 0, ior);
		if (ret == SCSI_RET_SUCCESS)
			break;
	}
	if (ret == SCSI_RET_SUCCESS) {
		int			val;

		cap = (scsi_rcap_data_t*) tgt->cmd_ptr;
		disk_size = (cap->lba1<<24) |
			    (cap->lba2<<16) |
			    (cap->lba3<< 8) |
			     cap->lba4;
	if(scsi_debug)
	printf("rz%d holds %d blocks\n", tgt->unit_no, disk_size);
		val = (cap->blen1<<24) |
		      (cap->blen2<<16) |
		      (cap->blen3<<8 ) |
		       cap->blen4;
		if (val != DEV_BSIZE) panic("read capacity bad");
	} else {
		printf("Unknown disk capacity??\n");
		disk_size = -1;
	}

	/*
	 * Find out about the phys disk geometry
	 */

	ior->io_op = 0;
	ret = scsi_read_capacity( sc, tgt, 1, ior);
	/* scsi_error(...) */
	cap = (scsi_rcap_data_t*) tgt->cmd_ptr;
	secs_per_cyl =	(cap->lba1<<24) | (cap->lba2<<16) |
			(cap->lba3<< 8) |  cap->lba4;
	secs_per_cyl += 1;
	sector_size =	(cap->blen1<<24) | (cap->blen2<<16) |
			(cap->blen3<<8 ) |  cap->blen4;
	if (scsi_debug)
	printf("rz%d: %d sect/cyl %d bytes/sec\n", tgt->unit_no,
		secs_per_cyl, sector_size);

#ifdef	SCSI2
	/*
	 * ... and a bunch of other things for scsi2
	 */
#endif	SCSI2

	/*
	 * Get partition table off pack
	 */

	/* first look for a BSD label */	
	ior->io_data = buff;
	ior->io_count = DEV_BSIZE;
	ior->io_op = IO_READ;
	tgt->ior = ior;
	scdisk_read( sc, tgt, BSD_PARTITION_BYTE_OFFSET/DEV_BSIZE, ior);
	iowait(ior);
	label = (scsi_bsd_label_t*) &buff[BSD_PARTITION_BYTE_OFFSET % DEV_BSIZE];

	if (label->magic  == BSD_PARTITION_MAGIC &&
	    label->magic2 == BSD_PARTITION_MAGIC) {

		if (scsi_debug)
		printf("{Using OSF label}");
		tgt->dev_info.disk = *label;

	} else {
		/* then look for a DEC label */
		register scsi_dec_label_t	*part;

		ior->io_data = buff;
		ior->io_count = DEV_BSIZE;
		ior->io_op = IO_READ;
		tgt->ior = ior;
		scdisk_read( sc, tgt, DEC_PARTITION_BYTE_OFFSET/DEV_BSIZE, ior);
		iowait(ior);
		part = (scsi_dec_label_t*) &buff[DEC_PARTITION_BYTE_OFFSET % DEV_BSIZE];

		label = &tgt->dev_info.disk;
		*label = scsi_default_label;

		if (part->magic == DEC_PARTITION_MAGIC) {
			if (scsi_debug)
			printf("{Using DEC label}");
			for (i = 0; i < 8; i++) {
				label->partitions[i].size = part->partitions[i].n_sectors;
				label->partitions[i].offset = part->partitions[i].offset;
			}
		} else {
			printf("No valid disk label\n");
			/* Validate partitions a and c for initialization */
			tgt->dev_info.disk.partitions[0].size = disk_size;
			tgt->dev_info.disk.partitions[2].size = disk_size;
		}
		label->npartitions = 8;
		label->secsize = sector_size;
		label->secpercyl = secs_per_cyl;
		label->secperunit = disk_size;
	}

	ret = SCSI_RET_SUCCESS;
done:
	kfree(ior, sizeof(*ior));
	return ret;
}

/*
 * Disk strategy routine
 */
scdisk_strategy(ior)
	register io_req_t	ior;
{
	target_info_t  *tgt;
	register scsi_softc_t	*sc;
	scsi_ret_t      ret;
	register int    i = ior->io_unit, part;
	register unsigned rec, max;

	sc = scsi_softc[controller(i)];
	tgt = sc->target[slave(i)];
	part = partition(i);

	/*
	 * Validate request 
	 */
	rec = ior->io_recnum;
	max = tgt->dev_info.disk.partitions[part].size;
	if (max == -1)
		max = tgt->dev_info.disk.secperunit -
			tgt->dev_info.disk.partitions[part].offset;
	i = btodb(ior->io_count + DEV_BSIZE - 1);
	if ((rec + i) > max) {
		ior->io_error = D_INVALID_SIZE;
		ior->io_op |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}
	/*
	 * Find location on disk: secno and cyl (for disksort) 
	 */
	rec += tgt->dev_info.disk.partitions[part].offset;
	ior->io_residual = rec / tgt->dev_info.disk.secpercyl;

	/*
	 * Enqueue operation 
	 */
	i = splbio();
	if (tgt->ior) {
		disksort(tgt->ior, ior);
	} else {
		ior->io_next = 0;
		tgt->ior = ior;
		scdisk_start(sc,tgt,FALSE);
	}
	splx(i);
}

/*#define CHECKSUM*/
#ifdef	CHECKSUM
int max_checksum_size = 0x2000;
#endif	CHECKSUM

/*
 * Start/completion routine for disks
 */
scdisk_start( sc, tgt, done)
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	boolean_t	done;
{
	register io_req_t	ior = tgt->ior;
	register unsigned	part, secno;
	scsi_ret_t		ret;

	if (done) {
		register int	xferred;

		/* XXXX error checking XXX */

		/* See if we requested more than max
		 * We use io_residual in a flip-side way here
		 */
		if (ior->io_count > (xferred = sc->max_dma_data)) {
			ior->io_residual += xferred;
			ior->io_count -= xferred;
			ior->io_data += xferred;
			ior->io_recnum += xferred / DEV_BSIZE;
			goto start;
		} else
		if (xferred = ior->io_residual) {
			ior->io_data -= xferred;
			ior->io_count += xferred;
			ior->io_recnum -= xferred / DEV_BSIZE;
			ior->io_residual = 0;
		}

#ifdef	CHECKSUM
		if ((ior->io_op & IO_READ) && (ior->io_count < max_checksum_size)) {
			part = partition(ior->io_unit);
			secno = ior->io_recnum + tgt->dev_info.disk.partitions[part].offset;
			scdisk_bcheck(secno, ior->io_data, ior->io_count);
		}
#endif	CHECKSUM

		/* dequeue next one */
		tgt->ior = ior->io_next;

		iodone(ior);
		if ((ior = tgt->ior) == 0)
			return;
#ifdef	CHECKSUM
		if (((ior->io_op & IO_READ) == 0) && (ior->io_count < max_checksum_size)) {
			part = partition(ior->io_unit);
			secno = ior->io_recnum + tgt->dev_info.disk.partitions[part].offset;
			scdisk_checksum(secno, ior->io_data, ior->io_count);
		}
#endif	CHECKSUM
	}
	ior->io_residual = 0;
start:
	part = partition(ior->io_unit);
	secno = ior->io_recnum + tgt->dev_info.disk.partitions[part].offset;
	if (ior->io_op & IO_READ)
		ret = scdisk_read (sc, tgt, secno, ior);
	else
		ret = scdisk_write(sc, tgt, secno, ior);
}

scdisk_get_status()
{
	return D_INVALID_OPERATION;
}

scdisk_set_status()
{
	return D_INVALID_OPERATION;
}


#ifdef	CHECKSUM

#define SUMSIZE 0x10000
#define SUMHASH(b)	(((b)>>1) & (SUMSIZE - 1))
struct {
	long blockno;
	long sum;
} scdisk_checksums[SUMSIZE];

scdisk_checksum(bno, addr, size)
	long bno;
	register unsigned int *addr;
{
	register int i = size/sizeof(int);
	register unsigned int sum = -1;

	while (i-- > 0)
		sum ^= *addr++;
	scdisk_checksums[SUMHASH(bno)].blockno = bno;
	scdisk_checksums[SUMHASH(bno)].sum = sum;
}

scdisk_bcheck(bno, addr, size)
	long bno;
	register unsigned int *addr;
{
	register int i = size/sizeof(int);
	register unsigned int sum = -1;
	unsigned int *start = addr;

	if (scdisk_checksums[SUMHASH(bno)].blockno != bno) {
if (scsi_debug) printf("No checksum for block x%x\n", bno);
		return;
	}

	while (i-- > 0)
		sum ^= *addr++;

	if (scdisk_checksums[SUMHASH(bno)].sum != sum) {
		printf("Bad checksum (x%x != x%x), bno x%x size x%x at x%x\n",
			sum,
			scdisk_checksums[bno & (SUMSIZE - 1)].sum,
			bno, size, start);
		gimmeabreak();
		scdisk_checksums[SUMHASH(bno)].sum = sum;
	}
}


#endif CHECKSUM

/*#define PERF*/
#ifdef	PERF
test_read_size = 512;
test_read_skew = 12;
test_read_nreads = 1000;
read_test()
{
	static char	buffer[8192*2];
	struct io_req	io;
	register int 	i, rec;

	bzero(&io, sizeof(io));
	io.io_unit = 0;
	io.io_op = IO_READ;
	io.io_count = test_read_size;
	io.io_data = buffer;

	for (rec = 0, i = 0; i < test_read_nreads; i++) {
		io.io_op = IO_READ;
		io.io_recnum = rec;
		scdisk_strategy(&io);
		rec += test_read_skew;
		iowait(&io);
	}
}
#endif	PERF

/*#define WDEBUG*/
#ifdef	WDEBUG

int buggo_write_size = 8192;
int buggo_dev = 9;
int	buggo_out_buffer[8192/2];
int	buggo_in_buffer[8192/2];
buggotest(n, pattern, verbose)
{
	struct io_req	io;
	register int 	i, rec;

	if (n <= 0)
		n = 1;

	if(pattern)
		for (i = 0; i < buggo_write_size/4; i++)
			buggo_out_buffer[i] = i + pattern;

	for (i = 0; i < n; i++) {
		register int j;

		buggo_out_buffer[0] = i + pattern;
		buggo_out_buffer[(buggo_write_size/4)-1] = i + pattern;
		bzero(&io, sizeof(io));
		io.io_unit = buggo_dev;
		io.io_data = (char*)buggo_out_buffer;
		io.io_op = IO_WRITE;
		io.io_count = buggo_write_size;
		io.io_recnum = i % 1024;
		scdisk_strategy(&io);

		bzero(buggo_in_buffer, sizeof(buggo_in_buffer));
		iowait(&io);

		if (verbose)
			printf("Done write with %x", io.io_error);

		bzero(&io, sizeof(io));
		io.io_unit = buggo_dev;
		io.io_data = (char*)buggo_in_buffer;
		io.io_op = IO_READ;
		io.io_count = buggo_write_size;
		io.io_recnum = i % 1024;
		scdisk_strategy(&io);
		iowait(&io);

		if (verbose)
			printf("Done read with %x", io.io_error);

		for  (j = 0; j < buggo_write_size/4; j++)
			if (buggo_out_buffer[j] != buggo_in_buffer[j]){
				printf("Difference at %d-th word: %x %x\n",
					buggo_out_buffer[j], buggo_in_buffer[j]);
				return i;
			}
	}
	printf("Test ok\n");
	return n;
}


#endif	WDEBUG
