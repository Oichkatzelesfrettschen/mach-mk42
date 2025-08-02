#ifndef	_exc
#define	_exc

/* Module exc */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_exc
    ();
#include <mach/std_types.h>

/* Routine exception_raise */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t exception_raise
#if	defined(LINTLIBRARY)
    (exception_port, thread, task, exception, code, subcode)
	mach_port_t exception_port;
	mach_port_t thread;
	mach_port_t task;
	int exception;
	int code;
	int subcode;
{ return exception_raise(exception_port, thread, task, exception, code, subcode); }
#else
    ();
#endif

#endif	_exc
