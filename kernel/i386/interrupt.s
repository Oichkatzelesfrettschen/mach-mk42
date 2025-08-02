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
 * $Log:	interrupt.s,v $
 * Revision 2.8  91/02/05  17:12:22  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:58  mrt]
 * 
 * Revision 2.7  91/01/08  17:32:06  rpd
 * 	Need special interrupt_return
 * 	[90/12/21  14:36:12  rvb]
 * 
 * Revision 2.6  90/12/20  16:35:58  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07  15:43:38  jeffreyh]
 * 
 * Revision 2.5  90/12/04  14:46:08  jsb
 * 	iPSC2 -> iPSC386.
 * 	[90/12/04  11:16:47  jsb]
 * 
 * Revision 2.4  90/11/26  14:48:33  rvb
 * 	Change Prime copyright as per Peter J. Weyman authorization.
 * 	[90/11/19            rvb]
 * 
 * Revision 2.3  90/09/23  17:45:14  jsb
 * 	Added support for iPSC386.
 * 	[90/09/21  16:40:09  jsb]
 * 
 * Revision 2.2  90/05/03  15:27:54  dbg
 * 	Stole from Prime.
 * 	Pass new parameters to clock_interrupt (no longer called
 * 	hardclock).  Set curr_ipl correctly around call to clock_interrupt.
 * 	Moved softclock logic to splx.
 * 	Added kdb_kintr to find registers for kdb.
 * 	[90/02/14            dbg]
 * 
 */

/*
Copyright (c) 1988,1989 Prime Computer, Inc.  Natick, MA 01760
All Rights Reserved.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and
without fee is hereby granted, provided that the above
copyright notice appears in all copies and that both the
copyright notice and this permission notice appear in
supporting documentation, and that the name of Prime
Computer, Inc. not be used in advertising or publicity
pertaining to distribution of the software without
specific, written prior permission.

THIS SOFTWARE IS PROVIDED "AS IS", AND PRIME COMPUTER,
INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT SHALL PRIME COMPUTER, INC.  BE LIABLE FOR ANY
SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR
OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <i386/asm.h>
#include <i386/ipl.h>
#include <i386/pic.h>
#include <assym.s>

/*
 *	Jumped to from locore.s.  The register save area is on top
 *	of the stack.  All registers are available.
 */
ENTRY(interrupt)

/ 	1a. Now we must acknowledge the interrupt and issue an EOI command to 
/	the pics, we send a NON-SPECIFIC EOI, as we assume that the pic 
/	automatically interrupts us with only the highest priority interrupt.

	movl	EXT(master_icw),%edx	/  2 / EOI for master.
	movw	EXT(PICM_OCW2),%ax	/  2 /
	OUTB				/  4 /

	movw	EXT(PICS_OCW2),%ax	/  2 / EOI for slave.
	addw	$SIZE_PIC,%dx		/  2 /
	OUTB				/  4 /

/ 	2. Now we must change the interrupt priority level, with interrupts 
/	turned off.  First we get the interrupt number off the stack and get
/	the interrupt level associated with it, then we call set_spl().

	movl	R_TRAPNO(%esp), %edi		/  2 /
	subl	$0x40,%edi			/ interrupt vectors we use
						/ start at 0x40, not 0

/	check for spurious interrupt
	movl	EXT(master_icw), %edx
#ifdef	AT386
/	AT slaves the second pic on IRQ2
	cmpl	$2, %edi			/ IRQ2
#else	AT386
/	EXL and iPSC386 slave the second pic on IRQ7
	cmpl	$7, %edi			/ IRQ7
#endif	AT386
	je	int_check
	cmpl	$15, %edi			/ IRQ15
	jne	int_ok
	addw	$SIZE_PIC,%dx			/  2 /
int_check:
	INB					/ read ISR
	testb	$0x80, %al			/ return if IS7 is off
	jz	interrupt_return

int_ok:
	movzbl	EXT(intpri)(%edi), %eax		/  4 / intpri[int#]  
	call	EXT(set_spl)			/ ** /
						/ interrupts are enabled
/ 
/ 	4. Otherwise, re-enable interrupts and call the relevant interrupt 
/	handler as per the ivect[] array set up in pic_init().
no_spl:
	pushl	%eax			/  2 / save old IPL
/	pushl	%edi			/  2 / push int# as int handler arg
	pushl	EXT(iunit)(,%edi,4)	/  2 / push unit# as int handler arg
	movl	$EXT(ivect), %ebx	/  2 / get base of ivect[] array
	call	*(%ebx, %edi, 4)	/  4 / *ivect[int#]()
return_from_interrupt:			/ label for kdb_kintr
	addl	$4,%esp			/ remove interrupt number from stack
	cli				/  3 / disable interrupts

/	 5. Having dealt with the interrupt now we must return to the previous 
/	 interrupt priority level, this is done with interrupts turned off.

	popl	%eax			/ get old IPL from stack
	cmpl	EXT(curr_ipl), %eax	/ if different from current IPL,
	je	no_splx
	call	EXT(set_spl)		/ reset IPL to old value
no_splx:

/	Return to caller.

interrupt_return:
	jmp	EXT(interrupt_return)	/ use common interrupt return code

/*
 * kdb_kintr:	enter kdb from keyboard interrupt.
 * Chase down the stack frames until we find one whose return
 * address is the interrupt handler.   At that point, we have:
 *
 * frame->	saved %ebp
 *		return address == return_from_interrupt
 *		ivect
 *		saved SPL
 *		register save area
 *
 * Call kdb, passing it that register save area.
 */
ENTRY(kdb_kintr)
	movl	%ebp,%eax		/* save caller's frame pointer */
	pushl	%ebp			/ make our frame
	movl	%esp,%ebp
	movl	$return_from_interrupt,%edx
					/ get interrupt return address for
					/ check
0:	cmpl	4(%eax),%edx		/ does this frame return to
					/ interrupt handler?
	je	1f			/ if not:
	movl	(%eax),%eax		/ try next frame
	jmp	0b
1:					/ found frame
	lea	16(%eax),%eax		/ point to register save area
	pushl	%eax			/ push it
	call	EXT(kdb_kbd_trap)	/ call kdb
	leave				/ exit frame
	ret

