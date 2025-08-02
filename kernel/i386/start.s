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


