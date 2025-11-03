/*
 * Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
 * Open Software Foundation, Inc. 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of ("OSF") or Open Software 
 * Foundation not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
 */
/*
 * OSF Research Institute MK6.1 (unencumbered) 1/31/1995
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */
/*
 * CMU_HISTORY
 * Revision 2.3  92/04/01  19:36:09  rpd
 * 	Fixed to handle kernels with non-contiguous text and data.
 * 	[92/03/13            rpd]
 * 
 * Revision 2.2  92/01/03  20:28:52  dbg
 * 	Created.
 * 	[91/09/12            dbg]
 * 
 */

/*
 * COFF-dependent (really, i860 COFF-dependent) routines for makeboot
 */

#include "bootstrap.h"

/*
 *	Following three includes come from kernel sources via export area.
 *	For i860, they are unrestricted.
 */
#include <machine/coff.h>
#include <machine/exec.h>
#include <machine/syms.h>

int coff_recog(struct file *, objfmt_t, char **);
int coff_load(struct file *, objfmt_t, char *);
void coff_symload(struct file *, mach_port_t, task_port_t,
		  const char *, objfmt_t);

struct objfmt_switch coff_switch = {
    "coff",
    coff_recog,
    coff_load,
    coff_symload
};

int
coff_recog(struct file *fp, objfmt_t ofmt, char **hdr)
{
    struct exec_hdrs	*x = (struct exec_hdrs *)*hdr;

    if (x->fh.f_magic == I860MAGIC)
	    return (1);
    else
	    return (0);
}

/*
 * Convert a coff-style file header to a loader_info-style one.
 * On return, *sym_size contains the symbol table size (alone),
 * while the first sym_size member of lp contains the total
 * space needed for symbol table-related data.
 */
int
coff_load(
	struct file	*fp,
	objfmt_t 	ofmt,
	char		*hdr)
{
	struct loader_info	*lp = &ofmt->info;
	struct exec_hdrs	*x = (struct exec_hdrs *)hdr;
	struct stat		sbuf;

	if (x->fh.f_magic != I860MAGIC)
	    return (1);
	switch ((int)x->ah.magic) {
	    case ZMAGIC:
		if (x->ah.tsize == 0) {
		    return (1);
		}
		lp->text_start	= x->ah.text_start;
		lp->text_size	= x->ah.tsize;
		lp->text_offset	= x->sh0.s_scnptr;  /* assume .text first */
		lp->data_start	= x->ah.data_start;
		lp->data_size	= x->ah.dsize;
		lp->data_offset	= x->sh1.s_scnptr;  /* assume .data second */
		lp->bss_size	= x->ah.bsize;
		break;
	    case OMAGIC:
	    case NMAGIC:
	    default:
		return (1);
	}
	lp->entry_1 = x->ah.entry;
	lp->entry_2 = 0;
	lp->sym_size[0] = x->fh.f_nsyms;
	lp->sym_offset[0] = x->fh.f_symptr;
	if (lp->sym_offset[0] == 0) {
		lp->sym_size[0] = 0;
	}
	else {
		if (SYMESZ != AUXESZ) {
			printf("can't determine symtab size\n");
			return (1);
		}
		lp->sym_size[0] *= SYMESZ;
		/*
		 * Compute length of string table.
		 */
		lp->str_offset = lp->sym_offset[0] + lp->sym_size[0];
		lp->str_size = fp->f_di.di_ic.i_size - lp->str_offset;
	}
	lp->format = COFF_F;
	return (0);
}

void
coff_symload(
	struct file	*fp,
	mach_port_t host_port,
	task_port_t task,
	const char *symtab_name,
	objfmt_t ofmt)
{
	struct loader_info	*lp = &ofmt->info;
	kern_return_t result;
	vm_offset_t	  symtab;
	int           symsize;
	vm_size_t     table_size;

    	/*
	 * Allocate space for the symbol table, preceding it by the header.
	 */

	symsize = lp->sym_size[0];

    	table_size = symsize + lp->str_size + sizeof(int);

	result = vm_allocate(mach_task_self(), &symtab, table_size, TRUE);
	if (result) {
	  	printf("[ error %d allocating space for %s symbol table ]\n",
		       result, symtab_name);
		return;
    	}

	*(int*)symtab = symsize;

	result = read_file(fp, lp->sym_offset[0], symtab + sizeof(int),
			   symsize + lp->str_size);
	if (result) 
		printf("[ no valid symbol table present for %s ]\n", symtab_name);
	else {
		/*
		 * Load the symbols into the kernel.
		 */
		result = host_load_symbol_table(host_port, task,
						symtab_name, symtab,
						table_size);
    	}

	(void) vm_deallocate(mach_task_self(), symtab, table_size);
}
