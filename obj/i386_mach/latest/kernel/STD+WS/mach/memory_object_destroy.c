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


/* SimpleRoutine memory_object_destroy */
mig_external kern_return_t memory_object_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_control_t memory_control,
	kern_return_t reason
)
#else
	(memory_control, reason)
	memory_object_control_t memory_control;
	kern_return_t reason;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t reasonType;
		kern_return_t reason;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 32;

#if	UseStaticMsgType
	static mach_msg_type_t reasonType = {
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
	InP->reasonType = reasonType;
#else	UseStaticMsgType
	InP->reasonType.msgt_name = 2;
	InP->reasonType.msgt_size = 32;
	InP->reasonType.msgt_number = 1;
	InP->reasonType.msgt_inline = TRUE;
	InP->reasonType.msgt_longform = FALSE;
	InP->reasonType.msgt_deallocate = FALSE;
	InP->reasonType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->reason /* reason */ = /* reason */ reason;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_control;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2092;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}
