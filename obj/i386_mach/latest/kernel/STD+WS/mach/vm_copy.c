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


/* Routine vm_copy */
mig_external kern_return_t vm_copy
#if	(defined(__STDC__) || defined(c_plusplus))
(
	vm_task_t target_task,
	vm_address_t source_address,
	vm_size_t size,
	vm_address_t dest_address
)
#else
	(target_task, source_address, size, dest_address)
	vm_task_t target_task;
	vm_address_t source_address;
	vm_size_t size;
	vm_address_t dest_address;
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

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck
	unsigned int msgh_size = 48;

#if	UseStaticMsgType
	static mach_msg_type_t source_addressType = {
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
	static mach_msg_type_t dest_addressType = {
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
	static mach_msg_type_t RetCodeCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->source_addressType = source_addressType;
#else	UseStaticMsgType
	InP->source_addressType.msgt_name = 2;
	InP->source_addressType.msgt_size = 32;
	InP->source_addressType.msgt_number = 1;
	InP->source_addressType.msgt_inline = TRUE;
	InP->source_addressType.msgt_longform = FALSE;
	InP->source_addressType.msgt_deallocate = FALSE;
	InP->source_addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->source_address /* source_address */ = /* source_address */ source_address;

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
	InP->dest_addressType = dest_addressType;
#else	UseStaticMsgType
	InP->dest_addressType.msgt_name = 2;
	InP->dest_addressType.msgt_size = 32;
	InP->dest_addressType.msgt_number = 1;
	InP->dest_addressType.msgt_inline = TRUE;
	InP->dest_addressType.msgt_longform = FALSE;
	InP->dest_addressType.msgt_deallocate = FALSE;
	InP->dest_addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->dest_address /* dest_address */ = /* dest_address */ dest_address;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = target_task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2028;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, msgh_size, sizeof(Reply), InP->Head.msgh_reply_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (msg_result != MACH_MSG_SUCCESS) {
		if ((msg_result == MACH_SEND_INVALID_REPLY) ||
		    (msg_result == MACH_RCV_INVALID_NAME))
			mig_dealloc_reply_port();
		return msg_result;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
#endif	TypeCheck

	if (OutP->Head.msgh_id != 2128)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 32) || !msgh_simple) &&
	    ((msgh_size != sizeof(mig_reply_header_t)) ||
	     !msgh_simple ||
	     (OutP->RetCode == KERN_SUCCESS)))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->RetCodeType != * (int *) &RetCodeCheck)
#else	UseStaticMsgType
	if ((OutP->RetCodeType.msgt_inline != TRUE) ||
	    (OutP->RetCodeType.msgt_longform != FALSE) ||
	    (OutP->RetCodeType.msgt_name != MACH_MSG_TYPE_INTEGER_32) ||
	    (OutP->RetCodeType.msgt_number != 1) ||
	    (OutP->RetCodeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->RetCode != KERN_SUCCESS)
		return OutP->RetCode;

	return OutP->RetCode;
}
