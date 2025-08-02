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
 * $Log:	global.h,v $
 * Revision 2.4  91/02/05  17:54:29  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:54:12  mrt]
 * 
 * Revision 2.3  90/06/19  23:00:54  rpd
 * 	Added UserFilePrefix.
 * 	[90/06/03            rpd]
 * 
 * Revision 2.2  90/06/02  15:04:42  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:10:53  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 17-Sep-87  Bennet Yee (bsy) at Carnegie-Mellon University
 *	Added GenSymTab
 *
 * 16-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Added CamelotPrefix
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_GLOBAL_H
#define	_GLOBAL_H

#define	EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <sys/types.h>
#include "string.h"

extern boolean_t BeQuiet;	/* no warning messages */
extern boolean_t BeVerbose;	/* summarize types, routines */
extern boolean_t UseMsgRPC;
extern boolean_t GenSymTab;

extern boolean_t IsCamelot;
extern boolean_t IsKernelUser;
extern boolean_t IsKernelServer;

extern string_t RCSId;

extern string_t SubsystemName;
extern u_int SubsystemBase;

extern string_t MsgOption;
extern string_t MsgKind;
extern string_t WaitTime;
extern string_t ErrorProc;
extern string_t ServerPrefix;
extern string_t UserPrefix;
extern string_t CamelotPrefix;

extern int yylineno;
extern string_t yyinname;

extern void init_global();

extern string_t UserFilePrefix;
extern string_t HeaderFileName;
extern string_t InternalHeaderFileName;
extern string_t TrapHeaderFileName;
extern string_t UserFileName;
extern string_t ServerFileName;

extern identifier_t InitRoutineName;
extern identifier_t ServerProcName;

extern void more_global();

extern char NewCDecl[];
extern char LintLib[];

#endif	_GLOBAL_H
