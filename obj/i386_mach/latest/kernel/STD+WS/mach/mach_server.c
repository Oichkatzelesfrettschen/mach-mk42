/* Module mach */

#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <ipc/ipc_port.h>

#ifndef	mig_internal
#define	mig_internal	static
#endif

#ifndef	mig_external
#define mig_external
#endif

#ifndef	TypeCheck
#define	TypeCheck 1
#endif

#ifndef	UseExternRCSId
#ifdef	hc
#define	UseExternRCSId		1
#endif
#endif

#ifndef	UseStaticMsgType
#if	!defined(hc) || defined(__STDC__)
#define	UseStaticMsgType	1
#endif
#endif

/* Due to pcc compiler bug, cannot use void */
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(hc)
#define novalue void
#else
#define novalue int
#endif

#define msgh_request_port	msgh_remote_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_REMOTE(bits)
#define msgh_reply_port		msgh_local_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_LOCAL(bits)

#include <mach/std_types.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_tt.h>
#include <kern/ipc_host.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/host.h>
#include <kern/processor.h>
#include <vm/vm_object.h>
#include <vm/vm_map.h>
#include <ipc/ipc_space.h>
#include <mach/mach_types.h>

/* Routine task_create */
mig_internal novalue _Xtask_create
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t inherit_memoryType;
		boolean_t inherit_memory;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t child_taskType;
		mach_port_t child_task;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_create
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, boolean_t inherit_memory, task_t *child_task);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t inherit_memoryCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t child_taskType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t target_task;
	task_t child_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->inherit_memoryType != * (int *) &inherit_memoryCheck)
#else	UseStaticMsgType
	if ((In0P->inherit_memoryType.msgt_inline != TRUE) ||
	    (In0P->inherit_memoryType.msgt_longform != FALSE) ||
	    (In0P->inherit_memoryType.msgt_name != 0) ||
	    (In0P->inherit_memoryType.msgt_number != 1) ||
	    (In0P->inherit_memoryType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_create(target_task, In0P->inherit_memory, &child_task);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->child_taskType = child_taskType;
#else	UseStaticMsgType
	OutP->child_taskType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->child_taskType.msgt_size = 32;
	OutP->child_taskType.msgt_number = 1;
	OutP->child_taskType.msgt_inline = TRUE;
	OutP->child_taskType.msgt_longform = FALSE;
	OutP->child_taskType.msgt_deallocate = FALSE;
	OutP->child_taskType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->child_task /* convert_task_to_port child_task */ = /* child_task */ convert_task_to_port(child_task);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_terminate */
mig_internal novalue _Xtask_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_terminate(target_task);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_threads */
mig_internal novalue _Xtask_threads
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t thread_listType;
		thread_array_t thread_list;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_threads
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, thread_array_t *thread_list, mach_msg_type_number_t *thread_listCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t thread_listType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	task_t target_task;
	mach_msg_type_number_t thread_listCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_threads(target_task, &OutP->thread_list, &thread_listCnt);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->thread_listType = thread_listType;
#else	UseStaticMsgType
	OutP->thread_listType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->thread_listType.msgtl_size = 32;
	OutP->thread_listType.msgtl_header.msgt_name = 0;
	OutP->thread_listType.msgtl_header.msgt_size = 0;
	OutP->thread_listType.msgtl_header.msgt_number = 0;
	OutP->thread_listType.msgtl_header.msgt_inline = FALSE;
	OutP->thread_listType.msgtl_header.msgt_longform = TRUE;
	OutP->thread_listType.msgtl_header.msgt_deallocate = FALSE;
	OutP->thread_listType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->thread_listType.msgtl_number /* thread_listCnt */ = /* thread_listType.msgtl_number */ thread_listCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_terminate */
mig_internal novalue _Xthread_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t target_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_terminate(target_thread);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_allocate */
mig_internal novalue _Xvm_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t anywhereType;
		boolean_t anywhere;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addressType;
		vm_address_t address;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t *address, vm_size_t size, boolean_t anywhere);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t anywhereCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t addressType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->anywhereType != * (int *) &anywhereCheck)
#else	UseStaticMsgType
	if ((In0P->anywhereType.msgt_inline != TRUE) ||
	    (In0P->anywhereType.msgt_longform != FALSE) ||
	    (In0P->anywhereType.msgt_name != 0) ||
	    (In0P->anywhereType.msgt_number != 1) ||
	    (In0P->anywhereType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_allocate(target_task, &In0P->address, In0P->size, In0P->anywhere);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->addressType = addressType;
#else	UseStaticMsgType
	OutP->addressType.msgt_name = 2;
	OutP->addressType.msgt_size = 32;
	OutP->addressType.msgt_number = 1;
	OutP->addressType.msgt_inline = TRUE;
	OutP->addressType.msgt_longform = FALSE;
	OutP->addressType.msgt_deallocate = FALSE;
	OutP->addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	/* 0 */ OutP->address = In0P->address;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_deallocate */
mig_internal novalue _Xvm_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_deallocate(target_task, In0P->address, In0P->size);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_protect */
mig_internal novalue _Xvm_protect
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t set_maximumType;
		boolean_t set_maximum;
		mach_msg_type_t new_protectionType;
		vm_prot_t new_protection;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_protect
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size, boolean_t set_maximum, vm_prot_t new_protection);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t set_maximumCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t new_protectionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->set_maximumType != * (int *) &set_maximumCheck)
#else	UseStaticMsgType
	if ((In0P->set_maximumType.msgt_inline != TRUE) ||
	    (In0P->set_maximumType.msgt_longform != FALSE) ||
	    (In0P->set_maximumType.msgt_name != 0) ||
	    (In0P->set_maximumType.msgt_number != 1) ||
	    (In0P->set_maximumType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_protectionType != * (int *) &new_protectionCheck)
#else	UseStaticMsgType
	if ((In0P->new_protectionType.msgt_inline != TRUE) ||
	    (In0P->new_protectionType.msgt_longform != FALSE) ||
	    (In0P->new_protectionType.msgt_name != 2) ||
	    (In0P->new_protectionType.msgt_number != 1) ||
	    (In0P->new_protectionType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_protect(target_task, In0P->address, In0P->size, In0P->set_maximum, In0P->new_protection);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_inherit */
mig_internal novalue _Xvm_inherit
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t new_inheritanceType;
		vm_inherit_t new_inheritance;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_inherit
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size, vm_inherit_t new_inheritance);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t new_inheritanceCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_inheritanceType != * (int *) &new_inheritanceCheck)
#else	UseStaticMsgType
	if ((In0P->new_inheritanceType.msgt_inline != TRUE) ||
	    (In0P->new_inheritanceType.msgt_longform != FALSE) ||
	    (In0P->new_inheritanceType.msgt_name != 2) ||
	    (In0P->new_inheritanceType.msgt_number != 1) ||
	    (In0P->new_inheritanceType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_inherit(target_task, In0P->address, In0P->size, In0P->new_inheritance);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_read */
mig_internal novalue _Xvm_read
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t dataType;
		pointer_t data;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_read
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size, pointer_t *data, mach_msg_type_number_t *dataCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t dataType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	9,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	vm_map_t target_task;
	mach_msg_type_number_t dataCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_read(target_task, In0P->address, In0P->size, &OutP->data, &dataCnt);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->dataType = dataType;
#else	UseStaticMsgType
	OutP->dataType.msgtl_name = 9;
	OutP->dataType.msgtl_size = 8;
	OutP->dataType.msgtl_header.msgt_name = 0;
	OutP->dataType.msgtl_header.msgt_size = 0;
	OutP->dataType.msgtl_header.msgt_number = 0;
	OutP->dataType.msgtl_header.msgt_inline = FALSE;
	OutP->dataType.msgtl_header.msgt_longform = TRUE;
	OutP->dataType.msgtl_header.msgt_deallocate = FALSE;
	OutP->dataType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->dataType.msgtl_number /* dataCnt */ = /* dataType.msgtl_number */ dataCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_write */
mig_internal novalue _Xvm_write
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_long_t dataType;
		pointer_t data;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_write
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, pointer_t data, mach_msg_type_number_t dataCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->dataType.msgtl_header.msgt_inline != FALSE) ||
	    (In0P->dataType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->dataType.msgtl_name != 9) ||
	    (In0P->dataType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_write(target_task, In0P->address, In0P->data, In0P->dataType.msgtl_number);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_copy */
mig_internal novalue _Xvm_copy
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t source_addressType;
		vm_address_t source_address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t dest_addressType;
		vm_address_t dest_address;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_copy
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t source_address, vm_size_t size, vm_address_t dest_address);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t source_addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t dest_addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->source_addressType != * (int *) &source_addressCheck)
#else	UseStaticMsgType
	if ((In0P->source_addressType.msgt_inline != TRUE) ||
	    (In0P->source_addressType.msgt_longform != FALSE) ||
	    (In0P->source_addressType.msgt_name != 2) ||
	    (In0P->source_addressType.msgt_number != 1) ||
	    (In0P->source_addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->dest_addressType != * (int *) &dest_addressCheck)
#else	UseStaticMsgType
	if ((In0P->dest_addressType.msgt_inline != TRUE) ||
	    (In0P->dest_addressType.msgt_longform != FALSE) ||
	    (In0P->dest_addressType.msgt_name != 2) ||
	    (In0P->dest_addressType.msgt_number != 1) ||
	    (In0P->dest_addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_copy(target_task, In0P->source_address, In0P->size, In0P->dest_address);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_region */
mig_internal novalue _Xvm_region
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t protectionType;
		vm_prot_t protection;
		mach_msg_type_t max_protectionType;
		vm_prot_t max_protection;
		mach_msg_type_t inheritanceType;
		vm_inherit_t inheritance;
		mach_msg_type_t is_sharedType;
		boolean_t is_shared;
		mach_msg_type_t object_nameType;
		memory_object_name_t object_name;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_region
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t *address, vm_size_t *size, vm_prot_t *protection, vm_prot_t *max_protection, vm_inherit_t *inheritance, boolean_t *is_shared, memory_object_name_t *object_name, vm_offset_t *offset);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t addressType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t protectionType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t max_protectionType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t inheritanceType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t is_sharedType = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t object_nameType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t offsetType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_region(target_task, &In0P->address, &OutP->size, &OutP->protection, &OutP->max_protection, &OutP->inheritance, &OutP->is_shared, &OutP->object_name, &OutP->offset);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 96;

#if	UseStaticMsgType
	OutP->addressType = addressType;
#else	UseStaticMsgType
	OutP->addressType.msgt_name = 2;
	OutP->addressType.msgt_size = 32;
	OutP->addressType.msgt_number = 1;
	OutP->addressType.msgt_inline = TRUE;
	OutP->addressType.msgt_longform = FALSE;
	OutP->addressType.msgt_deallocate = FALSE;
	OutP->addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	/* 0 */ OutP->address = In0P->address;

#if	UseStaticMsgType
	OutP->sizeType = sizeType;
#else	UseStaticMsgType
	OutP->sizeType.msgt_name = 2;
	OutP->sizeType.msgt_size = 32;
	OutP->sizeType.msgt_number = 1;
	OutP->sizeType.msgt_inline = TRUE;
	OutP->sizeType.msgt_longform = FALSE;
	OutP->sizeType.msgt_deallocate = FALSE;
	OutP->sizeType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->protectionType = protectionType;
#else	UseStaticMsgType
	OutP->protectionType.msgt_name = 2;
	OutP->protectionType.msgt_size = 32;
	OutP->protectionType.msgt_number = 1;
	OutP->protectionType.msgt_inline = TRUE;
	OutP->protectionType.msgt_longform = FALSE;
	OutP->protectionType.msgt_deallocate = FALSE;
	OutP->protectionType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->max_protectionType = max_protectionType;
#else	UseStaticMsgType
	OutP->max_protectionType.msgt_name = 2;
	OutP->max_protectionType.msgt_size = 32;
	OutP->max_protectionType.msgt_number = 1;
	OutP->max_protectionType.msgt_inline = TRUE;
	OutP->max_protectionType.msgt_longform = FALSE;
	OutP->max_protectionType.msgt_deallocate = FALSE;
	OutP->max_protectionType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->inheritanceType = inheritanceType;
#else	UseStaticMsgType
	OutP->inheritanceType.msgt_name = 2;
	OutP->inheritanceType.msgt_size = 32;
	OutP->inheritanceType.msgt_number = 1;
	OutP->inheritanceType.msgt_inline = TRUE;
	OutP->inheritanceType.msgt_longform = FALSE;
	OutP->inheritanceType.msgt_deallocate = FALSE;
	OutP->inheritanceType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->is_sharedType = is_sharedType;
#else	UseStaticMsgType
	OutP->is_sharedType.msgt_name = 0;
	OutP->is_sharedType.msgt_size = 32;
	OutP->is_sharedType.msgt_number = 1;
	OutP->is_sharedType.msgt_inline = TRUE;
	OutP->is_sharedType.msgt_longform = FALSE;
	OutP->is_sharedType.msgt_deallocate = FALSE;
	OutP->is_sharedType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->object_nameType = object_nameType;
#else	UseStaticMsgType
	OutP->object_nameType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->object_nameType.msgt_size = 32;
	OutP->object_nameType.msgt_number = 1;
	OutP->object_nameType.msgt_inline = TRUE;
	OutP->object_nameType.msgt_longform = FALSE;
	OutP->object_nameType.msgt_deallocate = FALSE;
	OutP->object_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->offsetType = offsetType;
#else	UseStaticMsgType
	OutP->offsetType.msgt_name = 2;
	OutP->offsetType.msgt_size = 32;
	OutP->offsetType.msgt_number = 1;
	OutP->offsetType.msgt_inline = TRUE;
	OutP->offsetType.msgt_longform = FALSE;
	OutP->offsetType.msgt_deallocate = FALSE;
	OutP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_statistics */
mig_internal novalue _Xvm_statistics
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t vm_statsType;
		vm_statistics_data_t vm_stats;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_statistics
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_statistics_data_t *vm_stats);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t vm_statsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		13,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_statistics(target_task, &OutP->vm_stats);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 88;

#if	UseStaticMsgType
	OutP->vm_statsType = vm_statsType;
#else	UseStaticMsgType
	OutP->vm_statsType.msgt_name = 2;
	OutP->vm_statsType.msgt_size = 32;
	OutP->vm_statsType.msgt_number = 13;
	OutP->vm_statsType.msgt_inline = TRUE;
	OutP->vm_statsType.msgt_longform = FALSE;
	OutP->vm_statsType.msgt_deallocate = FALSE;
	OutP->vm_statsType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_pageable */
mig_internal novalue _Xvm_pageable
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t wired_accessType;
		vm_prot_t wired_access;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_pageable
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size, vm_prot_t wired_access);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t wired_accessCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->wired_accessType != * (int *) &wired_accessCheck)
#else	UseStaticMsgType
	if ((In0P->wired_accessType.msgt_inline != TRUE) ||
	    (In0P->wired_accessType.msgt_longform != FALSE) ||
	    (In0P->wired_accessType.msgt_name != 2) ||
	    (In0P->wired_accessType.msgt_number != 1) ||
	    (In0P->wired_accessType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_pageable(target_task, In0P->address, In0P->size, In0P->wired_access);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_ports_register */
mig_internal novalue _Xmach_ports_register
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t init_port_setType;
		mach_port_array_t init_port_set;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_ports_register
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, mach_port_array_t init_port_set, mach_msg_type_number_t init_port_setCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->init_port_setType.msgtl_header.msgt_inline != FALSE) ||
	    (In0P->init_port_setType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->init_port_setType.msgtl_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->init_port_setType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_ports_register(target_task, In0P->init_port_set, In0P->init_port_setType.msgtl_number);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_ports_lookup */
mig_internal novalue _Xmach_ports_lookup
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t init_port_setType;
		mach_port_array_t init_port_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_ports_lookup
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, mach_port_array_t *init_port_set, mach_msg_type_number_t *init_port_setCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t init_port_setType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	task_t target_task;
	mach_msg_type_number_t init_port_setCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_ports_lookup(target_task, &OutP->init_port_set, &init_port_setCnt);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->init_port_setType = init_port_setType;
#else	UseStaticMsgType
	OutP->init_port_setType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->init_port_setType.msgtl_size = 32;
	OutP->init_port_setType.msgtl_header.msgt_name = 0;
	OutP->init_port_setType.msgtl_header.msgt_size = 0;
	OutP->init_port_setType.msgtl_header.msgt_number = 0;
	OutP->init_port_setType.msgtl_header.msgt_inline = FALSE;
	OutP->init_port_setType.msgtl_header.msgt_longform = TRUE;
	OutP->init_port_setType.msgtl_header.msgt_deallocate = FALSE;
	OutP->init_port_setType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->init_port_setType.msgtl_number /* init_port_setCnt */ = /* init_port_setType.msgtl_number */ init_port_setCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* SimpleRoutine memory_object_data_provided */
mig_internal novalue _Xmemory_object_data_provided
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_long_t dataType;
		pointer_t data;
		mach_msg_type_t lock_valueType;
		vm_prot_t lock_value;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_provided
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, vm_offset_t offset, pointer_t data, mach_msg_type_number_t dataCnt, vm_prot_t lock_value);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t offsetCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t lock_valueCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->dataType.msgtl_header.msgt_inline != FALSE) ||
	    (In0P->dataType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->dataType.msgtl_name != 9) ||
	    (In0P->dataType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->lock_valueType != * (int *) &lock_valueCheck)
#else	UseStaticMsgType
	if ((In0P->lock_valueType.msgt_inline != TRUE) ||
	    (In0P->lock_valueType.msgt_longform != FALSE) ||
	    (In0P->lock_valueType.msgt_name != 2) ||
	    (In0P->lock_valueType.msgt_number != 1) ||
	    (In0P->lock_valueType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_provided(vm_object_lookup(In0P->Head.msgh_request_port), In0P->offset, In0P->data, In0P->dataType.msgtl_number, In0P->lock_value);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_data_unavailable */
mig_internal novalue _Xmemory_object_data_unavailable
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t sizeType;
		vm_size_t size;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_unavailable
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, vm_offset_t offset, vm_size_t size);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t offsetCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_unavailable(vm_object_lookup(In0P->Head.msgh_request_port), In0P->offset, In0P->size);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* Routine memory_object_get_attributes */
mig_internal novalue _Xmemory_object_get_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t object_readyType;
		boolean_t object_ready;
		mach_msg_type_t may_cacheType;
		boolean_t may_cache;
		mach_msg_type_t copy_strategyType;
		memory_object_copy_strategy_t copy_strategy;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_get_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, boolean_t *object_ready, boolean_t *may_cache, memory_object_copy_strategy_t *copy_strategy);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t object_readyType = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t may_cacheType = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t copy_strategyType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_get_attributes(vm_object_lookup(In0P->Head.msgh_request_port), &OutP->object_ready, &OutP->may_cache, &OutP->copy_strategy);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 56;

#if	UseStaticMsgType
	OutP->object_readyType = object_readyType;
#else	UseStaticMsgType
	OutP->object_readyType.msgt_name = 0;
	OutP->object_readyType.msgt_size = 32;
	OutP->object_readyType.msgt_number = 1;
	OutP->object_readyType.msgt_inline = TRUE;
	OutP->object_readyType.msgt_longform = FALSE;
	OutP->object_readyType.msgt_deallocate = FALSE;
	OutP->object_readyType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->may_cacheType = may_cacheType;
#else	UseStaticMsgType
	OutP->may_cacheType.msgt_name = 0;
	OutP->may_cacheType.msgt_size = 32;
	OutP->may_cacheType.msgt_number = 1;
	OutP->may_cacheType.msgt_inline = TRUE;
	OutP->may_cacheType.msgt_longform = FALSE;
	OutP->may_cacheType.msgt_deallocate = FALSE;
	OutP->may_cacheType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->copy_strategyType = copy_strategyType;
#else	UseStaticMsgType
	OutP->copy_strategyType.msgt_name = 2;
	OutP->copy_strategyType.msgt_size = 32;
	OutP->copy_strategyType.msgt_number = 1;
	OutP->copy_strategyType.msgt_inline = TRUE;
	OutP->copy_strategyType.msgt_longform = FALSE;
	OutP->copy_strategyType.msgt_deallocate = FALSE;
	OutP->copy_strategyType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_set_default_memory_manager */
mig_internal novalue _Xvm_set_default_memory_manager
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t default_managerType;
		mach_port_t default_manager;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t default_managerType;
		mach_port_t default_manager;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_set_default_memory_manager
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, mach_port_t *default_manager);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t default_managerType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->default_managerType.msgt_inline != TRUE) ||
	    (In0P->default_managerType.msgt_longform != FALSE) ||
	    (In0P->default_managerType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->default_managerType.msgt_number != 1) ||
	    (In0P->default_managerType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = vm_set_default_memory_manager(convert_port_to_host_priv(In0P->Head.msgh_request_port), &In0P->default_manager);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->default_managerType = default_managerType;
#else	UseStaticMsgType
	OutP->default_managerType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->default_managerType.msgt_size = 32;
	OutP->default_managerType.msgt_number = 1;
	OutP->default_managerType.msgt_inline = TRUE;
	OutP->default_managerType.msgt_longform = FALSE;
	OutP->default_managerType.msgt_deallocate = FALSE;
	OutP->default_managerType.msgt_unused = 0;
#endif	UseStaticMsgType

	/* 0 */ OutP->default_manager = In0P->default_manager;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* SimpleRoutine memory_object_lock_request */
mig_internal novalue _Xmemory_object_lock_request
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t should_cleanType;
		boolean_t should_clean;
		mach_msg_type_t should_flushType;
		boolean_t should_flush;
		mach_msg_type_t lock_valueType;
		vm_prot_t lock_value;
		mach_msg_type_t reply_toType;
		mach_port_t reply_to;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_lock_request
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, vm_offset_t offset, vm_size_t size, boolean_t should_clean, boolean_t should_flush, vm_prot_t lock_value, mach_port_t reply_to, mach_msg_type_name_t reply_toPoly);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t offsetCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t should_cleanCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t should_flushCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t lock_valueCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 72))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->should_cleanType != * (int *) &should_cleanCheck)
#else	UseStaticMsgType
	if ((In0P->should_cleanType.msgt_inline != TRUE) ||
	    (In0P->should_cleanType.msgt_longform != FALSE) ||
	    (In0P->should_cleanType.msgt_name != 0) ||
	    (In0P->should_cleanType.msgt_number != 1) ||
	    (In0P->should_cleanType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->should_flushType != * (int *) &should_flushCheck)
#else	UseStaticMsgType
	if ((In0P->should_flushType.msgt_inline != TRUE) ||
	    (In0P->should_flushType.msgt_longform != FALSE) ||
	    (In0P->should_flushType.msgt_name != 0) ||
	    (In0P->should_flushType.msgt_number != 1) ||
	    (In0P->should_flushType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->lock_valueType != * (int *) &lock_valueCheck)
#else	UseStaticMsgType
	if ((In0P->lock_valueType.msgt_inline != TRUE) ||
	    (In0P->lock_valueType.msgt_longform != FALSE) ||
	    (In0P->lock_valueType.msgt_name != 2) ||
	    (In0P->lock_valueType.msgt_number != 1) ||
	    (In0P->lock_valueType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->reply_toType.msgt_inline != TRUE) ||
	    (In0P->reply_toType.msgt_longform != FALSE) ||
	    (MACH_MSG_TYPE_PORT_ANY(In0P->reply_toType.msgt_name) && msgh_simple) ||
	    (In0P->reply_toType.msgt_number != 1) ||
	    (In0P->reply_toType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_lock_request(vm_object_lookup(In0P->Head.msgh_request_port), In0P->offset, In0P->size, In0P->should_clean, In0P->should_flush, In0P->lock_value, In0P->reply_to, In0P->reply_toType.msgt_name);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* Routine xxx_host_info */
mig_internal novalue _Xxxx_host_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t infoType;
		machine_info_data_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t xxx_host_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t target_task, machine_info_data_t *info);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		5,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = xxx_host_info(In0P->Head.msgh_request_port, &OutP->info);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 56;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgt_name = 2;
	OutP->infoType.msgt_size = 32;
	OutP->infoType.msgt_number = 5;
	OutP->infoType.msgt_inline = TRUE;
	OutP->infoType.msgt_longform = FALSE;
	OutP->infoType.msgt_deallocate = FALSE;
	OutP->infoType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine xxx_slot_info */
mig_internal novalue _Xxxx_slot_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t slotType;
		int slot;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t infoType;
		machine_slot_data_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t xxx_slot_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, int slot, machine_slot_data_t *info);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t slotCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->slotType != * (int *) &slotCheck)
#else	UseStaticMsgType
	if ((In0P->slotType.msgt_inline != TRUE) ||
	    (In0P->slotType.msgt_longform != FALSE) ||
	    (In0P->slotType.msgt_name != 2) ||
	    (In0P->slotType.msgt_number != 1) ||
	    (In0P->slotType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = xxx_slot_info(target_task, In0P->slot, &OutP->info);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 68;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgt_name = 2;
	OutP->infoType.msgt_size = 32;
	OutP->infoType.msgt_number = 8;
	OutP->infoType.msgt_inline = TRUE;
	OutP->infoType.msgt_longform = FALSE;
	OutP->infoType.msgt_deallocate = FALSE;
	OutP->infoType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine xxx_cpu_control */
mig_internal novalue _Xxxx_cpu_control
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t cpuType;
		int cpu;
		mach_msg_type_t runningType;
		boolean_t running;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t xxx_cpu_control
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, int cpu, boolean_t running);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t cpuCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t runningCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->cpuType != * (int *) &cpuCheck)
#else	UseStaticMsgType
	if ((In0P->cpuType.msgt_inline != TRUE) ||
	    (In0P->cpuType.msgt_longform != FALSE) ||
	    (In0P->cpuType.msgt_name != 2) ||
	    (In0P->cpuType.msgt_number != 1) ||
	    (In0P->cpuType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->runningType != * (int *) &runningCheck)
#else	UseStaticMsgType
	if ((In0P->runningType.msgt_inline != TRUE) ||
	    (In0P->runningType.msgt_longform != FALSE) ||
	    (In0P->runningType.msgt_name != 0) ||
	    (In0P->runningType.msgt_number != 1) ||
	    (In0P->runningType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = xxx_cpu_control(target_task, In0P->cpu, In0P->running);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_suspend */
mig_internal novalue _Xtask_suspend
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_suspend
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_suspend(target_task);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_resume */
mig_internal novalue _Xtask_resume
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_resume
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	task_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_resume(target_task);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_get_special_port */
mig_internal novalue _Xtask_get_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t which_portType;
		int which_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t special_portType;
		mach_port_t special_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_get_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, int which_port, mach_port_t *special_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t which_portCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t special_portType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->which_portType != * (int *) &which_portCheck)
#else	UseStaticMsgType
	if ((In0P->which_portType.msgt_inline != TRUE) ||
	    (In0P->which_portType.msgt_longform != FALSE) ||
	    (In0P->which_portType.msgt_name != 2) ||
	    (In0P->which_portType.msgt_number != 1) ||
	    (In0P->which_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_get_special_port(task, In0P->which_port, &OutP->special_port);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->special_portType = special_portType;
#else	UseStaticMsgType
	OutP->special_portType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->special_portType.msgt_size = 32;
	OutP->special_portType.msgt_number = 1;
	OutP->special_portType.msgt_inline = TRUE;
	OutP->special_portType.msgt_longform = FALSE;
	OutP->special_portType.msgt_deallocate = FALSE;
	OutP->special_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_set_special_port */
mig_internal novalue _Xtask_set_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t which_portType;
		int which_port;
		mach_msg_type_t special_portType;
		mach_port_t special_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_set_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, int which_port, mach_port_t special_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t which_portCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->which_portType != * (int *) &which_portCheck)
#else	UseStaticMsgType
	if ((In0P->which_portType.msgt_inline != TRUE) ||
	    (In0P->which_portType.msgt_longform != FALSE) ||
	    (In0P->which_portType.msgt_name != 2) ||
	    (In0P->which_portType.msgt_number != 1) ||
	    (In0P->which_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->special_portType.msgt_inline != TRUE) ||
	    (In0P->special_portType.msgt_longform != FALSE) ||
	    (In0P->special_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->special_portType.msgt_number != 1) ||
	    (In0P->special_portType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_set_special_port(task, In0P->which_port, In0P->special_port);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_info */
mig_internal novalue _Xtask_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t task_info_outType;
		int task_info_out[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_task, int flavor, task_info_t task_info_out, mach_msg_type_number_t *task_info_outCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t task_info_outType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	1024,
	};
#endif	UseStaticMsgType

	task_t target_task;
	mach_msg_type_number_t task_info_outCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	task_info_outCnt = 1024;

	OutP->RetCode = task_info(target_task, In0P->flavor, OutP->task_info_out, &task_info_outCnt);
	task_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->task_info_outType = task_info_outType;
#else	UseStaticMsgType
	OutP->task_info_outType.msgtl_name = 2;
	OutP->task_info_outType.msgtl_size = 32;
	OutP->task_info_outType.msgtl_header.msgt_name = 0;
	OutP->task_info_outType.msgtl_header.msgt_size = 0;
	OutP->task_info_outType.msgtl_header.msgt_number = 0;
	OutP->task_info_outType.msgtl_header.msgt_inline = TRUE;
	OutP->task_info_outType.msgtl_header.msgt_longform = TRUE;
	OutP->task_info_outType.msgtl_header.msgt_deallocate = FALSE;
	OutP->task_info_outType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->task_info_outType.msgtl_number /* task_info_outCnt */ = /* task_info_outType.msgtl_number */ task_info_outCnt;

	msgh_size_delta = (4 * task_info_outCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_create */
mig_internal novalue _Xthread_create
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t child_threadType;
		mach_port_t child_thread;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_create
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t parent_task, thread_t *child_thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t child_threadType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t parent_task;
	thread_t child_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	parent_task /* convert_port_to_task 0 Head.msgh_request_port */ = /* parent_task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_create(parent_task, &child_thread);
	task_deallocate(parent_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->child_threadType = child_threadType;
#else	UseStaticMsgType
	OutP->child_threadType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->child_threadType.msgt_size = 32;
	OutP->child_threadType.msgt_number = 1;
	OutP->child_threadType.msgt_inline = TRUE;
	OutP->child_threadType.msgt_longform = FALSE;
	OutP->child_threadType.msgt_deallocate = FALSE;
	OutP->child_threadType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->child_thread /* convert_thread_to_port child_thread */ = /* child_thread */ convert_thread_to_port(child_thread);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_suspend */
mig_internal novalue _Xthread_suspend
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_suspend
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t target_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_suspend(target_thread);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_resume */
mig_internal novalue _Xthread_resume
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_resume
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t target_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_resume(target_thread);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_abort */
mig_internal novalue _Xthread_abort
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_abort
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t target_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_abort(target_thread);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_get_state */
mig_internal novalue _Xthread_get_state
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t old_stateType;
		int old_state[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_get_state
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread, int flavor, thread_state_t old_state, mach_msg_type_number_t *old_stateCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t old_stateType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	1024,
	};
#endif	UseStaticMsgType

	thread_t target_thread;
	mach_msg_type_number_t old_stateCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	old_stateCnt = 1024;

	OutP->RetCode = thread_get_state(target_thread, In0P->flavor, OutP->old_state, &old_stateCnt);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->old_stateType = old_stateType;
#else	UseStaticMsgType
	OutP->old_stateType.msgtl_name = 2;
	OutP->old_stateType.msgtl_size = 32;
	OutP->old_stateType.msgtl_header.msgt_name = 0;
	OutP->old_stateType.msgtl_header.msgt_size = 0;
	OutP->old_stateType.msgtl_header.msgt_number = 0;
	OutP->old_stateType.msgtl_header.msgt_inline = TRUE;
	OutP->old_stateType.msgtl_header.msgt_longform = TRUE;
	OutP->old_stateType.msgtl_header.msgt_deallocate = FALSE;
	OutP->old_stateType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->old_stateType.msgtl_number /* old_stateCnt */ = /* old_stateType.msgtl_number */ old_stateCnt;

	msgh_size_delta = (4 * old_stateCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_set_state */
mig_internal novalue _Xthread_set_state
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
		mach_msg_type_long_t new_stateType;
		int new_state[1024];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_set_state
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread, int flavor, thread_state_t new_state, mach_msg_type_number_t new_stateCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t target_thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 44) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->new_stateType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->new_stateType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->new_stateType.msgtl_name != 2) ||
	    (In0P->new_stateType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (4 * In0P->new_stateType.msgtl_number);
	if (msgh_size != 44 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_set_state(target_thread, In0P->flavor, In0P->new_state, In0P->new_stateType.msgtl_number);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_get_special_port */
mig_internal novalue _Xthread_get_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t which_portType;
		int which_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t special_portType;
		mach_port_t special_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_get_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, int which_port, mach_port_t *special_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t which_portCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t special_portType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->which_portType != * (int *) &which_portCheck)
#else	UseStaticMsgType
	if ((In0P->which_portType.msgt_inline != TRUE) ||
	    (In0P->which_portType.msgt_longform != FALSE) ||
	    (In0P->which_portType.msgt_name != 2) ||
	    (In0P->which_portType.msgt_number != 1) ||
	    (In0P->which_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_get_special_port(thread, In0P->which_port, &OutP->special_port);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->special_portType = special_portType;
#else	UseStaticMsgType
	OutP->special_portType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->special_portType.msgt_size = 32;
	OutP->special_portType.msgt_number = 1;
	OutP->special_portType.msgt_inline = TRUE;
	OutP->special_portType.msgt_longform = FALSE;
	OutP->special_portType.msgt_deallocate = FALSE;
	OutP->special_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_set_special_port */
mig_internal novalue _Xthread_set_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t which_portType;
		int which_port;
		mach_msg_type_t special_portType;
		mach_port_t special_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_set_special_port
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, int which_port, mach_port_t special_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t which_portCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->which_portType != * (int *) &which_portCheck)
#else	UseStaticMsgType
	if ((In0P->which_portType.msgt_inline != TRUE) ||
	    (In0P->which_portType.msgt_longform != FALSE) ||
	    (In0P->which_portType.msgt_name != 2) ||
	    (In0P->which_portType.msgt_number != 1) ||
	    (In0P->which_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->special_portType.msgt_inline != TRUE) ||
	    (In0P->special_portType.msgt_longform != FALSE) ||
	    (In0P->special_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->special_portType.msgt_number != 1) ||
	    (In0P->special_portType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_set_special_port(thread, In0P->which_port, In0P->special_port);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_info */
mig_internal novalue _Xthread_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t thread_info_outType;
		int thread_info_out[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t target_thread, int flavor, thread_info_t thread_info_out, mach_msg_type_number_t *thread_info_outCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t thread_info_outType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	1024,
	};
#endif	UseStaticMsgType

	thread_t target_thread;
	mach_msg_type_number_t thread_info_outCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* target_thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	thread_info_outCnt = 1024;

	OutP->RetCode = thread_info(target_thread, In0P->flavor, OutP->thread_info_out, &thread_info_outCnt);
	thread_deallocate(target_thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->thread_info_outType = thread_info_outType;
#else	UseStaticMsgType
	OutP->thread_info_outType.msgtl_name = 2;
	OutP->thread_info_outType.msgtl_size = 32;
	OutP->thread_info_outType.msgtl_header.msgt_name = 0;
	OutP->thread_info_outType.msgtl_header.msgt_size = 0;
	OutP->thread_info_outType.msgtl_header.msgt_number = 0;
	OutP->thread_info_outType.msgtl_header.msgt_inline = TRUE;
	OutP->thread_info_outType.msgtl_header.msgt_longform = TRUE;
	OutP->thread_info_outType.msgtl_header.msgt_deallocate = FALSE;
	OutP->thread_info_outType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->thread_info_outType.msgtl_number /* thread_info_outCnt */ = /* thread_info_outType.msgtl_number */ thread_info_outCnt;

	msgh_size_delta = (4 * thread_info_outCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_set_emulation */
mig_internal novalue _Xtask_set_emulation
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t routine_entry_ptType;
		vm_address_t routine_entry_pt;
		mach_msg_type_t routine_numberType;
		int routine_number;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_set_emulation
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t target_port, vm_address_t routine_entry_pt, int routine_number);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t routine_entry_ptCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t routine_numberCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t target_port;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->routine_entry_ptType != * (int *) &routine_entry_ptCheck)
#else	UseStaticMsgType
	if ((In0P->routine_entry_ptType.msgt_inline != TRUE) ||
	    (In0P->routine_entry_ptType.msgt_longform != FALSE) ||
	    (In0P->routine_entry_ptType.msgt_name != 2) ||
	    (In0P->routine_entry_ptType.msgt_number != 1) ||
	    (In0P->routine_entry_ptType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->routine_numberType != * (int *) &routine_numberCheck)
#else	UseStaticMsgType
	if ((In0P->routine_numberType.msgt_inline != TRUE) ||
	    (In0P->routine_numberType.msgt_longform != FALSE) ||
	    (In0P->routine_numberType.msgt_name != 2) ||
	    (In0P->routine_numberType.msgt_number != 1) ||
	    (In0P->routine_numberType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_port /* convert_port_to_task 0 Head.msgh_request_port */ = /* target_port */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_set_emulation(target_port, In0P->routine_entry_pt, In0P->routine_number);
	task_deallocate(target_port);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_names */
mig_internal novalue _Xport_names
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t port_names_pType;
		mach_port_array_t port_names_p;
		mach_msg_type_long_t port_typesType;
		mach_port_type_array_t port_types;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_names
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_array_t *port_names_p, mach_msg_type_number_t *port_names_pCnt, mach_port_type_array_t *port_types, mach_msg_type_number_t *port_typesCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t port_names_pType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t port_typesType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	ipc_space_t task;
	mach_msg_type_number_t port_names_pCnt;
	mach_msg_type_number_t port_typesCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_names(task, &OutP->port_names_p, &port_names_pCnt, &OutP->port_types, &port_typesCnt);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 64;

#if	UseStaticMsgType
	OutP->port_names_pType = port_names_pType;
#else	UseStaticMsgType
	OutP->port_names_pType.msgtl_name = 2;
	OutP->port_names_pType.msgtl_size = 32;
	OutP->port_names_pType.msgtl_header.msgt_name = 0;
	OutP->port_names_pType.msgtl_header.msgt_size = 0;
	OutP->port_names_pType.msgtl_header.msgt_number = 0;
	OutP->port_names_pType.msgtl_header.msgt_inline = FALSE;
	OutP->port_names_pType.msgtl_header.msgt_longform = TRUE;
	OutP->port_names_pType.msgtl_header.msgt_deallocate = FALSE;
	OutP->port_names_pType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->port_names_pType.msgtl_number /* port_names_pCnt */ = /* port_names_pType.msgtl_number */ port_names_pCnt;

#if	UseStaticMsgType
	OutP->port_typesType = port_typesType;
#else	UseStaticMsgType
	OutP->port_typesType.msgtl_name = 2;
	OutP->port_typesType.msgtl_size = 32;
	OutP->port_typesType.msgtl_header.msgt_name = 0;
	OutP->port_typesType.msgtl_header.msgt_size = 0;
	OutP->port_typesType.msgtl_header.msgt_number = 0;
	OutP->port_typesType.msgtl_header.msgt_inline = FALSE;
	OutP->port_typesType.msgtl_header.msgt_longform = TRUE;
	OutP->port_typesType.msgtl_header.msgt_deallocate = FALSE;
	OutP->port_typesType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->port_typesType.msgtl_number /* port_typesCnt */ = /* port_typesType.msgtl_number */ port_typesCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_type */
mig_internal novalue _Xport_type
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t port_type_pType;
		mach_port_type_t port_type_p;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_type
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name, mach_port_type_t *port_type_p);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t port_type_pType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_type(task, In0P->port_name, &OutP->port_type_p);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->port_type_pType = port_type_pType;
#else	UseStaticMsgType
	OutP->port_type_pType.msgt_name = 2;
	OutP->port_type_pType.msgt_size = 32;
	OutP->port_type_pType.msgt_number = 1;
	OutP->port_type_pType.msgt_inline = TRUE;
	OutP->port_type_pType.msgt_longform = FALSE;
	OutP->port_type_pType.msgt_deallocate = FALSE;
	OutP->port_type_pType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_rename */
mig_internal novalue _Xport_rename
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t old_nameType;
		mach_port_t old_name;
		mach_msg_type_t new_nameType;
		mach_port_t new_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_rename
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t old_name, mach_port_t new_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t old_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t new_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->old_nameType != * (int *) &old_nameCheck)
#else	UseStaticMsgType
	if ((In0P->old_nameType.msgt_inline != TRUE) ||
	    (In0P->old_nameType.msgt_longform != FALSE) ||
	    (In0P->old_nameType.msgt_name != 2) ||
	    (In0P->old_nameType.msgt_number != 1) ||
	    (In0P->old_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_nameType != * (int *) &new_nameCheck)
#else	UseStaticMsgType
	if ((In0P->new_nameType.msgt_inline != TRUE) ||
	    (In0P->new_nameType.msgt_longform != FALSE) ||
	    (In0P->new_nameType.msgt_name != 2) ||
	    (In0P->new_nameType.msgt_number != 1) ||
	    (In0P->new_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_rename(task, In0P->old_name, In0P->new_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_allocate */
mig_internal novalue _Xport_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t *port_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_allocate(task, &OutP->port_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->port_nameType = port_nameType;
#else	UseStaticMsgType
	OutP->port_nameType.msgt_name = 2;
	OutP->port_nameType.msgt_size = 32;
	OutP->port_nameType.msgt_number = 1;
	OutP->port_nameType.msgt_inline = TRUE;
	OutP->port_nameType.msgt_longform = FALSE;
	OutP->port_nameType.msgt_deallocate = FALSE;
	OutP->port_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_deallocate */
mig_internal novalue _Xport_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_deallocate(task, In0P->port_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_backlog */
mig_internal novalue _Xport_set_backlog
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
		mach_msg_type_t backlogType;
		int backlog;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_backlog
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name, int backlog);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t backlogCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->backlogType != * (int *) &backlogCheck)
#else	UseStaticMsgType
	if ((In0P->backlogType.msgt_inline != TRUE) ||
	    (In0P->backlogType.msgt_longform != FALSE) ||
	    (In0P->backlogType.msgt_name != 2) ||
	    (In0P->backlogType.msgt_number != 1) ||
	    (In0P->backlogType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_backlog(task, In0P->port_name, In0P->backlog);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_status */
mig_internal novalue _Xport_status
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t enabledType;
		mach_port_t enabled;
		mach_msg_type_t num_msgsType;
		int num_msgs;
		mach_msg_type_t backlogType;
		int backlog;
		mach_msg_type_t ownershipType;
		boolean_t ownership;
		mach_msg_type_t receive_rightsType;
		boolean_t receive_rights;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name, mach_port_t *enabled, int *num_msgs, int *backlog, boolean_t *ownership, boolean_t *receive_rights);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t enabledType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t num_msgsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t backlogType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t ownershipType = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t receive_rightsType = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_status(task, In0P->port_name, &OutP->enabled, &OutP->num_msgs, &OutP->backlog, &OutP->ownership, &OutP->receive_rights);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 72;

#if	UseStaticMsgType
	OutP->enabledType = enabledType;
#else	UseStaticMsgType
	OutP->enabledType.msgt_name = 2;
	OutP->enabledType.msgt_size = 32;
	OutP->enabledType.msgt_number = 1;
	OutP->enabledType.msgt_inline = TRUE;
	OutP->enabledType.msgt_longform = FALSE;
	OutP->enabledType.msgt_deallocate = FALSE;
	OutP->enabledType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->num_msgsType = num_msgsType;
#else	UseStaticMsgType
	OutP->num_msgsType.msgt_name = 2;
	OutP->num_msgsType.msgt_size = 32;
	OutP->num_msgsType.msgt_number = 1;
	OutP->num_msgsType.msgt_inline = TRUE;
	OutP->num_msgsType.msgt_longform = FALSE;
	OutP->num_msgsType.msgt_deallocate = FALSE;
	OutP->num_msgsType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->backlogType = backlogType;
#else	UseStaticMsgType
	OutP->backlogType.msgt_name = 2;
	OutP->backlogType.msgt_size = 32;
	OutP->backlogType.msgt_number = 1;
	OutP->backlogType.msgt_inline = TRUE;
	OutP->backlogType.msgt_longform = FALSE;
	OutP->backlogType.msgt_deallocate = FALSE;
	OutP->backlogType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->ownershipType = ownershipType;
#else	UseStaticMsgType
	OutP->ownershipType.msgt_name = 0;
	OutP->ownershipType.msgt_size = 32;
	OutP->ownershipType.msgt_number = 1;
	OutP->ownershipType.msgt_inline = TRUE;
	OutP->ownershipType.msgt_longform = FALSE;
	OutP->ownershipType.msgt_deallocate = FALSE;
	OutP->ownershipType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->receive_rightsType = receive_rightsType;
#else	UseStaticMsgType
	OutP->receive_rightsType.msgt_name = 0;
	OutP->receive_rightsType.msgt_size = 32;
	OutP->receive_rightsType.msgt_number = 1;
	OutP->receive_rightsType.msgt_inline = TRUE;
	OutP->receive_rightsType.msgt_longform = FALSE;
	OutP->receive_rightsType.msgt_deallocate = FALSE;
	OutP->receive_rightsType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_allocate */
mig_internal novalue _Xport_set_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t set_nameType;
		mach_port_t set_name;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t *set_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t set_nameType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_allocate(task, &OutP->set_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->set_nameType = set_nameType;
#else	UseStaticMsgType
	OutP->set_nameType.msgt_name = 2;
	OutP->set_nameType.msgt_size = 32;
	OutP->set_nameType.msgt_number = 1;
	OutP->set_nameType.msgt_inline = TRUE;
	OutP->set_nameType.msgt_longform = FALSE;
	OutP->set_nameType.msgt_deallocate = FALSE;
	OutP->set_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_deallocate */
mig_internal novalue _Xport_set_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t set_nameType;
		mach_port_t set_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t set_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t set_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->set_nameType != * (int *) &set_nameCheck)
#else	UseStaticMsgType
	if ((In0P->set_nameType.msgt_inline != TRUE) ||
	    (In0P->set_nameType.msgt_longform != FALSE) ||
	    (In0P->set_nameType.msgt_name != 2) ||
	    (In0P->set_nameType.msgt_number != 1) ||
	    (In0P->set_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_deallocate(task, In0P->set_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_add */
mig_internal novalue _Xport_set_add
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t set_nameType;
		mach_port_t set_name;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_add
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t set_name, mach_port_t port_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t set_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->set_nameType != * (int *) &set_nameCheck)
#else	UseStaticMsgType
	if ((In0P->set_nameType.msgt_inline != TRUE) ||
	    (In0P->set_nameType.msgt_longform != FALSE) ||
	    (In0P->set_nameType.msgt_name != 2) ||
	    (In0P->set_nameType.msgt_number != 1) ||
	    (In0P->set_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_add(task, In0P->set_name, In0P->port_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_remove */
mig_internal novalue _Xport_set_remove
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_remove
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_remove(task, In0P->port_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_set_status */
mig_internal novalue _Xport_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t set_nameType;
		mach_port_t set_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t membersType;
		mach_port_array_t members;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t set_name, mach_port_array_t *members, mach_msg_type_number_t *membersCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t set_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t membersType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	ipc_space_t task;
	mach_msg_type_number_t membersCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->set_nameType != * (int *) &set_nameCheck)
#else	UseStaticMsgType
	if ((In0P->set_nameType.msgt_inline != TRUE) ||
	    (In0P->set_nameType.msgt_longform != FALSE) ||
	    (In0P->set_nameType.msgt_name != 2) ||
	    (In0P->set_nameType.msgt_number != 1) ||
	    (In0P->set_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_status(task, In0P->set_name, &OutP->members, &membersCnt);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->membersType = membersType;
#else	UseStaticMsgType
	OutP->membersType.msgtl_name = 2;
	OutP->membersType.msgtl_size = 32;
	OutP->membersType.msgtl_header.msgt_name = 0;
	OutP->membersType.msgtl_header.msgt_size = 0;
	OutP->membersType.msgtl_header.msgt_number = 0;
	OutP->membersType.msgtl_header.msgt_inline = FALSE;
	OutP->membersType.msgtl_header.msgt_longform = TRUE;
	OutP->membersType.msgtl_header.msgt_deallocate = FALSE;
	OutP->membersType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->membersType.msgtl_number /* membersCnt */ = /* membersType.msgtl_number */ membersCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_insert_send */
mig_internal novalue _Xport_insert_send
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t my_portType;
		mach_port_t my_port;
		mach_msg_type_t his_nameType;
		mach_port_t his_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_insert_send
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t my_port, mach_port_t his_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t his_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->my_portType.msgt_inline != TRUE) ||
	    (In0P->my_portType.msgt_longform != FALSE) ||
	    (In0P->my_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->my_portType.msgt_number != 1) ||
	    (In0P->my_portType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->his_nameType != * (int *) &his_nameCheck)
#else	UseStaticMsgType
	if ((In0P->his_nameType.msgt_inline != TRUE) ||
	    (In0P->his_nameType.msgt_longform != FALSE) ||
	    (In0P->his_nameType.msgt_name != 2) ||
	    (In0P->his_nameType.msgt_number != 1) ||
	    (In0P->his_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_insert_send(task, In0P->my_port, In0P->his_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_extract_send */
mig_internal novalue _Xport_extract_send
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t his_nameType;
		mach_port_t his_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t his_portType;
		mach_port_t his_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_extract_send
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t his_name, mach_port_t *his_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t his_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t his_portType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->his_nameType != * (int *) &his_nameCheck)
#else	UseStaticMsgType
	if ((In0P->his_nameType.msgt_inline != TRUE) ||
	    (In0P->his_nameType.msgt_longform != FALSE) ||
	    (In0P->his_nameType.msgt_name != 2) ||
	    (In0P->his_nameType.msgt_number != 1) ||
	    (In0P->his_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_extract_send(task, In0P->his_name, &OutP->his_port);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->his_portType = his_portType;
#else	UseStaticMsgType
	OutP->his_portType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->his_portType.msgt_size = 32;
	OutP->his_portType.msgt_number = 1;
	OutP->his_portType.msgt_inline = TRUE;
	OutP->his_portType.msgt_longform = FALSE;
	OutP->his_portType.msgt_deallocate = FALSE;
	OutP->his_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_insert_receive */
mig_internal novalue _Xport_insert_receive
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t my_portType;
		mach_port_t my_port;
		mach_msg_type_t his_nameType;
		mach_port_t his_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_insert_receive
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t my_port, mach_port_t his_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t his_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->my_portType.msgt_inline != TRUE) ||
	    (In0P->my_portType.msgt_longform != FALSE) ||
	    (In0P->my_portType.msgt_name != MACH_MSG_TYPE_PORT_RECEIVE) ||
	    (In0P->my_portType.msgt_number != 1) ||
	    (In0P->my_portType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->his_nameType != * (int *) &his_nameCheck)
#else	UseStaticMsgType
	if ((In0P->his_nameType.msgt_inline != TRUE) ||
	    (In0P->his_nameType.msgt_longform != FALSE) ||
	    (In0P->his_nameType.msgt_name != 2) ||
	    (In0P->his_nameType.msgt_number != 1) ||
	    (In0P->his_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_insert_receive(task, In0P->my_port, In0P->his_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine port_extract_receive */
mig_internal novalue _Xport_extract_receive
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t his_nameType;
		mach_port_t his_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t his_portType;
		mach_port_t his_port;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_extract_receive
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t his_name, mach_port_t *his_port);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t his_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t his_portType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_RECEIVE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->his_nameType != * (int *) &his_nameCheck)
#else	UseStaticMsgType
	if ((In0P->his_nameType.msgt_inline != TRUE) ||
	    (In0P->his_nameType.msgt_longform != FALSE) ||
	    (In0P->his_nameType.msgt_name != 2) ||
	    (In0P->his_nameType.msgt_number != 1) ||
	    (In0P->his_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_extract_receive(task, In0P->his_name, &OutP->his_port);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->his_portType = his_portType;
#else	UseStaticMsgType
	OutP->his_portType.msgt_name = MACH_MSG_TYPE_PORT_RECEIVE;
	OutP->his_portType.msgt_size = 32;
	OutP->his_portType.msgt_number = 1;
	OutP->his_portType.msgt_inline = TRUE;
	OutP->his_portType.msgt_longform = FALSE;
	OutP->his_portType.msgt_deallocate = FALSE;
	OutP->his_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (IP_VALID((ipc_port_t) InHeadP->msgh_reply_port) &&
	    IP_VALID((ipc_port_t) OutP->his_port) &&
	    ipc_port_check_circularity((ipc_port_t) OutP->his_port, (ipc_port_t) InHeadP->msgh_reply_port))
		OutHeadP->msgh_bits |= MACH_MSGH_BITS_CIRCULAR;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_map */
mig_internal novalue _Xvm_map
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t maskType;
		vm_address_t mask;
		mach_msg_type_t anywhereType;
		boolean_t anywhere;
		mach_msg_type_t memory_objectType;
		mach_port_t memory_object;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t copyType;
		boolean_t copy;
		mach_msg_type_t cur_protectionType;
		vm_prot_t cur_protection;
		mach_msg_type_t max_protectionType;
		vm_prot_t max_protection;
		mach_msg_type_t inheritanceType;
		vm_inherit_t inheritance;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t addressType;
		vm_address_t address;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_map
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t *address, vm_size_t size, vm_address_t mask, boolean_t anywhere, mach_port_t memory_object, vm_offset_t offset, boolean_t copy, vm_prot_t cur_protection, vm_prot_t max_protection, vm_inherit_t inheritance);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t maskCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t anywhereCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t offsetCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t copyCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t cur_protectionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t max_protectionCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t inheritanceCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t addressType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 104) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->maskType != * (int *) &maskCheck)
#else	UseStaticMsgType
	if ((In0P->maskType.msgt_inline != TRUE) ||
	    (In0P->maskType.msgt_longform != FALSE) ||
	    (In0P->maskType.msgt_name != 2) ||
	    (In0P->maskType.msgt_number != 1) ||
	    (In0P->maskType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->anywhereType != * (int *) &anywhereCheck)
#else	UseStaticMsgType
	if ((In0P->anywhereType.msgt_inline != TRUE) ||
	    (In0P->anywhereType.msgt_longform != FALSE) ||
	    (In0P->anywhereType.msgt_name != 0) ||
	    (In0P->anywhereType.msgt_number != 1) ||
	    (In0P->anywhereType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_objectType.msgt_inline != TRUE) ||
	    (In0P->memory_objectType.msgt_longform != FALSE) ||
	    (In0P->memory_objectType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_objectType.msgt_number != 1) ||
	    (In0P->memory_objectType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->copyType != * (int *) &copyCheck)
#else	UseStaticMsgType
	if ((In0P->copyType.msgt_inline != TRUE) ||
	    (In0P->copyType.msgt_longform != FALSE) ||
	    (In0P->copyType.msgt_name != 0) ||
	    (In0P->copyType.msgt_number != 1) ||
	    (In0P->copyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->cur_protectionType != * (int *) &cur_protectionCheck)
#else	UseStaticMsgType
	if ((In0P->cur_protectionType.msgt_inline != TRUE) ||
	    (In0P->cur_protectionType.msgt_longform != FALSE) ||
	    (In0P->cur_protectionType.msgt_name != 2) ||
	    (In0P->cur_protectionType.msgt_number != 1) ||
	    (In0P->cur_protectionType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->max_protectionType != * (int *) &max_protectionCheck)
#else	UseStaticMsgType
	if ((In0P->max_protectionType.msgt_inline != TRUE) ||
	    (In0P->max_protectionType.msgt_longform != FALSE) ||
	    (In0P->max_protectionType.msgt_name != 2) ||
	    (In0P->max_protectionType.msgt_number != 1) ||
	    (In0P->max_protectionType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->inheritanceType != * (int *) &inheritanceCheck)
#else	UseStaticMsgType
	if ((In0P->inheritanceType.msgt_inline != TRUE) ||
	    (In0P->inheritanceType.msgt_longform != FALSE) ||
	    (In0P->inheritanceType.msgt_name != 2) ||
	    (In0P->inheritanceType.msgt_number != 1) ||
	    (In0P->inheritanceType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_map(target_task, &In0P->address, In0P->size, In0P->mask, In0P->anywhere, null_conversion(In0P->memory_object), In0P->offset, In0P->copy, In0P->cur_protection, In0P->max_protection, In0P->inheritance);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->memory_object))
		ipc_port_release_send((ipc_port_t) In0P->memory_object);

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->addressType = addressType;
#else	UseStaticMsgType
	OutP->addressType.msgt_name = 2;
	OutP->addressType.msgt_size = 32;
	OutP->addressType.msgt_number = 1;
	OutP->addressType.msgt_inline = TRUE;
	OutP->addressType.msgt_longform = FALSE;
	OutP->addressType.msgt_deallocate = FALSE;
	OutP->addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	/* 0 */ OutP->address = In0P->address;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* SimpleRoutine memory_object_data_error */
mig_internal novalue _Xmemory_object_data_error
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t error_valueType;
		kern_return_t error_value;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_error
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, vm_offset_t offset, vm_size_t size, kern_return_t error_value);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t offsetCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t error_valueCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->error_valueType != * (int *) &error_valueCheck)
#else	UseStaticMsgType
	if ((In0P->error_valueType.msgt_inline != TRUE) ||
	    (In0P->error_valueType.msgt_longform != FALSE) ||
	    (In0P->error_valueType.msgt_name != 2) ||
	    (In0P->error_valueType.msgt_number != 1) ||
	    (In0P->error_valueType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_error(vm_object_lookup(In0P->Head.msgh_request_port), In0P->offset, In0P->size, In0P->error_value);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_set_attributes */
mig_internal novalue _Xmemory_object_set_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t object_readyType;
		boolean_t object_ready;
		mach_msg_type_t may_cacheType;
		boolean_t may_cache;
		mach_msg_type_t copy_strategyType;
		memory_object_copy_strategy_t copy_strategy;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_set_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, boolean_t object_ready, boolean_t may_cache, memory_object_copy_strategy_t copy_strategy);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t object_readyCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t may_cacheCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t copy_strategyCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->object_readyType != * (int *) &object_readyCheck)
#else	UseStaticMsgType
	if ((In0P->object_readyType.msgt_inline != TRUE) ||
	    (In0P->object_readyType.msgt_longform != FALSE) ||
	    (In0P->object_readyType.msgt_name != 0) ||
	    (In0P->object_readyType.msgt_number != 1) ||
	    (In0P->object_readyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->may_cacheType != * (int *) &may_cacheCheck)
#else	UseStaticMsgType
	if ((In0P->may_cacheType.msgt_inline != TRUE) ||
	    (In0P->may_cacheType.msgt_longform != FALSE) ||
	    (In0P->may_cacheType.msgt_name != 0) ||
	    (In0P->may_cacheType.msgt_number != 1) ||
	    (In0P->may_cacheType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->copy_strategyType != * (int *) &copy_strategyCheck)
#else	UseStaticMsgType
	if ((In0P->copy_strategyType.msgt_inline != TRUE) ||
	    (In0P->copy_strategyType.msgt_longform != FALSE) ||
	    (In0P->copy_strategyType.msgt_name != 2) ||
	    (In0P->copy_strategyType.msgt_number != 1) ||
	    (In0P->copy_strategyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_set_attributes(vm_object_lookup(In0P->Head.msgh_request_port), In0P->object_ready, In0P->may_cache, In0P->copy_strategy);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_destroy */
mig_internal novalue _Xmemory_object_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t reasonType;
		kern_return_t reason;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_object_t memory_control, kern_return_t reason);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t reasonCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->reasonType != * (int *) &reasonCheck)
#else	UseStaticMsgType
	if ((In0P->reasonType.msgt_inline != TRUE) ||
	    (In0P->reasonType.msgt_longform != FALSE) ||
	    (In0P->reasonType.msgt_name != 2) ||
	    (In0P->reasonType.msgt_number != 1) ||
	    (In0P->reasonType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_destroy(vm_object_lookup(In0P->Head.msgh_request_port), In0P->reason);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* Routine port_set_backup */
mig_internal novalue _Xport_set_backup
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t port_nameType;
		mach_port_t port_name;
		mach_msg_type_t backupType;
		port_t backup;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t previousType;
		mach_port_t previous;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t port_set_backup
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t port_name, port_t backup, mach_port_t *previous);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t port_nameCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t previousType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->port_nameType != * (int *) &port_nameCheck)
#else	UseStaticMsgType
	if ((In0P->port_nameType.msgt_inline != TRUE) ||
	    (In0P->port_nameType.msgt_longform != FALSE) ||
	    (In0P->port_nameType.msgt_name != 2) ||
	    (In0P->port_nameType.msgt_number != 1) ||
	    (In0P->port_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->backupType.msgt_inline != TRUE) ||
	    (In0P->backupType.msgt_longform != FALSE) ||
	    (In0P->backupType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->backupType.msgt_number != 1) ||
	    (In0P->backupType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = port_set_backup(task, In0P->port_name, In0P->backup, &OutP->previous);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->previousType = previousType;
#else	UseStaticMsgType
	OutP->previousType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->previousType.msgt_size = 32;
	OutP->previousType.msgt_number = 1;
	OutP->previousType.msgt_inline = TRUE;
	OutP->previousType.msgt_longform = FALSE;
	OutP->previousType.msgt_deallocate = FALSE;
	OutP->previousType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_machine_attribute */
mig_internal novalue _Xvm_machine_attribute
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t attributeType;
		vm_machine_attribute_t attribute;
		mach_msg_type_t valueType;
		vm_machine_attribute_val_t value;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t valueType;
		vm_machine_attribute_val_t value;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_machine_attribute
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t target_task, vm_address_t address, vm_size_t size, vm_machine_attribute_t attribute, vm_machine_attribute_val_t *value);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t attributeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t valueCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t valueType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	vm_map_t target_task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->attributeType != * (int *) &attributeCheck)
#else	UseStaticMsgType
	if ((In0P->attributeType.msgt_inline != TRUE) ||
	    (In0P->attributeType.msgt_longform != FALSE) ||
	    (In0P->attributeType.msgt_name != 2) ||
	    (In0P->attributeType.msgt_number != 1) ||
	    (In0P->attributeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->valueType != * (int *) &valueCheck)
#else	UseStaticMsgType
	if ((In0P->valueType.msgt_inline != TRUE) ||
	    (In0P->valueType.msgt_longform != FALSE) ||
	    (In0P->valueType.msgt_name != 2) ||
	    (In0P->valueType.msgt_number != 1) ||
	    (In0P->valueType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	target_task /* convert_port_to_map 0 Head.msgh_request_port */ = /* target_task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_machine_attribute(target_task, In0P->address, In0P->size, In0P->attribute, &In0P->value);
	vm_map_deallocate(target_task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->valueType = valueType;
#else	UseStaticMsgType
	OutP->valueType.msgt_name = 2;
	OutP->valueType.msgt_size = 32;
	OutP->valueType.msgt_number = 1;
	OutP->valueType.msgt_inline = TRUE;
	OutP->valueType.msgt_longform = FALSE;
	OutP->valueType.msgt_deallocate = FALSE;
	OutP->valueType.msgt_unused = 0;
#endif	UseStaticMsgType

	/* 0 */ OutP->value = In0P->value;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

mig_external boolean_t mach_server
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	register mach_msg_header_t *InP =  InHeadP;
	register mig_reply_header_t *OutP = (mig_reply_header_t *) OutHeadP;

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	OutP->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InP->msgh_bits), 0);
	OutP->Head.msgh_size = sizeof *OutP;
	OutP->Head.msgh_remote_port = InP->msgh_reply_port;
	OutP->Head.msgh_local_port = MACH_PORT_NULL;
	OutP->Head.msgh_kind = InP->msgh_kind;
	OutP->Head.msgh_id = InP->msgh_id + 100;

#if	UseStaticMsgType
	OutP->RetCodeType = RetCodeType;
#else	UseStaticMsgType
	OutP->RetCodeType.msgt_name = MACH_MSG_TYPE_INTEGER_32;
	OutP->RetCodeType.msgt_size = 32;
	OutP->RetCodeType.msgt_number = 1;
	OutP->RetCodeType.msgt_inline = TRUE;
	OutP->RetCodeType.msgt_longform = FALSE;
	OutP->RetCodeType.msgt_deallocate = FALSE;
	OutP->RetCodeType.msgt_unused = 0;
#endif	UseStaticMsgType
	OutP->RetCode = MIG_BAD_ID;

	if ((InP->msgh_id > 2100) || (InP->msgh_id < 2000))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			_Xtask_create,
			_Xtask_terminate,
			0,
			0,
			_Xtask_threads,
			0,
			0,
			0,
			0,
			_Xthread_terminate,
			0,
			0,
			0,
			0,
			_Xvm_allocate,
			0,
			_Xvm_deallocate,
			_Xvm_protect,
			_Xvm_inherit,
			_Xvm_read,
			_Xvm_write,
			_Xvm_copy,
			_Xvm_region,
			_Xvm_statistics,
			0,
			_Xvm_pageable,
			_Xmach_ports_register,
			_Xmach_ports_lookup,
			0,
			0,
			0,
			_Xmemory_object_data_provided,
			_Xmemory_object_data_unavailable,
			_Xmemory_object_get_attributes,
			_Xvm_set_default_memory_manager,
			0,
			_Xmemory_object_lock_request,
			0,
			0,
			0,
			_Xxxx_host_info,
			_Xxxx_slot_info,
			_Xxxx_cpu_control,
			0,
			0,
			0,
			0,
			0,
			0,
			_Xtask_suspend,
			_Xtask_resume,
			_Xtask_get_special_port,
			_Xtask_set_special_port,
			_Xtask_info,
			_Xthread_create,
			_Xthread_suspend,
			_Xthread_resume,
			_Xthread_abort,
			_Xthread_get_state,
			_Xthread_set_state,
			_Xthread_get_special_port,
			_Xthread_set_special_port,
			_Xthread_info,
			_Xtask_set_emulation,
			0,
			0,
			_Xport_names,
			_Xport_type,
			_Xport_rename,
			_Xport_allocate,
			_Xport_deallocate,
			_Xport_set_backlog,
			_Xport_status,
			_Xport_set_allocate,
			_Xport_set_deallocate,
			_Xport_set_add,
			_Xport_set_remove,
			_Xport_set_status,
			_Xport_insert_send,
			_Xport_extract_send,
			_Xport_insert_receive,
			_Xport_extract_receive,
			_Xvm_map,
			_Xmemory_object_data_error,
			_Xmemory_object_set_attributes,
			_Xmemory_object_destroy,
			0,
			0,
			0,
			0,
			0,
			_Xport_set_backup,
			_Xvm_machine_attribute,
			0,
		};

		if (routines[InP->msgh_id - 2000])
			(routines[InP->msgh_id - 2000]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
