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
 * $Log:	ipc_clpacket.h,v $
 * Revision 2.2  91/02/05  17:20:54  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:43:40  mrt]
 * 
 * Revision 2.1  90/12/11  09:41:42  jsb
 * Created.
 * 
 */
/*
 *	File:	ipc/ipc_clpacket.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1990
 *
 *	Definitions for data structures used by cluster IPC implementation.
 */

struct packet {
	unsigned long	pkt_remote;		/* dest on send, src on rcv */
	unsigned long	pkt_msgid;		/* id of msg this pkt is in */
	unsigned short	pkt_id;			/* id of this pkt */
	unsigned short	pkt_last;		/* id of last pkt in msg */
	char *		pkt_page;		/* page-alligned if != 0 */
	char *		pkt_data;		/* inline data */
	unsigned long	pkt_data_len;		/* amount of inline data */
	struct packet *	pkt_next;		/* next packet in msg */
};

struct msg {
	struct packet	*msg_first_pkt;
	struct packet	*msg_last_pkt;
	struct msg	*msg_next;
};
