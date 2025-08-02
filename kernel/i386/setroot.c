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
 * $Log:	setroot.c,v $
 * Revision 2.6  91/02/05  17:14:38  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:37:59  mrt]
 * 
 * Revision 2.5  90/12/04  14:46:35  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:19:36  jsb]
 * 
 * Revision 2.4  90/11/24  15:14:53  jsb
 * 	Added missing newline.
 * 	[90/11/24  11:45:51  jsb]
 * 
 * Revision 2.3  90/09/23  16:47:18  jsb
 * 	If iPSC386, root device is sd (scsi), not hd.
 * 
 * Revision 2.2  90/05/03  15:37:31  dbg
 * 	Created.
 * 	[90/02/20            dbg]
 * 
 */

/*
 * Get root device.  Temporarily hard-coded.
 */
#if	iPSC386
char *root_name = "sd0a";
#else	iPSC386
char *root_name = "hd0a";
#endif	iPSC386

get_root_device()
{
	printf("root on %s\n", root_name);
}
