/* Module device_reply */

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
#include <device/device_types.h>
#include <device/net_status.h>

/* SimpleRoutine device_open_reply */
mig_internal novalue _Xdevice_open_reply
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t device_portType;
		mach_port_t device_port;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t device_open_reply
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t reply_port, kern_return_t return_code, mach_port_t device_port);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeCheck = {
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
	static mach_msg_type_t device_portCheck = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
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
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->return_codeType != * (int *) &return_codeCheck)
#else	UseStaticMsgType
	if ((In0P->return_codeType.msgt_inline != TRUE) ||
	    (In0P->return_codeType.msgt_longform != FALSE) ||
	    (In0P->return_codeType.msgt_name != 2) ||
	    (In0P->return_codeType.msgt_number != 1) ||
	    (In0P->return_codeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->device_portType != * (int *) &device_portCheck)
#else	UseStaticMsgType
	if ((In0P->device_portType.msgt_inline != TRUE) ||
	    (In0P->device_portType.msgt_longform != FALSE) ||
	    (In0P->device_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->device_portType.msgt_number != 1) ||
	    (In0P->device_portType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = device_open_reply(In0P->Head.msgh_request_port, In0P->return_code, In0P->device_port);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine device_write_reply */
mig_internal novalue _Xdevice_write_reply
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t device_write_reply
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t reply_port, kern_return_t return_code, int bytes_written);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeCheck = {
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
	static mach_msg_type_t bytes_writtenCheck = {
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
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->return_codeType != * (int *) &return_codeCheck)
#else	UseStaticMsgType
	if ((In0P->return_codeType.msgt_inline != TRUE) ||
	    (In0P->return_codeType.msgt_longform != FALSE) ||
	    (In0P->return_codeType.msgt_name != 2) ||
	    (In0P->return_codeType.msgt_number != 1) ||
	    (In0P->return_codeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->bytes_writtenType != * (int *) &bytes_writtenCheck)
#else	UseStaticMsgType
	if ((In0P->bytes_writtenType.msgt_inline != TRUE) ||
	    (In0P->bytes_writtenType.msgt_longform != FALSE) ||
	    (In0P->bytes_writtenType.msgt_name != 2) ||
	    (In0P->bytes_writtenType.msgt_number != 1) ||
	    (In0P->bytes_writtenType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = device_write_reply(In0P->Head.msgh_request_port, In0P->return_code, In0P->bytes_written);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine device_write_reply_inband */
mig_internal novalue _Xdevice_write_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t device_write_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t reply_port, kern_return_t return_code, int bytes_written);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeCheck = {
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
	static mach_msg_type_t bytes_writtenCheck = {
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
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->return_codeType != * (int *) &return_codeCheck)
#else	UseStaticMsgType
	if ((In0P->return_codeType.msgt_inline != TRUE) ||
	    (In0P->return_codeType.msgt_longform != FALSE) ||
	    (In0P->return_codeType.msgt_name != 2) ||
	    (In0P->return_codeType.msgt_number != 1) ||
	    (In0P->return_codeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->bytes_writtenType != * (int *) &bytes_writtenCheck)
#else	UseStaticMsgType
	if ((In0P->bytes_writtenType.msgt_inline != TRUE) ||
	    (In0P->bytes_writtenType.msgt_longform != FALSE) ||
	    (In0P->bytes_writtenType.msgt_name != 2) ||
	    (In0P->bytes_writtenType.msgt_number != 1) ||
	    (In0P->bytes_writtenType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = device_write_reply_inband(In0P->Head.msgh_request_port, In0P->return_code, In0P->bytes_written);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine device_read_reply */
mig_internal novalue _Xdevice_read_reply
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_long_t dataType;
		io_buf_ptr_t data;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t device_read_reply
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t reply_port, kern_return_t return_code, io_buf_ptr_t data, mach_msg_type_number_t dataCnt);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeCheck = {
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
	if ((msgh_size != 48) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->return_codeType != * (int *) &return_codeCheck)
#else	UseStaticMsgType
	if ((In0P->return_codeType.msgt_inline != TRUE) ||
	    (In0P->return_codeType.msgt_longform != FALSE) ||
	    (In0P->return_codeType.msgt_name != 2) ||
	    (In0P->return_codeType.msgt_number != 1) ||
	    (In0P->return_codeType.msgt_size != 32))
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

	OutP->RetCode = device_read_reply(In0P->Head.msgh_request_port, In0P->return_code, In0P->data, In0P->dataType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

/* SimpleRoutine device_read_reply_inband */
mig_internal novalue _Xdevice_read_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t dataType;
		char data[128];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t device_read_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t reply_port, kern_return_t return_code, io_buf_ptr_inband_t data, mach_msg_type_number_t dataCnt);
#else
		();
#endif

#if	TypeCheck
	boolean_t msgh_simple;
#endif	TypeCheck

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeCheck = {
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
	if ((msgh_size < 36) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->return_codeType != * (int *) &return_codeCheck)
#else	UseStaticMsgType
	if ((In0P->return_codeType.msgt_inline != TRUE) ||
	    (In0P->return_codeType.msgt_longform != FALSE) ||
	    (In0P->return_codeType.msgt_name != 2) ||
	    (In0P->return_codeType.msgt_number != 1) ||
	    (In0P->return_codeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->dataType.msgt_inline != TRUE) ||
	    (In0P->dataType.msgt_longform != FALSE) ||
	    (In0P->dataType.msgt_name != 8) ||
	    (In0P->dataType.msgt_size != 8))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (1 * In0P->dataType.msgt_number)+3 &~ 3;
	if (msgh_size != 36 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = device_read_reply_inband(In0P->Head.msgh_request_port, In0P->return_code, In0P->data, In0P->dataType.msgt_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;
}

mig_external boolean_t device_reply_server
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

	if ((InP->msgh_id > 2905) || (InP->msgh_id < 2900))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xdevice_open_reply,
			0,
			_Xdevice_write_reply,
			_Xdevice_write_reply_inband,
			_Xdevice_read_reply,
			_Xdevice_read_reply_inband,
		};

		if (routines[InP->msgh_id - 2900])
			(routines[InP->msgh_id - 2900]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
