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
 * $Log:	io_emulate.c,v $
 * Revision 2.4  91/02/05  17:12:27  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:35:06  mrt]
 * 
 * Revision 2.3  90/06/02  14:48:35  rpd
 * 	Converted to new IPC.
 * 	[90/06/01            rpd]
 * 
 * Revision 2.2  90/05/21  13:26:32  dbg
 * 	Created.
 * 	[90/05/09            dbg]
 * 
 */

#include <mach/boolean.h>
#include <mach/port.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <i386/thread.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_right.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_entry.h>

extern ipc_port_t	iopl_device_port;

/*
 * Emulate IO instructions if task has send rights to the IOPL device port.
 */
boolean_t
emulate_io(regs)
	register struct i386_saved_state *regs;
{
	unsigned char	inst[4];	/* addr/rep/ins/imm */
	int		byte;		/* instruction byte */
	boolean_t	word = FALSE;	/* 16-bit data prefix */
	int		nb;		/* number of bytes in instruction */
	int		io_port;

	/*
	 * Get the instruction.
	 */
	if (copyin(regs->eip, &inst, sizeof(inst)))
	    return (FALSE);

	/*
	 * Check it.
	 */
	nb = 0;
	byte = inst[nb++];
	if (byte == 0x66) {
	    byte = inst[nb++];
	    word = TRUE;
	}
	switch (byte) {
	    case 0xE4:		/* inb imm */
	    case 0xE5:		/* inl imm */
	    case 0xE6:		/* outb imm */
	    case 0xE7:		/* outl imm */
		/* port is immediate byte */
		io_port = inst[nb++];
		break;

	    case 0xEC:		/* inb dx */
	    case 0xED:		/* inl dx */
	    case 0xEE:		/* outb dx */
	    case 0xEF:		/* outl dx */
		/* port is in DX register */
		io_port = regs->edx & 0xFFFF;
		break;

	    case 0x6C:		/* insb */
	    case 0x6D:		/* insl */
	    case 0x6E:		/* outsb */
	    case 0x6F:		/* outsl */
		return (FALSE);		/* cant do string IO yet */

	    default:
		return (FALSE);		/* not an IO instruction */
	}

	/*
	 *	Check for send rights to the IOPL device port.
	 */
	if (iopl_device_port == IP_NULL)
	    return (FALSE);	/* no one has open */
	{
	    ipc_space_t	space = current_space();
	    mach_port_t	name;
	    ipc_entry_t	entry;
	    boolean_t	has_rights = FALSE;

	    is_write_lock(space);
	    assert(space->is_active);

	    if (ipc_right_reverse(space, (ipc_object_t) iopl_device_port,
				  &name, &entry)) {
		/* iopl_device_port is locked and active */
		if (entry->ie_bits & MACH_PORT_TYPE_SEND)
		    has_rights = TRUE;
		ip_unlock(iopl_device_port);
	    }

	    is_write_unlock(space);
	    if (!has_rights)
		return (FALSE);	/* no rights */
	}

	/*
	 * Emulate the instruction.
	 */
	switch (byte) {
	    case 0xE4:		/* inb imm */
	    case 0xEC:		/* inb dx */
		regs->eax = (regs->eax & 0xffffff00)
			| inb(io_port);
		break;

	    case 0xE5:		/* inl imm */
	    case 0xED:		/* inl dx */
		if (word) {
		    regs->eax = (regs->eax & 0xffff0000)
			| inw(io_port);
		}
		else {
		    regs->eax = inl(io_port);
		}
		break;

	    case 0xE6:		/* outb imm */
	    case 0xEE:		/* outb dx */
		outb(io_port, regs->eax & 0xff);
		break;

	    case 0xE7:		/* outl imm */
	    case 0xEF:		/* outl dx */
		if (word) {
		    outw(io_port, regs->eax & 0xffff);
		}
		else {
		    outl(io_port, regs->eax);
		}
		break;
	}
	/*
	 * Advance EIP over instruction.
	 */
	regs->eip += nb;

	return (TRUE);
}
