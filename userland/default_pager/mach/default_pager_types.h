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


#ifndef	_MACH_DEFAULT_PAGER_TYPES_H_
#define _MACH_DEFAULT_PAGER_TYPES_H_

/*
 *	Remember to update the mig type definitions
 *	in default_pager_types.defs when adding/removing fields.
 */

typedef struct default_pager_info {
	vm_size_t dpi_total_space;	/* size of backing store */
	vm_size_t dpi_free_space;	/* how much of it is unused */
	vm_size_t dpi_page_size;	/* the pager's vm page size */
} default_pager_info_t;


typedef struct default_pager_object {
	vm_offset_t dpo_object;		/* object managed by the pager */
	vm_size_t dpo_size;		/* backing store used for the object */
} default_pager_object_t;

typedef default_pager_object_t *default_pager_object_array_t;


typedef struct default_pager_page {
	vm_offset_t dpp_offset;		/* offset of the page in its object */
} default_pager_page_t;

typedef default_pager_page_t *default_pager_page_array_t;

#endif	/* _MACH_DEFAULT_PAGER_TYPES_H_ */
