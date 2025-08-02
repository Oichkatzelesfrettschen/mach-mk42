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
 * $Log:	rz_labels.h,v $
 * Revision 2.4  91/02/05  17:43:56  mrt
 * 	Added author notices
 * 	[91/02/04  11:17:04  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:43  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:02  af
 * 	Added BSD/OSF1 labels and the copyright that goes with it.
 * 	[90/11/26            af]
 * 
 * Revision 2.1.1.1  90/11/01  03:44:04  af
 * 	Created.
 * 	[90/10/22            af]
 */
/*
 *	File: rz_labels.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Definitions of various disk label formats.
 */

#ifndef	_MIPS_RZ_LABELS_H_
#define	_MIPS_RZ_LABELS_H_

/*
 * Copyright (c) 1987, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)disklabel.h	7.10 (Berkeley) 6/27/88
 */


/*
 * Berkeley BSD 4.3Reno Disk Label,
 * also used in OSF/1.
 */

typedef struct {
	int		magic;			/* the magic number */
#	define		BSD_PARTITION_MAGIC	0x82564557
	short		type;			/* drive type */
	short		subtype;		/* controller/type specific */
	char		typename[16];		/* type name, e.g. "eagle" */

	/*
	 * packname contains the pack identifier and is returned when
	 * the disklabel is read off the disk or in-core copy.
	 */
	char		packname[16];			/* pack identifier */

	/*
	 * disk geometry:
	 */
	unsigned int	secsize;		/* # of bytes per sector */
	unsigned int	nsectors;		/* # of data sectors per track */
	unsigned int	ntracks;		/* # of tracks per cylinder */
	unsigned int	ncylinders;		/* # of data cylinders per unit */
	unsigned int	secpercyl;		/* # of data sectors per cylinder */
	unsigned int	secperunit;		/* # of data sectors per unit */

	/*
	 * Spares (bad sector replacements) below
	 * are not counted in nsectors or secpercyl.
	 * Spare sectors are assumed to be physical sectors
	 * which occupy space at the end of each track and/or cylinder.
	 */
	unsigned short	sparespertrack;		/* # of spare sectors per track */
	unsigned short	sparespercyl;		/* # of spare sectors per cylinder */

	/*
	 * Alternate cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	unsigned int	acylinders;		/* # of alt. cylinders per unit */

	/*
	 * hardware characteristics:
	 *
	 * interleave, trackskew and cylskew describe perturbations
	 * in the media format used to compensate for a slow controller.
	 * Interleave is physical sector interleave, set up by the formatter
	 * or controller when formatting.  When interleaving is in use,
	 * logically adjacent sectors are not physically contiguous,
	 * but instead are separated by some number of sectors.
	 * It is specified as the ratio of physical sectors traversed
	 * per logical sector.  Thus an interleave of 1:1 implies contiguous
	 * layout, while 2:1 implies that logical sector 0 is separated
	 * by one sector from logical sector 1.
	 * trackskew is the offset of sector 0 on track N
	 * relative to sector 0 on track N-1 on the same cylinder.
	 * Finally, cylskew is the offset of sector 0 on cylinder N
	 * relative to sector 0 on cylinder N-1.
	 */
	unsigned short	rpm;			/* rotational speed */
	unsigned short	interleave;		/* hardware sector interleave */
	unsigned short	trackskew;		/* sector 0 skew, per track */
	unsigned short	cylskew;		/* sector 0 skew, per cylinder */
	unsigned int	headswitch;		/* head switch time, usec */
	unsigned int	trkseek;		/* track-to-track seek, usec */
	unsigned int	flags;			/* generic flags */
#	define		NDDATA 5
	unsigned int	drivedata[NDDATA];	/* drive-type specific information */
#	define		NSPARE 5
	unsigned int	spare[NSPARE];		/* reserved for future use */
	unsigned int	magic2;			/* the magic number (again) */
	unsigned short	checksum;		/* xor of data incl. partitions */

	/*
	 * filesystem and partition information:
	 */
	unsigned short	npartitions;		/* number of partitions in following */
	unsigned int	bbsize;			/* size of boot area at sn0, bytes */
	unsigned int	sbsize;			/* max size of fs superblock, bytes */
	struct	partition {			/* the partition table */
		unsigned int	size;		/* number of sectors in partition */
		unsigned int	offset;		/* starting sector */
		unsigned int	fsize;		/* filesystem basic fragment size */
		unsigned char	fstype;		/* filesystem type, see below */
		unsigned char	frag;		/* filesystem fragments per block */
		unsigned short	cpg;		/* filesystem cylinders per group */
	} partitions[8];			/* actually may be more */
} scsi_bsd_label_t;

/* known values for the type field */
#define	LAB_DTYPE_SMD		1		/* SMD, XSMD; VAX hp/up */
#define	LAB_DTYPE_MSCP		2		/* MSCP */
#define	LAB_DTYPE_DEC		3		/* other DEC (rk, rl) */
#define	LAB_DTYPE_SCSI		4		/* SCSI */
#define	LAB_DTYPE_ESDI		5		/* ESDI interface */
#define	LAB_DTYPE_ST506		6		/* ST506 etc. */
#define	LAB_DTYPE_FLOPPY	10		/* floppy */

/*
 * Known values for fstype.
 * Used to interpret other filesystem-specific
 * per-partition information.
 */
#define	LAB_FS_UNUSED		0		/* unused */
#define	LAB_FS_SWAP		1		/* swap */
#define	LAB_FS_V6		2		/* Sixth Edition */
#define	LAB_FS_V7		3		/* Seventh Edition */
#define	LAB_FS_SYSV		4		/* System V */
#define	LAB_FS_V71K		5		/* V7 with 1K blocks (4.1, 2.9) */
#define	LAB_FS_V8		6		/* Eighth Edition, 4K blocks */
#define	LAB_FS_BSDFFS		7		/* 4.2BSD fast file system */

/*
 * Known values for the flags field:
 */
#define	LAB_D_REMOVABLE		0x01		/* removable media */
#define	LAB_D_ECC		0x02		/* supports ECC */
#define	LAB_D_BADSECT		0x04		/* supports bad sector forw */
#define	LAB_D_RAMDISK		0x08		/* disk emulator */
#define	LAB_D_CHAIN		0x10		/* can do back-back transfers */

/*
 * Physical location on disk.
 */

#define	BSD_PARTITION_BYTE_OFFSET	64

/*
 * Definition of the DEC disk label,
 * which is located (you guessed it)
 * at the end of the 4.3 superblock.
 */

struct partition_info {
	unsigned int	n_sectors;	/* how big the partition is */
	unsigned int	offset;		/* sector no. of start of part. */
};

typedef struct {
	int	magic;
#	define	DEC_PARTITION_MAGIC	0x032957
	int	in_use;
	struct  partition_info partitions[8];
} scsi_dec_label_t;

/*
 * Physical location on disk.
 * This is independent of the filesystem we use,
 * although of course we'll be in trouble if we
 * screwup the 4.3 SBLOCK..
 */

#define	DEC_PARTITION_BYTE_OFFSET	((2*8192)-sizeof(scsi_dec_label_t))


/*
 * Definitions for the primary boot information
 * This is common to all, the prom knows it.
 */

typedef struct {
	int		pad[2];
	unsigned int	magic;
#	define		DEC_BOOT0_MAGIC	0x2757a
	int		mode;
	unsigned int	phys_base;
	unsigned int	virt_base;
	unsigned int	n_sectors;
	unsigned int	start_sector;
} scsi_dec_boot0_t;

#endif	_MIPS_RZ_LABELS_H_

