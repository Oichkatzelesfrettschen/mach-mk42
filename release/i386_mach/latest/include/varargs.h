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
 * $Log:	varargs.h,v $
 * Revision 2.3  91/02/14  14:19:10  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:45:21  mrt]
 * 
 * Revision 2.2  90/08/06  15:09:08  rwd
 * 	Added i386 case.
 * 
 * 	RCS-ed, added mips case.  Mips also needs it in Mach standalone
 * 	programs.
 * 	[89/10/28            af]
 * 
 * Revision 2.1  89/10/28  10:48:05  af
 * Created.
 * 
 * 
 */

#if	defined(vax) || defined(sun3) || defined(mips) || defined(i386)
#define	va_dcl	int	va_alist;
typedef	char *	va_list;

#define	va_start(pvar)	(pvar) = (va_list)&va_alist
#define	va_end(pvar)
#ifdef	mips
# define va_arg(pvar, type) ((type *)(pvar = \
		(va_list) (sizeof(type) > 4 ? ((int)pvar + 2*8 - 1) & -8 \
				   : ((int)pvar + 2*4 - 1) & -4)))[-1]
#else	mips
#define	va_arg(pvar,type)	(		\
		(pvar) += ((sizeof(type)+3) & ~0x3),	\
		*((type *)((pvar) - ((sizeof(type)+3) & ~0x3))) )
#endif	mips
#endif	/* vax */

/*
 * Try to make varargs work for the Multimax so that _doprnt can be
 * declared as
 *	_doprnt(file, fmt, list)
 *	FILE	*file;
 *	char	*fmt;
 *	va_list *list;
 * and use
 *
 *	n = va_arg(*list, type)
 *
 * without needing to drag in extra declarations
 *
 * and printf becomes
 *
 * printf(fmt, va_alist)
 *	char	*fmt;
 *	va_dcl
 * {
 *	va_list listp;
 *	va_start(listp);
 *	_doprnt((FILE *)0, fmt, &listp);
 *	va_end(listp);
 * }
 */

#if	defined(multimax) && defined(KERNEL)

/*
 * the vararglist pointer is an elaborate structure (ecch)
 */
typedef struct va_list {
	char	*va_item;	/* current item */
	int	*va_ptr1,	/* arglist pointers for 1, 2, n */
		*va_ptr2,
		*va_ptrn;
	int	va_ct;		/* current argument number */
} va_list;

#define	va_alist	va_arg1, va_arg2, va_argn
#define	va_dcl		int	va_arg1, va_arg2, va_argn;

#define	va_start(pvar)	(		\
	(pvar).va_ptr1 = &va_arg1,	\
	(pvar).va_ptr2 = &va_arg2,	\
	(pvar).va_ptrn = &va_argn,	\
	(pvar).va_ct = 0 )

#define	va_end(pvar)

#define	va_arg(pvar, type)	(	\
	(pvar).va_ct++,			\
	(pvar).va_item = (char *)	\
	  ( ((pvar).va_ct == 1)		\
	    ? (pvar).va_ptr1		\
	    : ((pvar).va_ct == 2)	\
	      ? (pvar).va_ptr2		\
	      : (pvar).va_ptrn++ ) ,	\
	*((type *)((pvar).va_item)) )

/* what a mess! */
#endif	/* defined(multimax) && defined(KERNEL) */
