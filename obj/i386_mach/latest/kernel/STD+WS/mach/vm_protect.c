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


/* Routine vm_protect */
mig_external kern_return_t vm_protect
#if	(defined(__STDC__) || defined(c_plusplus))
(
	vm_task_t target_task,
	vm_address_t address,
	vm_size_t size,
	boolean_t set_maximum,
	vm_prot_t new_protection
)
#else
	(target_task, address, size, set_maximum, new_protection)
	vm_task_t target_task;
	vm_address_t address;
	vm_size_t size;
	boolean_t set_maximum;
	vm_prot_t new_protection;
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
	unsigned int msgh_size = 56;

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
	static mach_msg_type_t set_maximumType = {
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
	static mach_msg_type_t new_protectionType = {
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

	InP->address /* address */ = /* address */ address;

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
	InP->set_maximumType = set_maximumType;
#else	UseStaticMsgType
	InP->set_maximumType.msgt_name = 0;
	InP->set_maximumType.msgt_size = 32;
	InP->set_maximumType.msgt_number = 1;
	InP->set_maximumType.msgt_inline = TRUE;
	InP->set_maximumType.msgt_longform = FALSE;
	InP->set_maximumType.msgt_deallocate = FALSE;
	InP->set_maximumType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->set_maximum /* set_maximum */ = /* set_maximum */ set_maximum;

#if	UseStaticMsgType
	InP->new_protectionType = new_protectionType;
#else	UseStaticMsgType
	InP->new_protectionType.msgt_name = 2;
	InP->new_protectionType.msgt_size = 32;
	InP->new_protectionType.msgt_number = 1;
	InP->new_protectionType.msgt_inline = TRUE;
	InP->new_protectionType.msgt_longform = FALSE;
	InP->new_protectionType.msgt_deallocate = FALSE;
	InP->new_protectionType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_protection /* new_protection */ = /* new_protection */ new_protection;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = target_task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2024;

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

	if (OutP->Head.msgh_id != 2124)
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
