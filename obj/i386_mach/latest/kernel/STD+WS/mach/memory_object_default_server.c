/* Module memory_object_default */

#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>

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

/* Due to pcc compiler bug, cannot use void */
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(hc)
#define novalue void
#else
#define novalue int
#endif

#define msgh_request_port	msgh_local_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_LOCAL(bits)
#define msgh_reply_port		msgh_remote_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_REMOTE(bits)

#include <mach/std_types.h>
#include <mach/mach_types.h>

/* SimpleRoutine memory_object_create */
mig_internal novalue _Xmemory_object_create
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_memory_objectType;
		memory_object_t new_memory_object;
		mach_msg_type_t new_object_sizeType;
		vm_size_t new_object_size;
		mach_msg_type_t new_control_portType;
		memory_object_control_t new_control_port;
		mach_msg_type_t new_nameType;
		memory_object_name_t new_name;
		mach_msg_type_t new_page_sizeType;
		vm_size_t new_page_size;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_create
#if	(defined(__STDC__) || defined(c_plusplus))
		(memory_object_t old_memory_object, memory_object_t new_memory_object, vm_size_t new_object_size, memory_object_control_t new_control_port, memory_object_name_t new_name, vm_size_t new_page_size);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t new_memory_objectCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_RECEIVE,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t new_object_sizeCheck = {
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
	static mach_msg_type_t new_control_portCheck = {
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
	static mach_msg_type_t new_nameCheck = {
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
	static mach_msg_type_t new_page_sizeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 64) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_memory_objectType != * (int *) &new_memory_objectCheck)
#else	UseStaticMsgType
	if ((In0P->new_memory_objectType.msgt_inline != TRUE) ||
	    (In0P->new_memory_objectType.msgt_longform != FALSE) ||
	    (In0P->new_memory_objectType.msgt_name != MACH_MSG_TYPE_PORT_RECEIVE) ||
	    (In0P->new_memory_objectType.msgt_number != 1) ||
	    (In0P->new_memory_objectType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_object_sizeType != * (int *) &new_object_sizeCheck)
#else	UseStaticMsgType
	if ((In0P->new_object_sizeType.msgt_inline != TRUE) ||
	    (In0P->new_object_sizeType.msgt_longform != FALSE) ||
	    (In0P->new_object_sizeType.msgt_name != 2) ||
	    (In0P->new_object_sizeType.msgt_number != 1) ||
	    (In0P->new_object_sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_control_portType != * (int *) &new_control_portCheck)
#else	UseStaticMsgType
	if ((In0P->new_control_portType.msgt_inline != TRUE) ||
	    (In0P->new_control_portType.msgt_longform != FALSE) ||
	    (In0P->new_control_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->new_control_portType.msgt_number != 1) ||
	    (In0P->new_control_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_nameType != * (int *) &new_nameCheck)
#else	UseStaticMsgType
	if ((In0P->new_nameType.msgt_inline != TRUE) ||
	    (In0P->new_nameType.msgt_longform != FALSE) ||
	    (In0P->new_nameType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->new_nameType.msgt_number != 1) ||
	    (In0P->new_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_page_sizeType != * (int *) &new_page_sizeCheck)
#else	UseStaticMsgType
	if ((In0P->new_page_sizeType.msgt_inline != TRUE) ||
	    (In0P->new_page_sizeType.msgt_longform != FALSE) ||
	    (In0P->new_page_sizeType.msgt_name != 2) ||
	    (In0P->new_page_sizeType.msgt_number != 1) ||
	    (In0P->new_page_sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_create(In0P->Head.msgh_request_port, In0P->new_memory_object, In0P->new_object_size, In0P->new_control_port, In0P->new_name, In0P->new_page_size);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine memory_object_data_initialize */
mig_internal novalue _Xmemory_object_data_initialize
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t memory_control_portType;
		memory_object_control_t memory_control_port;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_long_t dataType;
		pointer_t data;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t memory_object_data_initialize
#if	(defined(__STDC__) || defined(c_plusplus))
		(memory_object_t memory_object, memory_object_control_t memory_control_port, vm_offset_t offset, pointer_t data, mach_msg_type_number_t dataCnt);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t memory_control_portCheck = {
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

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->memory_control_portType != * (int *) &memory_control_portCheck)
#else	UseStaticMsgType
	if ((In0P->memory_control_portType.msgt_inline != TRUE) ||
	    (In0P->memory_control_portType.msgt_longform != FALSE) ||
	    (In0P->memory_control_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->memory_control_portType.msgt_number != 1) ||
	    (In0P->memory_control_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->offsetType != * (int *) &offsetCheck)
#else	UseStaticMsgType
	if ((In0P->offsetType.msgt_inline != TRUE) ||
	    (In0P->offsetType.msgt_longform != FALSE) ||
	    (In0P->offsetType.msgt_name != 2) ||
	    (In0P->offsetType.msgt_number != 1) ||
	    (In0P->offsetType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->dataType.msgtl_header.msgt_inline != FALSE) ||
	    (In0P->dataType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->dataType.msgtl_name != 9) ||
	    (In0P->dataType.msgtl_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = memory_object_data_initialize(In0P->Head.msgh_request_port, In0P->memory_control_port, In0P->offset, In0P->data, In0P->dataType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

mig_external boolean_t memory_object_default_server
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	register mach_msg_header_t *InP =  InHeadP;
	register mig_reply_header_t *OutP = (mig_reply_header_t *) OutHeadP;

#if	UseStaticMsgType
	static mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	OutP->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InP->msgh_bits), 0);
	OutP->Head.msgh_size = sizeof *OutP;
	OutP->Head.msgh_remote_port = InP->msgh_reply_port;
	OutP->Head.msgh_local_port = MACH_PORT_NULL;
	OutP->Head.msgh_kind = InP->msgh_kind;
	OutP->Head.msgh_id = InP->msgh_id + 100;

#if	UseStaticMsgType
	OutP->RetCodeType = RetCodeType;
#else	UseStaticMsgType
	OutP->RetCodeType.msgt_name = MACH_MSG_TYPE_INTEGER_32;
	OutP->RetCodeType.msgt_size = 32;
	OutP->RetCodeType.msgt_number = 1;
	OutP->RetCodeType.msgt_inline = TRUE;
	OutP->RetCodeType.msgt_longform = FALSE;
	OutP->RetCodeType.msgt_deallocate = FALSE;
	OutP->RetCodeType.msgt_unused = 0;
#endif	UseStaticMsgType
	OutP->RetCode = MIG_BAD_ID;

	if ((InP->msgh_id > 2252) || (InP->msgh_id < 2250))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xmemory_object_create,
			_Xmemory_object_data_initialize,
			0,
		};

		if (routines[InP->msgh_id - 2250])
			(routines[InP->msgh_id - 2250]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
