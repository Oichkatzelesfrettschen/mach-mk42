#ifndef	_memory_object_default
#define	_memory_object_default

/* Module memory_object_default */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_memory_object_default
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>

/* SimpleRoutine memory_object_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_create
#if	defined(LINTLIBRARY)
    (old_memory_object, new_memory_object, new_object_size, new_control_port, new_name, new_page_size)
	memory_object_t old_memory_object;
	memory_object_t new_memory_object;
	vm_size_t new_object_size;
	memory_object_control_t new_control_port;
	memory_object_name_t new_name;
	vm_size_t new_page_size;
{ return memory_object_create(old_memory_object, new_memory_object, new_object_size, new_control_port, new_name, new_page_size); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_initialize */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_initialize
#if	defined(LINTLIBRARY)
    (memory_object, memory_control_port, offset, data, dataCnt)
	memory_object_t memory_object;
	memory_object_control_t memory_control_port;
	vm_offset_t offset;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
{ return memory_object_data_initialize(memory_object, memory_control_port, offset, data, dataCnt); }
#else
    ();
#endif

#endif	_memory_object_default
