/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
 * Open Software Foundation, Inc. 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of ("OSF") or Open Software 
 * Foundation not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include "bootstrap.h"

#include <mach/mach_host.h>
#include <mach/machine/vm_param.h>

#define	STACK_SIZE	(64*1024)

char *
set_regs(mach_port_t		host_port,
	 mach_port_t		user_task,
	 mach_port_t		user_thread,
	 struct loader_info	*lp,
	 unsigned long		mapend,
	 int			arg_size)
{
	vm_offset_t	stack_start;
	vm_offset_t	stack_end;
	struct i386_thread_state	regs;
	unsigned int		reg_size;

	/*
	 * Add space for 5 ints to arguments, for
	 * PS program. XXX
	 */
	arg_size += 5 * sizeof(int);

	/*
	 * Allocate stack.
	 */
	stack_end = mapend;
	if (!stack_end)
	    stack_end = VM_MAX_ADDRESS;
	stack_start = stack_end - STACK_SIZE;
	(void)vm_allocate(user_task,
			  &stack_start,
			  (vm_size_t)(stack_end - stack_start),
			  FALSE);
	if (mapend)
		(void)vm_wire(host_port, user_task, stack_start,
			      (vm_size_t)(stack_end - stack_start),
			      VM_PROT_READ|VM_PROT_WRITE);
	reg_size = i386_THREAD_STATE_COUNT;
	(void)thread_get_state(user_thread,
				i386_THREAD_STATE,
				(thread_state_t)&regs,
				&reg_size);

	regs.eip = lp->entry_1;
	regs.uesp = (int)((stack_end - arg_size) & ~(sizeof(int)-1));

	(void)thread_set_state(user_thread,
				i386_THREAD_STATE,
				(thread_state_t)&regs,
				reg_size);
	return ((char *)regs.uesp);
}

