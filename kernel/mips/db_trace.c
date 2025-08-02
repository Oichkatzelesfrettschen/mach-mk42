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
 * $Log:	db_trace.c,v $
 * Revision 2.7  91/02/14  14:37:26  mrt
 * 	Split trace function from ddb interface.  Added (optional)
 * 	code to walk through a user-mode, optimized CThread program
 * 	like the U*x server.  Should make it MI someday.
 * 	[91/02/12  12:18:59  af]
 * 
 * Revision 2.6  91/02/05  17:48:14  mrt
 * 	Added author notices
 * 	[91/02/04  11:22:18  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:25:54  mrt]
 * 
 * Revision 2.5  91/01/09  19:56:45  rpd
 * 	Fixed stack tracing for threads without kernel stacks.
 * 	[91/01/09            rpd]
 * 
 * Revision 2.4  91/01/08  15:49:38  rpd
 * 	Quit tracing after hitting a zero pc.
 * 	[90/12/31            rpd]
 * 	Changed for new pcb organization.
 * 	[90/11/12            rpd]
 * 
 * Revision 2.3  90/09/09  14:33:21  rpd
 * 	Fixed trace when hitting a breakpoint in user space.
 * 	Fixed to trace crossing over to user space for any
 * 	thread (e.g. "t/tu <thread>" command).
 * 	Documented magic adjustments of pc and ra in various places.
 * 	Locals work, provided you either do not strip or use
 * 	my fixed version of xstrip.
 * 	[90/08/30  16:56:46  af]
 * 
 * Revision 2.2  90/08/27  22:07:51  dbg
 * 	Fix some sloppy type (non)-declarations.
 * 	[90/08/27            dbg]
 * 
 * 	Got rid of ddb_regs: all lives on the stack now.
 * 	BEWARE: to make this possible the "valuep" field in
 * 	the db_regs array is _not_ a pointer, but a displacement
 * 	off the base of the exception frame.  Use _exclusively_
 * 	the (MI) db_read/write_variable functions to access registers.
 * 	Updated db_getreg_val accordingly.
 * 	Cleaned up a lot the trace functions, I hope they still work
 * 	without a valid symtab...
 * 	Made apparent that we cannot get a stack trace given _only_
 * 	a stack pointer, as the stack is not self-describing.
 * 	Not sure why local variables do not come out, xstrip problem ?
 * 	[90/08/20  10:09:54  af]
 * 
 * 	Turn debug off
 * 	[90/08/18  00:17:56  af]
 * 
 * 	Created, from my old KDB code.  History summary:
 * 		From jsb: better stack traces.
 * 		[90/04/23            rpd]
 * 		Changed tracing code to reflect 11 arguments for all traps.
 * 		[90/05/23            rwd]
 * 		Switch over to the MI KDB, fixes and cleanups.
 * 		[90/01/20            af]
 * 		Created.
 * 		[89/08/08            af]
 * 
 */
/*
 *	File: db_trace.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/89
 *
 *	Stack traceback routines for MIPS
 */

#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <ddb/db_variables.h>
#include <ddb/db_sym.h>
#include <machine/db_trace.h>

#include <kern/thread.h>

#include <mips/mips_cpu.h>
#include <mach/vm_param.h>

#define	DEBUG 0

extern int start();	/* lowest kernel code address */
extern unsigned db_maxoff;

#define REG_ARG(i)	(4+i)
#define SAVES_RA(x)	isa_spill((x),31)

extern	db_sym_t	localsym();

/*
 * Machine register set.
 */
struct mips_saved_state *db_cur_exc_frame = 0;

static boolean_t
db_setf_regs(vp, valuep, op)
	db_expr_t	*valuep;
	struct db_variable *vp;
	int		op;
{
	register int   *regp;

	regp = (int *) ((char *) db_cur_exc_frame + (int) (vp->valuep));

	if (op == DB_VAR_GET) {
		*valuep = *regp;
	} else if (op == DB_VAR_SET) {
		*regp = *valuep;
	} else
		return FALSE;
	return TRUE;
}

struct db_variable db_regs[] = {
	"at",	(int*)&(((struct mips_saved_state *)0)->at),  db_setf_regs,
	"v0",	(int*)&(((struct mips_saved_state *)0)->v0),  db_setf_regs,
	"v1",	(int*)&(((struct mips_saved_state *)0)->v1),  db_setf_regs,
	"a0",	(int*)&(((struct mips_saved_state *)0)->a0),  db_setf_regs,
	"a1",	(int*)&(((struct mips_saved_state *)0)->a1),  db_setf_regs,
	"a2",	(int*)&(((struct mips_saved_state *)0)->a2),  db_setf_regs,
	"a3",	(int*)&(((struct mips_saved_state *)0)->a3),  db_setf_regs,
	"t0",	(int*)&(((struct mips_saved_state *)0)->t0),  db_setf_regs,
	"t1",	(int*)&(((struct mips_saved_state *)0)->t1),  db_setf_regs,
	"t2",	(int*)&(((struct mips_saved_state *)0)->t2),  db_setf_regs,
	"t3",	(int*)&(((struct mips_saved_state *)0)->t3),  db_setf_regs,
	"t4",	(int*)&(((struct mips_saved_state *)0)->t4),  db_setf_regs,
	"t5",	(int*)&(((struct mips_saved_state *)0)->t5),  db_setf_regs,
	"t6",	(int*)&(((struct mips_saved_state *)0)->t6),  db_setf_regs,
	"t7",	(int*)&(((struct mips_saved_state *)0)->t7),  db_setf_regs,
	"s0",	(int*)&(((struct mips_saved_state *)0)->s0),  db_setf_regs,
	"s1",	(int*)&(((struct mips_saved_state *)0)->s1),  db_setf_regs,
	"s2",	(int*)&(((struct mips_saved_state *)0)->s2),  db_setf_regs,
	"s3",	(int*)&(((struct mips_saved_state *)0)->s3),  db_setf_regs,
	"s4",	(int*)&(((struct mips_saved_state *)0)->s4),  db_setf_regs,
	"s5",	(int*)&(((struct mips_saved_state *)0)->s5),  db_setf_regs,
	"s6",	(int*)&(((struct mips_saved_state *)0)->s6),  db_setf_regs,
	"s7",	(int*)&(((struct mips_saved_state *)0)->s7),  db_setf_regs,
	"t8",	(int*)&(((struct mips_saved_state *)0)->t8),  db_setf_regs,
	"t9",	(int*)&(((struct mips_saved_state *)0)->t9),  db_setf_regs,
	"k0",	(int*)&(((struct mips_saved_state *)0)->k0),  db_setf_regs,
	"k1",	(int*)&(((struct mips_saved_state *)0)->k1),  db_setf_regs,
	"gp",	(int*)&(((struct mips_saved_state *)0)->gp),  db_setf_regs,
	"sp",	(int*)&(((struct mips_saved_state *)0)->sp),  db_setf_regs,
	"fp",	(int*)&(((struct mips_saved_state *)0)->fp),  db_setf_regs,
	"ra",	(int*)&(((struct mips_saved_state *)0)->ra),  db_setf_regs,
	"sr",	(int*)&(((struct mips_saved_state *)0)->sr),  db_setf_regs,
	"mdlo",	(int*)&(((struct mips_saved_state *)0)->mdlo),  db_setf_regs,
	"mdhi",	(int*)&(((struct mips_saved_state *)0)->mdhi),  db_setf_regs,
	"bad",	(int*)&(((struct mips_saved_state *)0)->bad_address), db_setf_regs,
	"cs",	(int*)&(((struct mips_saved_state *)0)->cause),  db_setf_regs,
	"pc",	(int*)&(((struct mips_saved_state *)0)->pc),  db_setf_regs,
};
struct db_variable *db_eregs = db_regs + sizeof(db_regs)/sizeof(db_regs[0]);

/*
 * Stack trace.
 */
void db_mips_stack_trace();

void
db_stack_trace_cmd(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
#define	F_KERNEL_ONLY	1
#define	F_TRACE_THREAD	2
#define	F_DO_LOCALS	4
#define	F_PRINT_FRAME	8
	unsigned	flags = F_KERNEL_ONLY;

	long int	the_pc, the_ra;
	register char 	c;

	extern vm_map_t	db_curmap;
	vm_map_t	savemap = db_curmap;

	extern unsigned current_kstack;
	unsigned	kstackp = current_kstack;
	unsigned	stackp;


	while ((c = *modif++) != 0) {
	    if (c == 't')
		flags |= F_TRACE_THREAD;
	    if (c == 'u')
		flags &= ~F_KERNEL_ONLY;
	    if (c == 'l')
		flags |= F_DO_LOCALS;
	    if (c == 'e')
		flags |= F_PRINT_FRAME;
	}

	if (!have_addr) {
	    stackp = db_cur_exc_frame->sp;
	    the_pc = db_cur_exc_frame->pc;
	    the_ra = db_cur_exc_frame->ra;
	}
	else if (flags & F_TRACE_THREAD) {
	    register thread_t th;
	    register pcb_t t_pcb;
	    register task_t tk;

	    th = (thread_t) addr;
	    t_pcb = (pcb_t) db_get_value((int)&th->pcb, 4, FALSE);
	    kstackp = db_get_value((int)&th->kernel_stack, 4, FALSE);
	    if (kstackp == 0) {
		register struct mips_saved_state *mss = &t_pcb->mss;
		stackp = db_get_value((int)&mss->sp, 4, FALSE);
		the_pc = db_get_value((int)&mss->pc, 4, FALSE);
		the_ra = db_get_value((int)&mss->ra, 4, FALSE);
	    } else {
		register struct mips_kernel_state *mks = STACK_MKS(kstackp);
		stackp = db_get_value((int)&mks->sp, 4, FALSE);
		the_pc = db_get_value((int)&mks->pc, 4, FALSE);
		the_ra = the_pc; /* ??? */
	    }
	    tk = (task_t) db_get_value((int)&th->task, 4, FALSE);
	    db_curmap = (vm_map_t) db_get_value((int)&tk->map, 4, FALSE);
	}
	else {
	    db_error("You can only do this by fixing $sp, $pc and $ra");
	}
	db_mips_stack_trace(count,stackp,the_pc,the_ra,flags,kstackp);
	db_curmap = savemap;
}


void
db_mips_stack_trace(count, stackp, the_pc, the_ra, flags, kstackp)
	unsigned	stackp, kstackp;
	long int	the_pc, the_ra;
{
	static char	*ubanner = "*** User stack:\n";

	char		*name;
	db_sym_t	sym;
	unsigned	framep, argp;

	int		narg, nloc;
	unsigned	minpc;
	boolean_t	top_frame = 1;
	struct frame_info f;
	int		proc_off;
	int 		Nest;

	if (the_pc == 0) {
		db_printf("Bad pc\n");
		goto out;/* sanity */
	}
	if (the_ra)
		the_ra -= 8;	/* backup jal */

	minpc = (unsigned)start;

	if (ISA_KUSEG(the_pc))
	    if (flags & F_KERNEL_ONLY)
		goto out;
	    else {
		db_printf("%s", ubanner);
		minpc = VM_MIN_ADDRESS;
	    }

	if (count == -1)
	    count = 65535;

	while (count--) {
		int dummy;

		if (the_pc == 0)
			break;

		bzero(&f, sizeof f);

		/* 1: find out where we are */
		sym = db_search_symbol(the_pc, DB_STGY_PROC, &proc_off);
		if (sym == DB_SYM_NULL) {
			/* no procedures ? wont be able to do much */
			db_find_xtrn_sym_and_offset(the_pc,&name,&proc_off);
		} else
			db_symbol_values(sym, &name, 0);

		if (name == 0 || proc_off > db_maxoff) {
			proc_off = -1;
			db_printf("%x(", guess_procedure(the_pc));
		} else {
			db_printf("%s(", name);
		}

		/* 2: find out more about it */
		guess_frame(sym, the_pc, &f);
		nloc = f.nloc;
		narg = f.narg;

		/*
		 * Where are the arguments ?
		 * Where is the next frame ?
		 */
		if (top_frame) {
			if (f.at_entry)
				framep = stackp;

			if (f.mod_sp)
				/* New frame allocated */
				framep = stackp + f.framesize;

			argp = framep;

			/* Top procedure made any calls ? */
			if (!f.at_entry && f.mod_sp)
				goto saved_ra;
		} else if (f.isvector) {
			struct mips_exception_link *link;
			struct mips_saved_state *exc;

			/* Mach syscalls are "varargs": see
			 * how we call them in locore.s
			 */
			if (stackp == kstackp - 28)
				stackp = kstackp;
			link = (struct mips_exception_link *)stackp;
			exc = (struct mips_saved_state *)
				db_get_value(&link->eframe, 4, 0);
			the_pc = db_get_value(&exc->pc, 4, 0);
			db_printf(")\n");
			if (flags & F_PRINT_FRAME)
				print_exception_frame(exc, the_pc);
			/* back up the ra before the jal */
			the_ra = db_get_value(&exc->ra, 4, FALSE) - 8;
			stackp = db_get_value(&exc->sp, 4, FALSE);
#if	DEBUG
			db_printf("{Vector %x %x %x}\n",
				the_pc, the_ra, stackp);
#endif	DEBUG
			if (ISA_KUSEG(the_pc)) {
				if (flags & F_KERNEL_ONLY)
					goto out;
				db_printf("%s", ubanner);
				minpc = VM_MIN_ADDRESS;
			}
			top_frame = TRUE;
			continue;
		} else {
			/* Frame is allocated, or else */
			framep = stackp + f.framesize;
			argp = framep;
			if (f.at_entry) {
				/* Not the top frame and RA not saved ?
				 * Must be start, or else we dont know
				 */
				the_ra = 0;
			} else {
saved_ra:
				the_ra = db_get_value(framep + f.saved_pc_off, 4, FALSE);
#if	DEBUG
				db_printf("{%x}", the_ra);
#endif	DEBUG
				if (the_ra < minpc)
					the_ra = 0;
				else
					/* backup before jal */
					the_ra -= 8;
			}
		}

		/* 2.5: Print arguments */
		if (top_frame) {
			register int i;
			for (i = 0; narg && i < 4; i++, argp += 4) {
				if (sym = localsym(sym, 0, &Nest)) {
					db_symbol_values(sym,&name,0);
#if	DEBUG
					if (Nest != 1)
						db_printf("@"); /* oops! */
#endif
					db_printf("%s=", name);
				}
				if (saves_arg(the_pc, REG_ARG(i))) {
					db_printf("%x", db_get_value(argp, 4, FALSE));
				} else
					db_printf("%x", db_getreg_val(REG_ARG(i)));
				if (--narg)
					db_printf(",");
			}
		}
		for (; narg--; argp += 4) {
			if (sym = localsym(sym, 0, &Nest)) {
				db_symbol_values(sym,&name, 0);
#if	DEBUG
				if (Nest != 1)
					db_printf("@"); /* XXX oops! */
#endif
				db_printf("%s=", name);
			}
			db_printf("%x", db_get_value(argp, 4, FALSE));
			if (narg)
				db_printf(",");
		}
		db_printf(")");

		/* 3: give more detail about where we are */
		if (proc_off != -1)
			db_printf("+%x", proc_off);
		if (f.filename)
			db_printf(" [%s:%d]", f.filename, f.linenum);
		db_printf("\n");

		/* 4: possibly print local vars */
		if (flags & F_DO_LOCALS) {
			unsigned w;
			boolean_t isReg;

			while (sym = localsym(sym, &isReg, &Nest)) {
				if (Nest != 2)	/* 2 = top-level locals */
					continue;

				db_symbol_values(sym, &name, &w);
				if (isReg)
					w = db_getreg_val(w);
				else
					w = db_get_value(w + stackp, 4, FALSE);
				db_printf("\t%s=%x\n", name, w);
			}
		}

		/* 5: find out where we go after this */
		if ((the_pc = the_ra) == 0)
			break;

		if (f.mod_sp)
			stackp = framep;
		top_frame = FALSE;
	}
out:
#if	DEBUG
	db_printf("{fp=%x, esp=%x}\n", framep, db_cur_exc_frame->sp);
#endif	DEBUG
	return;
}

#define	EXTRA_UX_SUPPORT	1

#ifdef	EXTRA_UX_SUPPORT
int db_cproc_next_offset = 0;
int db_cproc_list_offset = 8;
int db_cproc_wait_offset = 12;
int db_cproc_context_offset = 20;
int db_cproc_state_offset = 28;

#include <machine/setjmp.h>

db_all_cprocs()
{
	int cproc, next;
	jmp_buf_t	save_db_jmpbuf;
	extern jmp_buf_t db_jmpbuf;

	if (!db_value_of_name("cproc_list", &next)) {
		db_printf("No cprocs.\n");
		return;
	}

	save_db_jmpbuf = db_jmpbuf;

	while (next) {
		char pstate[4];
		int s, w, n, sp;

		if (_setjmp(&db_jmpbuf))
			break;
		cproc = db_get_value(next, 4, FALSE);
		if (cproc == 0) break;
		next = cproc + db_cproc_list_offset;

		if (_setjmp(&db_jmpbuf))
			continue;
		s = db_get_value(cproc+db_cproc_state_offset,4,FALSE);
		w = db_get_value(cproc+db_cproc_wait_offset,4,FALSE);
		n = db_get_value(cproc+db_cproc_next_offset,4,FALSE);
		sp = db_get_value(cproc+db_cproc_context_offset,4,FALSE);

		db_cproc_state(s, pstate);

		db_printf("CThread %x %s", cproc, pstate);
		if (w) db_printf(" awaits %x", w);
		if (n) db_printf(" next %x", n);

		if (s == 0) /* running, context is stale */
			db_printf(" context %x\n", sp);
		else {
			int pc,ra;

			db_printf(" traceback:\n");
			pc = db_get_value(sp+(13*4),4,FALSE);
			ra = db_get_value(sp+(14*4),4,FALSE);
			db_mips_stack_trace(-1,sp+(15*4),pc,ra,F_DO_LOCALS,0);
		}
	}
	db_jmpbuf = save_db_jmpbuf;
}
static db_cproc_state(state, s)
	char	s[4];
{
	if (state & 1) *s++ = 'S'; else *s++ = 'R';
	if (state & 2) *s++ = 'B';
	if (state & 4) *s++ = 'C';
	*s = 0;
}

#endif	EXTRA_UX_SUPPORT

guess_procedure(pc)
unsigned pc;
{
    unsigned	    w;
    int             bw_pc, fw_pc;

    if (pc < (unsigned)start)
    	fw_pc = VM_MIN_ADDRESS;
    else
        fw_pc = (unsigned)start;

    for (bw_pc = pc; bw_pc >= fw_pc; bw_pc -= 4) {
	w = db_get_value(bw_pc, 4, FALSE);
	if (isa_ret(w))
	    break;
    }
    return (bw_pc < fw_pc) ?
	fw_pc : bw_pc + 8;	/* after j ra */
}

int curproc;

guess_frame(sym, pc, fr)
	db_sym_t sym;
	long int pc;
	frame_info_t fr;
{
	int inc;
	unsigned w;
	int bw_pc, fw_pc, binc, finc;
	char *name;

	if (sym != DB_SYM_NULL)
		db_symbol_values(sym, &name, &curproc);
	else
		curproc = guess_procedure(pc);

	if (curproc == 0)
		curproc = pc;

	/*
	 * Given the pc, figure out how we might have
	 * modified the sp.
	 */
	if (findproc(sym, fr, pc)) {
		fr->at_entry = 1;
		if (fr->mod_sp) {
			/* maybe the sp has not yet been modified */
			inc = 0;
			for (fw_pc = curproc; fw_pc < pc; fw_pc += 4){
				w = db_get_value(fw_pc, 4, FALSE);
				inc -= stack_modified(w,db_getreg_val);
				if (SAVES_RA(w))
					fr->at_entry = 0;
			}
			fr->mod_sp = (inc > 0);
			if (inc && inc != fr->framesize) {
#if	DEBUG
				db_printf("[?frame %x != %x]", inc, fr->framesize);
#endif	DEBUG
			}
		}
		return;
	}
	/* Not found in symtab, play guessing games */

	/* Guess 1: did we save the RA and where, or not */
	fr->at_entry = 1;
	inc = 0;
	for (fw_pc = curproc; fw_pc < pc; fw_pc += 4){
		w = db_get_value(fw_pc, 4, FALSE);
		inc -= stack_modified(w,db_getreg_val);
		if (SAVES_RA(w))
			fr->at_entry = 0;
	}

	/* Guess 2: did we alter the SP */
	fr->mod_sp = (inc > 0);
	fr->framesize = inc;
	fr->nloc = inc / 4;

	/* Defaults for the unknowns */
	fr->narg = 5;
	fr->isleaf = 0;
	fr->isvector = 0;
	fr->saved_pc_off = -4;
}

db_getreg_val(r)
	db_expr_t	r;
{
	if (r == 0)
		return 0;
	db_read_variable(&db_regs[r - 1], &r);
	return r;
}


static int saves_arg(pc,n)
long pc;
{
	long fw_pc;
	unsigned w;

	for (fw_pc = curproc; fw_pc < pc; fw_pc += 4) {
		w = db_get_value(fw_pc , 4, FALSE);
		if (isa_spill(w,n))
			return 1;
	}
	return 0;
}

print_exception_frame(fp, epc)
	register struct mips_saved_state *fp;
	unsigned epc;
{
	db_printf("\t Exception: taken at pc=%x, frame (at %x) :\n", epc, fp);
	db_printf("\t\t tlblo=%-13X tlbhi=%-13X tlbix=%-13X tlbcx=%-13X\n",
	    db_get_value(&fp->tlb_low, 4,FALSE), db_get_value(&fp->tlb_high,4, FALSE),
	    db_get_value(&fp->tlb_index,4,FALSE), db_get_value(&fp->tlb_context,4,FALSE));
	db_printf("\t\t at=%-16X v0=%-16X v1=%-16X a0=%-16X\n",
	    db_get_value(&fp->at, 4, FALSE), db_get_value(&fp->v0, 4, FALSE),
	    db_get_value(&fp->v1, 4, FALSE), db_get_value(&fp->a0, 4, FALSE));
	db_printf("\t\t a1=%-16X a2=%-16X a3=%-16X t0=%-16X\n",
	    db_get_value(&fp->a1, 4, FALSE), db_get_value(&fp->a2, 4, FALSE),
	    db_get_value(&fp->a3, 4, FALSE), db_get_value(&fp->t0, 4, FALSE));
	db_printf("\t\t t1=%-16X t2=%-16X t3=%-16X t4=%-16X\n",
	    db_get_value(&fp->t1, 4, FALSE), db_get_value(&fp->t2, 4, FALSE),
	    db_get_value(&fp->t3, 4, FALSE), db_get_value(&fp->t4, 4, FALSE));
	db_printf("\t\t t5=%-16X t6=%-16X t7=%-16X s0=%-16X\n",
	    db_get_value(&fp->t5, 4, FALSE), db_get_value(&fp->t6, 4, FALSE),
	    db_get_value(&fp->t7, 4, FALSE), db_get_value(&fp->s0, 4, FALSE));
	db_printf("\t\t s1=%-16X s2=%-16X s3=%-16X s4=%-16X\n",
	    db_get_value(&fp->s1, 4, FALSE), db_get_value(&fp->s2, 4, FALSE),
	    db_get_value(&fp->s3, 4, FALSE), db_get_value(&fp->s4, 4, FALSE));
	db_printf("\t\t s5=%-16X s6=%-16X s7=%-16X t8=%-16X\n",
	    db_get_value(&fp->s5, 4, FALSE), db_get_value(&fp->s6, 4, FALSE),
	    db_get_value(&fp->s7, 4, FALSE), db_get_value(&fp->t8, 4, FALSE));
	db_printf("\t\t t9=%-16X k0=%-16X k1=%-16X gp=%-16X\n",
	    db_get_value(&fp->t9, 4, FALSE), db_get_value(&fp->k0, 4, FALSE),
	    db_get_value(&fp->k1, 4, FALSE), db_get_value(&fp->gp, 4, FALSE));
	db_printf("\t\t sp=%-16X fp=%-16X ra=%-16X sr=%-16X\n",
	    db_get_value(&fp->sp, 4, FALSE), db_get_value(&fp->fp, 4, FALSE),
	    db_get_value(&fp->ra, 4, FALSE), db_get_value(&fp->sr, 4, FALSE));
	db_printf("\t\t lo=%-16X hi=%-16X bad=%-15X cs=%-16X\n",
	    db_get_value(&fp->mdlo, 4, FALSE), db_get_value(&fp->mdhi, 4, FALSE),
	    db_get_value(&fp->bad_address,4,FALSE), db_get_value(&fp->cause,4,FALSE));
	db_printf("\t\t pc=%-16X\n",
	    db_get_value(&fp->pc, 4, FALSE));
}
