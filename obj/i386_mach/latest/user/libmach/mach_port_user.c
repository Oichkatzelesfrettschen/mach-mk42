#include "mach_port.h"
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

mig_external void init_mach_port
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

/* Routine mach_port_names */
mig_external kern_return_t mach_port_names
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_array_t *names,
	mach_msg_type_number_t *namesCnt,
	mach_port_type_array_t *types,
	mach_msg_type_number_t *typesCnt
)
#else
	(task, names, namesCnt, types, typesCnt)
	ipc_space_t task;
	mach_port_array_t *names;
	mach_msg_type_number_t *namesCnt;
	mach_port_type_array_t *types;
	mach_msg_type_number_t *typesCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t namesType;
		mach_port_array_t names;
		mach_msg_type_long_t typesType;
		mach_port_type_array_t types;
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
	unsigned int msgh_size = 24;

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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3200;

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

	if (OutP->Head.msgh_id != 3300)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 64) || msgh_simple) &&
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
	if ((OutP->namesType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->namesType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->namesType.msgtl_name != 15) ||
	    (OutP->namesType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*names /* names */ = /* *names */ OutP->names;

	*namesCnt /* namesType.msgtl_number */ = /* *namesCnt */ OutP->namesType.msgtl_number;

#if	TypeCheck
	if ((OutP->typesType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->typesType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->typesType.msgtl_name != 2) ||
	    (OutP->typesType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*types /* types */ = /* *types */ OutP->types;

	*typesCnt /* typesType.msgtl_number */ = /* *typesCnt */ OutP->typesType.msgtl_number;

	return OutP->RetCode;
}

/* Routine mach_port_type */
mig_external kern_return_t mach_port_type
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_type_t *ptype
)
#else
	(task, name, ptype)
	ipc_space_t task;
	mach_port_t name;
	mach_port_type_t *ptype;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t ptypeType;
		mach_port_type_t ptype;
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
	static mach_msg_type_t ptypeCheck = {
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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3201;

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

	if (OutP->Head.msgh_id != 3301)
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
	if (* (int *) &OutP->ptypeType != * (int *) &ptypeCheck)
#else	UseStaticMsgType
	if ((OutP->ptypeType.msgt_inline != TRUE) ||
	    (OutP->ptypeType.msgt_longform != FALSE) ||
	    (OutP->ptypeType.msgt_name != 2) ||
	    (OutP->ptypeType.msgt_number != 1) ||
	    (OutP->ptypeType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*ptype /* ptype */ = /* *ptype */ OutP->ptype;

	return OutP->RetCode;
}

/* Routine mach_port_rename */
mig_external kern_return_t mach_port_rename
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t old_name,
	mach_port_t new_name
)
#else
	(task, old_name, new_name)
	ipc_space_t task;
	mach_port_t old_name;
	mach_port_t new_name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t old_nameType;
		mach_port_t old_name;
		mach_msg_type_t new_nameType;
		mach_port_t new_name;
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
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t old_nameType = {
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
	static mach_msg_type_t new_nameType = {
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
	InP->old_nameType = old_nameType;
#else	UseStaticMsgType
	InP->old_nameType.msgt_name = 15;
	InP->old_nameType.msgt_size = 32;
	InP->old_nameType.msgt_number = 1;
	InP->old_nameType.msgt_inline = TRUE;
	InP->old_nameType.msgt_longform = FALSE;
	InP->old_nameType.msgt_deallocate = FALSE;
	InP->old_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->old_name /* old_name */ = /* old_name */ old_name;

#if	UseStaticMsgType
	InP->new_nameType = new_nameType;
#else	UseStaticMsgType
	InP->new_nameType.msgt_name = 15;
	InP->new_nameType.msgt_size = 32;
	InP->new_nameType.msgt_number = 1;
	InP->new_nameType.msgt_inline = TRUE;
	InP->new_nameType.msgt_longform = FALSE;
	InP->new_nameType.msgt_deallocate = FALSE;
	InP->new_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->new_name /* new_name */ = /* new_name */ new_name;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3202;

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

	if (OutP->Head.msgh_id != 3302)
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

/* Routine mig_mach_port_allocate_name */
mig_external kern_return_t mig_mach_port_allocate_name
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_right_t right,
	mach_port_t name
)
#else
	(task, right, name)
	ipc_space_t task;
	mach_port_right_t right;
	mach_port_t name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t rightType;
		mach_port_right_t right;
		mach_msg_type_t nameType;
		mach_port_t name;
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
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t rightType = {
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
	InP->rightType = rightType;
#else	UseStaticMsgType
	InP->rightType.msgt_name = 2;
	InP->rightType.msgt_size = 32;
	InP->rightType.msgt_number = 1;
	InP->rightType.msgt_inline = TRUE;
	InP->rightType.msgt_longform = FALSE;
	InP->rightType.msgt_deallocate = FALSE;
	InP->rightType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->right /* right */ = /* right */ right;

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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3203;

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

	if (OutP->Head.msgh_id != 3303)
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

/* Routine mig_mach_port_allocate */
mig_external kern_return_t mig_mach_port_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_right_t right,
	mach_port_t *name
)
#else
	(task, right, name)
	ipc_space_t task;
	mach_port_right_t right;
	mach_port_t *name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t rightType;
		mach_port_right_t right;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t nameType;
		mach_port_t name;
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
	static mach_msg_type_t rightType = {
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
	static mach_msg_type_t nameCheck = {
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
	InP->rightType = rightType;
#else	UseStaticMsgType
	InP->rightType.msgt_name = 2;
	InP->rightType.msgt_size = 32;
	InP->rightType.msgt_number = 1;
	InP->rightType.msgt_inline = TRUE;
	InP->rightType.msgt_longform = FALSE;
	InP->rightType.msgt_deallocate = FALSE;
	InP->rightType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->right /* right */ = /* right */ right;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3204;

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

	if (OutP->Head.msgh_id != 3304)
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
	if (* (int *) &OutP->nameType != * (int *) &nameCheck)
#else	UseStaticMsgType
	if ((OutP->nameType.msgt_inline != TRUE) ||
	    (OutP->nameType.msgt_longform != FALSE) ||
	    (OutP->nameType.msgt_name != 15) ||
	    (OutP->nameType.msgt_number != 1) ||
	    (OutP->nameType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*name /* name */ = /* *name */ OutP->name;

	return OutP->RetCode;
}

/* Routine mach_port_destroy */
mig_external kern_return_t mach_port_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name
)
#else
	(task, name)
	ipc_space_t task;
	mach_port_t name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
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
	unsigned int msgh_size = 32;

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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3205;

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

	if (OutP->Head.msgh_id != 3305)
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

/* Routine mig_mach_port_deallocate */
mig_external kern_return_t mig_mach_port_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name
)
#else
	(task, name)
	ipc_space_t task;
	mach_port_t name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
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
	unsigned int msgh_size = 32;

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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3206;

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

	if (OutP->Head.msgh_id != 3306)
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

/* Routine mach_port_get_refs */
mig_external kern_return_t mach_port_get_refs
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_right_t right,
	mach_port_urefs_t *refs
)
#else
	(task, name, right, refs)
	ipc_space_t task;
	mach_port_t name;
	mach_port_right_t right;
	mach_port_urefs_t *refs;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t rightType;
		mach_port_right_t right;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t refsType;
		mach_port_urefs_t refs;
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
	unsigned int msgh_size = 40;

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
	static mach_msg_type_t rightType = {
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
	static mach_msg_type_t refsCheck = {
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
	InP->rightType = rightType;
#else	UseStaticMsgType
	InP->rightType.msgt_name = 2;
	InP->rightType.msgt_size = 32;
	InP->rightType.msgt_number = 1;
	InP->rightType.msgt_inline = TRUE;
	InP->rightType.msgt_longform = FALSE;
	InP->rightType.msgt_deallocate = FALSE;
	InP->rightType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->right /* right */ = /* right */ right;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3207;

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

	if (OutP->Head.msgh_id != 3307)
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
	if (* (int *) &OutP->refsType != * (int *) &refsCheck)
#else	UseStaticMsgType
	if ((OutP->refsType.msgt_inline != TRUE) ||
	    (OutP->refsType.msgt_longform != FALSE) ||
	    (OutP->refsType.msgt_name != 2) ||
	    (OutP->refsType.msgt_number != 1) ||
	    (OutP->refsType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*refs /* refs */ = /* *refs */ OutP->refs;

	return OutP->RetCode;
}

/* Routine mach_port_mod_refs */
mig_external kern_return_t mach_port_mod_refs
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_right_t right,
	mach_port_delta_t delta
)
#else
	(task, name, right, delta)
	ipc_space_t task;
	mach_port_t name;
	mach_port_right_t right;
	mach_port_delta_t delta;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t rightType;
		mach_port_right_t right;
		mach_msg_type_t deltaType;
		mach_port_delta_t delta;
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
	static mach_msg_type_t rightType = {
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
	static mach_msg_type_t deltaType = {
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
	InP->rightType = rightType;
#else	UseStaticMsgType
	InP->rightType.msgt_name = 2;
	InP->rightType.msgt_size = 32;
	InP->rightType.msgt_number = 1;
	InP->rightType.msgt_inline = TRUE;
	InP->rightType.msgt_longform = FALSE;
	InP->rightType.msgt_deallocate = FALSE;
	InP->rightType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->right /* right */ = /* right */ right;

#if	UseStaticMsgType
	InP->deltaType = deltaType;
#else	UseStaticMsgType
	InP->deltaType.msgt_name = 2;
	InP->deltaType.msgt_size = 32;
	InP->deltaType.msgt_number = 1;
	InP->deltaType.msgt_inline = TRUE;
	InP->deltaType.msgt_longform = FALSE;
	InP->deltaType.msgt_deallocate = FALSE;
	InP->deltaType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->delta /* delta */ = /* delta */ delta;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3208;

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

	if (OutP->Head.msgh_id != 3308)
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

/* Routine mach_port_get_receive_status */
mig_external kern_return_t mach_port_get_receive_status
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_status_t *status
)
#else
	(task, name, status)
	ipc_space_t task;
	mach_port_t name;
	mach_port_status_t *status;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t statusType;
		mach_port_status_t status;
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
	static mach_msg_type_t statusCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3209;

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

	if (OutP->Head.msgh_id != 3309)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 68) || !msgh_simple) &&
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
	if (* (int *) &OutP->statusType != * (int *) &statusCheck)
#else	UseStaticMsgType
	if ((OutP->statusType.msgt_inline != TRUE) ||
	    (OutP->statusType.msgt_longform != FALSE) ||
	    (OutP->statusType.msgt_name != 2) ||
	    (OutP->statusType.msgt_number != 8) ||
	    (OutP->statusType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*status /* status */ = /* *status */ OutP->status;

	return OutP->RetCode;
}

/* Routine mach_port_set_qlimit */
mig_external kern_return_t mach_port_set_qlimit
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_msgcount_t qlimit
)
#else
	(task, name, qlimit)
	ipc_space_t task;
	mach_port_t name;
	mach_port_msgcount_t qlimit;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t qlimitType;
		mach_port_msgcount_t qlimit;
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
	unsigned int msgh_size = 40;

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
	static mach_msg_type_t qlimitType = {
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
	InP->qlimitType = qlimitType;
#else	UseStaticMsgType
	InP->qlimitType.msgt_name = 2;
	InP->qlimitType.msgt_size = 32;
	InP->qlimitType.msgt_number = 1;
	InP->qlimitType.msgt_inline = TRUE;
	InP->qlimitType.msgt_longform = FALSE;
	InP->qlimitType.msgt_deallocate = FALSE;
	InP->qlimitType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->qlimit /* qlimit */ = /* qlimit */ qlimit;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3210;

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

	if (OutP->Head.msgh_id != 3310)
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

/* Routine mach_port_set_mscount */
mig_external kern_return_t mach_port_set_mscount
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_mscount_t mscount
)
#else
	(task, name, mscount)
	ipc_space_t task;
	mach_port_t name;
	mach_port_mscount_t mscount;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t mscountType;
		mach_port_mscount_t mscount;
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
	unsigned int msgh_size = 40;

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
	static mach_msg_type_t mscountType = {
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
	InP->mscountType = mscountType;
#else	UseStaticMsgType
	InP->mscountType.msgt_name = 2;
	InP->mscountType.msgt_size = 32;
	InP->mscountType.msgt_number = 1;
	InP->mscountType.msgt_inline = TRUE;
	InP->mscountType.msgt_longform = FALSE;
	InP->mscountType.msgt_deallocate = FALSE;
	InP->mscountType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mscount /* mscount */ = /* mscount */ mscount;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3211;

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

	if (OutP->Head.msgh_id != 3311)
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

/* Routine mach_port_get_set_status */
mig_external kern_return_t mach_port_get_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_array_t *members,
	mach_msg_type_number_t *membersCnt
)
#else
	(task, name, members, membersCnt)
	ipc_space_t task;
	mach_port_t name;
	mach_port_array_t *members;
	mach_msg_type_number_t *membersCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t membersType;
		mach_port_array_t members;
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

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3212;

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

	if (OutP->Head.msgh_id != 3312)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 48) || msgh_simple) &&
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
	if ((OutP->membersType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->membersType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->membersType.msgtl_name != 15) ||
	    (OutP->membersType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*members /* members */ = /* *members */ OutP->members;

	*membersCnt /* membersType.msgtl_number */ = /* *membersCnt */ OutP->membersType.msgtl_number;

	return OutP->RetCode;
}

/* Routine mach_port_move_member */
mig_external kern_return_t mach_port_move_member
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t member,
	mach_port_t after
)
#else
	(task, member, after)
	ipc_space_t task;
	mach_port_t member;
	mach_port_t after;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memberType;
		mach_port_t member;
		mach_msg_type_t afterType;
		mach_port_t after;
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
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t memberType = {
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
	static mach_msg_type_t afterType = {
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
	InP->memberType = memberType;
#else	UseStaticMsgType
	InP->memberType.msgt_name = 15;
	InP->memberType.msgt_size = 32;
	InP->memberType.msgt_number = 1;
	InP->memberType.msgt_inline = TRUE;
	InP->memberType.msgt_longform = FALSE;
	InP->memberType.msgt_deallocate = FALSE;
	InP->memberType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->member /* member */ = /* member */ member;

#if	UseStaticMsgType
	InP->afterType = afterType;
#else	UseStaticMsgType
	InP->afterType.msgt_name = 15;
	InP->afterType.msgt_size = 32;
	InP->afterType.msgt_number = 1;
	InP->afterType.msgt_inline = TRUE;
	InP->afterType.msgt_longform = FALSE;
	InP->afterType.msgt_deallocate = FALSE;
	InP->afterType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->after /* after */ = /* after */ after;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3213;

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

	if (OutP->Head.msgh_id != 3313)
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

/* Routine mig_mach_port_insert_right */
mig_external kern_return_t mig_mach_port_insert_right
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_port_t poly,
	mach_msg_type_name_t polyPoly
)
#else
	(task, name, poly, polyPoly)
	ipc_space_t task;
	mach_port_t name;
	mach_port_t poly;
	mach_msg_type_name_t polyPoly;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t polyType;
		mach_port_t poly;
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
	boolean_t msgh_simple = TRUE;
	unsigned int msgh_size = 40;

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
	static mach_msg_type_t polyType = {
		/* msgt_name = */		((mach_msg_type_name_t) -1),
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
	InP->polyType = polyType;
#else	UseStaticMsgType
	InP->polyType.msgt_size = 32;
	InP->polyType.msgt_number = 1;
	InP->polyType.msgt_inline = TRUE;
	InP->polyType.msgt_longform = FALSE;
	InP->polyType.msgt_deallocate = FALSE;
	InP->polyType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->poly /* poly */ = /* poly */ poly;

	if (MACH_MSG_TYPE_PORT_ANY(polyPoly))
		msgh_simple = FALSE;

	InP->polyType.msgt_name /* polyPoly */ = /* polyType.msgt_name */ polyPoly;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3215;

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

	if (OutP->Head.msgh_id != 3315)
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

/* Routine mach_port_extract_right */
mig_external kern_return_t mach_port_extract_right
#if	(defined(__STDC__) || defined(c_plusplus))
(
	ipc_space_t task,
	mach_port_t name,
	mach_msg_type_name_t msgt_name,
	mach_port_t *poly,
	mach_msg_type_name_t *polyPoly
)
#else
	(task, name, msgt_name, poly, polyPoly)
	ipc_space_t task;
	mach_port_t name;
	mach_msg_type_name_t msgt_name;
	mach_port_t *poly;
	mach_msg_type_name_t *polyPoly;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t nameType;
		mach_port_t name;
		mach_msg_type_t msgt_nameType;
		mach_msg_type_name_t msgt_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t polyType;
		mach_port_t poly;
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
	unsigned int msgh_size = 40;

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
	static mach_msg_type_t msgt_nameType = {
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
	InP->msgt_nameType = msgt_nameType;
#else	UseStaticMsgType
	InP->msgt_nameType.msgt_name = 2;
	InP->msgt_nameType.msgt_size = 32;
	InP->msgt_nameType.msgt_number = 1;
	InP->msgt_nameType.msgt_inline = TRUE;
	InP->msgt_nameType.msgt_longform = FALSE;
	InP->msgt_nameType.msgt_deallocate = FALSE;
	InP->msgt_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->msgt_name /* msgt_name */ = /* msgt_name */ msgt_name;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = task;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 3216;

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

	if (OutP->Head.msgh_id != 3316)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size != 40)) &&
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
	if ((OutP->polyType.msgt_inline != TRUE) ||
	    (OutP->polyType.msgt_longform != FALSE) ||
	    (MACH_MSG_TYPE_PORT_ANY(OutP->polyType.msgt_name) && msgh_simple) ||
	    (OutP->polyType.msgt_number != 1) ||
	    (OutP->polyType.msgt_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*poly /* poly */ = /* *poly */ OutP->poly;

	*polyPoly /* polyType.msgt_name */ = /* *polyPoly */ OutP->polyType.msgt_name;

	return OutP->RetCode;
}
