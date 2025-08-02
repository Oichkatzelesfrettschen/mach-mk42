#ifndef	_default_pager_object
#define	_default_pager_object

/* Module default_pager_object */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_default_pager_object
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>

/* Routine default_pager_object_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t default_pager_object_create
#if	defined(LINTLIBRARY)
    (pager, memory_object, object_size)
	memory_object_t pager;
	memory_object_t *memory_object;
	vm_size_t object_size;
{ return default_pager_object_create(pager, memory_object, object_size); }
#else
    ();
#endif

#endif	_default_pager_object
