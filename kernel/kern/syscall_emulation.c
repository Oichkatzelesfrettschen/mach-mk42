/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * $Log:	syscall_emulation.c,v $
 * Revision 2.5  91/02/05  17:29:26  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:17:54  mrt]
 * 
 * Revision 2.4  90/12/05  23:50:11  af
 * 
 * 
 * Revision 2.3  90/12/05  20:42:15  af
 * 	I beg your pardon, Ultrix uses up to syscall #257 inclusive.
 * 	[90/12/03  22:57:35  af]
 * 
 * Revision 2.2  89/11/29  14:09:18  af
 * 	For mips, grow the max syscall limit, as Ultrix uses up to 256.
 * 	Rcs-ed.
 * 	[89/11/16            af]
 * 
 */

#include <mach/mach_types.h>
#include <kern/syscall_emulation.h>

#include <mach/error.h>
#include <kern/task.h>
#include <kern/zalloc.h>
#include <mach/vm_param.h>

zone_t		eml_zone;
#ifdef	mips
int		eml_max_emulate_count = 258 - EML_MIN_SYSCALL;
#else	mips
int		eml_max_emulate_count = 256;	/* XXX */
#endif	mips
int		eml_max_syscall;	/* inclusive */

/*
 *  eml_init:	initialize user space emulation code
 */
void eml_init()
{
	
	eml_max_syscall = eml_max_emulate_count + EML_MIN_SYSCALL - 1;

	eml_zone = zinit((vm_size_t)(sizeof(struct eml_dispatch)
			 	     + (eml_max_emulate_count-1)
				       * sizeof (eml_routine_t)),
			 PAGE_SIZE,
			 PAGE_SIZE,
			 FALSE,
			 "emulation routines");
}

/*
 * eml_task_reference() [Exported]
 *
 *	Bumps the reference count on the common emulation
 *	vector.
 */

void eml_task_reference(task, parent)
	task_t	task, parent;
{
	register eml_dispatch_t	eml;

	if (parent == TASK_NULL)
	    eml = EML_DISPATCH_NULL;
	else
	    eml = parent->eml_dispatch;

	if (eml != EML_DISPATCH_NULL) {
	    simple_lock(&eml->lock);
	    eml->ref_count++;
	    simple_unlock(&eml->lock);
	}
	task->eml_dispatch = eml;
}


/*
 * eml_task_deallocate() [Exported]
 *
 *	Cleans up after the emulation code when a process exits.
 */
 
void eml_task_deallocate(task)
	task_t task;
{
	register eml_dispatch_t	eml;

	eml = task->eml_dispatch;
	if (eml != EML_DISPATCH_NULL) {
	    simple_lock(&eml->lock);
	    if (--eml->ref_count > 0) {
		simple_unlock(&eml->lock);
	    }
	    else {
		zfree(eml_zone, (vm_offset_t)eml);
	    }
	}
}

/*
 *   task_set_emulation:  [Server Entry]
 *   set up for user space emulation of syscalls within this task.
 */
kern_return_t task_set_emulation( task, routine_entry_pt, routine_number)
	task_t		task;
	vm_offset_t 	routine_entry_pt;
	int		routine_number;
{
	register int	i;
	register eml_dispatch_t	eml, new_eml;

	if (task == TASK_NULL)
	        return( EML_BAD_TASK );

	if ( routine_number > eml_max_syscall
	     || routine_number < EML_MIN_SYSCALL )
		return( EML_BAD_CNT );

	/*
	 * If either the task does not have an emulation vector, or
	 * it points to a common one, then give it a new one.
	 */

	new_eml = EML_DISPATCH_NULL;
    Restart:
	task_lock(task);

	eml = task->eml_dispatch;
	if (eml == EML_DISPATCH_NULL) {

	    if (new_eml == EML_DISPATCH_NULL) {
		task_unlock(task);
		new_eml = (eml_dispatch_t) zalloc(eml_zone);
		simple_lock_init(&new_eml->lock);
		new_eml->ref_count = 1;
		new_eml->disp_count = eml_max_emulate_count;

		goto Restart;
	    }
	    /* zero the vectors and install the new dispatch table */
	    for (i = 0; i < new_eml->disp_count; i++)
		new_eml->disp_vector[i] = EML_ROUTINE_NULL;
	    simple_lock(&new_eml->lock);
	    task->eml_dispatch = new_eml;
	    eml = new_eml;
	}
	else {
	    simple_lock(&eml->lock);
	    if (eml->ref_count > 1) {
		/* copy old one */
		if (new_eml == EML_DISPATCH_NULL) {
		    simple_unlock(&eml->lock);
		    task_unlock(task);
		    new_eml = (eml_dispatch_t) zalloc(eml_zone);
		    simple_lock_init(&new_eml->lock);
		    new_eml->ref_count = 1;
		    new_eml->disp_count = eml_max_emulate_count;

		    goto Restart;
		}
		/*
		 * copy the old vectors
		 */
		for (i = 0; i < new_eml->disp_count; i++) {
		    new_eml->disp_vector[i] = eml->disp_vector[i];
		}

		/*
		 * install the new dispatch vector
		 */
		simple_lock(&new_eml->lock);
		task->eml_dispatch = new_eml;

		/*
		 * remove the reference to the old dispatch vector
		 */
		eml->ref_count--;
		simple_unlock(&eml->lock);

		eml = new_eml;

	    }
	    else {
		/* did not need new one after all */
		if (new_eml != EML_DISPATCH_NULL) {
		    zfree(eml_zone, (vm_offset_t)new_eml);
		}
	    }
	}

	eml->disp_vector[routine_number - EML_MIN_SYSCALL] = routine_entry_pt;
	simple_unlock(&eml->lock);
	task_unlock(task);

	return( KERN_SUCCESS );
}

