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

