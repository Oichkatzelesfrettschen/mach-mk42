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
 * $Log:	tc.h,v $
 * Revision 2.5  91/02/05  17:46:48  mrt
 * 	Added author notices
 * 	[91/02/04  11:20:40  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:19:35  mrt]
 * 
 * Revision 2.4  90/12/05  23:36:07  af
 * 
 * 
 * Revision 2.2.1.1  90/10/03  11:52:39  af
 * 	Created, from the DEC specs:
 * 	"TURBOchannel Hardware Specification"
 * 	EK-369AA-OD-005, Version 005, July 1990
 * 	[90/09/03            af]
 */
/*
 *	File: tc.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Definitions for the TURBOchannel BUS.
 */

/*
 * Address map specifications for any TC option
 * These are offset from the option's base address
 */

#define TC_OFF_ROM		0x000003e0	/* required ROM info */
#define TC_OFF_PROTO_ROM	0x003c03e0	/* 'obsolete' alternate */

#define TC_ROM_TEST_DATA_SIZE	16
#define TC_ROM_SLEN		4
#define TC_ROM_LLEN		8

typedef struct {
	unsigned char	value;
	char		pad[3];
} tc_padded_char_t;

typedef struct {
	tc_padded_char_t	rom_width;	/* legal: 1 2 4 */
	tc_padded_char_t	rom_stride;	/* legal: 4 */
	tc_padded_char_t	rom_size;	/* legal: 0-255, unit: 8kb */
	tc_padded_char_t	slot_size;	/* legal: 1-128, unit: 4Mb */
	unsigned char		test_data[TC_ROM_TEST_DATA_SIZE];
						/* must always contain:
						/* x55 x00 xaa xff
						/* (each byte is repeated
						/*  rom_stride times) */
	tc_padded_char_t	firmware_rev[TC_ROM_LLEN];
	tc_padded_char_t	vendor_name[TC_ROM_LLEN];
	tc_padded_char_t	module_name[TC_ROM_LLEN];
	tc_padded_char_t	host_firmware_type[TC_ROM_SLEN];
} tc_rommap_t;

typedef struct {
	unsigned char	present;			/* and do we handle it */
	unsigned char	slot_size;			/* how many TC slots */
	unsigned char	rom_width;			/* bytewide or.. */
	unsigned char	isa_ctlr;			/* ..such as the scsi */
	unsigned char	unit;				/* instance id */
	char		module_name[TC_ROM_LLEN+1];	/* ROM name */
	char		module_id[TC_ROM_LLEN * 2+1];	/* vendor and rev */
	unsigned int	k1seg_address;			/* TC starting address */
	char		*driver_name;			/* software name */
	int		(*intr)();			/* interrupt routine */
	struct bus_driver	*driver;		/* config info ?? */
} tc_option_t;


#ifdef	KERNEL
extern	tc_option_t	tc_slot_info[];
extern	unsigned int	tc_slot_phys_base[];

extern	void (*tc_slot_hand_fill)(/* tc_option_t * */);

/* XXX exported func prototypes */
#endif	KERNEL
