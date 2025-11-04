/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 */

/*
 * Machine-dependent VM type definitions (i386)
 */

#ifndef _MACH_MACHINE_VM_TYPES_H_
#define _MACH_MACHINE_VM_TYPES_H_

#include <stdint.h>

/*
 * Natural integer type for i386 architecture
 */
typedef uint32_t natural_t;
typedef int32_t integer_t;

/*
 * VM address and size types for i386
 */
typedef uint32_t vm_offset_t;
typedef uint32_t vm_size_t;

#endif /* _MACH_MACHINE_VM_TYPES_H_ */
