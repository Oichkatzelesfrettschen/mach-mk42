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
 * Revision 2.9  91/02/14  14:35:43  mrt
 * 	Added flag not to invoke vm_fault(), used in crashed kernels.
 * 	[91/02/12  12:15:09  af]
 * 
 * Revision 2.8  91/02/05  17:47:51  mrt
 * 	Added author notices
 * 	[91/02/04  11:21:54  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:25:29  mrt]
 * 
 * Revision 2.7  91/01/08  15:49:22  rpd
 * 	Changed kdb_trap to return TRUE if it actually enters kdb.
 * 	[90/12/23            rpd]
 * 	Fixed to use kdbsplhigh/kdbsplx.
 * 	[90/11/26            rpd]
 * 
 * Revision 2.6  90/12/05  23:37:26  af
 * 
 * 
 * Revision 2.5  90/12/05  20:49:41  af
 * 	Adapts for new drivers, made clear we do not expect
 * 	any environment pointer in kdb_init().
 * 	[90/12/03            af]
 * 
 * Revision 2.4  90/10/25  14:46:35  rwd
 * 	Support for watchpoints.
 * 	[90/10/16            rpd]
 * 
 * Revision 2.3  90/09/09  14:33:17  rpd
 * 	Reversed return code in kdb_intr, so that the do key press does not
 * 	generate garbage chars (if kdb enabled).
 * 	[90/08/30  17:03:22  af]
 * 
 * Revision 2.2  90/08/27  22:07:24  dbg
 * 	Got rid of ddb_regs, exception status is kept on stack.
 * 	I still have to decide to switch over to dbg's simpler
 * 	way to access memory or not. [how do you trace another
 * 	thread's user stack in his way ?]
 * 	Created.
 * 	[90/08/14            af]
 * 
 */
/*
 *	File: db_interface.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/90
 *
 *	Interface to new debugger.
 *
 */
#include <sys/reboot.h>

#include <mips/pmap.h>
#include <mips/prom_interface.h>
#include <mips/mips_cpu.h>

#include <mach/vm_param.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <kern/task.h>
#include <kern/thread.h>

#include <mips/thread.h>
#include <mips/db_machdep.h>
#include <mips/setjmp.h>

/*
 *  kdb_init - initialize kernel debugger
 */

kdb_init(argc,argv)
{
	extern char end[];

	if ((boothowto & RB_KDB) == 0)	/* will never work */
		return;
	read_mips_symtab(end);
	kdb_enable();
}

/*
 *  kdb_enable - enable kdb usage
 */
kdb_enable()
{
	register struct restart_blk *rb = (struct restart_blk *)RESTART_ADDR;

	extern int kdb_breakpoint(), halt_all_cpus();

	if (boothowto & RB_KDB) {
		rb-> rb_bpaddr = kdb_breakpoint;
	} else
		rb-> rb_bpaddr = halt_all_cpus;
	init_restartblk();
}


/*
 * Received keyboard interrupt sequence.
 */
int	kdbactive = 0;

kdb_kintr()
{
	if (kdbactive == 0 && (boothowto & RB_KDB)) {
		printf("\n\nkernel: keyboard interrupt\n");
		gimmeabreak();
		return 1;
	}
	return 0;
}

/*
 *  kdb_trap - field a BPT trap
 *
 *  Esp points to the saved register state
 *
 *  Flag is TRUE when we are called from trap() [because of a fatal error],
 *  it is FALSE when we are called from kdb_breakpoint().  Only in this
 *  latter case is the TLB info in *esp valid.
 */
vm_map_t db_curmap;
jmp_buf_t *db_nofault = 0;

kdb_trap(esp, flag)
	register struct mips_saved_state *esp;
{
	int    type, s;
	struct mips_saved_state *old_exc_frame;

	if ((boothowto&RB_KDB) == 0)
		return(0);

	s = kdbsplhigh();	/* if not already */

#ifdef notyet
/* XXX use setjmp or something to catch recursive traps */
	if (db_nofault) {
		jmp_buf_t *lifesaver = db_nofault;
		db_nofault = 0;
		kdbsplx(s);
		_longjmp(lifesaver, 1);
		/*NOTREACHED*/
	}
#endif

	type = esp->cause & CAUSE_EXC_MASK;
	if (flag) {
		esp->tlb_low = 0xbad00bad;
		esp->tlb_high = 0xbad00bad;
		esp->tlb_index = 0xbad00bad;
		esp->tlb_context = 0xbad00bad;

		if (flag == 1)
			kdbprinttrap(type >> CAUSE_EXC_SHIFT, esp);
	}

	/*
	 * Make things reentrant, so that we can debug the debugger
	 */
	old_exc_frame = db_cur_exc_frame;
	db_cur_exc_frame = esp;

	/*
	 * Get ourselves a default VM map
	 */
	db_curmap = kernel_map;
	if (current_thread()) {
		unsigned kdbpeek();
		task_t tk = (task_t)kdbpeek(&(current_thread()->task));
		if (tk)
			db_curmap = (vm_map_t)kdbpeek(&(tk->map));
	}

	kdbactive++;
	dz_pollc(0, TRUE);
	db_trap(type, flag);
	dz_pollc(0, FALSE);
	kdbactive--;

	db_cur_exc_frame = old_exc_frame;

	if (!flag && isa_kbreak(kdbpeek(esp->pc)))
		esp->pc += 4;	/* No loops! */

	kdbsplx(s);
	return(1);
}

#define TRAP_TYPES 17
char	*kdb_trap_names[TRAP_TYPES] = {
					/* Hardware trap codes */
	"Interrupt",
	"TLB mod",
	"TLB miss (read)",
	"TLB miss (write)",
	"Read Address Error",
	"Write Address Error",
	"Instruction Bus Error",
	"Data Bus Error",
	"Syscall",
	"Breakpoint",
	"Illegal Instruction",
	"Coprocessor Unusable",
	"Overflow",
	"13", "14", "15",
					/* Software trap codes */
	"AST"
};

/*
 * Print trap reason.
 */
kdbprinttrap(type, code)
	int	type, code;
{
	printf("kernel: ");
	if (type >= TRAP_TYPES)
	    printf("type %d", type);
	else
	    printf("%s", kdb_trap_names[type]);
	printf(" trap, frame @ %x\n", code);
}

/*
 * Peek/Poke routines
 */
extern unsigned *kdb_vtophys(), kdbgetmem();/*forward*/

unsigned
kdbpeek(addr)
	unsigned int *addr;
{
	if (ISA_KUSEG(addr)) {
		addr = kdb_vtophys(db_curmap, addr);
	}
	/*
	 * 	Kernel address.  If it needs mapping do as above,
	 *	otherwise just get it.
	 */
	else if (ISA_K2SEG(addr)) {
		addr = kdb_vtophys(kernel_map, addr);
	}
	return kdbgetmem(addr);
}

kdbpoke(addr, value)
	unsigned int *addr;
	unsigned int value;
{
	if (ISA_KUSEG(addr)) {
		addr = kdb_vtophys(db_curmap, addr);
	}
	/*
	 * 	Kernel address.  If it needs mapping do as above,
	 *	otherwise just get it.
	 */
	else if (ISA_K2SEG(addr)) {
		addr = kdb_vtophys(kernel_map, addr);
	}

	return kdbputmem(addr, value);
}

/*
 *   kdb_vtophys - map addresses by hand, so that TLB is
 *   minimally affected.  But we do take faults, if necessary.
 */
boolean_t db_no_vm_fault = FALSE;

unsigned
*kdb_vtophys(map, addr)
	vm_map_t map;
{
	pmap_t		pmap;
	vm_offset_t	pa;	/* physical address */
	int		ret;

	if (map == VM_MAP_NULL)
		db_error("No VM map");
	pmap = map->pmap;
retry:
	pa = pmap_resident_extract(pmap, addr);
	if (pa == 0) {
		if (db_no_vm_fault)
			db_error("Paged out page");

		/* NOTE: we might get hung if map is locked!! */
		ret = vm_fault(map, trunc_page(addr),
			VM_PROT_READ,
			FALSE);
		if (ret != KERN_SUCCESS)
			db_error("Invalid virtual address");
		goto retry;
	}
	return (unsigned *) PHYS_TO_K0SEG(pa);
}


/*
 *   kdbgetmem - read long word from kernel address space
 */
unsigned
kdbgetmem(addr)
	unsigned char	*addr;
{
	unsigned 		temp = 0;
	int			i;
	extern unsigned		kdb_getiomem();

	if (ISA_K1SEG(addr))
		return kdb_getiomem(addr);

	if (mem_size && (K0SEG_TO_PHYS(addr) > mem_size))
		db_error("Beyond physical memory");
#if	BYTE_MSF
	for (i = 0; i <= 3; i++) {
#else	BYTE_MSF
	for (i = 3; i >= 0; i--) {
#endif	BYTE_MSF
		temp <<= 8;
		temp |= addr[i] & 0xFF;
	}
	return temp;
}


/*
 *  kdbputmem - write word to kernel address space
 */

kdbputmem(addr, value)
	unsigned char	*addr;
	unsigned	 value;
{
	int		 i;
	extern		 kdb_putiomem();

	if (ISA_K1SEG(addr))
		return kdb_putiomem(addr, value);

	if (mem_size && (K0SEG_TO_PHYS(addr) > mem_size))
		return 0;
#if	BYTE_MSF
	for (i = 3; i >= 0; i--) {
#else	BYTE_MSF
	for (i = 0; i < 4; i++) {
#endif	BYTE_MSF
		*addr++ = value & 0xFF;
		value >>= 8;
	}
	/* Might be text, flush I-cache */
	mipscache_Iflush((unsigned)(addr - 4) & ~0x3, 2 * sizeof(int));
}

/*
 * Read bytes (??from kernel address space??) for debugger.
 */

extern jmp_buf_t	db_jmpbuf;

void
db_read_bytes(addr, size, data)
	register unsigned *addr;
	register int	   size;
	register unsigned *data;
{
	db_nofault = &db_jmpbuf;

	while (size >= 4)
		*data++ = kdbpeek(addr++), size -= 4;

	if (size) {
		unsigned tmp;
		register char *dst = (char*)data;

		tmp = kdbpeek(addr);
		while (size--) {
			*dst++ = tmp & 0xFF;
			tmp >>= 8;
		}
	}

	db_nofault = 0;
}

/*
 * Write bytes (?? to kernel address space??) for debugger.
 */
void
db_write_bytes(addr, size, data)
	register unsigned *addr;
	register int	   size;
	register unsigned *data;
{
	register char	*dst;

	db_nofault = &db_jmpbuf;

	while (size >= 4)
		kdbpoke(addr++, *data++), size -= 4;
	if (size) {
		unsigned tmp = kdbpeek(addr), tmp1 = 0;
		register char *src = (char*)data;

		tmp >>= (size << 3);
		tmp <<= (size << 3);
		while (size--) {
			tmp1 <<= 8;
			tmp1 |= *src++;
		}
		kdbpoke(addr, tmp|tmp1);
	}

	db_nofault = 0;
}

/*
 *	TLB utilities
 */

char *tlbhi_fmt = TLB_HI_FMT;
char *tlblo_fmt = TLB_LO_FMT;

tlbdump(from,to)
{
	int hi,lo, save_hi, save_lo, s;

	save_hi = tlb_read_tlbhi();
	save_lo = tlb_read_tlblo();

	if ((from == to) && (from == 0))
		to = 63;
	if (from < 0)
		from = 0;
	if (to > 63)
		to = 63;
	do {
		s = kdbsplhigh();
		tlb_read_line(from);
		hi = tlb_read_tlbhi();
		lo = tlb_read_tlblo();
		tlb_write_line(-3, save_hi, save_lo);
		kdbsplx(s);

		if (hi == K1SEG_BASE)
			continue;

		printf("%d: %b -> %b\n", from, hi, tlbhi_fmt, lo, tlblo_fmt);
	} while (from++ < to);
}

tlbpid()
{
	printf("tlbhi=%b\n", tlb_read_tlbhi(), tlbhi_fmt);
}
