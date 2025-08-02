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


/* Routine task_info */
mig_external kern_return_t task_info
#if	(defined(__STDC__) || defined(c_plusplus))
(
	task_t target_task,
	int flavor,
	task_info_t task_info_out,
	mach_msg_type_number_t *task_info_outCnt
)
#else
	(target_task, flavor, task_info_out, task_info_outCnt)
	task_t target_task;
	int flavor;
	task_info_t task_info_out;
	mach_msg_type_number_t *task_info_outCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t task_info_outType;
		int task_info_out[1024];
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
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorType = {
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
	InP->flavorType = flavorType;
#else	UseStaticMsgType
	InP->flavorType.msgt_name = 2;
	InP->flavorType.msgt_size = 32;
	InP->flavorType.msgt_number = 1;
	InP->flavorType.msgt_inline = TRUE;
	InP->flavorType.msgt_longform = FALSE;
	InP->flavorType.msgt_deallocate = FALSE;
	InP->flavorType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->flavor /* flavor */ = /* flavor */ flavor;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = target_task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2060;

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

	if (OutP->Head.msgh_id != 2160)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size < 44) || !msgh_simple) &&
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
	if ((OutP->task_info_outType.msgtl_header.msgt_inline != TRUE) ||
	    (OutP->task_info_outType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->task_info_outType.msgtl_name != 2) ||
	    (OutP->task_info_outType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (4 * OutP->task_info_outType.msgtl_number);
	if (msgh_size != 44 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->task_info_outType.msgtl_number / 1 > *task_info_outCnt) {
		bcopy((char *) OutP->task_info_out, (char *) task_info_out, 4 * *task_info_outCnt);
		*task_info_outCnt /* task_info_outType.msgtl_number 1 */ = /* *task_info_outCnt */ OutP->task_info_outType.msgtl_number / 1;
		return MIG_ARRAY_TOO_LARGE;
	}
	bcopy((char *) OutP->task_info_out, (char *) task_info_out, 4 * OutP->task_info_outType.msgtl_number);

	*task_info_outCnt /* task_info_outType.msgtl_number */ = /* *task_info_outCnt */ OutP->task_info_outType.msgtl_number;

	OutP = &Mess.Out;
	return OutP->RetCode;
}
