/*
 * mach/mach.h - Umbrella header for Mach interfaces
 * 
 * This header aggregates the essential Mach headers needed for
 * userspace programs.
 */

#ifndef _MACH_MACH_H_
#define _MACH_MACH_H_

/* Basic types and constants */
#include <mach/mach_types.h>
#include <mach/mach_param.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>

/* Message passing */
#include <mach/message.h>
#include <mach/port.h>

/* System calls and traps */
#include <mach/mach_traps.h>
#include <mach/syscall_sw.h>

/* Task and thread management */
#include <mach/task_info.h>
#include <mach/thread_info.h>
#include <mach/task_special_ports.h>
#include <mach/thread_special_ports.h>

/* Virtual memory */
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <mach/vm_inherit.h>
#include <mach/vm_attributes.h>
#include <mach/vm_statistics.h>

/* Memory objects */
#include <mach/memory_object.h>

/* Time */
#include <mach/time_value.h>

/* Errors */
#include <mach/error.h>
#include <mach/mig_errors.h>

/* Exceptions and notifications */
#include <mach/exception.h>
#include <mach/notify.h>

/* Host information */
#include <mach/host_info.h>
#include <mach/processor_info.h>

/* Policy */
#include <mach/policy.h>

/* Thread status */
#include <mach/thread_status.h>
#include <mach/thread_switch.h>

#endif /* _MACH_MACH_H_ */
