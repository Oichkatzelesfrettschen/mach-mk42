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
 * $Log:	cswitch.s,v $
 * Revision 2.6  91/02/05  17:10:57  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:50  mrt]
 * 
 * Revision 2.5  91/01/09  22:41:09  rpd
 * 	Renamed to Load_context and Switch_task_context.
 * 	Removed ktss munging.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.4  91/01/08  15:10:09  rpd
 * 	Minor cleanup.
 * 	[90/12/31            rpd]
 * 	Added switch_task_context, switch_thread_context.
 * 	[90/12/12            rpd]
 * 
 * 	Reorganized the pcb.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.3  90/12/20  16:35:48  jeffreyh
 * 	Changes for __STDC__
 * 
 * 	Changes for __STDC__
 * 	[90/12/07  15:45:37  jeffreyh]
 * 
 *
 * Revision 2.2  90/05/03  15:25:00  dbg
 * 	Created.
 * 	[90/02/08            dbg]
 * 
 */

/*
 * Context switch routines for i386.
 */

ENTRY(Load_context)
	movl	S_ARG0,%ecx			/ get thread
	movl	TH_KERNEL_STACK(%ecx),%ecx	/ get kernel stack
	movl	KSS_ESP(%ecx),%esp		/ switch stacks
	movl	KSS_ESI(%ecx),%esi		/ restore registers
	movl	KSS_EDI(%ecx),%edi
	movl	KSS_EBP(%ecx),%ebp
	movl	KSS_EBX(%ecx),%ebx
	pushl	KSS_EIP(%ecx)			/ push return PC on stack
	pushl	KSS_IPL(%ecx)			/ get IPL
	call	EXT(splx)			/ restore it
	addl	$4,%esp				/ pop argument to splx
	ret					/ resume

ENTRY(Switch_task_context)
	movl	S_ARG0,%ecx			/ get old thread
	movl	TH_KERNEL_STACK(%ecx),%ecx	/ save regs in kernel stack
	movl	%ebx,KSS_EBX(%ecx)		/ save registers
	movl	%ebp,KSS_EBP(%ecx)
	movl	%edi,KSS_EDI(%ecx)
	movl	%esi,KSS_ESI(%ecx)
	call	EXT(splhi)			/ block all interrupts
						/ clobbers %eax,%edx
	movl	%eax,KSS_IPL(%ecx)		/ save old IPL
	movl	S_ARG0,%ebp			/ get old thread
	movl	S_ARG1,%edi			/ get continuation
	movl	S_ARG2,%esi			/ get new thread
	popl	%edx				/ pop return PC
	movl	%edx,KSS_EIP(%ecx)		/ save PC
	movl	%esp,KSS_ESP(%ecx)		/ save SP

	movl	%esi,_active_threads		/ new thread is active
	movl	TH_KERNEL_STACK(%esi),%ecx	/ get kernel stack
	movl	KSS_ESP(%ecx),%esp		/ switch stacks

	pushl	%edi				/ push continuation
	pushl	%ebp				/ push old thread
	call	EXT(thread_continue)
	addl	$8,%esp				/ pop arguments

	movl	TH_KERNEL_STACK(%esi),%ecx	/ get kernel stack again
	movl	KSS_ESI(%ecx),%esi		/ restore registers
	movl	KSS_EDI(%ecx),%edi
	movl	KSS_EBP(%ecx),%ebp
	movl	KSS_EBX(%ecx),%ebx
	pushl	KSS_EIP(%ecx)			/ push return PC on stack
	pushl	KSS_IPL(%ecx)			/ get IPL
	call	EXT(splx)			/ restore it
	addl	$4,%esp				/ pop argument to splx
	ret					/ resume
