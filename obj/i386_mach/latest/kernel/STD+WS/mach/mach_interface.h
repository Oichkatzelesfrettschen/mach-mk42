#ifndef	_mach
#define	_mach

/* Module mach */

#include <mach/kern_return.h>

#ifdef	mig_external
mig_external
#else
extern
#endif
void init_mach
    ();
#include <mach/std_types.h>
#include <mach/mach_types.h>

/* Routine task_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_create
#if	defined(LINTLIBRARY)
    (target_task, inherit_memory, child_task)
	task_t target_task;
	boolean_t inherit_memory;
	task_t *child_task;
{ return task_create(target_task, inherit_memory, child_task); }
#else
    ();
#endif

/* Routine task_terminate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_terminate
#if	defined(LINTLIBRARY)
    (target_task)
	task_t target_task;
{ return task_terminate(target_task); }
#else
    ();
#endif

/* Routine task_threads */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_threads
#if	defined(LINTLIBRARY)
    (target_task, thread_list, thread_listCnt)
	task_t target_task;
	thread_array_t *thread_list;
	mach_msg_type_number_t *thread_listCnt;
{ return task_threads(target_task, thread_list, thread_listCnt); }
#else
    ();
#endif

/* Routine thread_terminate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_terminate
#if	defined(LINTLIBRARY)
    (target_thread)
	thread_t target_thread;
{ return thread_terminate(target_thread); }
#else
    ();
#endif

/* Routine vm_allocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_allocate
#if	defined(LINTLIBRARY)
    (target_task, address, size, anywhere)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t size;
	boolean_t anywhere;
{ return vm_allocate(target_task, address, size, anywhere); }
#else
    ();
#endif

/* Routine vm_deallocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_deallocate
#if	defined(LINTLIBRARY)
    (target_task, address, size)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
{ return vm_deallocate(target_task, address, size); }
#else
    ();
#endif

/* Routine vm_protect */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_protect
#if	defined(LINTLIBRARY)
    (target_task, address, size, set_maximum, new_protection)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	boolean_t set_maximum;
	vm_prot_t new_protection;
{ return vm_protect(target_task, address, size, set_maximum, new_protection); }
#else
    ();
#endif

/* Routine vm_inherit */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_inherit
#if	defined(LINTLIBRARY)
    (target_task, address, size, new_inheritance)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_inherit_t new_inheritance;
{ return vm_inherit(target_task, address, size, new_inheritance); }
#else
    ();
#endif

/* Routine vm_read */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_read
#if	defined(LINTLIBRARY)
    (target_task, address, size, data, dataCnt)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	pointer_t *data;
	mach_msg_type_number_t *dataCnt;
{ return vm_read(target_task, address, size, data, dataCnt); }
#else
    ();
#endif

/* Routine vm_write */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_write
#if	defined(LINTLIBRARY)
    (target_task, address, data, dataCnt)
	vm_task_t target_task;
	vm_address_t address;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
{ return vm_write(target_task, address, data, dataCnt); }
#else
    ();
#endif

/* Routine vm_copy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_copy
#if	defined(LINTLIBRARY)
    (target_task, source_address, size, dest_address)
	vm_task_t target_task;
	vm_address_t source_address;
	vm_size_t size;
	vm_address_t dest_address;
{ return vm_copy(target_task, source_address, size, dest_address); }
#else
    ();
#endif

/* Routine vm_region */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_region
#if	defined(LINTLIBRARY)
    (target_task, address, size, protection, max_protection, inheritance, is_shared, object_name, offset)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t *size;
	vm_prot_t *protection;
	vm_prot_t *max_protection;
	vm_inherit_t *inheritance;
	boolean_t *is_shared;
	memory_object_name_t *object_name;
	vm_offset_t *offset;
{ return vm_region(target_task, address, size, protection, max_protection, inheritance, is_shared, object_name, offset); }
#else
    ();
#endif

/* Routine vm_statistics */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_statistics
#if	defined(LINTLIBRARY)
    (target_task, vm_stats)
	vm_task_t target_task;
	vm_statistics_data_t *vm_stats;
{ return vm_statistics(target_task, vm_stats); }
#else
    ();
#endif

/* Routine vm_pageable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_pageable
#if	defined(LINTLIBRARY)
    (target_task, address, size, wired_access)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_prot_t wired_access;
{ return vm_pageable(target_task, address, size, wired_access); }
#else
    ();
#endif

/* Routine mach_ports_register */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_ports_register
#if	defined(LINTLIBRARY)
    (target_task, init_port_set, init_port_setCnt)
	task_t target_task;
	mach_port_array_t init_port_set;
	mach_msg_type_number_t init_port_setCnt;
{ return mach_ports_register(target_task, init_port_set, init_port_setCnt); }
#else
    ();
#endif

/* Routine mach_ports_lookup */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t mach_ports_lookup
#if	defined(LINTLIBRARY)
    (target_task, init_port_set, init_port_setCnt)
	task_t target_task;
	mach_port_array_t *init_port_set;
	mach_msg_type_number_t *init_port_setCnt;
{ return mach_ports_lookup(target_task, init_port_set, init_port_setCnt); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_provided */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_provided
#if	defined(LINTLIBRARY)
    (memory_control, offset, data, dataCnt, lock_value)
	memory_object_control_t memory_control;
	vm_offset_t offset;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
	vm_prot_t lock_value;
{ return memory_object_data_provided(memory_control, offset, data, dataCnt, lock_value); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_unavailable */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_unavailable
#if	defined(LINTLIBRARY)
    (memory_control, offset, size)
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
{ return memory_object_data_unavailable(memory_control, offset, size); }
#else
    ();
#endif

/* Routine memory_object_get_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_get_attributes
#if	defined(LINTLIBRARY)
    (memory_control, object_ready, may_cache, copy_strategy)
	memory_object_control_t memory_control;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{ return memory_object_get_attributes(memory_control, object_ready, may_cache, copy_strategy); }
#else
    ();
#endif

/* Routine vm_set_default_memory_manager */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_set_default_memory_manager
#if	defined(LINTLIBRARY)
    (host_priv, default_manager)
	host_priv_t host_priv;
	mach_port_t *default_manager;
{ return vm_set_default_memory_manager(host_priv, default_manager); }
#else
    ();
#endif

/* SimpleRoutine memory_object_lock_request */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_lock_request
#if	defined(LINTLIBRARY)
    (memory_control, offset, size, should_clean, should_flush, lock_value, reply_to)
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	mach_port_t reply_to;
{ return memory_object_lock_request(memory_control, offset, size, should_clean, should_flush, lock_value, reply_to); }
#else
    ();
#endif

/* Routine xxx_host_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xxx_host_info
#if	defined(LINTLIBRARY)
    (target_task, info)
	mach_port_t target_task;
	machine_info_data_t *info;
{ return xxx_host_info(target_task, info); }
#else
    ();
#endif

/* Routine xxx_slot_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xxx_slot_info
#if	defined(LINTLIBRARY)
    (target_task, slot, info)
	task_t target_task;
	int slot;
	machine_slot_data_t *info;
{ return xxx_slot_info(target_task, slot, info); }
#else
    ();
#endif

/* Routine xxx_cpu_control */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t xxx_cpu_control
#if	defined(LINTLIBRARY)
    (target_task, cpu, running)
	task_t target_task;
	int cpu;
	boolean_t running;
{ return xxx_cpu_control(target_task, cpu, running); }
#else
    ();
#endif

/* Routine task_suspend */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_suspend
#if	defined(LINTLIBRARY)
    (target_task)
	task_t target_task;
{ return task_suspend(target_task); }
#else
    ();
#endif

/* Routine task_resume */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_resume
#if	defined(LINTLIBRARY)
    (target_task)
	task_t target_task;
{ return task_resume(target_task); }
#else
    ();
#endif

/* Routine task_get_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_get_special_port
#if	defined(LINTLIBRARY)
    (task, which_port, special_port)
	task_t task;
	int which_port;
	mach_port_t *special_port;
{ return task_get_special_port(task, which_port, special_port); }
#else
    ();
#endif

/* Routine task_set_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_set_special_port
#if	defined(LINTLIBRARY)
    (task, which_port, special_port)
	task_t task;
	int which_port;
	mach_port_t special_port;
{ return task_set_special_port(task, which_port, special_port); }
#else
    ();
#endif

/* Routine task_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_info
#if	defined(LINTLIBRARY)
    (target_task, flavor, task_info_out, task_info_outCnt)
	task_t target_task;
	int flavor;
	task_info_t task_info_out;
	mach_msg_type_number_t *task_info_outCnt;
{ return task_info(target_task, flavor, task_info_out, task_info_outCnt); }
#else
    ();
#endif

/* Routine thread_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_create
#if	defined(LINTLIBRARY)
    (parent_task, child_thread)
	task_t parent_task;
	thread_t *child_thread;
{ return thread_create(parent_task, child_thread); }
#else
    ();
#endif

/* Routine thread_suspend */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_suspend
#if	defined(LINTLIBRARY)
    (target_thread)
	thread_t target_thread;
{ return thread_suspend(target_thread); }
#else
    ();
#endif

/* Routine thread_resume */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_resume
#if	defined(LINTLIBRARY)
    (target_thread)
	thread_t target_thread;
{ return thread_resume(target_thread); }
#else
    ();
#endif

/* Routine thread_abort */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_abort
#if	defined(LINTLIBRARY)
    (target_thread)
	thread_t target_thread;
{ return thread_abort(target_thread); }
#else
    ();
#endif

/* Routine thread_get_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_get_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, old_state, old_stateCnt)
	thread_t target_thread;
	int flavor;
	thread_state_t old_state;
	mach_msg_type_number_t *old_stateCnt;
{ return thread_get_state(target_thread, flavor, old_state, old_stateCnt); }
#else
    ();
#endif

/* Routine thread_set_state */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_set_state
#if	defined(LINTLIBRARY)
    (target_thread, flavor, new_state, new_stateCnt)
	thread_t target_thread;
	int flavor;
	thread_state_t new_state;
	mach_msg_type_number_t new_stateCnt;
{ return thread_set_state(target_thread, flavor, new_state, new_stateCnt); }
#else
    ();
#endif

/* Routine thread_get_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_get_special_port
#if	defined(LINTLIBRARY)
    (thread, which_port, special_port)
	thread_t thread;
	int which_port;
	mach_port_t *special_port;
{ return thread_get_special_port(thread, which_port, special_port); }
#else
    ();
#endif

/* Routine thread_set_special_port */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_set_special_port
#if	defined(LINTLIBRARY)
    (thread, which_port, special_port)
	thread_t thread;
	int which_port;
	mach_port_t special_port;
{ return thread_set_special_port(thread, which_port, special_port); }
#else
    ();
#endif

/* Routine thread_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t thread_info
#if	defined(LINTLIBRARY)
    (target_thread, flavor, thread_info_out, thread_info_outCnt)
	thread_t target_thread;
	int flavor;
	thread_info_t thread_info_out;
	mach_msg_type_number_t *thread_info_outCnt;
{ return thread_info(target_thread, flavor, thread_info_out, thread_info_outCnt); }
#else
    ();
#endif

/* Routine task_set_emulation */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t task_set_emulation
#if	defined(LINTLIBRARY)
    (target_port, routine_entry_pt, routine_number)
	task_t target_port;
	vm_address_t routine_entry_pt;
	int routine_number;
{ return task_set_emulation(target_port, routine_entry_pt, routine_number); }
#else
    ();
#endif

/* Routine port_names */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_names
#if	defined(LINTLIBRARY)
    (task, port_names_p, port_names_pCnt, port_types, port_typesCnt)
	ipc_space_t task;
	mach_port_array_t *port_names_p;
	mach_msg_type_number_t *port_names_pCnt;
	mach_port_type_array_t *port_types;
	mach_msg_type_number_t *port_typesCnt;
{ return port_names(task, port_names_p, port_names_pCnt, port_types, port_typesCnt); }
#else
    ();
#endif

/* Routine port_type */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_type
#if	defined(LINTLIBRARY)
    (task, port_name, port_type_p)
	ipc_space_t task;
	mach_port_t port_name;
	mach_port_type_t *port_type_p;
{ return port_type(task, port_name, port_type_p); }
#else
    ();
#endif

/* Routine port_rename */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_rename
#if	defined(LINTLIBRARY)
    (task, old_name, new_name)
	ipc_space_t task;
	mach_port_t old_name;
	mach_port_t new_name;
{ return port_rename(task, old_name, new_name); }
#else
    ();
#endif

/* Routine port_allocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_allocate
#if	defined(LINTLIBRARY)
    (task, port_name)
	ipc_space_t task;
	mach_port_t *port_name;
{ return port_allocate(task, port_name); }
#else
    ();
#endif

/* Routine port_deallocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_deallocate
#if	defined(LINTLIBRARY)
    (task, port_name)
	ipc_space_t task;
	mach_port_t port_name;
{ return port_deallocate(task, port_name); }
#else
    ();
#endif

/* Routine port_set_backlog */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_backlog
#if	defined(LINTLIBRARY)
    (task, port_name, backlog)
	ipc_space_t task;
	mach_port_t port_name;
	int backlog;
{ return port_set_backlog(task, port_name, backlog); }
#else
    ();
#endif

/* Routine port_status */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_status
#if	defined(LINTLIBRARY)
    (task, port_name, enabled, num_msgs, backlog, ownership, receive_rights)
	ipc_space_t task;
	mach_port_t port_name;
	mach_port_t *enabled;
	int *num_msgs;
	int *backlog;
	boolean_t *ownership;
	boolean_t *receive_rights;
{ return port_status(task, port_name, enabled, num_msgs, backlog, ownership, receive_rights); }
#else
    ();
#endif

/* Routine port_set_allocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_allocate
#if	defined(LINTLIBRARY)
    (task, set_name)
	ipc_space_t task;
	mach_port_t *set_name;
{ return port_set_allocate(task, set_name); }
#else
    ();
#endif

/* Routine port_set_deallocate */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_deallocate
#if	defined(LINTLIBRARY)
    (task, set_name)
	ipc_space_t task;
	mach_port_t set_name;
{ return port_set_deallocate(task, set_name); }
#else
    ();
#endif

/* Routine port_set_add */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_add
#if	defined(LINTLIBRARY)
    (task, set_name, port_name)
	ipc_space_t task;
	mach_port_t set_name;
	mach_port_t port_name;
{ return port_set_add(task, set_name, port_name); }
#else
    ();
#endif

/* Routine port_set_remove */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_remove
#if	defined(LINTLIBRARY)
    (task, port_name)
	ipc_space_t task;
	mach_port_t port_name;
{ return port_set_remove(task, port_name); }
#else
    ();
#endif

/* Routine port_set_status */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_status
#if	defined(LINTLIBRARY)
    (task, set_name, members, membersCnt)
	ipc_space_t task;
	mach_port_t set_name;
	mach_port_array_t *members;
	mach_msg_type_number_t *membersCnt;
{ return port_set_status(task, set_name, members, membersCnt); }
#else
    ();
#endif

/* Routine port_insert_send */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_insert_send
#if	defined(LINTLIBRARY)
    (task, my_port, his_name)
	ipc_space_t task;
	mach_port_t my_port;
	mach_port_t his_name;
{ return port_insert_send(task, my_port, his_name); }
#else
    ();
#endif

/* Routine port_extract_send */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_extract_send
#if	defined(LINTLIBRARY)
    (task, his_name, his_port)
	ipc_space_t task;
	mach_port_t his_name;
	mach_port_t *his_port;
{ return port_extract_send(task, his_name, his_port); }
#else
    ();
#endif

/* Routine port_insert_receive */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_insert_receive
#if	defined(LINTLIBRARY)
    (task, my_port, his_name)
	ipc_space_t task;
	mach_port_t my_port;
	mach_port_t his_name;
{ return port_insert_receive(task, my_port, his_name); }
#else
    ();
#endif

/* Routine port_extract_receive */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_extract_receive
#if	defined(LINTLIBRARY)
    (task, his_name, his_port)
	ipc_space_t task;
	mach_port_t his_name;
	mach_port_t *his_port;
{ return port_extract_receive(task, his_name, his_port); }
#else
    ();
#endif

/* Routine vm_map */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_map
#if	defined(LINTLIBRARY)
    (target_task, address, size, mask, anywhere, memory_object, offset, copy, cur_protection, max_protection, inheritance)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t size;
	vm_address_t mask;
	boolean_t anywhere;
	memory_object_t memory_object;
	vm_offset_t offset;
	boolean_t copy;
	vm_prot_t cur_protection;
	vm_prot_t max_protection;
	vm_inherit_t inheritance;
{ return vm_map(target_task, address, size, mask, anywhere, memory_object, offset, copy, cur_protection, max_protection, inheritance); }
#else
    ();
#endif

/* SimpleRoutine memory_object_data_error */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_data_error
#if	defined(LINTLIBRARY)
    (memory_control, offset, size, error_value)
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	kern_return_t error_value;
{ return memory_object_data_error(memory_control, offset, size, error_value); }
#else
    ();
#endif

/* SimpleRoutine memory_object_set_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_set_attributes
#if	defined(LINTLIBRARY)
    (memory_control, object_ready, may_cache, copy_strategy)
	memory_object_control_t memory_control;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
{ return memory_object_set_attributes(memory_control, object_ready, may_cache, copy_strategy); }
#else
    ();
#endif

/* SimpleRoutine memory_object_destroy */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t memory_object_destroy
#if	defined(LINTLIBRARY)
    (memory_control, reason)
	memory_object_control_t memory_control;
	kern_return_t reason;
{ return memory_object_destroy(memory_control, reason); }
#else
    ();
#endif

/* Routine port_set_backup */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t port_set_backup
#if	defined(LINTLIBRARY)
    (task, port_name, backup, previous)
	ipc_space_t task;
	mach_port_t port_name;
	port_t backup;
	mach_port_t *previous;
{ return port_set_backup(task, port_name, backup, previous); }
#else
    ();
#endif

/* Routine vm_machine_attribute */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t vm_machine_attribute
#if	defined(LINTLIBRARY)
    (target_task, address, size, attribute, value)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	vm_machine_attribute_t attribute;
	vm_machine_attribute_val_t *value;
{ return vm_machine_attribute(target_task, address, size, attribute, value); }
#else
    ();
#endif

#endif	_mach
