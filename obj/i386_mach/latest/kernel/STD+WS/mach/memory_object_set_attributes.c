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


/* SimpleRoutine memory_object_set_attributes */
mig_external kern_return_t memory_object_set_attributes
#if	(defined(__STDC__) || defined(c_plusplus))
(
	memory_object_control_t memory_control,
	boolean_t object_ready,
	boolean_t may_cache,
	memory_object_copy_strategy_t copy_strategy
)
#else
	(memory_control, object_ready, may_cache, copy_strategy)
	memory_object_control_t memory_control;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t object_readyType;
		boolean_t object_ready;
		mach_msg_type_t may_cacheType;
		boolean_t may_cache;
		mach_msg_type_t copy_strategyType;
		memory_object_copy_strategy_t copy_strategy;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 48;

#if	UseStaticMsgType
	static mach_msg_type_t object_readyType = {
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
	static mach_msg_type_t may_cacheType = {
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
	static mach_msg_type_t copy_strategyType = {
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
	InP->object_readyType = object_readyType;
#else	UseStaticMsgType
	InP->object_readyType.msgt_name = 0;
	InP->object_readyType.msgt_size = 32;
	InP->object_readyType.msgt_number = 1;
	InP->object_readyType.msgt_inline = TRUE;
	InP->object_readyType.msgt_longform = FALSE;
	InP->object_readyType.msgt_deallocate = FALSE;
	InP->object_readyType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->object_ready /* object_ready */ = /* object_ready */ object_ready;

#if	UseStaticMsgType
	InP->may_cacheType = may_cacheType;
#else	UseStaticMsgType
	InP->may_cacheType.msgt_name = 0;
	InP->may_cacheType.msgt_size = 32;
	InP->may_cacheType.msgt_number = 1;
	InP->may_cacheType.msgt_inline = TRUE;
	InP->may_cacheType.msgt_longform = FALSE;
	InP->may_cacheType.msgt_deallocate = FALSE;
	InP->may_cacheType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->may_cache /* may_cache */ = /* may_cache */ may_cache;

#if	UseStaticMsgType
	InP->copy_strategyType = copy_strategyType;
#else	UseStaticMsgType
	InP->copy_strategyType.msgt_name = 2;
	InP->copy_strategyType.msgt_size = 32;
	InP->copy_strategyType.msgt_number = 1;
	InP->copy_strategyType.msgt_inline = TRUE;
	InP->copy_strategyType.msgt_longform = FALSE;
	InP->copy_strategyType.msgt_deallocate = FALSE;
	InP->copy_strategyType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->copy_strategy /* copy_strategy */ = /* copy_strategy */ copy_strategy;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = memory_control;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2091;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}
