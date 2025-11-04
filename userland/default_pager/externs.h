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

#include <stddef.h>		/* for size_t */
#include <mach.h>

/* default_pager.c */
extern kern_return_t mach_msg_server(boolean_t (*)(mach_msg_header_t *,
						   mach_msg_header_t *),
				     mach_msg_size_t,
				     mach_port_t,
				     int);
extern void panic(const char *, ...);
extern void printf(const char *, ...);
extern void bzero(void *, size_t);
extern void bcopy(void *, void *, size_t);
extern void *kalloc(size_t);
extern void kfree(void *, size_t);
extern boolean_t seqnos_memory_object_server(mach_msg_header_t *,
					     mach_msg_header_t *);
extern boolean_t seqnos_memory_object_default_server(mach_msg_header_t *,
						     mach_msg_header_t *);
extern kern_return_t seqnos_memory_object_create(mach_port_t,
						 mach_port_seqno_t,
						 mach_port_t, vm_size_t,
						 mach_port_t, mach_port_t,
						 vm_size_t);
extern kern_return_t seqnos_memory_object_init(mach_port_t, mach_port_seqno_t,
					       mach_port_t, mach_port_t,
					       vm_size_t);
extern kern_return_t seqnos_memory_object_synchronize(mach_port_t,
						      mach_port_seqno_t,
						      mach_port_t,
						      vm_offset_t,
						      vm_offset_t,
						      vm_sync_t);
extern kern_return_t seqnos_memory_object_terminate(mach_port_t,
						    mach_port_seqno_t,
						    mach_port_t,
						    mach_port_t);
extern kern_return_t seqnos_memory_object_data_request(memory_object_t,
						       mach_port_seqno_t,
						       mach_port_t,
						       vm_offset_t,
						       vm_size_t,
						       vm_prot_t);
extern kern_return_t seqnos_memory_object_data_initialize(memory_object_t,
							  mach_port_seqno_t,
							  mach_port_t,
							  vm_offset_t,
							  pointer_t,
							  vm_size_t);
extern kern_return_t seqnos_memory_object_copy(memory_object_t,
					       mach_port_seqno_t,
					       memory_object_control_t,
					       vm_offset_t,
					       vm_size_t,
					       memory_object_t);
extern kern_return_t seqnos_memory_object_lock_completed(memory_object_t,
							 mach_port_seqno_t,
							 mach_port_t,
							 vm_offset_t,
							 vm_size_t);
extern kern_return_t seqnos_memory_object_data_unlock(memory_object_t,
						      mach_port_seqno_t,
						      mach_port_t,
						      vm_offset_t,
						      pointer_t,
						      vm_size_t);
extern kern_return_t seqnos_memory_object_supply_completed(memory_object_t,
							   mach_port_seqno_t,
							   mach_port_t,
							   vm_offset_t,
							   vm_size_t,
							   kern_return_t,
							   vm_offset_t);
extern kern_return_t seqnos_memory_object_data_return(memory_object_t,
						      mach_port_seqno_t,
						      mach_port_t,
						      vm_offset_t,
						      pointer_t,
						      vm_size_t,
						      boolean_t,
						      boolean_t);
extern kern_return_t seqnos_memory_object_change_completed(memory_object_t,
						mach_port_seqno_t,
						memory_object_control_t,
						memory_object_flavor_t);
extern kern_return_t seqnos_memory_object_notify(memory_object_rep_t,
					         memory_object_control_t,
					         mach_port_t,
					         vm_size_t);
extern kern_return_t seqnos_memory_object_rejected(memory_object_t,
					           memory_object_control_t,
					           kern_return_t);
extern kern_return_t thread_switch(mach_port_t, int, int);
extern boolean_t default_pager_object_server(mach_msg_header_t *,
					     mach_msg_header_t *);
extern void safe_gets(char *, size_t);
extern char *strbuild(char *, ...);
extern void printf_init(mach_port_t);
extern void panic_init(mach_port_t);

/* file_io.c */
extern boolean_t strprefix(const char *, const char *);
extern void ovbcopy(char *, char *, size_t);
extern kern_return_t vm_wire(mach_port_t, mach_port_t,
			     vm_offset_t, vm_size_t, vm_prot_t);
