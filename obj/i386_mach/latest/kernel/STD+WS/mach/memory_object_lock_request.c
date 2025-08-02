#undef	KERNEL
#include "mach_user_internal.h"
#include "mach_interface.h"
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mach_types.h>
#include <mach/mig_errors.h>
#include <mach/msg_type.h>
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


/* SimpleRoutine memory_object_lock_request */
mig_external kern_return_t memory_object_lock_request
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_control_t memory_control,
	vm_offset_t offset,
	vm_size_t size,
	boolean_t should_clean,
	boolean_t should_flush,
	vm_prot_t lock_value,
	mach_port_t reply_to
)
#else
	(memory_control, offset, size, should_clean, should_flush, lock_value, reply_to)
	memory_object_control_t memory_control;
	vm_offset_t offset;
	vm_size_t size;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	mach_port_t reply_to;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 72;

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
	static mach_msg_type_t should_cleanType = {
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
	static mach_msg_type_t should_flushType = {
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
	static mach_msg_type_t lock_valueType = {
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
	static mach_msg_type_t reply_toType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND_ONCE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

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
	InP->sizeType = sizeType;
#else	UseStaticMsgType
	InP->sizeType.msgt_name = 2;
	InP->sizeType.msgt_size = 32;
	InP->sizeType.msgt_number = 1;
	InP->sizeType.msgt_inline = TRUE;
	InP->sizeType.msgt_longform = FALSE;
	InP->sizeType.msgt_deallocate = FALSE;
	InP->sizeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->size /* size */ = /* size */ size;

#if	UseStaticMsgType
	InP->should_cleanType = should_cleanType;
#else	UseStaticMsgType
	InP->should_cleanType.msgt_name = 0;
	InP->should_cleanType.msgt_size = 32;
	InP->should_cleanType.msgt_number = 1;
	InP->should_cleanType.msgt_inline = TRUE;
	InP->should_cleanType.msgt_longform = FALSE;
	InP->should_cleanType.msgt_deallocate = FALSE;
	InP->should_cleanType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->should_clean /* should_clean */ = /* should_clean */ should_clean;

#if	UseStaticMsgType
	InP->should_flushType = should_flushType;
#else	UseStaticMsgType
	InP->should_flushType.msgt_name = 0;
	InP->should_flushType.msgt_size = 32;
	InP->should_flushType.msgt_number = 1;
	InP->should_flushType.msgt_inline = TRUE;
	InP->should_flushType.msgt_longform = FALSE;
	InP->should_flushType.msgt_deallocate = FALSE;
	InP->should_flushType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->should_flush /* should_flush */ = /* should_flush */ should_flush;

#if	UseStaticMsgType
	InP->lock_valueType = lock_valueType;
#else	UseStaticMsgType
	InP->lock_valueType.msgt_name = 2;
	InP->lock_valueType.msgt_size = 32;
	InP->lock_valueType.msgt_number = 1;
	InP->lock_valueType.msgt_inline = TRUE;
	InP->lock_valueType.msgt_longform = FALSE;
	InP->lock_valueType.msgt_deallocate = FALSE;
	InP->lock_valueType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->lock_value /* lock_value */ = /* lock_value */ lock_value;

#if	UseStaticMsgType
	InP->reply_toType = reply_toType;
#else	UseStaticMsgType
	InP->reply_toType.msgt_name = MACH_MSG_TYPE_MAKE_SEND_ONCE;
	InP->reply_toType.msgt_size = 32;
	InP->reply_toType.msgt_number = 1;
	InP->reply_toType.msgt_inline = TRUE;
	InP->reply_toType.msgt_longform = FALSE;
	InP->reply_toType.msgt_deallocate = FALSE;
	InP->reply_toType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->reply_to /* reply_to */ = /* reply_to */ reply_to;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_control;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2043;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}
