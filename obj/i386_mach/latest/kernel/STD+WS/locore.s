#define	TH_PCB	36
#define	TH_TASK	12
#define	TH_RECOVER	104
#define TH_KERNEL_STACK	40
#define	TASK_EMUL	108
#define	EML_MIN_SYSCALL	-52
#define	DISP_COUNT	4
#define	DISP_VECTOR	8
#define K_SIZE	28
#define	K_EBX	0
#define	K_ESP	4
#define	K_EBP	8
#define	K_EDI	12
#define	K_ESI	16
#define	K_EIP	20
#define	K_IPL	24
#define	KSS_EBX	4068
#define	KSS_ESP	4072
#define	KSS_EBP	4076
#define	KSS_EDI	4080
#define	KSS_ESI	4084
#define	KSS_EIP	4088
#define	KSS_IPL	4092
#define R_SIZE	92
#define	R_CS	60
#define	R_UESP	68
#define	R_EAX	44
#define	R_TRAPNO	48
#define	R_ERR	52
#define	R_EFLAGS	64
#define	R_EIP	56
#define	NBPG	4096
#define	VM_MIN_ADDRESS	0
#define	VM_MAX_ADDRESS	-1073741824
#define	KERNELBASE	-1073741824
#define KERNEL_STACK_SIZE	4096
#define	PDESHIFT	22
#define	PTESHIFT	12
#define	PTEMASK	1023
#define	PTE_PFN	-4096
#define	PTE_V	1
#define	PTE_W	2
#define	PTE_INVALID	-2
#define	IDTSZ	256
#define	GDTSZ	12
#define	LDTSZ	4
#define	KERNEL_CS	64
#define	KERNEL_DS	72
#define	KERNEL_TSS	88
#define	KERNEL_LDT	80
#define	PTES_PER_PAGE	1024
#define	INTEL_PTE_KERNEL	3
#define	KERNELBASEPDE	3072
#define	T_INVALID_OPCODE	6
#define	T_PAGE_FAULT	14
#define	T_GP_FAULT	13
#define	T_PF_USER_READ	5
#define	TSS_ESP0	4
#define	TSS_SS0	8
#define	RB_KDB	4
#define	K_TRAP_GATE	143
#define	U_TRAP_GATE	239
#define	K_INTR_GATE	142
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
 * $Log:	start.s,v $
 * Revision 2.9  91/02/05  17:14:50  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:38:15  mrt]
 * 
 * Revision 2.8  90/12/20  16:37:02  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07  15:43:21  jeffreyh]
 * 
 *
 * Revision 2.7  90/12/04  14:46:38  jsb
 * 	iPSC2 -> iPSC386; ipsc2_foo -> ipsc_foo;
 * 	changes for merged intel/pmap.{c,h}.
 * 	[90/12/04  11:20:35  jsb]
 * 
 * Revision 2.6  90/11/24  15:14:56  jsb
 * 	Added AT386 conditional around "BIOS/DOS hack".
 * 	[90/11/24  11:44:47  jsb]
 * 
 * Revision 2.5  90/11/05  14:27:51  rpd
 * 	Since we steal pages after esym for page tables, use first_avail
 * 	to record the last page +1 that we stole.
 * 	Tell bios to warm boot on reboot.
 * 	[90/09/05            rvb]
 * 
 * Revision 2.4  90/09/23  17:45:20  jsb
 * 	Added support for iPSC386.
 * 	[90/09/21  16:42:34  jsb]
 * 
 * Revision 2.3  90/08/27  21:58:29  dbg
 * 	Change fix_desc to match new fake_descriptor format.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.2  90/05/03  15:37:40  dbg
 * 	Created.
 * 	[90/02/14            dbg]
 * 
 */

#include <mach_kdb.h>
#include <i386/asm.h>

/*
 * Startup code for i386.
 * Kernel is loaded starting at 1MB.
 * Protected mode, no paging.
 */

	.data
/*
 * 'interrupt' (initial) stack.
 */
	.align	4
	.globl	EXT(intstack)
LEXT(intstack)
	.set	., .+4096
	.globl	EXT(eintstack)
LEXT(eintstack)

	.align	8
	.globl	EXT(gdtptr)
LEXT(gdtptr)
	.word	Times(8,GDTSZ)-1
	.long	EXT(gdt)

	.align	8
	.globl	EXT(idtptr)
LEXT(idtptr)
	.word	Times(8,IDTSZ)-1
	.long	EXT(idt)

	.text
	.align	2
	.globl	EXT(pstart)
LEXT(pstart)
#if	AT386
/ BIOS/DOS hack				/* this says don't test memory */
	movl	$0x472, %edx		/ on reboot/reset
	movw	$0x1234, %ax		/ pretty obscure
	movw	%ax, (%edx)
/ BIOS/DOS hack
#endif	AT386
	movl	$KERNELBASE,%edx	/ virtual start - real start
	negl	%edx			/ negate to use as index

	popl	%eax			/ pop boottype
	movl	%eax,EXT(boottype)(%edx)

	mov	$0,%ax			/ fs, gs must be zeroed;
	mov	%ax,%fs			/* some bootstrappers don't do this */
	mov	%ax,%gs			/ for us

#if	iPSC386
	popl	%eax			/ physical memory - in K
	movl	%eax,EXT(ipsc_basemem)(%edx)

	popl	%eax			/ physical node slot number
	movl	%eax,EXT(ipsc_physnode)(%edx)

	popl	%eax			/ logical node number of this node
	movl	%eax,EXT(ipsc_this_node)(%edx)

	popl	%eax			/ number of nodes in cube
	movl	%eax,EXT(ipsc_num_nodes)(%edx)
#else	iPSC386
	popl	%eax			/ extmem - in K
	movl	%eax,EXT(extmem)(%edx)

	popl	%eax			/ cnvmem - in K
	movl	%eax,EXT(cnvmem)(%edx)
#endif	iPSC386

	popl	%eax			/ boothowto
	movl	%eax,EXT(boothowto)(%edx)

#if	MACH_KDB
	testl	$RB_KDB,%eax		/ did boot load symbols?
	je	0f			/ if so:
	popl	%eax			/ get esym
	orl	%eax,%eax		/ test it
	jne	1f			/ if symbols were not loaded:
0:
#endif	MACH_KDB
	lea	EXT(end)(%edx),%eax	/ get first available address
1:
	subl	%edx,%eax		/ convert to virtual address
	movl	%eax,EXT(esym)(%edx)	/ store it

/*
 * Fix initial descriptor tables.
 */
	lea	EXT(idt)(%edx),%esi	/ fix IDT
	movl	$IDTSZ,%ecx
	call	fix_desc

	lea	EXT(gdt)(%edx),%esi	/ fix GDT
	movl	$GDTSZ,%ecx
	call	fix_desc

	lea	EXT(ldt)(%edx),%esi	/ fix LDT
	movl	$LDTSZ,%ecx
	call	fix_desc

/*
 * Build initial page table directory and page tables.
 */

	movl	EXT(esym)(%edx),%ebx	/ get end of kernel - virtual address
	addl	$(NBPG-1),%ebx		/ round
	andl	$(-NBPG),%ebx		/ to machine page size
	movl	%ebx,EXT(kpde)(%edx)	/ save as kernel page table directory
	addl	%edx,%ebx		/ convert to physical address

	movl	%ebx,%edi		/ clear page table directory
	movl	$PTES_PER_PAGE,%ecx	/ one page of ptes
	xorl	%eax,%eax
	cld
	rep
	stosl				/ edi now points to next page

/*
 * Use next few pages for page tables.
 */
	addl	$KERNELBASEPDE,%ebx	/ point to pde for kernel base
	movl	%edi,%esi		/ point to end of current pte page

/*
 * Enter 1-1 mappings for kernel and for kernel page tables.
 */
	movl	$INTEL_PTE_KERNEL,%eax	/ set up pte prototype
0:
	cmpl	%esi,%edi		/ at end of pte page?
	jb	1f			/ if so:
	pushl	%eax			/    save template
	movl	%edi,%eax		/    get pte address (physical)
	andl	$(-NBPG),%eax		/    mask out offset in page
	orl	$INTEL_PTE_KERNEL,%eax	/    add pte bits
	movl	%eax,(%ebx)		/    set pde
	addl	$4,%ebx			/    point to next pde
	movl	%edi,%esi		/    point to
	addl	$NBPG,%esi		/    end of new pte page
	popl	%eax			/    restore template
1:
	movl	%eax,(%edi)		/ set pte
	addl	$4,%edi			/ advance to next pte
	addl	$NBPG,%eax		/ advance to next phys page
	cmpl	%edi,%eax		/ have we mapped this pte page yet?
	jb	0b			/ loop if not

/*
 * Zero rest of last pte page.
 */
	xor	%eax,%eax		/* don't map yet */
2:	cmpl	%esi,%edi		/* at end of pte page? */
	jae	3f
	movl	%eax,(%edi)		/* zero mapping */
	addl	$4,%edi
	jmp	2b
3:
	movl	%edi,EXT(first_avail)(%edx)
					/ save first available phys addr

/*
 * pmap_bootstrap will enter rest of mappings.
 */

/*
 * Set up the various tables.
 */
	movl	EXT(kpde)(%edx),%eax	/ get PDE virtual address
	addl	%edx,%eax		/ convert to physical address
	movl	%eax,%cr3		/ set PDE base

	movl	KERNELBASEPDE(%eax),%ecx
					/ point to pte for KERNELBASE
	movl	%ecx,(%eax)		/ set it also as pte for location
					/ 0..3fffff, so that the code
					/ that enters paged mode is mapped
					/ to identical addresses after
					/ paged mode is enabled

	movl	$_pag_start,%ebx	/ first paged code address

	movl	%cr0,%eax
	orl	$0x80000000,%eax
	movl	%eax,%cr0		/ enable paging

	jmp	*%ebx			/ flush prefetch queue

/*
 * We are now paging, and can run with correct addresses.
 */
_pag_start:
	lea	EXT(gdtptr),%eax	/ point to GDT descriptor
	lgdt	(%eax)			/ load GDT pointer

	lea	EXT(idtptr),%eax	/ point to IDT descriptor
	lidt	(%eax)			/ load IDT pointer

	movl	$KERNEL_LDT,%eax	/ get LDT segment
	lldt	%ax			/ load LDT

	pushl	$KERNEL_CS		/ switch to
	pushl	$kstart			/ kernel CS
	lret
kstart:
	movl	$KERNEL_TSS,%eax
	ltr	%ax			/ point to kernel task segment
					/ set up registers:
	mov	$KERNEL_DS,%ax		/     point to data segment
	mov	%ax,%ds
	mov	%ax,%es
	mov	%ax,%ss			/ set kernel stack segment
	lea	EXT(eintstack),%esp	/* and stack pointer ('interrupt' stk) */
	mov	%ax,EXT(ktss)+TSS_SS0	/ set kernel stack segment
					/ for traps to kernel
	call	EXT(machine_startup)	/ run C code
	/*NOTREACHED*/
	hlt

/*
 * Convert a descriptor from fake to real format.
 * %esi	= descriptor table address (physical)
 * %ecx = number of descriptors
 *
 * Fake descriptor format:
 *	bytes 0..3		base 31..0
 *	bytes 4..5		limit 15..0
 *	byte  6			access byte 2 | limit 19..16
 *	byte  7			access byte 1
 *
 * Real descriptor format:
 *	bytes 0..1		limit 15..0
 *	bytes 2..3		base 15..0
 *	byte  4			base 23..16
 *	byte  5			access byte 1
 *	byte  6			access byte 2 | limit 19..16
 *	byte  7			base 31..24
 *
 * Fake gate format:
 *	bytes 0..3		offset
 *	bytes 4..5		selector
 *	byte  6			word count << 4 (to match fake descriptor)
 *	byte  7			access byte 1
 *
 * Real gate format:
 *	bytes 0..1		offset 15..0
 *	bytes 2..3		selector
 *	byte  4			word count
 *	byte  5			access byte 1
 *	bytes 6..7		offset 31..16
 */
fix_desc:
0:
	movw	6(%esi),%bx		/ get access byte
	movb	%bh,%al
	andb	$0x14,%al
	cmpb	$0x04,%al		/ gate or descriptor?
	je	1f

/ descriptor
	movl	0(%esi),%eax		/ get base in eax
	rol	$16,%eax		/ swap 15..0 with 31..16
					/ (15..0 in correct place)
	movb	%al,%bl			/ combine bits 23..16 with ACC1
					/ in bh/bl
	movb	%ah,7(%esi)		/ store bits 31..24 in correct place
	movw	4(%esi),%ax		/ move limit bits 0..15 to word 0
	movl	%eax,0(%esi)		/ store (bytes 0..3 correct)
	movw	%bx,4(%esi)		/ store bytes 4..5
	jmp	2f

/ gate
1:
	movw	4(%esi),%ax		/ get selector
	shrb	$4,%bl			/ shift word count to proper place
	movw	%bx,4(%esi)		/ store word count / ACC1
	movw	2(%esi),%bx		/ get offset 16..31
	movw	%bx,6(%esi)		/ store in correct place
	movw	%ax,2(%esi)		/ store selector in correct place
2:
	addl	$8,%esi			/ bump to next descriptor
	loop	0b			/ repeat
	ret				/ all done

/*
 * put arg in kbd leds and spin a while
 * eats eax, ecx, edx
 */
#define	K_RDWR		0x60
#define	K_CMD_LEDS	0xed
#define	K_STATUS	0x64
#define	K_IBUF_FULL	0x02		/* input (to kbd) buffer full */
#define	K_OBUF_FULL	0x01		/* output (from kbd) buffer full */

ENTRY(set_kbd_leds)
	mov	S_ARG0,%cl		/ save led value
	
0:	inb	$K_STATUS,%al		/ get kbd status
	testb	$K_IBUF_FULL,%al	/ input busy?
	jne	0b			/ loop until not
	
	mov	$K_CMD_LEDS,%al		/ K_CMD_LEDS
	outb	%al,$K_RDWR		/ to kbd

0:	inb	$K_STATUS,%al		/ get kbd status
	testb	$K_OBUF_FULL,%al	/ output present?
	je	0b			/ loop if not

	inb	$K_RDWR,%al		/ read status (and discard)

0:	inb	$K_STATUS,%al		/ get kbd status
	testb	$K_IBUF_FULL,%al	/ input busy?
	jne	0b			/ loop until not
	
	mov	%cl,%al			/ move led value
	outb	%al,$K_RDWR		/ to kbd

	movl	$10000000,%ecx		/ spin
0:	nop
	nop
	loop	0b			/ a while

	ret


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
 * $Log:	locore.s,v $
 * Revision 2.11  91/02/05  17:12:59  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:35:44  mrt]
 * 
 * Revision 2.10  91/01/09  22:41:32  rpd
 * 	Removed k_user_regs.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.9  91/01/08  17:32:10  rpd
 * 	interrupt_returns must check for EFL_VM.
 * 	[90/12/21  14:37:44  rvb]
 * 
 * 	Add trapv86 for VM thread.
 * 	[90/12/19  17:00:56  rvb]
 * 
 * Revision 2.8  91/01/08  15:10:49  rpd
 * 	Replaced thread_bootstrap_user, thread_bootstrap_kernel
 * 	with thread_exception_return, thread_syscall_return.
 * 	Updated mach_trap_table indexing for new layout.
 * 	[90/12/17            rpd]
 * 
 * 	Renamed thread_bootstrap to thread_bootstrap_user.
 * 	Added thread_bootstrap_kernel.
 * 	[90/12/14            rpd]
 * 
 * 	Reorganized the pcb.
 * 	Added copyinmsg, copyoutmsg synonyms for copyin, copyout.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.7  90/12/20  16:36:21  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07  15:43:29  jeffreyh]
 * 
 *
 * Revision 2.6  90/12/04  14:46:11  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:17:03  jsb]
 * 
 * Revision 2.5  90/09/23  17:45:16  jsb
 * 	Added support for iPSC386.
 * 	[90/09/21  16:40:55  jsb]
 * 
 * Revision 2.4  90/08/27  21:57:24  dbg
 * 	Remove interrupt/trap vectors - get from idt.s.
 * 	Fix copyout to check user address on each page boundary.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.3  90/05/21  13:26:44  dbg
 * 	Add inl, outl.
 * 	[90/05/17            dbg]
 * 
 * Revision 2.2  90/05/03  15:33:45  dbg
 * 	Created.
 * 	[90/02/14            dbg]
 * 
 */

/*
 * Fault recovery.
 */
#define	RECOVER_TABLE_START	\
	.text	2		;\
	.globl	_recover_table	;\
_recover_table:			;\
	.text

#define	RECOVER(addr)		\
	.text	2		;\
	.long	9f		;\
	.long	addr		;\
	.text			;\
9:

#define	RECOVER_TABLE_END		\
	.text	2			;\
	.globl	_recover_table_end	;\
_recover_table_end:			;\
	.text

/*
 * Allocate recovery table.
 */
	RECOVER_TABLE_START

/*
 * Trap/interrupt entry points.
 *
 * All traps must create the following save area on the kernel stack:
 *
 *	gs
 *	fs
 *	es
 *	ds
 *	edi
 *	esi
 *	ebp
 *	[ kernel esp - not used ]
 *	ebx
 *	edx
 *	ecx
 *	eax
 *	trap number
 *	error code
 *	eip
 *	cs
 *	eflags
 *	user esp - if from user
 *	user ss  - if from user
 *	es       - if from V86 thread
 *	ds       - if from V86 thread
 *	fs       - if from V86 thread
 *	gs       - if from V86 thread
 *
 * This structure is known to the floating-point emulator.
 */

/*
 * General protection fault.  Check for a GP fault in the kernel_return
 * sequence; if there, report it as a GP fault on the user's instruction.
 *
 * esp->	segment number in error
 *		eip
 *		cs
 *		eflags
 *		old registers (trap is from kernel)
 */
ENTRY(t_gen_prot)
	testl	$3,8(%esp)		/ is trap from kernel mode?
	jne	take_gp_fault		/ if so:
					/ check for the kernel exit sequence
	cmpl	$_kret_iret,4(%esp)	/ on IRET?
	je	gp_iret
	cmpl	$_kret_popl_ds,4(%esp)	/ popping DS?
	je	gp_popl_ds	
	cmpl	$_kret_popl_es,4(%esp)	/ popping ES?
	je	gp_popl_es
	cmpl	$_kret_popl_fs,4(%esp)	/ popping FS?
	je	gp_popl_fs
	cmpl	$_kret_popl_gs,4(%esp)	/ popping GS?
	je	gp_popl_gs
take_gp_fault:				/ if none of the above:
	pushl	$T_GP_FAULT		/ signal
	jmp	EXT(alltraps)		/ GP fault.

/*
 * GP fault on IRET: CS or SS is in error.
 * All registers contain the user's values.
 *
 * on SP is
 *  0	errcode
 *  4	eip
 *  8	cs		--> trapno
 * 12	efl		--> errcode
 * 16	user eip
 * 20	user cs
 * 24	user eflags
 * 28	user esp
 * 32	user ss
 */
gp_iret:
	movl	%eax,4(%esp)		/* save eax (we don't need saved eip) */
	popl	%eax			/ get error code
	movl	%eax,12-4(%esp)		/ put in user errcode
	popl	%eax			/ restore eax
	movl	$T_GP_FAULT,(%esp)	/ set trap number
	jmp	EXT(alltraps)		/ take GP fault

/*
 * Fault restoring a segment register.  The user's registers are still
 * saved on the stack.  The offending segment register has not been
 * popped.
 */
gp_popl_ds:
	popl	%eax			/ get error code (regs saved)
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_es			/ (DS on top of stack)
gp_popl_es:
	popl	%eax			/ get error code (regs saved)
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_fs			/ (ES on top of stack)
gp_popl_fs:
	popl	%eax			/ get error code (regs saved)
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_gs			/ (FS on top of stack)
gp_popl_gs:
	popl	%eax			/ get error code (regs saved)
	addl	$12,%esp		/ pop stack to user regs
	jmp	push_segregs		/ (GS on top of stack)

push_es:
	pushl	%es			/ restore es,
push_fs:
	pushl	%fs			/ restore fs,
push_gs:
	pushl	%gs			/ restore gs.
push_segregs:
	movl	%eax,R_ERR(%esp)	/ set bad segment as error code
	movl	$T_GP_FAULT,R_TRAPNO(%esp) / and GP trap
	jmp	_trap_set_segs		/ take trap

/*
 * Debug trap.  Check for single-stepping across system call into
 * kernel.  If this is the case, taking the debug trap has turned
 * off single-stepping - save the flags register with the trace
 * bit set.
 */
ENTRY(t_debug)
	cmpl	$syscall_entry,(%esp)	/ system call entry?
	jne	0f			/ if so:
					/ flags are sitting where syscall
					/ wants them
	addl	$8,%esp			/ remove eip/cs
	jmp	syscall_entry_2		/ continue system call entry

0:	pushl	$0			/ otherwise:
	pushl	$1			/ handle as normal
	jmp	_alltraps		/ debug fault

/*
 * All 'exceptions' enter here with:
 *	esp->   trap number
 *		error code
 *		old eip
 *		old cs
 *		old eflags
 *		old esp		if trapped from user
 *		old ss		if trapped from user
 */
ENTRY(alltraps)
	pusha			/ save the general registers
	pushl	%ds		/ and the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

LEXT(trap_set_segs)
	mov	$KERNEL_DS,%eax	/ switch to kernel data segment
	mov	%ax,%ds
	mov	%ax,%es
LEXT(take_trap)
	movl	R_EFLAGS(%esp), %eax
	btl	$17,%eax
	jc	v86_trap_handler

	pushl	%esp		/ pass register save area as parameter
				/ to trap
	call	EXT(trap)	/ call generic trap routine
	addl	$4,%esp		/ pop parameter

	.globl	EXT(kernel_return)
LEXT(kernel_return)
	testl	$3,R_CS(%esp)	/ was caller in user or kernel mode?
	jz	1f		/ if user mode: check for AST
0:
	cmpl	$0,EXT(need_ast)
	je	1f		/ if we need AST:
	
	pushl	%esp		/ push register save pointer again
	call	EXT(astintr)	/ take AST
	addl	$4,%esp		/ pop parameter
	jmp	0b		/ and check again (rare)

1:
_kret_popl_gs:
	popl	%gs		/ restore segment registers
_kret_popl_fs:
	popl	%fs
_kret_popl_es:
	popl	%es
_kret_popl_ds:
	popl	%ds
	popa			/ restore general registers
	addl	$8,%esp		/ discard trap number and error code
_kret_iret:
	iret			/ return from interrupt

	.globl	EXT(interrupt_return)
LEXT(interrupt_return)
	movl	R_EFLAGS(%esp), %eax
	btl	$17,%eax
	jc	0b
	jmp	EXT(kernel_return)

v86_trap_handler:
	pushl	%esp		/ pass register save area as parameter
				/ to trap
	call 	EXT(trapv86)
	addl	$4,%esp
	jmp	0b

/*
 *	Called as a function, makes the current thread
 *	return from the kernel as if from an exception.
 */

	.globl	EXT(thread_exception_return)
	.globl	EXT(thread_bootstrap_return)
LEXT(thread_exception_return)
LEXT(thread_bootstrap_return)
	movl	%esp,%ecx			/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	addl	$(-K_SIZE-R_SIZE+1),%ecx
	movl	%ecx,%esp			/ pop the stack
	jmp	EXT(interrupt_return)

/*
 *	Called as a function, makes the current thread
 *	return from the kernel as if from a syscall.
 *	Takes the syscall's return code as an argument.
 */

	.globl	EXT(thread_syscall_return)
LEXT(thread_syscall_return)
	movl	S_ARG0,%eax			/ get return value
	movl	%esp,%ecx			/ get kernel stack
	or	$(KERNEL_STACK_SIZE-1),%ecx
	addl	$(-K_SIZE-R_SIZE+1),%ecx
	movl	%ecx,%esp			/ pop the stack
	movl	%eax,R_EAX(%esp)		/ save return value
	jmp	EXT(kernel_return)

/*
 * All interrupts enter here.
 */
ENTRY(all_intrs)
	pusha			/ save the general registers
	pushl	%ds		/ and the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

	movl	$KERNEL_DS,%eax	/ switch to kernel data segments
	mov	%ax,%ds
	mov	%ax,%es

	jmp	EXT(interrupt)	/ call generic interrupt routine
				/ which returns to kernel_return

/*
 * System call enters through a call gate.  Flags are not saved -
 * we must shuffle stack to look like trap save area.
 *
 * esp->	old eip
 *		old cs
 *		old esp
 *		old ss
 *
 * eax contains system call number.
 */
ENTRY(syscall)
syscall_entry:
	pushf				/ save flags as soon as possible
syscall_entry_2:
	pushl	%eax			/ save system call number
	pushl	$0			/ clear trap number slot
		
	pusha				/ save the general registers
	pushl	%ds			/ and the segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs

	mov	$KERNEL_DS,%edx		/ switch to kernel data segment
	mov	%dx,%ds
	mov	%dx,%es

/*
 * Shuffle eflags,eip,cs into proper places
 */
	movl	R_EIP(%esp),%ebx	/ eflags are in EIP slot
	movl	R_CS(%esp),%ecx		/ eip is in CS slot
	movl	R_EFLAGS(%esp),%edx	/ cs is in EFLAGS slot
	movl	%ecx,R_EIP(%esp)	/ fix eip
	movl	%edx,R_CS(%esp)		/ fix cs
	movl	%ebx,R_EFLAGS(%esp)	/ fix eflags
/*
 * Check for MACH or emulated system call
 */
	movl	_active_threads,%ebx	/ point to current thread
	movl	TH_TASK(%ebx),%edx	/ point to task
	movl	TASK_EMUL(%edx),%edx	/ get emulation vector
	orl	%edx,%edx		/ if none,
	je	syscall_native		/    do native system call
	movl	%eax,%ecx		/ copy system call number
	subl	$EML_MIN_SYSCALL,%ecx	/ get displacement into syscall
					/ vector table
	jl	syscall_native		/ too low - native system call
	cmpl	DISP_COUNT(%edx),%ecx	/ check range
	jg	syscall_native		/ too high - native system call
	movl	DISP_VECTOR(%edx,%ecx,4),%edx
					/ get the emulation vector
	orl	%edx,%edx		/ emulated system call if not zero
	jnz	syscall_emul

/*
 * Native system call.
 */
syscall_native:
	movl	TH_KERNEL_STACK(%ebx),%ecx	/ point to kernel stack
	movl	%esp,%ebx		/ save in %ebx (across syscall)
	negl	%eax			/ get system call number
	jl	mach_call_range		/ out of range if it was positive
	cmpl	EXT(mach_trap_count),%eax
					/ check system call table bounds
	jg	mach_call_range		/ error if out of range
	shll	$4,%eax			/ manual indexing
	movl	EXT(mach_trap_table+4)(%eax),%edx
					/ get procedure pointer
	movl	EXT(mach_trap_table)(%eax),%ecx
					/ get number of arguments
	jecxz	mach_call_call		/ skip argument copy if none

	movl	R_UESP(%ebx),%esi	/ get user stack pointer
	lea	4(%esi,%ecx,4),%esi	/ skip user return address,
					/ and point past last argument
	cmpl	$VM_MAX_ADDRESS,%esi	/ in user space?
	ja	mach_call_addr		/ address error if not

0:	subl	$4,%esi
	RECOVER(mach_call_addr)
	pushl	(%esi)			/ push argument on stack
	loop	0b			/ loop for all arguments

mach_call_call:
	call	*%edx			/ call procedure
	movl	%ebx,%esp		/ pop parameters from stack
	movl	%eax,R_EAX(%ebx)	/ save return value in EAX slot
	jmp	EXT(kernel_return)	/ return from kernel

/*
 * Address out of range.  Change to page fault.
 * %esi holds failing address.
 */
mach_call_addr:
	movl	%ebx,%esp		/ clean parameters from stack
	movl	%esi,%cr2		/ save error address (!)
	movl	$T_PAGE_FAULT,R_TRAPNO(%esp)
					/ set page-fault trap
	movl	$T_PF_USER_READ,R_ERR(%esp)
					/ set error code - read user space
	jmp	EXT(take_trap)		/ treat as a trap

/*
 * System call out of range.  Treat as invalid-instruction trap.
 * (? general protection?)
 */
mach_call_range:
	movl	%ebx,%esp		/ clean parameters from stack
	movl	$T_INVALID_OPCODE,R_TRAPNO(%esp)
					/ set invalid-operation trap
	movl	$0,R_ERR(%esp)		/ clear error code
	jmp	EXT(take_trap)		/ treat as a trap

/*
 * User space emulation of system calls.
 * ebx - user address to handle syscall
 *
 * User stack will become:
 * uesp->	eflags
 *		eip
 * eax still contains syscall number.
 */
syscall_emul:
	movl	R_UESP(%esp),%edi	/ get user stack pointer
	cmpl	$VM_MAX_ADDRESS,%edi	/ in user space?
	ja	syscall_addr		/ address error if not
	subl	$8,%edi			/ push space for new arguments
	cmpl	$VM_MIN_ADDRESS,%edi	/ still in user space?
	jb	syscall_addr		/ error if not
	movl	R_EFLAGS(%esp),%eax	/ move flags
	RECOVER(syscall_addr)
	movl	%eax,0(%edi)		/ to user stack
	movl	R_EIP(%esp),%eax	/ move eip
	RECOVER(syscall_addr)
	movl	%eax,4(%edi)		/ to user stack
	movl	%edi,R_UESP(%esp)	/ set new user stack pointer
	movl	%edx,R_EIP(%esp)	/ change return address to trap
	jmp	EXT(kernel_return)	/ return to user

/*
 * Address error - address is in %edi.
 */
syscall_addr:
	movl	%edi,%cr2		/ set fault address (!)
	movl	$T_PAGE_FAULT,R_TRAPNO(%esp)
					/ set page-fault trap
	movl	$T_PF_USER_READ,R_ERR(%esp)
					/ set error code - read user space
	jmp	EXT(take_trap)		/ treat as a trap

/*
 * Copy from user address space.
 * arg0:	user address
 * arg1:	kernel address
 * arg2:	byte count
 */
ENTRY(copyin)
Entry(copyinmsg)
	movl	S_ARG0,%edx		/ get user start address
	cmpl	$VM_MIN_ADDRESS,%edx	/ within user space?
	jb	copy_fail
	cmpl	$VM_MAX_ADDRESS,%edx
	jae	copy_fail
	movl	%edx,%eax
	addl	S_ARG2,%eax		/ get user end address + 1
	cmpl	%edx,%eax
	jb	copy_fail		/ fail if wrap-around
	cmpl	$VM_MAX_ADDRESS,%eax
	ja	copy_fail		/ or above user range
	
	FRAME
	pushl	%esi
	pushl	%edi			/ save registers

	movl	%edx,%esi		/ user is source
	movl	B_ARG1,%edi		/ kernel is dest
	movl	B_ARG2,%ecx		/ set count
	cld				/ count up
	RECOVER(copy_fail_frame)
	rep
	movsb				/ move
	xorl	%eax,%eax		/ return 0 for success
copy_ret:
	popl	%edi			/ restore registers
	popl	%esi
	leave				/ pop frame
	ret				/ and return

copy_fail_frame:
	movl	$1,%eax			/ return 1 for failure
	jmp	copy_ret		/ pop frame and return

/*
 * Copy to user address space.
 * arg0:	kernel address
 * arg1:	user address
 * arg2:	byte count
 */
ENTRY(copyout)
Entry(copyoutmsg)
	movl	S_ARG1,%edx		/ get user start address
	cmpl	$VM_MIN_ADDRESS,%edx	/ within user space?
	jb	copy_fail
	cmpl	$VM_MAX_ADDRESS,%edx
	jae	copy_fail
	movl	%edx,%eax
	addl	S_ARG2,%eax		/ get user end address + 1
	cmpl	%edx,%eax
	jb	copy_fail		/ fail if wrap-around
	cmpl	$VM_MAX_ADDRESS,%eax
	ja	copy_fail		/ or above user range
	
	FRAME
	pushl	%esi
	pushl	%edi			/ save registers

	movl	B_ARG1,%edi		/ user is destination
	movl	B_ARG0,%esi		/ kernel is source

/*
 * Check whether user address space is writable
 * before writing to it - hardware is broken.
 */
	.globl	_copyout_retry
_copyout_retry:
	movl	%cr3,%edx		/ point to page directory
	movl	%edi,%eax		/ get page directory bits
	shrl	$PDESHIFT,%eax		/ from user address
	movl	KERNELBASE(%edx,%eax,4),%edx
					/ get page directory pointer
	testl	$PTE_V,%edx		/ present?
	jz	0f			/ if not, fault is OK
	andl	$PTE_PFN,%edx		/ isolate page frame address
	movl	%edi,%eax		/ get page table bits
	shrl	$PTESHIFT,%eax
	andl	$PTEMASK,%eax		/ from user address
	leal	KERNELBASE(%edx,%eax,4),%edx
					/ point to page table entry
	movl	(%edx),%eax		/ get it
	testl	$PTE_V,%eax		/ present?
	jz	0f			/ if not, fault is OK
	testl	$PTE_W,%eax		/ writable?
	jnz	0f			/ OK if so
/*
 * Not writable - must fake a fault.  Turn off access to the page.
 */
	andl	$PTE_INVALID,(%edx)	/ turn off valid bit
	movl	%cr3,%eax		/ invalidate TLB
	movl	%eax,%cr3
0:
/*
 * Copy only what fits on the current destination page.
 * Check for write-fault again on the next page.
 */
	leal	NBPG(%edi),%edx		/ point to
	andl	$(-NBPG),%edx		/ start of next page
	subl	%edi,%edx		/ get number of bytes to that point
	cmpl	B_ARG2,%edx		/ bigger than count?
	jle	1f			/ if so,
	movl	B_ARG2,%edx		/ use count
1:
	movl	%edx,%ecx		/ move this many bytes
	cld				/ count up
	RECOVER(copy_fail_frame)
	.globl	_copyout_copy
_copyout_copy:
	rep
	movsb				/ move
	subl	%edx,B_ARG2		/ decrement count
	jg	_copyout_retry		/ restart on next page if not done
	xorl	%eax,%eax		/ return 0 for success
	popl	%edi			/ restore registers
	popl	%esi
	leave				/ pop frame
	ret				/ and return

copy_fail:
	movl	$1,%eax			/ return 1 for failure
	ret
/*
 * FPU routines.
 */

/*
 * Initialize FPU.
 */
ENTRY(_fninit)
	fninit
	ret

/*
 * Read control word
 */
ENTRY(_fstcw)
	pushl	%eax		/ get stack space
	fstcw	(%esp)
	popl	%eax
	ret

/*
 * Set control word
 */
ENTRY(_fldcw)
	fldcw	4(%esp)
	ret

/*
 * Read status word
 */
ENTRY(_fnstsw)
	xor	%eax,%eax		/ clear high 16 bits of eax
	fnstsw	%ax			/ read FP status
	ret

/*
 * Clear FPU exceptions
 */
ENTRY(_fnclex)
	fnclex
	ret

/*
 * Clear task-switched flag.
 */
ENTRY(_clts)
	clts
	ret

/*
 * Save complete FPU state.  Save error for later.
 */
ENTRY(_fpsave)
	movl	4(%esp),%eax		/ get save area pointer
	fnsave	(%eax)			/ save complete state, including
					/ errors
	ret

/*
 * Restore FPU state.
 */
ENTRY(_fprestore)
	movl	4(%esp),%eax		/ get save area pointer
	frstor	(%eax)			/ restore complete state
	ret

/*
 * Set cr3
 */
ENTRY(set_cr3)
	movl	4(%esp),%eax		/ get new cr3 value
	movl	%eax,%cr3		/ load it
	ret

/*
 * Read cr3
 */
ENTRY(get_cr3)
	movl	%cr3,%eax
	ret

/*
 * Flush TLB
 */
ENTRY(flush_tlb)
	movl	%cr3,%eax		/ flush tlb by reloading CR3
	movl	%eax,%cr3		/ with itself
	ret

/*
 * Read cr2
 */
ENTRY(get_cr2)
	movl	%cr2,%eax
	ret

/*
 * Read ldtr
 */
ENTRY(get_ldt)
	xorl	%eax,%eax
	sldt	%ax
	ret

/*
 * Set ldtr
 */
ENTRY(set_ldt)
	lldt	4(%esp)
	ret

/*
 * Set task-switched flag.
 */
ENTRY(_setts)
	movl	%cr0,%eax		/ get cr0
	orl	$0x08,%eax		/ or in TS bit
	movl	%eax,%cr0		/ set cr0
	ret

/*
 * void outb(unsigned char *io_port,
 *	     unsigned char byte)
 *
 * Output a byte to an IO port.
 */
ENTRY(outb)
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outb	%al,%dx			/ send it out
#ifdef	iPSC386
	mull	%ecx			/ Delay a little to make H/W happy
#endif	iPSC386
	ret

/*
 * unsigned char inb(unsigned char *io_port)
 *
 * Input a byte from an IO port.
 */
ENTRY(inb)
	movl	S_ARG0,%edx		/ IO port address
	xor	%eax,%eax		/ clear high bits of register
	inb	%dx,%al			/ get the byte
#ifdef	iPSC386
/ Do a long multiply to delay a little to make H/W happy, must
/ save and restore EAX which is used to hold result of multiply
	pushl	%eax
	mull	%ecx
	popl	%eax
#endif	iPSC386
	ret

/*
 * void outw(unsigned short *io_port,
 *	     unsigned short word)
 *
 * Output a word to an IO port.
 */
ENTRY(outw)
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outw	%ax,%dx			/ send it out
	ret

/*
 * unsigned short inw(unsigned short *io_port)
 *
 * Input a word from an IO port.
 */
ENTRY(inw)
	movl	S_ARG0,%edx		/ IO port address
	xor	%eax,%eax		/ clear high bits of register
	inw	%dx,%ax			/ get the word
	ret

/*
 * void outl(unsigned int *io_port,
 *	     unsigned int byte)
 *
 * Output an int to an IO port.
 */
ENTRY(outl)
	movl	S_ARG0,%edx		/ IO port address
	movl	S_ARG1,%eax		/ data to output
	outl	%eax,%dx		/ send it out
	ret

/*
 * unsigned int inl(unsigned int *io_port)
 *
 * Input an int from an IO port.
 */
ENTRY(inl)
	movl	S_ARG0,%edx		/ IO port address
	inl	%dx,%eax		/ get the int
	ret

/*
 * void loutb(unsigned byte *io_port,
 *	      unsigned byte *data,
 *	      unsigned int count)
 *
 * Output an array of bytes to an IO port.
 */
ENTRY(loutb)
	pushl	%ebp			/ set up
	movl	%esp,%ebp		/ frame
	pushl	%esi			/ save register
	movl	B_ARG0,%edx		/ get io port number
	movl	B_ARG1,%esi		/ get data address
	movl	B_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	outsb				/ output

	popl	%esi			/ restore register
	leave				/ clean frame
	ret				/ exit


/*
 * void loutw(unsigned short *io_port,
 *	      unsigned short *data,
 *	      unsigned int count)
 *
 * Output an array of shorts to an IO port.
 */
ENTRY(loutw)
	pushl	%ebp			/ set up
	movl	%esp,%ebp		/ frame
	pushl	%esi			/ save register
	movl	B_ARG0,%edx		/ get io port number
	movl	B_ARG1,%esi		/ get data address
	movl	B_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	outsw				/ output

	popl	%esi			/ restore register
	leave				/ clean frame
	ret				/ exit


/*
 * void linb(unsigned char *io_port,
 *	     unsigned char *data,
 *	     unsigned int count)
 *
 * Input an array of bytes from an IO port.
 */
ENTRY(linb)
	pushl	%ebp			/ set up
	movl	%esp,%ebp		/ frame
	pushl	%edi			/ save register
	movl	B_ARG0,%edx		/ get io port number
	movl	B_ARG1,%edi		/ get data address
	movl	B_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	insb				/ input

	popl	%edi			/ restore register
	leave				/ clean frame
	ret				/ exit


/*
 * void linw(unsigned short *io_port,
 *	     unsigned short *data,
 *	     unsigned int count)
 *
 * Input an array of shorts from an IO port.
 */
ENTRY(linw)
	pushl	%ebp			/ set up
	movl	%esp,%ebp		/ frame
	pushl	%edi			/ save register
	movl	B_ARG0,%edx		/ get io port number
	movl	B_ARG1,%edi		/ get data address
	movl	B_ARG2,%ecx		/ get count

	cld				/ count up

	rep
	insw				/ input

	popl	%edi			/ restore register
	leave				/ clean frame
	ret				/ exit


/*
 * Done with recovery table.
 */
	RECOVER_TABLE_END

/*
 * Waste 10 microseconds.
 */
ENTRY(tenmicrosec)
	movl	EXT(microdata),%ecx	/ cycle count for 10 microsecond loop
tenmicroloop:
	loop	tenmicroloop
	ret

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
