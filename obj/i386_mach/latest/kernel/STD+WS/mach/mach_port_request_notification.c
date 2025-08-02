#undef	KERNEL
#include "mach_port_internal.h"
#include "mach_port_interface.h"
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


/* Routine mach_port_request_notification */
mig_external kern_return_t mach_port_request_notification
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_msg_id_t id,
	mach_port_mscount_t sync,
	mach_port_t notify,
	mach_msg_type_name_t notifyPoly,
	mach_port_t *previous
)
#else
	(task, name, id, sync, notify, notifyPoly, previous)
	ipc_space_t task;
	mach_port_t name;
	mach_msg_id_t id;
	mach_port_mscount_t sync;
	mach_port_t notify;
	mach_msg_type_name_t notifyPoly;
	mach_port_t *previous;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t idType;
		mach_msg_id_t id;
		mach_msg_type_t syncType;
		mach_port_mscount_t sync;
		mach_msg_type_t notifyType;
		mach_port_t notify;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t previousType;
		mach_port_t previous;
	} Reply;

	union {
		Request In;
		Reply Out;
	} Mess;

	register Request *InP = &Mess.In;
	register Reply *OutP = &Mess.Out;

	mach_msg_return_t msg_result;
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size = 56;

#if	UseStaticMsgType
	static mach_msg_type_t nameType = {
		/* msgt_name = */		15,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t idType = {
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
	static mach_msg_type_t syncType = {
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
	static mach_msg_type_t notifyType = {
		/* msgt_name = */		MACH_MSG_TYPE_POLYMORPHIC,
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
	static mach_msg_type_t previousCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND_ONCE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->nameType = nameType;
#else	UseStaticMsgType
	InP->nameType.msgt_name = 15;
	InP->nameType.msgt_size = 32;
	InP->nameType.msgt_number = 1;
	InP->nameType.msgt_inline = TRUE;
	InP->nameType.msgt_longform = FALSE;
	InP->nameType.msgt_deallocate = FALSE;
	InP->nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->name /* name */ = /* name */ name;

#if	UseStaticMsgType
	InP->idType = idType;
#else	UseStaticMsgType
	InP->idType.msgt_name = 2;
	InP->idType.msgt_size = 32;
	InP->idType.msgt_number = 1;
	InP->idType.msgt_inline = TRUE;
	InP->idType.msgt_longform = FALSE;
	InP->idType.msgt_deallocate = FALSE;
	InP->idType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->id /* id */ = /* id */ id;

#if	UseStaticMsgType
	InP->syncType = syncType;
#else	UseStaticMsgType
	InP->syncType.msgt_name = 2;
	InP->syncType.msgt_size = 32;
	InP->syncType.msgt_number = 1;
	InP->syncType.msgt_inline = TRUE;
	InP->syncType.msgt_longform = FALSE;
	InP->syncType.msgt_deallocate = FALSE;
	InP->syncType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->sync /* sync */ = /* sync */ sync;

#if	UseStaticMsgType
	InP->notifyType = notifyType;
#else	UseStaticMsgType
	InP->notifyType.msgt_size = 32;
	InP->notifyType.msgt_number = 1;
	InP->notifyType.msgt_inline = TRUE;
	InP->notifyType.msgt_longform = FALSE;
	InP->notifyType.msgt_deallocate = FALSE;
	InP->notifyType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->notify /* notify */ = /* notify */ notify;

	if (MACH_MSG_TYPE_PORT_ANY(notifyPoly))
		msgh_simple = FALSE;

	InP->notifyType.msgt_name /* notifyPoly */ = /* notifyType.msgt_name */ notifyPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3214;

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

	if (OutP->Head.msgh_id != 3314)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 40) || msgh_simple) &&
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
	if (* (int *) &OutP->previousType != * (int *) &previousCheck)
#else	UseStaticMsgType
	if ((OutP->previousType.msgt_inline != TRUE) ||
	    (OutP->previousType.msgt_longform != FALSE) ||
	    (OutP->previousType.msgt_name != MACH_MSG_TYPE_PORT_SEND_ONCE) ||
	    (OutP->previousType.msgt_number != 1) ||
	    (OutP->previousType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*previous /* previous */ = /* *previous */ OutP->previous;

	return OutP->RetCode;
}
