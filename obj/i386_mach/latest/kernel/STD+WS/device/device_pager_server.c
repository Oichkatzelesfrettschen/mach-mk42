/* Module memory_object */

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

/* SimpleRoutine memory_object_init */
mig_internal novalue _Xmemory_object_init
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t memory_object_nameType;
		memory_object_name_t memory_object_name;
		mach_msg_type_t memory_object_page_sizeType;
		vm_size_t memory_object_page_size;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_init
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t memory_object, memory_object_control_t memory_control, memory_object_name_t memory_object_name, vm_size_t memory_object_page_size);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t memory_object_page_sizeCheck = {
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
	if ((msgh_size != 48) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_object_nameType.msgt_inline != TRUE) ||
	    (In0P->memory_object_nameType.msgt_longform != FALSE) ||
	    (In0P->memory_object_nameType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_object_nameType.msgt_number != 1) ||
	    (In0P->memory_object_nameType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->memory_object_page_sizeType != * (int *) &memory_object_page_sizeCheck)
#else	UseStaticMsgType
	if ((In0P->memory_object_page_sizeType.msgt_inline != TRUE) ||
	    (In0P->memory_object_page_sizeType.msgt_longform != FALSE) ||
	    (In0P->memory_object_page_sizeType.msgt_name != 2) ||
	    (In0P->memory_object_page_sizeType.msgt_number != 1) ||
	    (In0P->memory_object_page_sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_init(null_conversion(In0P->Head.msgh_request_port), In0P->memory_control, In0P->memory_object_name, In0P->memory_object_page_size);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_terminate */
mig_internal novalue _Xmemory_object_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t memory_object_nameType;
		memory_object_name_t memory_object_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
		(memory_object_t memory_object, memory_object_control_t memory_control, memory_object_name_t memory_object_name);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_RECEIVE) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_object_nameType.msgt_inline != TRUE) ||
	    (In0P->memory_object_nameType.msgt_longform != FALSE) ||
	    (In0P->memory_object_nameType.msgt_name != MACH_MSG_TYPE_PORT_RECEIVE) ||
	    (In0P->memory_object_nameType.msgt_number != 1) ||
	    (In0P->memory_object_nameType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_terminate(In0P->Head.msgh_request_port, In0P->memory_control, In0P->memory_object_name);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_copy */
mig_internal novalue _Xmemory_object_copy
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t old_memory_controlType;
		memory_object_control_t old_memory_control;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t lengthType;
		vm_size_t length;
		mach_msg_type_t new_memory_objectType;
		memory_object_t new_memory_object;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_copy
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t old_memory_object, memory_object_control_t old_memory_control, vm_offset_t offset, vm_size_t length, memory_object_t new_memory_object);
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
	static mach_msg_type_t lengthCheck = {
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
	if ((In0P->old_memory_controlType.msgt_inline != TRUE) ||
	    (In0P->old_memory_controlType.msgt_longform != FALSE) ||
	    (In0P->old_memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->old_memory_controlType.msgt_number != 1) ||
	    (In0P->old_memory_controlType.msgt_size != 32))
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
	if (* (int *) &In0P->lengthType != * (int *) &lengthCheck)
#else	UseStaticMsgType
	if ((In0P->lengthType.msgt_inline != TRUE) ||
	    (In0P->lengthType.msgt_longform != FALSE) ||
	    (In0P->lengthType.msgt_name != 2) ||
	    (In0P->lengthType.msgt_number != 1) ||
	    (In0P->lengthType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->new_memory_objectType.msgt_inline != TRUE) ||
	    (In0P->new_memory_objectType.msgt_longform != FALSE) ||
	    (In0P->new_memory_objectType.msgt_name != MACH_MSG_TYPE_PORT_RECEIVE) ||
	    (In0P->new_memory_objectType.msgt_number != 1) ||
	    (In0P->new_memory_objectType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_copy(null_conversion(In0P->Head.msgh_request_port), In0P->old_memory_control, In0P->offset, In0P->length, In0P->new_memory_object);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_data_request */
mig_internal novalue _Xmemory_object_data_request
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t lengthType;
		vm_size_t length;
		mach_msg_type_t desired_accessType;
		vm_prot_t desired_access;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_request
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t memory_object, memory_object_control_t memory_control, vm_offset_t offset, vm_size_t length, vm_prot_t desired_access);
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
	static mach_msg_type_t lengthCheck = {
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
	static mach_msg_type_t desired_accessCheck = {
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
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
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
	if (* (int *) &In0P->lengthType != * (int *) &lengthCheck)
#else	UseStaticMsgType
	if ((In0P->lengthType.msgt_inline != TRUE) ||
	    (In0P->lengthType.msgt_longform != FALSE) ||
	    (In0P->lengthType.msgt_name != 2) ||
	    (In0P->lengthType.msgt_number != 1) ||
	    (In0P->lengthType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->desired_accessType != * (int *) &desired_accessCheck)
#else	UseStaticMsgType
	if ((In0P->desired_accessType.msgt_inline != TRUE) ||
	    (In0P->desired_accessType.msgt_longform != FALSE) ||
	    (In0P->desired_accessType.msgt_name != 2) ||
	    (In0P->desired_accessType.msgt_number != 1) ||
	    (In0P->desired_accessType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_request(null_conversion(In0P->Head.msgh_request_port), In0P->memory_control, In0P->offset, In0P->length, In0P->desired_access);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_data_unlock */
mig_internal novalue _Xmemory_object_data_unlock
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t lengthType;
		vm_size_t length;
		mach_msg_type_t desired_accessType;
		vm_prot_t desired_access;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_unlock
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t memory_object, memory_object_control_t memory_control, vm_offset_t offset, vm_size_t length, vm_prot_t desired_access);
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
	static mach_msg_type_t lengthCheck = {
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
	static mach_msg_type_t desired_accessCheck = {
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
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
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
	if (* (int *) &In0P->lengthType != * (int *) &lengthCheck)
#else	UseStaticMsgType
	if ((In0P->lengthType.msgt_inline != TRUE) ||
	    (In0P->lengthType.msgt_longform != FALSE) ||
	    (In0P->lengthType.msgt_name != 2) ||
	    (In0P->lengthType.msgt_number != 1) ||
	    (In0P->lengthType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->desired_accessType != * (int *) &desired_accessCheck)
#else	UseStaticMsgType
	if ((In0P->desired_accessType.msgt_inline != TRUE) ||
	    (In0P->desired_accessType.msgt_longform != FALSE) ||
	    (In0P->desired_accessType.msgt_name != 2) ||
	    (In0P->desired_accessType.msgt_number != 1) ||
	    (In0P->desired_accessType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_unlock(null_conversion(In0P->Head.msgh_request_port), In0P->memory_control, In0P->offset, In0P->length, In0P->desired_access);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_data_write */
mig_internal novalue _Xmemory_object_data_write
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
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
	mig_external kern_return_t memory_object_data_write
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t memory_object, memory_object_control_t memory_control, vm_offset_t offset, pointer_t data, mach_msg_type_number_t dataCnt);
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

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
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

	OutP->RetCode = memory_object_data_write(null_conversion(In0P->Head.msgh_request_port), In0P->memory_control, In0P->offset, In0P->data, In0P->dataType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_lock_completed */
mig_internal novalue _Xmemory_object_lock_completed
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t lengthType;
		vm_size_t length;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_lock_completed
#if	(defined(__STDC__) || defined(c_plusplus))
		(memory_object_t memory_object, memory_object_control_t memory_control, vm_offset_t offset, vm_size_t length);
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
	static mach_msg_type_t lengthCheck = {
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
	if ((msgh_size != 48) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->memory_controlType.msgt_inline != TRUE) ||
	    (In0P->memory_controlType.msgt_longform != FALSE) ||
	    (In0P->memory_controlType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_controlType.msgt_number != 1) ||
	    (In0P->memory_controlType.msgt_size != 32))
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
	if (* (int *) &In0P->lengthType != * (int *) &lengthCheck)
#else	UseStaticMsgType
	if ((In0P->lengthType.msgt_inline != TRUE) ||
	    (In0P->lengthType.msgt_longform != FALSE) ||
	    (In0P->lengthType.msgt_name != 2) ||
	    (In0P->lengthType.msgt_number != 1) ||
	    (In0P->lengthType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_lock_completed(In0P->Head.msgh_request_port, In0P->memory_control, In0P->offset, In0P->length);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

mig_external boolean_t memory_object_server
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

	if ((InP->msgh_id > 2206) || (InP->msgh_id < 2200))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xmemory_object_init,
			_Xmemory_object_terminate,
			_Xmemory_object_copy,
			_Xmemory_object_data_request,
			_Xmemory_object_data_unlock,
			_Xmemory_object_data_write,
			_Xmemory_object_lock_completed,
		};

		if (routines[InP->msgh_id - 2200])
			(routines[InP->msgh_id - 2200]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
