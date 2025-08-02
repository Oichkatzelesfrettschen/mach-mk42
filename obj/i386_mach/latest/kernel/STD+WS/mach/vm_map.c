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


/* Routine vm_map */
mig_external kern_return_t vm_map
#if	(defined(__STDC__) || defined(c_plusplus))
(
	vm_task_t target_task,
	vm_address_t *address,
	vm_size_t size,
	vm_address_t mask,
	boolean_t anywhere,
	memory_object_t memory_object,
	vm_offset_t offset,
	boolean_t copy,
	vm_prot_t cur_protection,
	vm_prot_t max_protection,
	vm_inherit_t inheritance
)
#else
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
		memory_object_t memory_object;
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
	unsigned int msgh_size = 104;

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
	static mach_msg_type_t maskType = {
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
	static mach_msg_type_t anywhereType = {
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
	static mach_msg_type_t memory_objectType = {
		/* msgt_name = */		MACH_MSG_TYPE_COPY_SEND,
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
	static mach_msg_type_t copyType = {
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
	static mach_msg_type_t cur_protectionType = {
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
	InP->addressType = addressType;
#else	UseStaticMsgType
	InP->addressType.msgt_name = 2;
	InP->addressType.msgt_size = 32;
	InP->addressType.msgt_number = 1;
	InP->addressType.msgt_inline = TRUE;
	InP->addressType.msgt_longform = FALSE;
	InP->addressType.msgt_deallocate = FALSE;
	InP->addressType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->address /* *address */ = /* address */ *address;

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
	InP->maskType = maskType;
#else	UseStaticMsgType
	InP->maskType.msgt_name = 2;
	InP->maskType.msgt_size = 32;
	InP->maskType.msgt_number = 1;
	InP->maskType.msgt_inline = TRUE;
	InP->maskType.msgt_longform = FALSE;
	InP->maskType.msgt_deallocate = FALSE;
	InP->maskType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mask /* mask */ = /* mask */ mask;

#if	UseStaticMsgType
	InP->anywhereType = anywhereType;
#else	UseStaticMsgType
	InP->anywhereType.msgt_name = 0;
	InP->anywhereType.msgt_size = 32;
	InP->anywhereType.msgt_number = 1;
	InP->anywhereType.msgt_inline = TRUE;
	InP->anywhereType.msgt_longform = FALSE;
	InP->anywhereType.msgt_deallocate = FALSE;
	InP->anywhereType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->anywhere /* anywhere */ = /* anywhere */ anywhere;

#if	UseStaticMsgType
	InP->memory_objectType = memory_objectType;
#else	UseStaticMsgType
	InP->memory_objectType.msgt_name = MACH_MSG_TYPE_COPY_SEND;
	InP->memory_objectType.msgt_size = 32;
	InP->memory_objectType.msgt_number = 1;
	InP->memory_objectType.msgt_inline = TRUE;
	InP->memory_objectType.msgt_longform = FALSE;
	InP->memory_objectType.msgt_deallocate = FALSE;
	InP->memory_objectType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->memory_object /* memory_object */ = /* memory_object */ memory_object;

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
	InP->copyType = copyType;
#else	UseStaticMsgType
	InP->copyType.msgt_name = 0;
	InP->copyType.msgt_size = 32;
	InP->copyType.msgt_number = 1;
	InP->copyType.msgt_inline = TRUE;
	InP->copyType.msgt_longform = FALSE;
	InP->copyType.msgt_deallocate = FALSE;
	InP->copyType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->copy /* copy */ = /* copy */ copy;

#if	UseStaticMsgType
	InP->cur_protectionType = cur_protectionType;
#else	UseStaticMsgType
	InP->cur_protectionType.msgt_name = 2;
	InP->cur_protectionType.msgt_size = 32;
	InP->cur_protectionType.msgt_number = 1;
	InP->cur_protectionType.msgt_inline = TRUE;
	InP->cur_protectionType.msgt_longform = FALSE;
	InP->cur_protectionType.msgt_deallocate = FALSE;
	InP->cur_protectionType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->cur_protection /* cur_protection */ = /* cur_protection */ cur_protection;

#if	UseStaticMsgType
	InP->max_protectionType = max_protectionType;
#else	UseStaticMsgType
	InP->max_protectionType.msgt_name = 2;
	InP->max_protectionType.msgt_size = 32;
	InP->max_protectionType.msgt_number = 1;
	InP->max_protectionType.msgt_inline = TRUE;
	InP->max_protectionType.msgt_longform = FALSE;
	InP->max_protectionType.msgt_deallocate = FALSE;
	InP->max_protectionType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->max_protection /* max_protection */ = /* max_protection */ max_protection;

#if	UseStaticMsgType
	InP->inheritanceType = inheritanceType;
#else	UseStaticMsgType
	InP->inheritanceType.msgt_name = 2;
	InP->inheritanceType.msgt_size = 32;
	InP->inheritanceType.msgt_number = 1;
	InP->inheritanceType.msgt_inline = TRUE;
	InP->inheritanceType.msgt_longform = FALSE;
	InP->inheritanceType.msgt_deallocate = FALSE;
	InP->inheritanceType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->inheritance /* inheritance */ = /* inheritance */ inheritance;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = target_task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2089;

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

	if (OutP->Head.msgh_id != 2189)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 40) || !msgh_simple) &&
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((OutP->addressType.msgt_inline != TRUE) ||
	    (OutP->addressType.msgt_longform != FALSE) ||
	    (OutP->addressType.msgt_name != 2) ||
	    (OutP->addressType.msgt_number != 1) ||
	    (OutP->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*address /* address */ = /* *address */ OutP->address;

	return OutP->RetCode;
}
