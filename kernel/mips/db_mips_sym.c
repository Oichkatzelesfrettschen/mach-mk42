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
 * $Log:	db_mips_sym.c,v $
 * Revision 2.5  91/02/05  17:48:07  mrt
 * 	Added author notices
 * 	[91/02/04  11:22:10  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:25:43  mrt]
 * 
 * Revision 2.4  90/12/05  23:37:31  af
 * 	Executable header is now declared in coff.h.
 * 	Dropped bogus extra param from fixup_symtab.
 * 	[90/12/02            af]
 * 
 * Revision 2.2.1.1  90/10/03  11:36:14  af
 * 	Changes to allow the kernel to preserve the symtab even when
 * 	booted off DEC prom code (as in netowrk download via tftp()).
 * 	Needs a companion little program that alters the coff header
 * 	in the image to pretend the symtab is part of the data section.
 * 
 * Revision 2.2  90/08/27  22:07:41  dbg
 * 	Cleanups.  Got rid of nlist emulation: now send back a pointer to the
 * 	real symbol, worry later about deciding of what type the symbol is.
 * 	The MI code now treats symbols as opaque entities.
 * 	Split findproc into the two suggested functions: the MI code only
 * 	asks for X_db_line_at_pc for filename and linenumber info.
 * 	Changed search procedures to understand the MI strategies defined
 * 	in ddb/db_syms.h.  This still needs some more factoring out into
 * 	smaller pieces.
 * 	Reduced the crap in localsym, who has a MI interface now.
 * 	Got rid of the external_symbols thing, useless.
 * 	[90/08/20  10:16:52  af]
 * 
 * 	Created from my old KDB code. History summary:
 * 		From jsb: multiple symbol table support.  Line number support.
 * 		Preliminary local variable support.
 * 		[90/04/23            rpd]
 * 		Took a first shot at getting locals to work.
 * 		Reworked fixup() to preserve static procedures
 * 		and removed some useless code.
 * 		Switched over to MI version of KDB.
 * 		[90/01/20  16:38:27  af]
 * 		Created.
 * 		[89/08/08            af]
 * 
 */
/*
 *	File: db_mips_sym.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	8/89
 *
 *	Symtab module for COFF files (MIPS compiler)
 */

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>

#include <vm/vm_kern.h>
#include <mips/thread.h>
#include <mips/db_trace.h>
#include <mips/syms.h>

#define IS_DATA(c,t)	(((c)==scData)||((c)==scSData)||\
			 ((c)==scCommon)||((c)==scSCommon))
#define IS_BSS(c,t)	(((c)==scBss)||((c)==scSBss))
#define IS_TEXT(c,t)	(((c)==scText)||((c)==scRData))
#define IS_LOCAL(c,t)	((t)==stLocal)
#define IS_PARAM(c,t)	((t)==stParam)
#define IS_ABS(c,t)	((c)==scAbs)
#define IS_REG(c,t)	((c)==scRegister)

char *fh_noname = "noname";
static unsigned long localval;

static EXTR *last_external_symbol;
static PDR  *last_procedure_symbol;
static SYMR *last_local_symbol;


/* lookup a symbol by name */
db_sym_t
X_db_lookup(stab, symstr)
	db_symtab_t	*stab;
	char *symstr;
{
	HDRR	*symtab;
	register int i;
	EXTR	*es;
	PDR	*pr;
	SYMR	*sp;

	if (stab == 0 || (symtab = (HDRR*)stab->private) == 0)
		return DB_SYM_NULL;

	/*
	 * This is a question possibly with ambiguous answers.
	 * We first look through external symbols, and failing that
	 * we try the procedure table.  If that fails too we
	 * try the local symbols (where it might be duplicated)
	 */
	for (es = (EXTR*)symtab->cbExtOffset, i = 0;
	     i < symtab->iextMax; i++, es++)
	     	if (strcmp(es->asym.iss, symstr) == 0) {
			last_external_symbol = es;
			return (db_sym_t)es;
		}
	for (pr = (PDR*)symtab->cbPdOffset, i = 0;
	     i < symtab->ipdMax; i++, pr++) {
		sp = (SYMR*)pr->isym;
	     	if (strcmp(sp->iss, symstr) == 0) {
			last_procedure_symbol = pr;
			return (db_sym_t)pr;
		}
	}
	for (sp = (SYMR*)symtab->cbSymOffset, i = 0;
	     i < symtab->isymMax; i++, sp++)
	     	if (strcmp(sp->iss, symstr) == 0) {
			last_local_symbol = sp;
			return (db_sym_t)sp;
		}
	return DB_SYM_NULL;
}

/*
 * Find the closest symbol to val, return it and
 * the difference between val and the symbol found.
 *
 * Matching symbol should be closer than *diffp, which
 * gets updated on succesful return
 */
db_sym_t
X_db_search_symbol(stab, value, strategy, diffp)
	db_symtab_t	*stab;
	db_expr_t	value;
	db_strategy_t	strategy;
	db_expr_t	*diffp;
{
	HDRR 		*symtab;
	register SYMR *sp, *best_sp = 0;
	register EXTR *es, *best_es = 0;
	register PDR  *pr, *best_pr = 0;
	int i;
	unsigned newdiff, diff = *diffp;
	char *last_thing_found = 0;

	if (stab == 0 || (symtab = (HDRR*)stab->private) == 0)
		return DB_SYM_NULL;

	/*
	 * Strategy says how to search:
	 *	DB_STGY_ANY	as in X_db_lookup()
	 *	DB_STGY_XTRN	externals first, then procedures
	 *	DB_STGY_PROC	procedures only
	 */

	if (strategy == DB_STGY_PROC)
		goto search_procedures;

	for (es = (EXTR*)symtab->cbExtOffset, i = 0;
	     i < symtab->iextMax; i++, es++) {
		newdiff = value - (unsigned)es->asym.value;
		if (newdiff < diff) {
			diff = newdiff;
			best_es = es;
			if (diff == 0)
				break;
		}
	}

	if (best_es) {
		last_external_symbol = best_es;
		last_thing_found = (char*)best_es;
		if (diff == 0)
			goto out;
	}

search_procedures:

	for (pr = (PDR*)symtab->cbPdOffset, i = 0;
	     i < symtab->ipdMax; i++, pr++) {
		newdiff = value - (unsigned)pr->adr;
		if (newdiff < diff) {
			diff = newdiff;
			best_pr = pr;
			if (diff == 0)
				break;
		}
	}

	if (best_pr) {
		last_procedure_symbol = best_pr;
		last_thing_found = (char*)best_pr;
		if (diff == 0)
			goto out;
	}

	if (strategy != DB_STGY_ANY)
		/*
		 * Return what the last space we searched gave,
		 * note that best_pr is set only if better than
		 * best_es (which could be null, yes).
		 */
		goto out;

	for (sp = (SYMR*)symtab->cbSymOffset, i = 0;
	     i < symtab->isymMax; i++, sp++) {
		newdiff = value - (unsigned)sp->value;
		if (newdiff < diff) {
			diff = newdiff;
			best_sp = sp;
			if (diff == 0)
				break;
		}
	}
	if (best_sp) {
		last_local_symbol = best_sp;
		last_thing_found = (char*)best_sp;
	}
out:
	*diffp = diff;
	return (db_sym_t)last_thing_found;
}

/*
 * Advance to the next local variable of procedure sym.
 * Leave its value in localval as a side effect.
 * Return 0 at end of file.
 */
db_sym_t
localsym(sym, isReg, Nesting)
	db_sym_t	sym;
	boolean_t	*isReg;
	int		*Nesting;
{
	HDRR	*symtab_hdr;
	register SYMR *sp;
	register SYMR *maxsym;
	static int nesting = 0;

	if (sym == DB_SYM_NULL)
		return sym;

	if (db_last_symtab == 0) {
		return DB_SYM_NULL;	/* sanity? */
	}
	symtab_hdr = (HDRR*)db_last_symtab->private;

	maxsym = ((SYMR*)symtab_hdr->cbSymOffset) + symtab_hdr->isymMax;
	/*
	 * "sym" is a token which we'll pass back.  We are basically
	 * scanning all local symbols starting from the procedure's
	 * itself (e.g. sym is a PDR on first call, a SYMR afterwards)
	 */
	if ((PDR*) sym == last_procedure_symbol) {
		sp = (SYMR*) last_procedure_symbol->isym;
		nesting = 1;
	} else
		sp = (SYMR*) sym;

	/* Note: a procedure's symbols really start _after_ pr->isym */
	while (++sp < maxsym && nesting > 0) {

		register int   sc, st;

		last_local_symbol = sp;

		sc = sp->sc;
		st = sp->st;

		if (IS_TEXT(sc,st)) {
			if (st==stBlock)
				nesting++;
			if (st==stEnd)
				nesting--;
			continue;
		}

		if (IS_LOCAL(sc,st) || IS_PARAM(sc,st)) {
			if (isReg)
				*isReg = IS_REG(sc,st);
			if (Nesting)
				*Nesting = nesting;
			return (db_sym_t)sp;
		}
	}
	return DB_SYM_NULL;
}

FDR *
findfdr(symtab, pr)
	HDRR *symtab;
	PDR  *pr;
{
	int i, j;
	register FDR *fh;
	register PDR *pd_base;

	for (i = 0; i < symtab->ifdMax; i++) {
		fh = ((FDR *)symtab->cbFdOffset) + i;
		pd_base = ((PDR*)symtab->cbPdOffset)+fh->ipdFirst;
		for (j = 0; j < fh->cpd; j++) {
			if (pr == &pd_base[j]) {
				return fh;
			}
		}
	}
	return 0;
}

/*
 *  If fh has no valid filename, return 0.
 *  Otherwise, remove all but the last directory from the pathname.
 */
char *
fdr_filename(fh)
	FDR *fh;
{
	char *s, *s0, *s1;

	if (! fh || ! fh->rss || (char *)fh->rss == fh_noname) {
		return 0;
	}
	for (s0 = s1 = s = (char *)fh->rss; *s; s++) {
		if (s[0] == '/' && s[1]) {
			s0 = s1;
			s1 = &s[1];
		}
	}
	return s0;
}

int
fdr_linenum(symtab, fh, pr, pc)
	HDRR *symtab;
	FDR *fh;
	PDR *pr;
	int pc;
{
	char *base;
	PDR *pr0, *pr_end;
	int halt, lineno, best_offset, best_lineno, delta, linepc, count;

	/*
	 *  If we don't have enough info, return linenum of 0.
	 */
	if (! fh || ! fh->cbLineOffset || ! pr) {
		return 0;
	}
	if (pr->iline == ilineNil || pr->lnLow == -1 || pr->lnHigh == -1) {
		return 0;
	}

	/*
	 *  Find line entry to stop at for this procedure,
	 *  which is either first line entry of next procedure,
	 *  or last line entry for file if there are no more
	 *  procedures with line numbers.
	 *
	 *  XXX why can't we just use fh->LnHigh?
	 */
	halt = fh->cline;
	pr_end = ((PDR*)symtab->cbPdOffset) + fh->ipdFirst + fh->cpd;
	for (pr0 = pr+1; pr0 < pr_end; pr0++) {
		if (pr0->iline != -1) {
			halt = pr0->iline;
			break;
		}
	}

	/*
	 *  When procedures are moved around the linenumbers
	 *  are attributed to the next procedure up.
	 *  This only happens in -O4 images, dear jsb.
	 */
	if (pr->iline >= halt) {
		return pr->lnLow; /* best effort */
	}

	base = (char *) pr->cbLineOffset;
	linepc = (int)(pr->adr >> 2);	/* in words */
	pc = (int)((unsigned)pc >> 2);
	halt += (pr->adr >> 2) - pr->iline;
	lineno = best_lineno = pr->lnLow;
	best_offset = 9999999;

	while (linepc < halt) {
		count = *base & 0x0f;
		delta = *base++ >> 4;
		if (delta == -8) {
			delta = (base[0] << 8) | (base[1] & 0xff);
			base += 2;
		}
		lineno += delta;/* first delta is 0 */
		if (pc >= linepc && (pc - linepc) < best_offset) {
			best_offset = pc - linepc;
			best_lineno = lineno;
		}
		linepc += count + 1;
	}

	return best_lineno;
}

/*
 * Find filename and lineno within, given the
 * procedure's symbol and the current pc.
 */
boolean_t
X_db_line_at_pc( stab, sym, filenamep, linep, pc)
	db_symtab_t	*stab;
	db_sym_t	sym;
	char		**filenamep;
	int		*linep;
	db_expr_t	pc;
{
	register FDR *fh;

	if (sym == DB_SYM_NULL)
		return FALSE;
	fh = findfdr(stab->private, sym);
	*filenamep = fdr_filename(fh);
	*linep = fdr_linenum(stab->private, fh, sym, pc);
	return TRUE;
}


/*
 * Find the descriptor for the function described by sym (a PDR)
 * return all we know about it.
 */
boolean_t
findproc( sym, fr, pc)
	db_sym_t	sym;
	frame_info_t 	fr;
	long 		pc;
{
	HDRR		*symtab_hdr;
	register PDR  *pr;
	register SYMR *sp;
	register FDR *fh;
	int i;

	if (sym == DB_SYM_NULL)
		return FALSE;

	pr = (PDR*) sym;	/* opaque revealed */

	(void) X_db_line_at_pc( db_last_symtab, sym,
				&fr->filename, &fr->linenum,pc);

	if (fr->filename == 0) {	/* XXX ie, stripped */
		fr->narg = 5;
	} else {
		fr->narg = 0;
		for (sp = 1 + (SYMR*)pr->isym; sp->st == stParam; sp++)
			fr->narg++;
	}

	/* Leaf procedures do not save the ra */
	fr->isleaf = (pr->regmask & 0x80000000) == 0;

	fr->framesize = pr->frameoffset;
	fr->saved_pc_off = pr->regoffset;
	if (pr->frameoffset) {
		fr->nloc = pr->frameoffset / 4 - i;
	} else {
		fr->nloc = 0;
	}
	fr->mod_sp = (pr->frameoffset != 0);

	fr->isvector = ((pr->pcreg == 0)
		&& (fr->framesize == sizeof(struct mips_saved_state)));
	return TRUE;
}

/*
 * Return name and value of a symbol.
 * Since we have symbols of various types this
 * is messier than it should be. Oh well.
 */
X_db_symbol_values(sym, namep, valuep)
	register db_sym_t	*sym;
	char		**namep;
	db_expr_t	*valuep;
{
	if ((EXTR*)sym  == last_external_symbol) {
ext:
		*namep = (char*) ((EXTR*)sym)->asym.iss;
		*valuep = ((EXTR*)sym)->asym.value;
/*		ret.n_type  = es->asym.st; ret.n_other = es->asym.sc; */
	} else
	if ((PDR*)sym  == last_procedure_symbol) {
proc:
		*namep = (char*) ((SYMR*)((PDR*)sym)->isym)->iss;
		*valuep = ((PDR*)sym)->adr;
/*		ret.n_type  = sp->st; ret.n_other = sp->sc; */
	} else
	if ((SYMR*)sym  == last_local_symbol) {
loc:
		*namep = (char*)((SYMR*)sym)->iss;
		*valuep = ((SYMR*)sym)->value;
/*		ret.n_type  = sp->st; ret.n_other = sp->sc; */
	} else {
#ifdef	paranoia
		/* This is hard, but doable */
		db_symtab_t *stab = 0;
		for (i = 0; i < db_nsymtab; i++)
			if ((char*)sym >= db_symtabs[i].start &&
			    (char*)sym < db_symtabs[i].end) {
				stab = &db_symtabs[i];
				break;
			}
		if (stab == 0) { /* won't happen, don't panic */
			*namep = "???";
			*valuep = 0;
		}
		if ((EXTR*)sym >= (EXTR*)stab->cbExtOffset &&
		    (EXTR*)sym < (((EXTR*)stab->cbExtOffset) + stab->iextMax))
			goto ext;		
		if ((PDR*)sym >= (PDR*)stab->cbPdOffset &&
		    (PDR*)sym < (((PDR*)stab->cbPdOffset) + stab->ipdMax))
			goto proc;
		if ((SYMR*)sym >= (SYMR*)stab->cbSymOffset &&
		    (SYMR*)sym < (((SYMR*)stab->cbSymOffset) + stab->isymMax))
			goto loc;
#endif paranoia
		panic("db_symbol_values");
	}
}

/*
 *	Initialization functions
 */
#include <mips/coff.h>

typedef struct {
	struct exechdr	hdr;
	HDRR	 	sym_hdr;
	char	 	syms[1];	/* VARSIZE */
} loader_info;

read_mips_symtab(start)
	loader_info *start;
{
	extern char *esym;
	int stsize, st_hdrsize;
	unsigned st_filptr;
	HDRR *symtab;

	if (start->sym_hdr.magic != magicSym) {
		/*
		 * Before giving up see if we have tricked DEC's
		 * prom loader into loading the symtab [tftp() prom dev]
		 */
		extern char end[], edata[];

		symtab = (HDRR*)edata;
		if (symtab->magic != magicSym) {
			dprintf("[ symbol table: bad magic]\n");
			bzero(edata, end - edata);
			return;
		}
		st_filptr = symtab->cbLineOffset - sizeof(HDRR);
		st_hdrsize = sizeof(HDRR);
		/*
		 * We tricked the prom by pretending the data segment
		 * covered the symtab as well, now we have to move it
		 * up to make room for the bss.
		 */
		stsize = (symtab->cbExtOffset + symtab->iextMax * cbEXTR)
			 - (st_filptr + st_hdrsize);
		ovbcopy(symtab, end, stsize);
		bzero(edata, end - edata);
		symtab = (HDRR*)end;
	} else {
		/* get symbol table header */
		st_hdrsize = start->hdr.f.f_nsyms;
		st_filptr  = start->hdr.f.f_symptr;
		if (st_filptr == 0) {
			dprintf("[ no symbol table ]\n");
			return;
		}
		symtab = &start->sym_hdr;
	}
	/* find out how much stuff is there */
	stsize = (symtab->cbExtOffset + symtab->iextMax * cbEXTR)
		 - (st_filptr + st_hdrsize);
	dprintf("[ preserving %d bytes of symbol table for mach ]\n", stsize);

	esym = (char*)symtab + stsize + st_hdrsize;

	/* Change all file pointers into memory pointers */
	fixup_symtab(symtab, (char*)symtab + st_hdrsize,
		     st_filptr + st_hdrsize);
	db_add_symbol_table((char*)symtab, esym, "mach", (unsigned)symtab);
}

read_symtab_from_file(fp, symtabname)
	struct file *fp;
	char *symtabname;
{
	union {
		struct filehdr filehdr;
		HDRR hdrr;
	} u0;
	kern_return_t result;
	int stsize, st_hdrsize, resid;
	unsigned st_filptr;
	HDRR *symtab;

	/*
	 *  Read in file header
	 */
	result = read_file(fp, 0, &u0.filehdr, sizeof(u0.filehdr), &resid);
	if (result || resid) {
		printf("[ error %d reading %s file header ]\n",
			result, symtabname);
		return;
	}
	st_hdrsize = u0.filehdr.f_nsyms;
	st_filptr  = u0.filehdr.f_symptr;
	if (st_filptr == 0) {
		printf("[ no valid symbol table present for %s ]\n",
			symtabname);
		return;
	}

	/*
	 *  Read in symbol table header
	 */
	symtab = &u0.hdrr;
	result = read_file(fp, st_filptr, symtab, sizeof(*symtab), &resid);
	if (result || resid) {
		printf("[ error %d reading %s symbol header ]\n",
			result, symtabname);
		return;
	}
	if (symtab->magic != magicSym) {
		printf("[ bad magic number (0x%x) in %s symbol header ]\n",
			symtab->magic, symtabname);
		return;
	}
	stsize = (symtab->cbExtOffset - st_filptr)
		 + symtab->iextMax * cbEXTR;

	/*
	 *  Read in entire symbol table
	 */
	printf("[ allocating %6d bytes of symbol table for %s ]\n",
		stsize, symtabname);
	symtab = (HDRR *) kmem_alloc(kernel_map, stsize);
	if (symtab == 0) {
		printf("[ error %d allocating space for %s symbol table ]\n",
			result, symtabname);
		return;
	}
	result = read_file(fp, st_filptr, symtab, stsize, &resid);
	if (result || resid) {
		printf("[ error %d reading %s symbol table ]\n",
			result, symtabname);
		kmem_free(kernel_map, symtab, stsize);
		return;
	}

	/*
	 *  Change all file pointers into memory pointers
	 */
	fixup_symtab(symtab, (char *) symtab + st_hdrsize,
		      st_filptr + st_hdrsize);
	db_add_symbol_table((char*)symtab, 0, symtabname, (unsigned)symtab);
}

/*
 * Fixup the symbol table to hold in-memory pointers, as opposed
 * to file displacements
 */
fixup_symtab(hdr, data, f_ptr)
	HDRR		*hdr;
	char		*data;
	int		f_ptr;
{
	int             f_idx, s_idx;
	FDR            *fh;
	SYMR	       *sh;
	OPTR	       *op;
	PDR	       *pr;
	EXTR	       *esh;
	static SYMR	static_procedure =
			{ (long)"<static procedure>", 0, stStatic, scText, 0, 0};

#define FIX(off) \
	if (hdr->off) hdr->off = (unsigned int)data + (hdr->off - f_ptr);

	FIX(cbLineOffset);
	FIX(cbDnOffset);
	FIX(cbPdOffset);
	FIX(cbSymOffset);
	FIX(cbOptOffset);
	FIX(cbAuxOffset);
	FIX(cbSsOffset);
	FIX(cbSsExtOffset);
	FIX(cbFdOffset);
	FIX(cbRfdOffset);
	FIX(cbExtOffset);
#undef FIX
	/*
	 * Now go on and fixup all the indexes within the symtab itself.
	 * We must work on a file-basis, hence the mess. 
	 */
	for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++) {
		register int stripped;
		register unsigned code_off;

		fh = (FDR *) (hdr->cbFdOffset + f_idx * sizeof(FDR));
		/* fix file descriptor itself */
		fh->issBase += hdr->cbSsOffset;
		fh->isymBase = hdr->cbSymOffset + fh->isymBase * sizeof(SYMR);
		fh->ioptBase = hdr->cbOptOffset + fh->ioptBase * sizeof(OPTR);
		/* cannot fix fh->ipdFirst since it is a short */
#define IPDFIRST	((long)hdr->cbPdOffset + fh->ipdFirst * sizeof(PDR))
		fh->iauxBase = hdr->cbAuxOffset + fh->iauxBase * sizeof(AUXU);
		fh->rfdBase = hdr->cbRfdOffset + fh->rfdBase * sizeof(FDR);
		fh->cbLineOffset += hdr->cbLineOffset;
		code_off = fh->adr;
		stripped = (fh->rss == -1);	/* lang==4 ? */
		if (stripped)
			fh->rss = (long)fh_noname;
		else
			fh->rss = (long)fh->rss + fh->issBase;

		/* fixup local symbols */
		for (s_idx = 0; s_idx < fh->csym; s_idx++) {
			sh = (SYMR*)(fh->isymBase + s_idx * sizeof(SYMR));
			sh->iss = (long) sh->iss + fh->issBase;
		}
		/* fixup procedure symbols */
		for (s_idx = 0; s_idx < fh->cpd; s_idx++) {
			pr = (PDR*)(IPDFIRST + s_idx * sizeof(PDR));
			if (stripped) {
				if (pr->isym == -1) {
					pr->isym = (long)&static_procedure;
				} else {
					esh = (EXTR*)(hdr->cbExtOffset + pr->isym * sizeof(EXTR));
					pr->isym = (long)&(esh->asym);
				}
			} else {
				pr->isym = fh->isymBase + pr->isym * sizeof(SYMR);
				if (s_idx == 0 && pr->adr != 0) {
					code_off -= pr->adr;
				}
				pr->adr += code_off;
			}

			/* Fix line numbers */
			pr->cbLineOffset += fh->cbLineOffset;

		}
		/* forget about everything else */
	}

	/* fixup external symbols */
	for (s_idx = 0; s_idx < hdr->iextMax; s_idx++) {
		esh = (EXTR*)(hdr->cbExtOffset + s_idx * sizeof(EXTR));
		esh->asym.iss = esh->asym.iss + hdr->cbSsExtOffset;
	}
}

