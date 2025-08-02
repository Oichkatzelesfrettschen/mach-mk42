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
 * $Log:	test.c,v $
 * Revision 2.4  91/02/05  17:39:21  mrt
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:06:37  mrt]
 * 
 * Revision 2.3  90/12/05  23:30:15  af
 * 	Created, standalone simple program to test new boot.
 * 	[90/12/02            af]
 * 
 */

main(argc,argv,envp)
	char **argv, **envp;
{
	prom_printf("Hi mom(%x %x %x)!\n", argc, argv, envp);
	prom_printf("argv -> %s\n", *argv);
	prom_printf("envp -> %s\n", *envp);
}
