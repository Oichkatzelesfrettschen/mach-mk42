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
 * $Log:	db_interface.c,v $
 * Revision 2.4  91/02/05  17:11:13  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:17  mrt]
 * 
 * Revision 2.3  90/12/04  14:45:55  jsb
 * 	Changes for merged intel/pmap.{c,h}.
 * 	[90/12/04  11:14:41  jsb]
 * 
 * Revision 2.2  90/10/25  14:44:43  rwd
 * 	Added watchpoint support.
 * 	[90/10/18            rpd]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 *
 */

/*
 * Interface to new debugger.
 */
#include <sys/reboot.h>
#include <vm/pmap.h>

#include <i386/thread.h>
#include <i386/db_machdep.h>
#include <i386/seg.h>
#include <i386/trap.h>
#include <i386/setjmp.h>

int	db_active = 0;

/*
 * Received keyboard interrupt sequence.
 */
kdb_kbd_trap(regs)
	struct i386_saved_state *regs;
{
	if (db_active == 0 && (boothowto & RB_KDB)) {
	    printf("\n\nkernel: keyboard interrupt\n");
	    kdb_trap(-1, 0, regs);
	}
}

/*
 *  kdb_trap - field a TRACE or BPT trap
 */

jmp_buf_t *db_nofault = 0;

kdb_trap(type, code, regs)
	int	type, code;
	register struct i386_saved_state *regs;
{
	if ((boothowto&RB_KDB) == 0)
	    return(0);

	switch (type) {
	    case T_INT3:	/* breakpoint */
	    case T_WATCHPOINT:	/* watchpoint */
	    case T_DEBUG:	/* single_step */
	    case -1:	/* keyboard interrupt */
		break;

	    default:
		kdbprinttrap(type, code);
		if (db_nofault) {
		    jmp_buf_t *no_fault = db_nofault;
		    db_nofault = 0;
		    _longjmp(no_fault);
		}
	}

	/*  Should switch to kdb`s own stack here. */

	ddb_regs = *regs;
	if ((regs->cs & 0x3) == 0) {
	    /*
	     * Kernel mode - esp and ss not saved
	     */
	    ddb_regs.uesp = (int)&regs->uesp;	/* kernel stack pointer */
	    ddb_regs.ss   = KERNEL_DS;
	}

	db_active++;
	cnpollc(TRUE);
	db_trap(type, code);
	cnpollc(FALSE);
	db_active--;

	regs->eip    = ddb_regs.eip;
	regs->efl    = ddb_regs.efl;
	regs->eax    = ddb_regs.eax;
	regs->ecx    = ddb_regs.ecx;
	regs->edx    = ddb_regs.edx;
	regs->ebx    = ddb_regs.ebx;
	if (regs->cs & 0x3) {
	    /*
	     * user mode - saved esp and ss valid
	     */
	    regs->uesp = ddb_regs.uesp;		/* user stack pointer */
	    regs->ss   = ddb_regs.ss & 0xffff;	/* user stack segment */
	}
	regs->ebp    = ddb_regs.ebp;
	regs->esi    = ddb_regs.esi;
	regs->edi    = ddb_regs.edi;
	regs->es     = ddb_regs.es & 0xffff;
	regs->cs     = ddb_regs.cs & 0xffff;
	regs->ds     = ddb_regs.ds & 0xffff;
	regs->fs     = ddb_regs.fs & 0xffff;
	regs->gs     = ddb_regs.gs & 0xffff;

	return (1);
}

extern char *	trap_type[];
extern int	TRAP_TYPES;

/*
 * Print trap reason.
 */
kdbprinttrap(type, code)
	int	type, code;
{
	printf("kernel: ");
	if (type > TRAP_TYPES)
	    printf("type %d", type);
	else
	    printf("%s", trap_type[type]);
	printf(" trap, code=%x\n", code);
}

/*
 * Read bytes from kernel address space for debugger.
 */

extern jmp_buf_t	db_jmpbuf;

void
db_read_bytes(addr, size, data)
	vm_offset_t	addr;
	register int	size;
	register char	*data;
{
	register char	*src;

	db_nofault = &db_jmpbuf;

	src = (char *)addr;
	while (--size >= 0)
	    *data++ = *src++;

	db_nofault = 0;
}

/*
 * Write bytes to kernel address space for debugger.
 */
void
db_write_bytes(addr, size, data)
	vm_offset_t	addr;
	register int	size;
	register char	*data;
{
	register char	*dst;

	register pt_entry_t *ptep0 = 0;
	pt_entry_t	oldmap0 = 0;
	vm_offset_t	addr1;
	register pt_entry_t *ptep1 = 0;
	pt_entry_t	oldmap1 = 0;
	extern char	etext;

	db_nofault = &db_jmpbuf;

	if (addr >= VM_MIN_KERNEL_ADDRESS &&
	    addr <= (vm_offset_t)&etext)
	{
	    ptep0 = pmap_pte(kernel_pmap, addr);
	    oldmap0 = *ptep0;
	    *ptep0 |= INTEL_PTE_WRITE;

	    addr1 = i386_trunc_page(addr + size - 1);
	    if (i386_trunc_page(addr) != addr1) {
		/* data crosses a page boundary */

		ptep1 = pmap_pte(kernel_pmap, addr1);
		oldmap1 = *ptep1;
		*ptep1 |= INTEL_PTE_WRITE;
	    }
	    flush_tlb();
	}

	dst = (char *)addr;

	while (--size >= 0)
	    *dst++ = *data++;

	db_nofault = 0;

	if (ptep0) {
	    *ptep0 = oldmap0;
	    if (ptep1) {
		*ptep1 = oldmap1;
	    }
	    flush_tlb();
	}
}

