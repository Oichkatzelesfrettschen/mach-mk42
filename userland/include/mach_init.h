/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

/*
 * Mach initialization and library support
 */

#ifndef _MACH_INIT_H_
#define _MACH_INIT_H_

#include <mach/mach_types.h>
#include <mach/port.h>

/*
 * Standard Mach ports (stubs for now)
 */
extern mach_port_t mach_task_self_;
#define mach_task_self() mach_task_self_

extern mach_port_t mach_thread_self(void);
extern mach_port_t mach_host_self(void);

/*
 * VM allocation stub
 */
extern kern_return_t vm_allocate(
    mach_port_t target_task,
    vm_offset_t *address,
    vm_size_t size,
    int flags);

extern kern_return_t vm_deallocate(
    mach_port_t target_task,
    vm_offset_t address,
    vm_size_t size);

#endif /* _MACH_INIT_H_ */
