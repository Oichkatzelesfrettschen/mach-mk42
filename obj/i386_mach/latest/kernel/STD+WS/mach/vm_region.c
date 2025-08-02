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


/* Routine vm_region */
mig_external kern_return_t vm_region
#if	(defined(__STDC__) || defined(c_plusplus))
(
	vm_task_t target_task,
	vm_address_t *address,
	vm_size_t *size,
	vm_prot_t *protection,
	vm_prot_t *max_protection,
	vm_inherit_t *inheritance,
	boolean_t *is_shared,
	memory_object_name_t *object_name,
	vm_offset_t *offset
)
#else
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
	unsigned int msgh_size = 32;

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
	static mach_msg_type_t protectionCheck = {
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
	static mach_msg_type_t is_sharedCheck = {
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
	static mach_msg_type_t object_nameCheck = {
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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = target_task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2029;

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

	if (OutP->Head.msgh_id != 2129)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 96) || msgh_simple) &&
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((OutP->sizeType.msgt_inline != TRUE) ||
	    (OutP->sizeType.msgt_longform != FALSE) ||
	    (OutP->sizeType.msgt_name != 2) ||
	    (OutP->sizeType.msgt_number != 1) ||
	    (OutP->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*size /* size */ = /* *size */ OutP->size;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->protectionType != * (int *) &protectionCheck)
#else	UseStaticMsgType
	if ((OutP->protectionType.msgt_inline != TRUE) ||
	    (OutP->protectionType.msgt_longform != FALSE) ||
	    (OutP->protectionType.msgt_name != 2) ||
	    (OutP->protectionType.msgt_number != 1) ||
	    (OutP->protectionType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*protection /* protection */ = /* *protection */ OutP->protection;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->max_protectionType != * (int *) &max_protectionCheck)
#else	UseStaticMsgType
	if ((OutP->max_protectionType.msgt_inline != TRUE) ||
	    (OutP->max_protectionType.msgt_longform != FALSE) ||
	    (OutP->max_protectionType.msgt_name != 2) ||
	    (OutP->max_protectionType.msgt_number != 1) ||
	    (OutP->max_protectionType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*max_protection /* max_protection */ = /* *max_protection */ OutP->max_protection;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->inheritanceType != * (int *) &inheritanceCheck)
#else	UseStaticMsgType
	if ((OutP->inheritanceType.msgt_inline != TRUE) ||
	    (OutP->inheritanceType.msgt_longform != FALSE) ||
	    (OutP->inheritanceType.msgt_name != 2) ||
	    (OutP->inheritanceType.msgt_number != 1) ||
	    (OutP->inheritanceType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*inheritance /* inheritance */ = /* *inheritance */ OutP->inheritance;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->is_sharedType != * (int *) &is_sharedCheck)
#else	UseStaticMsgType
	if ((OutP->is_sharedType.msgt_inline != TRUE) ||
	    (OutP->is_sharedType.msgt_longform != FALSE) ||
	    (OutP->is_sharedType.msgt_name != 0) ||
	    (OutP->is_sharedType.msgt_number != 1) ||
	    (OutP->is_sharedType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*is_shared /* is_shared */ = /* *is_shared */ OutP->is_shared;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->object_nameType != * (int *) &object_nameCheck)
#else	UseStaticMsgType
	if ((OutP->object_nameType.msgt_inline != TRUE) ||
	    (OutP->object_nameType.msgt_longform != FALSE) ||
	    (OutP->object_nameType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (OutP->object_nameType.msgt_number != 1) ||
	    (OutP->object_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*object_name /* object_name */ = /* *object_name */ OutP->object_name;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((OutP->offsetType.msgt_inline != TRUE) ||
	    (OutP->offsetType.msgt_longform != FALSE) ||
	    (OutP->offsetType.msgt_name != 2) ||
	    (OutP->offsetType.msgt_number != 1) ||
	    (OutP->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*offset /* offset */ = /* *offset */ OutP->offset;

	return OutP->RetCode;
}
