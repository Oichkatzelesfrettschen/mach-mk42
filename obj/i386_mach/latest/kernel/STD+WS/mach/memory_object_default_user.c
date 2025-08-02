#include "memory_object_default.h"
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

mig_external void init_memory_object_default
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

/* SimpleRoutine memory_object_create */
mig_external kern_return_t memory_object_create
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t old_memory_object,
	memory_object_t new_memory_object,
	vm_size_t new_object_size,
	memory_object_control_t new_control_port,
	memory_object_name_t new_name,
	vm_size_t new_page_size
)
#else
	(old_memory_object, new_memory_object, new_object_size, new_control_port, new_name, new_page_size)
	memory_object_t old_memory_object;
	memory_object_t new_memory_object;
	vm_size_t new_object_size;
	memory_object_control_t new_control_port;
	memory_object_name_t new_name;
	vm_size_t new_page_size;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_memory_objectType;
		memory_object_t new_memory_object;
		mach_msg_type_t new_object_sizeType;
		vm_size_t new_object_size;
		mach_msg_type_t new_control_portType;
		memory_object_control_t new_control_port;
		mach_msg_type_t new_nameType;
		memory_object_name_t new_name;
		mach_msg_type_t new_page_sizeType;
		vm_size_t new_page_size;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 64;

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
	static mach_msg_type_t new_object_sizeType = {
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
	static mach_msg_type_t new_control_portType = {
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
	static mach_msg_type_t new_nameType = {
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
	static mach_msg_type_t new_page_sizeType = {
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

#if	UseStaticMsgType
	InP->new_object_sizeType = new_object_sizeType;
#else	UseStaticMsgType
	InP->new_object_sizeType.msgt_name = 2;
	InP->new_object_sizeType.msgt_size = 32;
	InP->new_object_sizeType.msgt_number = 1;
	InP->new_object_sizeType.msgt_inline = TRUE;
	InP->new_object_sizeType.msgt_longform = FALSE;
	InP->new_object_sizeType.msgt_deallocate = FALSE;
	InP->new_object_sizeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_object_size /* new_object_size */ = /* new_object_size */ new_object_size;

#if	UseStaticMsgType
	InP->new_control_portType = new_control_portType;
#else	UseStaticMsgType
	InP->new_control_portType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->new_control_portType.msgt_size = 32;
	InP->new_control_portType.msgt_number = 1;
	InP->new_control_portType.msgt_inline = TRUE;
	InP->new_control_portType.msgt_longform = FALSE;
	InP->new_control_portType.msgt_deallocate = FALSE;
	InP->new_control_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_control_port /* new_control_port */ = /* new_control_port */ new_control_port;

#if	UseStaticMsgType
	InP->new_nameType = new_nameType;
#else	UseStaticMsgType
	InP->new_nameType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->new_nameType.msgt_size = 32;
	InP->new_nameType.msgt_number = 1;
	InP->new_nameType.msgt_inline = TRUE;
	InP->new_nameType.msgt_longform = FALSE;
	InP->new_nameType.msgt_deallocate = FALSE;
	InP->new_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_name /* new_name */ = /* new_name */ new_name;

#if	UseStaticMsgType
	InP->new_page_sizeType = new_page_sizeType;
#else	UseStaticMsgType
	InP->new_page_sizeType.msgt_name = 2;
	InP->new_page_sizeType.msgt_size = 32;
	InP->new_page_sizeType.msgt_number = 1;
	InP->new_page_sizeType.msgt_inline = TRUE;
	InP->new_page_sizeType.msgt_longform = FALSE;
	InP->new_page_sizeType.msgt_deallocate = FALSE;
	InP->new_page_sizeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_page_size /* new_page_size */ = /* new_page_size */ new_page_size;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_MOVE_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = old_memory_object;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2250;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine memory_object_data_initialize */
mig_external kern_return_t memory_object_data_initialize
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_t memory_object,
	memory_object_control_t memory_control_port,
	vm_offset_t offset,
	pointer_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(memory_object, memory_control_port, offset, data, dataCnt)
	memory_object_t memory_object;
	memory_object_control_t memory_control_port;
	vm_offset_t offset;
	pointer_t data;
	mach_msg_type_number_t dataCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_control_portType;
		memory_object_control_t memory_control_port;
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
	static mach_msg_type_t memory_control_portType = {
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
	InP->memory_control_portType = memory_control_portType;
#else	UseStaticMsgType
	InP->memory_control_portType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->memory_control_portType.msgt_size = 32;
	InP->memory_control_portType.msgt_number = 1;
	InP->memory_control_portType.msgt_inline = TRUE;
	InP->memory_control_portType.msgt_longform = FALSE;
	InP->memory_control_portType.msgt_deallocate = FALSE;
	InP->memory_control_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_control_port /* memory_control_port */ = /* memory_control_port */ memory_control_port;

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
	InP->Head.msgh_id = 2251;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}
