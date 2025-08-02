#ifndef	_mach_debug
#define	_mach_debug

/* Module mach_debug */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_mach_debug
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <mach_debug/mach_debug_types.h>

/* Routine host_zone_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t host_zone_info
#if	defined(LINTLIBRARY)
    (host, names, namesCnt, info, infoCnt)
	host_t host;
	zone_name_array_t *names;
	mach_msg_type_number_t *namesCnt;
	zone_info_array_t *info;
	mach_msg_type_number_t *infoCnt;
{ return host_zone_info(host, names, namesCnt, info, infoCnt); }
#else
    ();
#endif

/* Routine mach_port_get_srights */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_get_srights
#if	defined(LINTLIBRARY)
    (task, name, srights)
	ipc_space_t task;
	mach_port_t name;
	mach_port_rights_t *srights;
{ return mach_port_get_srights(task, name, srights); }
#else
    ();
#endif

/* Routine host_ipc_hash_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t host_ipc_hash_info
#if	defined(LINTLIBRARY)
    (host, info, infoCnt)
	host_t host;
	hash_info_bucket_array_t *info;
	mach_msg_type_number_t *infoCnt;
{ return host_ipc_hash_info(host, info, infoCnt); }
#else
    ();
#endif

/* Routine host_ipc_marequest_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t host_ipc_marequest_info
#if	defined(LINTLIBRARY)
    (host, max_requests, info, infoCnt)
	host_t host;
	unsigned *max_requests;
	hash_info_bucket_array_t *info;
	mach_msg_type_number_t *infoCnt;
{ return host_ipc_marequest_info(host, max_requests, info, infoCnt); }
#else
    ();
#endif

/* Routine mach_port_space_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_space_info
#if	defined(LINTLIBRARY)
    (task, info, table_info, table_infoCnt, tree_info, tree_infoCnt)
	ipc_space_t task;
	ipc_info_space_t *info;
	ipc_info_name_array_t *table_info;
	mach_msg_type_number_t *table_infoCnt;
	ipc_info_tree_name_array_t *tree_info;
	mach_msg_type_number_t *tree_infoCnt;
{ return mach_port_space_info(task, info, table_info, table_infoCnt, tree_info, tree_infoCnt); }
#else
    ();
#endif

/* Routine mach_port_dnrequest_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_port_dnrequest_info
#if	defined(LINTLIBRARY)
    (task, name, total, used)
	ipc_space_t task;
	mach_port_t name;
	unsigned *total;
	unsigned *used;
{ return mach_port_dnrequest_info(task, name, total, used); }
#else
    ();
#endif

/* Routine mach_vm_region_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_vm_region_info
#if	defined(LINTLIBRARY)
    (task, address, region, objects, objectsCnt)
	vm_task_t task;
	vm_address_t address;
	vm_info_region_t *region;
	vm_info_object_array_t *objects;
	mach_msg_type_number_t *objectsCnt;
{ return mach_vm_region_info(task, address, region, objects, objectsCnt); }
#else
    ();
#endif

/* Routine vm_mapped_pages_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_mapped_pages_info
#if	defined(LINTLIBRARY)
    (task, pages, pagesCnt)
	task_t task;
	page_address_array_t *pages;
	mach_msg_type_number_t *pagesCnt;
{ return vm_mapped_pages_info(task, pages, pagesCnt); }
#else
    ();
#endif

/* Routine host_stack_usage */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t host_stack_usage
#if	defined(LINTLIBRARY)
    (host, reserved, total, space, resident, maxusage, maxstack)
	host_t host;
	vm_size_t *reserved;
	unsigned *total;
	vm_size_t *space;
	vm_size_t *resident;
	vm_size_t *maxusage;
	vm_offset_t *maxstack;
{ return host_stack_usage(host, reserved, total, space, resident, maxusage, maxstack); }
#else
    ();
#endif

/* Routine processor_set_stack_usage */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t processor_set_stack_usage
#if	defined(LINTLIBRARY)
    (pset, total, space, resident, maxusage, maxstack)
	processor_set_name_t pset;
	unsigned *total;
	vm_size_t *space;
	vm_size_t *resident;
	vm_size_t *maxusage;
	vm_offset_t *maxstack;
{ return processor_set_stack_usage(pset, total, space, resident, maxusage, maxstack); }
#else
    ();
#endif

/* Routine host_virtual_physical_table_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t host_virtual_physical_table_info
#if	defined(LINTLIBRARY)
    (host, info, infoCnt)
	host_t host;
	hash_info_bucket_array_t *info;
	mach_msg_type_number_t *infoCnt;
{ return host_virtual_physical_table_info(host, info, infoCnt); }
#else
    ();
#endif

#endif	_mach_debug
