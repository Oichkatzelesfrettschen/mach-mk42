#ifndef	_memory_object
#define	_memory_object

/* Module memory_object */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_memory_object
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>

/* SimpleRoutine memory_object_init */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_init
#if	defined(LINTLIBRARY)
    (memory_object, memory_control, memory_object_name, memory_object_page_size)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	memory_object_name_t memory_object_name;
	vm_size_t memory_object_page_size;
{ return memory_object_init(memory_object, memory_control, memory_object_name, memory_object_page_size); }
#else
    ();
#endif

/* SimpleRoutine memory_object_terminate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_terminate
#if	defined(LINTLIBRARY)
    (memory_object, memory_control, memory_object_name)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	memory_object_name_t memory_object_name;
{ return memory_object_terminate(memory_object, memory_control, memory_object_name); }
#else
    ();
#endif

/* SimpleRoutine memory_object_copy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_copy
#if	defined(LINTLIBRARY)
    (old_memory_object, old_memory_control, offset, length, new_memory_object)
	memory_object_t old_memory_object;
	memory_object_control_t old_memory_control;
	vm_offset_t offset;
	vm_size_t length;
	memory_object_t new_memory_object;
{ return memory_object_copy(old_memory_object, old_memory_control, offset, length, new_memory_object); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_request */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_request
#if	defined(LINTLIBRARY)
    (memory_object, memory_control, offset, length, desired_access)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{ return memory_object_data_request(memory_object, memory_control, offset, length, desired_access); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_unlock */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_unlock
#if	defined(LINTLIBRARY)
    (memory_object, memory_control, offset, length, desired_access)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{ return memory_object_data_unlock(memory_object, memory_control, offset, length, desired_access); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_write */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_write
#if	defined(LINTLIBRARY)
    (memory_object, memory_control, offset, data, dataCnt)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
{ return memory_object_data_write(memory_object, memory_control, offset, data, dataCnt); }
#else
    ();
#endif

/* SimpleRoutine memory_object_lock_completed */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_lock_completed
#if	defined(LINTLIBRARY)
    (memory_object, memory_objectPoly, memory_control, offset, length)
	memory_object_t memory_object;
	mach_msg_type_name_t memory_objectPoly;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
{ return memory_object_lock_completed(memory_object, memory_objectPoly, memory_control, offset, length); }
#else
    ();
#endif

#endif	_memory_object
