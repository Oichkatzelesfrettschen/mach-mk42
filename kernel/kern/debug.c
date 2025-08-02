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
 * $Log:	debug.c,v $
 * Revision 2.11  91/02/05  17:25:52  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:11:43  mrt]
 * 
 * Revision 2.10  90/12/14  11:02:02  jsb
 * 	MACH_CLPORT support: print node number as well as cpu in panic.
 * 	[90/12/13  21:40:37  jsb]
 * 
 * Revision 2.9  90/12/04  14:51:00  jsb
 * 	Added i860 support for Debugger function.
 * 	[90/12/04  11:01:25  jsb]
 * 
 * Revision 2.8  90/11/05  14:30:49  rpd
 * 	Added Assert.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.7  90/10/25  14:45:10  rwd
 * 	Change sun3 debugger invocation.
 * 	[90/10/17            rwd]
 * 
 * Revision 2.6  90/09/09  23:20:09  rpd
 * 	Fixed panic and log to supply cnputc to _doprnt.
 * 	[90/09/09            rpd]
 * 
 * Revision 2.5  90/09/09  14:32:03  rpd
 * 	Use decl_simple_lock_data.
 * 	[90/08/30            rpd]
 * 
 * Revision 2.4  90/08/27  22:02:14  dbg
 * 	Pass extra argument to _doprnt.
 * 	[90/08/21            dbg]
 * 
 * Revision 2.3  90/05/03  15:46:53  dbg
 * 	Added i386 case.
 * 	[90/02/21            dbg]
 * 
 * Revision 2.2  89/11/29  14:09:04  af
 * 	Added mips case, RCS-ed.
 * 	[89/10/28            af]
 * 
 */

#include <mach_kdb.h>
#include <mach_clport.h>

#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <sys/varargs.h>
#include <kern/thread.h>

extern int cnputc();
extern void Debugger();

#if	MACH_KDB
extern int db_breakpoints_inserted;
#endif	MACH_KDB

void Assert(file, line)
	char *file;
	int line;
{
	printf("Assertion failed: file \"%s\", line %d\n", file, line);
#if	MACH_KDB
	if (db_breakpoints_inserted)
#endif	MACH_KDB
	Debugger("assertion failure");
}

void Debugger(message)
	char *	message;
{
#ifdef	lint
	message++;
#endif	lint

#ifdef	vax
	asm("bpt");
#endif	vax
#ifdef	sun
	current_thread()->pcb->pcb_flag |= TRACE_KDB;
	asm("orw  #0x00008000,sr");
#endif	sun
#ifdef	mips
	gimmeabreak();
#endif	mips
#ifdef	i386
	asm("int3");
#endif	i386
#ifdef	i860
	asm("trap r0,r0,r0");
#endif	i860
}

char			*panicstr;
decl_simple_lock_data(,	panic_lock)
int			paniccpu;

panic_init()
{
	simple_lock_init(&panic_lock);
}

/*VARARGS1*/
panic(s, va_alist)
	char *	s;
	va_dcl
{
	va_list	listp;

	simple_lock(&panic_lock);
	if (panicstr) {
	    if (cpu_number() != paniccpu) {
		simple_unlock(&panic_lock);
		halt_cpu();
		/* NOTREACHED */
	    }
	}
	else {
	    panicstr = s;
	    paniccpu = cpu_number();
	}
	simple_unlock(&panic_lock);
#if	MACH_CLPORT
	printf("panic: (node %d, cpu %d) ", node_self(), (unsigned) paniccpu);
#else	MACH_CLPORT
	printf("panic: (cpu %d) ", (unsigned) paniccpu);
#endif	MACH_CLPORT
	va_start(listp);
	_doprnt(s, &listp, cnputc, 0);
	va_end(listp);
	printf("\n");

#if	MACH_KDB
	Debugger("panic");
#endif	MACH_KDB
	halt_cpu();
}

/*
 * We'd like to use BSD's log routines here...
 */
/*VARARGS2*/
log(level, fmt, va_alist)
	int	level;
	char *	fmt;
	va_dcl
{
	va_list	listp;

#ifdef lint
	level++;
#endif lint
	va_start(listp);
	_doprnt(fmt, &listp, cnputc, 0);
	va_end(listp);
}
