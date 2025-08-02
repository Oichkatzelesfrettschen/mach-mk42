#include "exc.h"
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

mig_external void init_exc
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

/* Routine exception_raise */
mig_external kern_return_t exception_raise
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t exception_port,
	mach_port_t thread,
	mach_port_t task,
	int exception,
	int code,
	int subcode
)
#else
	(exception_port, thread, task, exception, code, subcode)
	mach_port_t exception_port;
	mach_port_t thread;
	mach_port_t task;
	int exception;
	int code;
	int subcode;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t threadType;
		mach_port_t thread;
		mach_msg_type_t taskType;
		mach_port_t task;
		mach_msg_type_t exceptionType;
		int exception;
		mach_msg_type_t codeType;
		int code;
		mach_msg_type_t subcodeType;
		int subcode;
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
	unsigned int msgh_size = 64;

#if	UseStaticMsgType
	static mach_msg_type_t threadType = {
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
	static mach_msg_type_t taskType = {
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
	static mach_msg_type_t exceptionType = {
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
	static mach_msg_type_t codeType = {
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
	static mach_msg_type_t subcodeType = {
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
	InP->threadType = threadType;
#else	UseStaticMsgType
	InP->threadType.msgt_name = MACH_MSG_TYPE_COPY_SEND;
	InP->threadType.msgt_size = 32;
	InP->threadType.msgt_number = 1;
	InP->threadType.msgt_inline = TRUE;
	InP->threadType.msgt_longform = FALSE;
	InP->threadType.msgt_deallocate = FALSE;
	InP->threadType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->thread /* thread */ = /* thread */ thread;

#if	UseStaticMsgType
	InP->taskType = taskType;
#else	UseStaticMsgType
	InP->taskType.msgt_name = MACH_MSG_TYPE_COPY_SEND;
	InP->taskType.msgt_size = 32;
	InP->taskType.msgt_number = 1;
	InP->taskType.msgt_inline = TRUE;
	InP->taskType.msgt_longform = FALSE;
	InP->taskType.msgt_deallocate = FALSE;
	InP->taskType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->task /* task */ = /* task */ task;

#if	UseStaticMsgType
	InP->exceptionType = exceptionType;
#else	UseStaticMsgType
	InP->exceptionType.msgt_name = 2;
	InP->exceptionType.msgt_size = 32;
	InP->exceptionType.msgt_number = 1;
	InP->exceptionType.msgt_inline = TRUE;
	InP->exceptionType.msgt_longform = FALSE;
	InP->exceptionType.msgt_deallocate = FALSE;
	InP->exceptionType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->exception /* exception */ = /* exception */ exception;

#if	UseStaticMsgType
	InP->codeType = codeType;
#else	UseStaticMsgType
	InP->codeType.msgt_name = 2;
	InP->codeType.msgt_size = 32;
	InP->codeType.msgt_number = 1;
	InP->codeType.msgt_inline = TRUE;
	InP->codeType.msgt_longform = FALSE;
	InP->codeType.msgt_deallocate = FALSE;
	InP->codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->code /* code */ = /* code */ code;

#if	UseStaticMsgType
	InP->subcodeType = subcodeType;
#else	UseStaticMsgType
	InP->subcodeType.msgt_name = 2;
	InP->subcodeType.msgt_size = 32;
	InP->subcodeType.msgt_number = 1;
	InP->subcodeType.msgt_inline = TRUE;
	InP->subcodeType.msgt_longform = FALSE;
	InP->subcodeType.msgt_deallocate = FALSE;
	InP->subcodeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->subcode /* subcode */ = /* subcode */ subcode;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = exception_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2400;

	msg_result = mach_msg_rpc_from_kernel(&InP->Head, msgh_size, sizeof(Reply));
	if (msg_result != MACH_MSG_SUCCESS) {
		return msg_result;
	}

#if	TypeCheck
	msgh_size = OutP->Head.msgh_size;
	msgh_simple = !(OutP->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
#endif	TypeCheck

	if (OutP->Head.msgh_id != 2500)
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
