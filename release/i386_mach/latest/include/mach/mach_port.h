#ifndef	_mach_port
#define	_mach_port

/* Module mach_port */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_mach_port
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>

/* Routine mach_port_names */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_names
#if	defined(LINTLIBRARY)
    (task, names, namesCnt, types, typesCnt)
	ipc_space_t task;
	mach_port_array_t *names;
	mach_msg_type_number_t *namesCnt;
	mach_port_type_array_t *types;
	mach_msg_type_number_t *typesCnt;
{ return mach_port_names(task, names, namesCnt, types, typesCnt); }
#else
    ();
#endif

/* Routine mach_port_type */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_type
#if	defined(LINTLIBRARY)
    (task, name, ptype)
	ipc_space_t task;
	mach_port_t name;
	mach_port_type_t *ptype;
{ return mach_port_type(task, name, ptype); }
#else
    ();
#endif

/* Routine mach_port_rename */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_rename
#if	defined(LINTLIBRARY)
    (task, old_name, new_name)
	ipc_space_t task;
	mach_port_t old_name;
	mach_port_t new_name;
{ return mach_port_rename(task, old_name, new_name); }
#else
    ();
#endif

/* Routine mach_port_allocate_name */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_allocate_name
#if	defined(LINTLIBRARY)
    (task, right, name)
	ipc_space_t task;
	mach_port_right_t right;
	mach_port_t name;
{ return mach_port_allocate_name(task, right, name); }
#else
    ();
#endif

/* Routine mach_port_allocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_allocate
#if	defined(LINTLIBRARY)
    (task, right, name)
	ipc_space_t task;
	mach_port_right_t right;
	mach_port_t *name;
{ return mach_port_allocate(task, right, name); }
#else
    ();
#endif

/* Routine mach_port_destroy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_destroy
#if	defined(LINTLIBRARY)
    (task, name)
	ipc_space_t task;
	mach_port_t name;
{ return mach_port_destroy(task, name); }
#else
    ();
#endif

/* Routine mach_port_deallocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_deallocate
#if	defined(LINTLIBRARY)
    (task, name)
	ipc_space_t task;
	mach_port_t name;
{ return mach_port_deallocate(task, name); }
#else
    ();
#endif

/* Routine mach_port_get_refs */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_get_refs
#if	defined(LINTLIBRARY)
    (task, name, right, refs)
	ipc_space_t task;
	mach_port_t name;
	mach_port_right_t right;
	mach_port_urefs_t *refs;
{ return mach_port_get_refs(task, name, right, refs); }
#else
    ();
#endif

/* Routine mach_port_mod_refs */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_mod_refs
#if	defined(LINTLIBRARY)
    (task, name, right, delta)
	ipc_space_t task;
	mach_port_t name;
	mach_port_right_t right;
	mach_port_delta_t delta;
{ return mach_port_mod_refs(task, name, right, delta); }
#else
    ();
#endif

/* Routine mach_port_get_receive_status */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_get_receive_status
#if	defined(LINTLIBRARY)
    (task, name, status)
	ipc_space_t task;
	mach_port_t name;
	mach_port_status_t *status;
{ return mach_port_get_receive_status(task, name, status); }
#else
    ();
#endif

/* Routine mach_port_set_qlimit */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_set_qlimit
#if	defined(LINTLIBRARY)
    (task, name, qlimit)
	ipc_space_t task;
	mach_port_t name;
	mach_port_msgcount_t qlimit;
{ return mach_port_set_qlimit(task, name, qlimit); }
#else
    ();
#endif

/* Routine mach_port_set_mscount */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_set_mscount
#if	defined(LINTLIBRARY)
    (task, name, mscount)
	ipc_space_t task;
	mach_port_t name;
	mach_port_mscount_t mscount;
{ return mach_port_set_mscount(task, name, mscount); }
#else
    ();
#endif

/* Routine mach_port_get_set_status */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_get_set_status
#if	defined(LINTLIBRARY)
    (task, name, members, membersCnt)
	ipc_space_t task;
	mach_port_t name;
	mach_port_array_t *members;
	mach_msg_type_number_t *membersCnt;
{ return mach_port_get_set_status(task, name, members, membersCnt); }
#else
    ();
#endif

/* Routine mach_port_move_member */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_move_member
#if	defined(LINTLIBRARY)
    (task, member, after)
	ipc_space_t task;
	mach_port_t member;
	mach_port_t after;
{ return mach_port_move_member(task, member, after); }
#else
    ();
#endif

/* Routine mach_port_request_notification */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_request_notification
#if	defined(LINTLIBRARY)
    (task, name, id, sync, notify, notifyPoly, previous)
	ipc_space_t task;
	mach_port_t name;
	mach_msg_id_t id;
	mach_port_mscount_t sync;
	mach_port_t notify;
	mach_msg_type_name_t notifyPoly;
	mach_port_t *previous;
{ return mach_port_request_notification(task, name, id, sync, notify, notifyPoly, previous); }
#else
    ();
#endif

/* Routine mach_port_insert_right */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_insert_right
#if	defined(LINTLIBRARY)
    (task, name, poly, polyPoly)
	ipc_space_t task;
	mach_port_t name;
	mach_port_t poly;
	mach_msg_type_name_t polyPoly;
{ return mach_port_insert_right(task, name, poly, polyPoly); }
#else
    ();
#endif

/* Routine mach_port_extract_right */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_extract_right
#if	defined(LINTLIBRARY)
    (task, name, msgt_name, poly, polyPoly)
	ipc_space_t task;
	mach_port_t name;
	mach_msg_type_name_t msgt_name;
	mach_port_t *poly;
	mach_msg_type_name_t *polyPoly;
{ return mach_port_extract_right(task, name, msgt_name, poly, polyPoly); }
#else
    ();
#endif

#endif	_mach_port
