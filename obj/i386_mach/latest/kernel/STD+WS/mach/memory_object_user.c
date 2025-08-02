#include "memory_object_user.h"
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mach_types.h>
#include <mach/mig_errors.h>
#include <mach/msg_type.h>
#ifndef	KERNEL
#include <strings.h>
#endif	KERNEL
/* LINTLIBRARY */

extern mach_port_t mig_get_reply_port();
extern void mig_dealloc_reply_port();

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

#define msgh_request_port	msgh_remote_port
#define msgh_reply_port		msgh_local_port

mig_external void init_memory_object
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_port_t rep_port)
#else
	(rep_port)
	mach_port_t rep_port;
#endif
{
#ifdef	lint
	rep_port++;
#endif
}

/* SimpleRoutine memory_object_init */
mig_external kern_return_t memory_object_init
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	memory_object_name_t memory_object_name,
	vm_size_t memory_object_page_size
)
#else
	(memory_object, memory_control, memory_object_name, memory_object_page_size)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	memory_object_name_t memory_object_name;
	vm_size_t memory_object_page_size;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 48;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t memory_object_nameType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t memory_object_page_sizeType = {
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
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = FALSE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->memory_object_nameType = memory_object_nameType;
#else	UseStaticMsgType
	InP->memory_object_nameType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_object_nameType.msgt_size = 32;
	InP->memory_object_nameType.msgt_number = 1;
	InP->memory_object_nameType.msgt_inline = TRUE;
	InP->memory_object_nameType.msgt_longform = FALSE;
	InP->memory_object_nameType.msgt_deallocate = FALSE;
	InP->memory_object_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_object_name /* memory_object_name */ = /* memory_object_name */ memory_object_name;

#if	UseStaticMsgType
	InP->memory_object_page_sizeType = memory_object_page_sizeType;
#else	UseStaticMsgType
	InP->memory_object_page_sizeType.msgt_name = 2;
	InP->memory_object_page_sizeType.msgt_size = 32;
	InP->memory_object_page_sizeType.msgt_number = 1;
	InP->memory_object_page_sizeType.msgt_inline = TRUE;
	InP->memory_object_page_sizeType.msgt_longform = FALSE;
	InP->memory_object_page_sizeType.msgt_deallocate = FALSE;
	InP->memory_object_page_sizeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_object_page_size /* memory_object_page_size */ = /* memory_object_page_size */ memory_object_page_size;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2200;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_terminate */
mig_external kern_return_t memory_object_terminate
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	memory_object_name_t memory_object_name
)
#else
	(memory_object, memory_control, memory_object_name)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	memory_object_name_t memory_object_name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_controlType;
		memory_object_control_t memory_control;
		mach_msg_type_t memory_object_nameType;
		memory_object_name_t memory_object_name;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MOVE_RECEIVE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		TRUE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t memory_object_nameType = {
		/* msgt_name = */		MACH_MSG_TYPE_MOVE_RECEIVE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		TRUE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MOVE_RECEIVE;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = TRUE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->memory_object_nameType = memory_object_nameType;
#else	UseStaticMsgType
	InP->memory_object_nameType.msgt_name = MACH_MSG_TYPE_MOVE_RECEIVE;
	InP->memory_object_nameType.msgt_size = 32;
	InP->memory_object_nameType.msgt_number = 1;
	InP->memory_object_nameType.msgt_inline = TRUE;
	InP->memory_object_nameType.msgt_longform = FALSE;
	InP->memory_object_nameType.msgt_deallocate = TRUE;
	InP->memory_object_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_object_name /* memory_object_name */ = /* memory_object_name */ memory_object_name;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_MOVE_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2201;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_copy */
mig_external kern_return_t memory_object_copy
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t old_memory_object,
	memory_object_control_t old_memory_control,
	vm_offset_t offset,
	vm_size_t length,
	memory_object_t new_memory_object
)
#else
	(old_memory_object, old_memory_control, offset, length, new_memory_object)
	memory_object_t old_memory_object;
	memory_object_control_t old_memory_control;
	vm_offset_t offset;
	vm_size_t length;
	memory_object_t new_memory_object;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 56;

#if	UseStaticMsgType
	static mach_msg_type_t old_memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
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

#if	UseStaticMsgType
	static mach_msg_type_t lengthType = {
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
	static mach_msg_type_t new_memory_objectType = {
		/* msgt_name = */		MACH_MSG_TYPE_MOVE_RECEIVE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		TRUE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->old_memory_controlType = old_memory_controlType;
#else	UseStaticMsgType
	InP->old_memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->old_memory_controlType.msgt_size = 32;
	InP->old_memory_controlType.msgt_number = 1;
	InP->old_memory_controlType.msgt_inline = TRUE;
	InP->old_memory_controlType.msgt_longform = FALSE;
	InP->old_memory_controlType.msgt_deallocate = FALSE;
	InP->old_memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->old_memory_control /* old_memory_control */ = /* old_memory_control */ old_memory_control;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

#if	UseStaticMsgType
	InP->lengthType = lengthType;
#else	UseStaticMsgType
	InP->lengthType.msgt_name = 2;
	InP->lengthType.msgt_size = 32;
	InP->lengthType.msgt_number = 1;
	InP->lengthType.msgt_inline = TRUE;
	InP->lengthType.msgt_longform = FALSE;
	InP->lengthType.msgt_deallocate = FALSE;
	InP->lengthType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->length /* length */ = /* length */ length;

#if	UseStaticMsgType
	InP->new_memory_objectType = new_memory_objectType;
#else	UseStaticMsgType
	InP->new_memory_objectType.msgt_name = MACH_MSG_TYPE_MOVE_RECEIVE;
	InP->new_memory_objectType.msgt_size = 32;
	InP->new_memory_objectType.msgt_number = 1;
	InP->new_memory_objectType.msgt_inline = TRUE;
	InP->new_memory_objectType.msgt_longform = FALSE;
	InP->new_memory_objectType.msgt_deallocate = TRUE;
	InP->new_memory_objectType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_memory_object /* new_memory_object */ = /* new_memory_object */ new_memory_object;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = old_memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2202;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_data_request */
mig_external kern_return_t memory_object_data_request
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t offset,
	vm_size_t length,
	vm_prot_t desired_access
)
#else
	(memory_object, memory_control, offset, length, desired_access)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 56;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
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

#if	UseStaticMsgType
	static mach_msg_type_t lengthType = {
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
	static mach_msg_type_t desired_accessType = {
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
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = FALSE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

#if	UseStaticMsgType
	InP->lengthType = lengthType;
#else	UseStaticMsgType
	InP->lengthType.msgt_name = 2;
	InP->lengthType.msgt_size = 32;
	InP->lengthType.msgt_number = 1;
	InP->lengthType.msgt_inline = TRUE;
	InP->lengthType.msgt_longform = FALSE;
	InP->lengthType.msgt_deallocate = FALSE;
	InP->lengthType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->length /* length */ = /* length */ length;

#if	UseStaticMsgType
	InP->desired_accessType = desired_accessType;
#else	UseStaticMsgType
	InP->desired_accessType.msgt_name = 2;
	InP->desired_accessType.msgt_size = 32;
	InP->desired_accessType.msgt_number = 1;
	InP->desired_accessType.msgt_inline = TRUE;
	InP->desired_accessType.msgt_longform = FALSE;
	InP->desired_accessType.msgt_deallocate = FALSE;
	InP->desired_accessType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->desired_access /* desired_access */ = /* desired_access */ desired_access;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2203;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_data_unlock */
mig_external kern_return_t memory_object_data_unlock
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t offset,
	vm_size_t length,
	vm_prot_t desired_access
)
#else
	(memory_object, memory_control, offset, length, desired_access)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 56;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
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

#if	UseStaticMsgType
	static mach_msg_type_t lengthType = {
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
	static mach_msg_type_t desired_accessType = {
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
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = FALSE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

#if	UseStaticMsgType
	InP->lengthType = lengthType;
#else	UseStaticMsgType
	InP->lengthType.msgt_name = 2;
	InP->lengthType.msgt_size = 32;
	InP->lengthType.msgt_number = 1;
	InP->lengthType.msgt_inline = TRUE;
	InP->lengthType.msgt_longform = FALSE;
	InP->lengthType.msgt_deallocate = FALSE;
	InP->lengthType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->length /* length */ = /* length */ length;

#if	UseStaticMsgType
	InP->desired_accessType = desired_accessType;
#else	UseStaticMsgType
	InP->desired_accessType.msgt_name = 2;
	InP->desired_accessType.msgt_size = 32;
	InP->desired_accessType.msgt_number = 1;
	InP->desired_accessType.msgt_inline = TRUE;
	InP->desired_accessType.msgt_longform = FALSE;
	InP->desired_accessType.msgt_deallocate = FALSE;
	InP->desired_accessType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->desired_access /* desired_access */ = /* desired_access */ desired_access;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2204;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_data_write */
mig_external kern_return_t memory_object_data_write
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t offset,
	pointer_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(memory_object, memory_control, offset, data, dataCnt)
	memory_object_t memory_object;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 56;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
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

#if	UseStaticMsgType
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = FALSE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

#if	UseStaticMsgType
	InP->dataType = dataType;
#else	UseStaticMsgType
	InP->dataType.msgtl_name = 9;
	InP->dataType.msgtl_size = 8;
	InP->dataType.msgtl_header.msgt_name = 0;
	InP->dataType.msgtl_header.msgt_size = 0;
	InP->dataType.msgtl_header.msgt_number = 0;
	InP->dataType.msgtl_header.msgt_inline = FALSE;
	InP->dataType.msgtl_header.msgt_longform = TRUE;
	InP->dataType.msgtl_header.msgt_deallocate = FALSE;
	InP->dataType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->data /* data */ = /* data */ data;

	InP->dataType.msgtl_number /* dataCnt */ = /* dataType.msgtl_number */ dataCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2205;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_lock_completed */
mig_external kern_return_t memory_object_lock_completed
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	mach_msg_type_name_t memory_objectPoly,
	memory_object_control_t memory_control,
	vm_offset_t offset,
	vm_size_t length
)
#else
	(memory_object, memory_objectPoly, memory_control, offset, length)
	memory_object_t memory_object;
	mach_msg_type_name_t memory_objectPoly;
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t length;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 48;

#if	UseStaticMsgType
	static mach_msg_type_t memory_controlType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
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

#if	UseStaticMsgType
	static mach_msg_type_t lengthType = {
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
	InP->memory_controlType = memory_controlType;
#else	UseStaticMsgType
	InP->memory_controlType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_controlType.msgt_size = 32;
	InP->memory_controlType.msgt_number = 1;
	InP->memory_controlType.msgt_inline = TRUE;
	InP->memory_controlType.msgt_longform = FALSE;
	InP->memory_controlType.msgt_deallocate = FALSE;
	InP->memory_controlType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control /* memory_control */ = /* memory_control */ memory_control;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

#if	UseStaticMsgType
	InP->lengthType = lengthType;
#else	UseStaticMsgType
	InP->lengthType.msgt_name = 2;
	InP->lengthType.msgt_size = 32;
	InP->lengthType.msgt_number = 1;
	InP->lengthType.msgt_inline = TRUE;
	InP->lengthType.msgt_longform = FALSE;
	InP->lengthType.msgt_deallocate = FALSE;
	InP->lengthType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->length /* length */ = /* length */ length;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(memory_objectPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2206;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}
