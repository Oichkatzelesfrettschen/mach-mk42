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


/* Routine port_status */
mig_external kern_return_t port_status
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t port_name,
	mach_port_t *enabled,
	int *num_msgs,
	int *backlog,
	boolean_t *ownership,
	boolean_t *receive_rights
)
#else
	(task, port_name, enabled, num_msgs, backlog, ownership, receive_rights)
	ipc_space_t task;
	mach_port_t port_name;
	mach_port_t *enabled;
	int *num_msgs;
	int *backlog;
	boolean_t *ownership;
	boolean_t *receive_rights;
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
	static mach_msg_type_t enabledCheck = {
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
	static mach_msg_type_t num_msgsCheck = {
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

#if	UseStaticMsgType
	static mach_msg_type_t ownershipCheck = {
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
	static mach_msg_type_t receive_rightsCheck = {
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
	InP->port_nameType = port_nameType;
#else	UseStaticMsgType
	InP->port_nameType.msgt_name = 2;
	InP->port_nameType.msgt_size = 32;
	InP->port_nameType.msgt_number = 1;
	InP->port_nameType.msgt_inline = TRUE;
	InP->port_nameType.msgt_longform = FALSE;
	InP->port_nameType.msgt_deallocate = FALSE;
	InP->port_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->port_name /* port_name */ = /* port_name */ port_name;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2079;

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

	if (OutP->Head.msgh_id != 2179)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 72) || !msgh_simple) &&
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
	if (* (int *) &OutP->enabledType != * (int *) &enabledCheck)
#else	UseStaticMsgType
	if ((OutP->enabledType.msgt_inline != TRUE) ||
	    (OutP->enabledType.msgt_longform != FALSE) ||
	    (OutP->enabledType.msgt_name != 2) ||
	    (OutP->enabledType.msgt_number != 1) ||
	    (OutP->enabledType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*enabled /* enabled */ = /* *enabled */ OutP->enabled;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->num_msgsType != * (int *) &num_msgsCheck)
#else	UseStaticMsgType
	if ((OutP->num_msgsType.msgt_inline != TRUE) ||
	    (OutP->num_msgsType.msgt_longform != FALSE) ||
	    (OutP->num_msgsType.msgt_name != 2) ||
	    (OutP->num_msgsType.msgt_number != 1) ||
	    (OutP->num_msgsType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*num_msgs /* num_msgs */ = /* *num_msgs */ OutP->num_msgs;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->backlogType != * (int *) &backlogCheck)
#else	UseStaticMsgType
	if ((OutP->backlogType.msgt_inline != TRUE) ||
	    (OutP->backlogType.msgt_longform != FALSE) ||
	    (OutP->backlogType.msgt_name != 2) ||
	    (OutP->backlogType.msgt_number != 1) ||
	    (OutP->backlogType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*backlog /* backlog */ = /* *backlog */ OutP->backlog;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->ownershipType != * (int *) &ownershipCheck)
#else	UseStaticMsgType
	if ((OutP->ownershipType.msgt_inline != TRUE) ||
	    (OutP->ownershipType.msgt_longform != FALSE) ||
	    (OutP->ownershipType.msgt_name != 0) ||
	    (OutP->ownershipType.msgt_number != 1) ||
	    (OutP->ownershipType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*ownership /* ownership */ = /* *ownership */ OutP->ownership;

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &OutP->receive_rightsType != * (int *) &receive_rightsCheck)
#else	UseStaticMsgType
	if ((OutP->receive_rightsType.msgt_inline != TRUE) ||
	    (OutP->receive_rightsType.msgt_longform != FALSE) ||
	    (OutP->receive_rightsType.msgt_name != 0) ||
	    (OutP->receive_rightsType.msgt_number != 1) ||
	    (OutP->receive_rightsType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*receive_rights /* receive_rights */ = /* *receive_rights */ OutP->receive_rights;

	return OutP->RetCode;
}
