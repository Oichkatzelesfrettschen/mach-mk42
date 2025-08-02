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
 * $Log:	scsi_defs.h,v $
 * Revision 2.4  91/02/05  17:45:43  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:29  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:11  mrt]
 * 
 * Revision 2.3  90/12/05  23:35:12  af
 * 	Cleanups, use BSD labels internally.
 * 	[90/12/03  23:47:29  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:39:55  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: scsi_defs.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Controller-independent definitions for the SCSI driver
 */

#ifndef	MIPS_SCSI_DEFS_H
#define	MIPS_SCSI_DEFS_H


/*
 * Internal error codes, and return values
 * XXX use the mach/error.h scheme XXX
 */
typedef unsigned int		scsi_ret_t;

#define	SCSI_ERR_GRAVITY(x)	((unsigned)(x)&0xf0000000)
#define	SCSI_ERR_GRAVE		0x80000000
#define SCSI_ERR_BAD		0x40000000

#define	SCSI_ERR_CLASS(x)	((unsigned)(x)&0x0fffffff)
#define	SCSI_ERR_STATUS		0x00000001
#define	SCSI_ERR_SENSE		0x00000002
#define SCSI_ERR_MSEL		0x00000004

extern	void	scsi_error(/* target_info_t *, unsigned, unsigned */);

#define	SCSI_RET_IN_PROGRESS	0x00
#define	SCSI_RET_SUCCESS	0x01
#define	SCSI_RET_RETRY		0x02


/*
 * Device-specific information kept by driver
 */
#include <mips/PMAX/rz_labels.h>

typedef scsi_bsd_label_t scsi_disk_info_t;

typedef struct {
	int nothing_today;
} scsi_tape_info_t;

/*
 * Device-specific operations
 */
typedef struct {
	char		*(*driver_name)();	/* my driver's name */
	scsi_ret_t	(*optimize)();		/* tune up internal params */
	scsi_ret_t	(*open)();		/* open time ops */
	int		(*strategy)();		/* sort/start routine */
	int		(*restart)();		/* completion routine */
	io_return_t	(*get_status)();	/* specialization */
	io_return_t	(*set_status)();	/* specialization */
} scsi_devsw_t;

extern scsi_devsw_t	scsi_devsw[];


#define	SCSI_TARGET_NAME_LEN	8+16+4+8	/* our way to keep it */

typedef struct {
	unsigned int	flags;
#define	TGT_DID_SYNCH		0x00000001	/* finished the synch neg */
#define	TGT_TRY_SYNCH		0x00000002	/* do the synch negotiation */
#define	TGT_FULLY_PROBED	0x00000004	/* can sleep to wait */
#define	TGT_ONLINE		0x00000008	/* did the once-only stuff */
#define	TGT_ALIVE		0x00000010
#define	TGT_DMA_RESTRICTED	0x00000020
#define	TGT_DISCONNECTED	0x00000040	/* waiting for reconnect */
#define	TGT_HW_SPECIFIC_BITS	0xffff0000	/* see specific HBA */
	char		*hw_state;	/* opaque */
	char		*dma_ptr;
	char		*cmd_ptr;
	scsi_devsw_t	*dev_ops;
	char		target_id;
	char		unit_no;
	char		sync_period;
	char		sync_offset;
	io_req_t	ior;
	volatile char	done;
	char		cur_cmd;
	char		tgt_name[SCSI_TARGET_NAME_LEN];
	union {
	    scsi_disk_info_t	disk;
	    scsi_tape_info_t	tape;
	} dev_info;
} target_info_t;

typedef struct {
	/* initiator (us) state */
	unsigned char	initiator_id;
	unsigned char	masterno;
	unsigned short	max_dma_data;
	int		(*go)();
	/* per-target state */
	target_info_t		*target[8];
} scsi_softc_t;

extern scsi_softc_t	*scsi_softc[];
extern scsi_softc_t	*scsi_master_alloc(/* int unit */);
extern target_info_t	*scsi_slave_alloc(/* int unit, int slave, char *hw */);

extern unsigned char	scsi_no_synchronous_xfer[];	/* one bitmap per controller */
extern int		scsi_debug;
#endif	MIPS_SCSI_DEFS_H
