#undef	KERNEL
#include "device.h"
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

mig_external void init_device
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

/* Routine device_open */
mig_external kern_return_t device_open
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t master_port,
	dev_mode_t mode,
	dev_name_t name,
	device_t *device
)
#else
	(master_port, mode, name, device)
	mach_port_t master_port;
	dev_mode_t mode;
	dev_name_t name;
	device_t *device;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_long_t nameType;
		dev_name_t name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t deviceType;
		device_t device;
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
	unsigned int msgh_size = 172;

#if	UseStaticMsgType
	static mach_msg_type_t modeType = {
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
	static mach_msg_type_long_t nameType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	12,
		/* msgtl_size = */	1024,
		/* msgtl_number = */	1,
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
	static mach_msg_type_t deviceCheck = {
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
	InP->modeType = modeType;
#else	UseStaticMsgType
	InP->modeType.msgt_name = 2;
	InP->modeType.msgt_size = 32;
	InP->modeType.msgt_number = 1;
	InP->modeType.msgt_inline = TRUE;
	InP->modeType.msgt_longform = FALSE;
	InP->modeType.msgt_deallocate = FALSE;
	InP->modeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mode /* mode */ = /* mode */ mode;

#if	UseStaticMsgType
	InP->nameType = nameType;
#else	UseStaticMsgType
	InP->nameType.msgtl_name = 12;
	InP->nameType.msgtl_size = 1024;
	InP->nameType.msgtl_number = 1;
	InP->nameType.msgtl_header.msgt_name = 0;
	InP->nameType.msgtl_header.msgt_size = 0;
	InP->nameType.msgtl_header.msgt_number = 0;
	InP->nameType.msgtl_header.msgt_inline = TRUE;
	InP->nameType.msgtl_header.msgt_longform = TRUE;
	InP->nameType.msgtl_header.msgt_deallocate = FALSE;
	InP->nameType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	(void) mig_strncpy(InP->name /* name */, /* name */ name, 128);
	InP->name /* name */[127] = '\0';

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = master_port;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2800;

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

	if (OutP->Head.msgh_id != 2900)
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
	if (* (int *) &OutP->deviceType != * (int *) &deviceCheck)
#else	UseStaticMsgType
	if ((OutP->deviceType.msgt_inline != TRUE) ||
	    (OutP->deviceType.msgt_longform != FALSE) ||
	    (OutP->deviceType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (OutP->deviceType.msgt_number != 1) ||
	    (OutP->deviceType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*device /* device */ = /* *device */ OutP->device;

	return OutP->RetCode;
}

/* Routine device_close */
mig_external kern_return_t device_close
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device
)
#else
	(device)
	device_t device;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
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
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2801;

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

	if (OutP->Head.msgh_id != 2901)
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

/* Routine device_write */
mig_external kern_return_t device_write
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	dev_mode_t mode,
	recnum_t recnum,
	io_buf_ptr_t data,
	mach_msg_type_number_t dataCnt,
	int *bytes_written
)
#else
	(device, mode, recnum, data, dataCnt, bytes_written)
	device_t device;
	dev_mode_t mode;
	recnum_t recnum;
	io_buf_ptr_t data;
	mach_msg_type_number_t dataCnt;
	int *bytes_written;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_t recnumType;
		recnum_t recnum;
		mach_msg_type_long_t dataType;
		io_buf_ptr_t data;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
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
	static mach_msg_type_t modeType = {
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
	static mach_msg_type_t recnumType = {
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
	static mach_msg_type_long_t dataType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	9,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
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

#if	UseStaticMsgType
	InP->modeType = modeType;
#else	UseStaticMsgType
	InP->modeType.msgt_name = 2;
	InP->modeType.msgt_size = 32;
	InP->modeType.msgt_number = 1;
	InP->modeType.msgt_inline = TRUE;
	InP->modeType.msgt_longform = FALSE;
	InP->modeType.msgt_deallocate = FALSE;
	InP->modeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mode /* mode */ = /* mode */ mode;

#if	UseStaticMsgType
	InP->recnumType = recnumType;
#else	UseStaticMsgType
	InP->recnumType.msgt_name = 2;
	InP->recnumType.msgt_size = 32;
	InP->recnumType.msgt_number = 1;
	InP->recnumType.msgt_inline = TRUE;
	InP->recnumType.msgt_longform = FALSE;
	InP->recnumType.msgt_deallocate = FALSE;
	InP->recnumType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->recnum /* recnum */ = /* recnum */ recnum;

#if	UseStaticMsgType
	InP->dataType = dataType;
#else	UseStaticMsgType
	InP->dataType.msgtl_name = 9;
	InP->dataType.msgtl_size = 8;
	InP->dataType.msgtl_header.msgt_name = 0;
	InP->dataType.msgtl_header.msgt_size = 0;
	InP->dataType.msgtl_header.msgt_number = 0;
	InP->dataType.msgtl_header.msgt_inline = FALSE;
	InP->dataType.msgtl_header.msgt_longform = TRUE;
	InP->dataType.msgtl_header.msgt_deallocate = FALSE;
	InP->dataType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->data /* data */ = /* data */ data;

	InP->dataType.msgtl_number /* dataCnt */ = /* dataType.msgtl_number */ dataCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2802;

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

	if (OutP->Head.msgh_id != 2902)
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
	if (* (int *) &OutP->bytes_writtenType != * (int *) &bytes_writtenCheck)
#else	UseStaticMsgType
	if ((OutP->bytes_writtenType.msgt_inline != TRUE) ||
	    (OutP->bytes_writtenType.msgt_longform != FALSE) ||
	    (OutP->bytes_writtenType.msgt_name != 2) ||
	    (OutP->bytes_writtenType.msgt_number != 1) ||
	    (OutP->bytes_writtenType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*bytes_written /* bytes_written */ = /* *bytes_written */ OutP->bytes_written;

	return OutP->RetCode;
}

/* Routine device_write_inband */
mig_external kern_return_t device_write_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	dev_mode_t mode,
	recnum_t recnum,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t dataCnt,
	int *bytes_written
)
#else
	(device, mode, recnum, data, dataCnt, bytes_written)
	device_t device;
	dev_mode_t mode;
	recnum_t recnum;
	io_buf_ptr_inband_t data;
	mach_msg_type_number_t dataCnt;
	int *bytes_written;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_t recnumType;
		recnum_t recnum;
		mach_msg_type_t dataType;
		char data[128];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
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
	unsigned int msgh_size = 44;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t modeType = {
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
	static mach_msg_type_t recnumType = {
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
	static mach_msg_type_t dataType = {
		/* msgt_name = */		8,
		/* msgt_size = */		8,
		/* msgt_number = */		128,
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

#if	UseStaticMsgType
	InP->modeType = modeType;
#else	UseStaticMsgType
	InP->modeType.msgt_name = 2;
	InP->modeType.msgt_size = 32;
	InP->modeType.msgt_number = 1;
	InP->modeType.msgt_inline = TRUE;
	InP->modeType.msgt_longform = FALSE;
	InP->modeType.msgt_deallocate = FALSE;
	InP->modeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mode /* mode */ = /* mode */ mode;

#if	UseStaticMsgType
	InP->recnumType = recnumType;
#else	UseStaticMsgType
	InP->recnumType.msgt_name = 2;
	InP->recnumType.msgt_size = 32;
	InP->recnumType.msgt_number = 1;
	InP->recnumType.msgt_inline = TRUE;
	InP->recnumType.msgt_longform = FALSE;
	InP->recnumType.msgt_deallocate = FALSE;
	InP->recnumType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->recnum /* recnum */ = /* recnum */ recnum;

#if	UseStaticMsgType
	InP->dataType = dataType;
#else	UseStaticMsgType
	InP->dataType.msgt_name = 8;
	InP->dataType.msgt_size = 8;
	InP->dataType.msgt_inline = TRUE;
	InP->dataType.msgt_longform = FALSE;
	InP->dataType.msgt_deallocate = FALSE;
	InP->dataType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (dataCnt > 128)
		return MIG_ARRAY_TOO_LARGE;
	bcopy((char *) data, (char *) InP->data, 1 * dataCnt);

	InP->dataType.msgt_number /* dataCnt */ = /* dataType.msgt_number */ dataCnt;

	msgh_size_delta = (1 * dataCnt)+3 &~ 3;
	msgh_size += msgh_size_delta;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2803;

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

	if (OutP->Head.msgh_id != 2903)
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
	if (* (int *) &OutP->bytes_writtenType != * (int *) &bytes_writtenCheck)
#else	UseStaticMsgType
	if ((OutP->bytes_writtenType.msgt_inline != TRUE) ||
	    (OutP->bytes_writtenType.msgt_longform != FALSE) ||
	    (OutP->bytes_writtenType.msgt_name != 2) ||
	    (OutP->bytes_writtenType.msgt_number != 1) ||
	    (OutP->bytes_writtenType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*bytes_written /* bytes_written */ = /* *bytes_written */ OutP->bytes_written;

	return OutP->RetCode;
}

/* Routine device_read */
mig_external kern_return_t device_read
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	dev_mode_t mode,
	recnum_t recnum,
	int bytes_wanted,
	io_buf_ptr_t *data,
	mach_msg_type_number_t *dataCnt
)
#else
	(device, mode, recnum, bytes_wanted, data, dataCnt)
	device_t device;
	dev_mode_t mode;
	recnum_t recnum;
	int bytes_wanted;
	io_buf_ptr_t *data;
	mach_msg_type_number_t *dataCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_t recnumType;
		recnum_t recnum;
		mach_msg_type_t bytes_wantedType;
		int bytes_wanted;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_long_t dataType;
		io_buf_ptr_t data;
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
	static mach_msg_type_t modeType = {
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
	static mach_msg_type_t recnumType = {
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
	static mach_msg_type_t bytes_wantedType = {
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
	InP->modeType = modeType;
#else	UseStaticMsgType
	InP->modeType.msgt_name = 2;
	InP->modeType.msgt_size = 32;
	InP->modeType.msgt_number = 1;
	InP->modeType.msgt_inline = TRUE;
	InP->modeType.msgt_longform = FALSE;
	InP->modeType.msgt_deallocate = FALSE;
	InP->modeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mode /* mode */ = /* mode */ mode;

#if	UseStaticMsgType
	InP->recnumType = recnumType;
#else	UseStaticMsgType
	InP->recnumType.msgt_name = 2;
	InP->recnumType.msgt_size = 32;
	InP->recnumType.msgt_number = 1;
	InP->recnumType.msgt_inline = TRUE;
	InP->recnumType.msgt_longform = FALSE;
	InP->recnumType.msgt_deallocate = FALSE;
	InP->recnumType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->recnum /* recnum */ = /* recnum */ recnum;

#if	UseStaticMsgType
	InP->bytes_wantedType = bytes_wantedType;
#else	UseStaticMsgType
	InP->bytes_wantedType.msgt_name = 2;
	InP->bytes_wantedType.msgt_size = 32;
	InP->bytes_wantedType.msgt_number = 1;
	InP->bytes_wantedType.msgt_inline = TRUE;
	InP->bytes_wantedType.msgt_longform = FALSE;
	InP->bytes_wantedType.msgt_deallocate = FALSE;
	InP->bytes_wantedType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->bytes_wanted /* bytes_wanted */ = /* bytes_wanted */ bytes_wanted;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2804;

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

	if (OutP->Head.msgh_id != 2904)
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
	if ((OutP->dataType.msgtl_header.msgt_inline != FALSE) ||
	    (OutP->dataType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->dataType.msgtl_name != 9) ||
	    (OutP->dataType.msgtl_size != 8))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*data /* data */ = /* *data */ OutP->data;

	*dataCnt /* dataType.msgtl_number */ = /* *dataCnt */ OutP->dataType.msgtl_number;

	return OutP->RetCode;
}

/* Routine device_read_inband */
mig_external kern_return_t device_read_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	dev_mode_t mode,
	recnum_t recnum,
	int bytes_wanted,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t *dataCnt
)
#else
	(device, mode, recnum, bytes_wanted, data, dataCnt)
	device_t device;
	dev_mode_t mode;
	recnum_t recnum;
	int bytes_wanted;
	io_buf_ptr_inband_t data;
	mach_msg_type_number_t *dataCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_t recnumType;
		recnum_t recnum;
		mach_msg_type_t bytes_wantedType;
		int bytes_wanted;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t dataType;
		char data[128];
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
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t modeType = {
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
	static mach_msg_type_t recnumType = {
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
	static mach_msg_type_t bytes_wantedType = {
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
	InP->modeType = modeType;
#else	UseStaticMsgType
	InP->modeType.msgt_name = 2;
	InP->modeType.msgt_size = 32;
	InP->modeType.msgt_number = 1;
	InP->modeType.msgt_inline = TRUE;
	InP->modeType.msgt_longform = FALSE;
	InP->modeType.msgt_deallocate = FALSE;
	InP->modeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->mode /* mode */ = /* mode */ mode;

#if	UseStaticMsgType
	InP->recnumType = recnumType;
#else	UseStaticMsgType
	InP->recnumType.msgt_name = 2;
	InP->recnumType.msgt_size = 32;
	InP->recnumType.msgt_number = 1;
	InP->recnumType.msgt_inline = TRUE;
	InP->recnumType.msgt_longform = FALSE;
	InP->recnumType.msgt_deallocate = FALSE;
	InP->recnumType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->recnum /* recnum */ = /* recnum */ recnum;

#if	UseStaticMsgType
	InP->bytes_wantedType = bytes_wantedType;
#else	UseStaticMsgType
	InP->bytes_wantedType.msgt_name = 2;
	InP->bytes_wantedType.msgt_size = 32;
	InP->bytes_wantedType.msgt_number = 1;
	InP->bytes_wantedType.msgt_inline = TRUE;
	InP->bytes_wantedType.msgt_longform = FALSE;
	InP->bytes_wantedType.msgt_deallocate = FALSE;
	InP->bytes_wantedType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->bytes_wanted /* bytes_wanted */ = /* bytes_wanted */ bytes_wanted;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2805;

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

	if (OutP->Head.msgh_id != 2905)
		return MIG_REPLY_MISMATCH;

#if	TypeCheck
	if (((msgh_size < 36) || !msgh_simple) &&
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
	if ((OutP->dataType.msgt_inline != TRUE) ||
	    (OutP->dataType.msgt_longform != FALSE) ||
	    (OutP->dataType.msgt_name != 8) ||
	    (OutP->dataType.msgt_size != 8))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (1 * OutP->dataType.msgt_number)+3 &~ 3;
	if (msgh_size != 36 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->dataType.msgt_number / 1 > *dataCnt) {
		bcopy((char *) OutP->data, (char *) data, 1 * *dataCnt);
		*dataCnt /* dataType.msgt_number 1 */ = /* *dataCnt */ OutP->dataType.msgt_number / 1;
		return MIG_ARRAY_TOO_LARGE;
	}
	bcopy((char *) OutP->data, (char *) data, 1 * OutP->dataType.msgt_number);

	*dataCnt /* dataType.msgt_number */ = /* *dataCnt */ OutP->dataType.msgt_number;

	OutP = &Mess.Out;
	return OutP->RetCode;
}

/* Routine device_set_status */
mig_external kern_return_t device_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	int flavor,
	dev_status_t status,
	mach_msg_type_number_t statusCnt
)
#else
	(device, flavor, status, statusCnt)
	device_t device;
	int flavor;
	dev_status_t status;
	mach_msg_type_number_t statusCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t flavorType;
		int flavor;
		mach_msg_type_long_t statusType;
		int status[1024];
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
	unsigned int msgh_size = 44;
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
	static mach_msg_type_long_t statusType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	1024,
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

#if	UseStaticMsgType
	InP->statusType = statusType;
#else	UseStaticMsgType
	InP->statusType.msgtl_name = 2;
	InP->statusType.msgtl_size = 32;
	InP->statusType.msgtl_header.msgt_name = 0;
	InP->statusType.msgtl_header.msgt_size = 0;
	InP->statusType.msgtl_header.msgt_number = 0;
	InP->statusType.msgtl_header.msgt_inline = TRUE;
	InP->statusType.msgtl_header.msgt_longform = TRUE;
	InP->statusType.msgtl_header.msgt_deallocate = FALSE;
	InP->statusType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	if (statusCnt > 1024)
		return MIG_ARRAY_TOO_LARGE;
	bcopy((char *) status, (char *) InP->status, 4 * statusCnt);

	InP->statusType.msgtl_number /* statusCnt */ = /* statusType.msgtl_number */ statusCnt;

	msgh_size_delta = (4 * statusCnt);
	msgh_size += msgh_size_delta;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2806;

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

	if (OutP->Head.msgh_id != 2906)
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

/* Routine device_get_status */
mig_external kern_return_t device_get_status
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	int flavor,
	dev_status_t status,
	mach_msg_type_number_t *statusCnt
)
#else
	(device, flavor, status, statusCnt)
	device_t device;
	int flavor;
	dev_status_t status;
	mach_msg_type_number_t *statusCnt;
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
		mach_msg_type_long_t statusType;
		int status[1024];
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
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2807;

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

	if (OutP->Head.msgh_id != 2907)
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
	if ((OutP->statusType.msgtl_header.msgt_inline != TRUE) ||
	    (OutP->statusType.msgtl_header.msgt_longform != TRUE) ||
	    (OutP->statusType.msgtl_name != 2) ||
	    (OutP->statusType.msgtl_size != 32))
		return MIG_TYPE_ERROR;
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (4 * OutP->statusType.msgtl_number);
	if (msgh_size != 44 + msgh_size_delta)
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	if (OutP->statusType.msgtl_number / 1 > *statusCnt) {
		bcopy((char *) OutP->status, (char *) status, 4 * *statusCnt);
		*statusCnt /* statusType.msgtl_number 1 */ = /* *statusCnt */ OutP->statusType.msgtl_number / 1;
		return MIG_ARRAY_TOO_LARGE;
	}
	bcopy((char *) OutP->status, (char *) status, 4 * OutP->statusType.msgtl_number);

	*statusCnt /* statusType.msgtl_number */ = /* *statusCnt */ OutP->statusType.msgtl_number;

	OutP = &Mess.Out;
	return OutP->RetCode;
}

/* Routine device_set_filter */
mig_external kern_return_t device_set_filter
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	mach_port_t receive_port,
	mach_msg_type_name_t receive_portPoly,
	int priority,
	filter_array_t filter,
	mach_msg_type_number_t filterCnt
)
#else
	(device, receive_port, receive_portPoly, priority, filter, filterCnt)
	device_t device;
	mach_port_t receive_port;
	mach_msg_type_name_t receive_portPoly;
	int priority;
	filter_array_t filter;
	mach_msg_type_number_t filterCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t receive_portType;
		mach_port_t receive_port;
		mach_msg_type_t priorityType;
		int priority;
		mach_msg_type_long_t filterType;
		short filter[64];
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
	unsigned int msgh_size = 52;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t receive_portType = {
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
	static mach_msg_type_t priorityType = {
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
	static mach_msg_type_long_t filterType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	1,
		/* msgtl_size = */	16,
		/* msgtl_number = */	64,
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
	InP->receive_portType = receive_portType;
#else	UseStaticMsgType
	InP->receive_portType.msgt_size = 32;
	InP->receive_portType.msgt_number = 1;
	InP->receive_portType.msgt_inline = TRUE;
	InP->receive_portType.msgt_longform = FALSE;
	InP->receive_portType.msgt_deallocate = FALSE;
	InP->receive_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->receive_port /* receive_port */ = /* receive_port */ receive_port;

	if (MACH_MSG_TYPE_PORT_ANY(receive_portPoly))
		msgh_simple = FALSE;

	InP->receive_portType.msgt_name /* receive_portPoly */ = /* receive_portType.msgt_name */ receive_portPoly;

#if	UseStaticMsgType
	InP->priorityType = priorityType;
#else	UseStaticMsgType
	InP->priorityType.msgt_name = 2;
	InP->priorityType.msgt_size = 32;
	InP->priorityType.msgt_number = 1;
	InP->priorityType.msgt_inline = TRUE;
	InP->priorityType.msgt_longform = FALSE;
	InP->priorityType.msgt_deallocate = FALSE;
	InP->priorityType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->priority /* priority */ = /* priority */ priority;

#if	UseStaticMsgType
	InP->filterType = filterType;
#else	UseStaticMsgType
	InP->filterType.msgtl_name = 1;
	InP->filterType.msgtl_size = 16;
	InP->filterType.msgtl_header.msgt_name = 0;
	InP->filterType.msgtl_header.msgt_size = 0;
	InP->filterType.msgtl_header.msgt_number = 0;
	InP->filterType.msgtl_header.msgt_inline = TRUE;
	InP->filterType.msgtl_header.msgt_longform = TRUE;
	InP->filterType.msgtl_header.msgt_deallocate = FALSE;
	InP->filterType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	if (filterCnt > 64)
		return MIG_ARRAY_TOO_LARGE;
	bcopy((char *) filter, (char *) InP->filter, 2 * filterCnt);

	InP->filterType.msgtl_number /* filterCnt */ = /* filterType.msgtl_number */ filterCnt;

	msgh_size_delta = (2 * filterCnt)+3 &~ 3;
	msgh_size += msgh_size_delta;

	InP->Head.msgh_bits = msgh_simple ?
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) :
		(MACH_MSGH_BITS_COMPLEX|
		 MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2808;

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

	if (OutP->Head.msgh_id != 2908)
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

/* Routine device_map */
mig_external kern_return_t device_map
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	vm_prot_t prot,
	vm_offset_t offset,
	vm_size_t size,
	memory_object_t *pager,
	int unmap
)
#else
	(device, prot, offset, size, pager, unmap)
	device_t device;
	vm_prot_t prot;
	vm_offset_t offset;
	vm_size_t size;
	memory_object_t *pager;
	int unmap;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t protType;
		vm_prot_t prot;
		mach_msg_type_t offsetType;
		vm_offset_t offset;
		mach_msg_type_t sizeType;
		vm_size_t size;
		mach_msg_type_t unmapType;
		int unmap;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t pagerType;
		memory_object_t pager;
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
	static mach_msg_type_t protType = {
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
	static mach_msg_type_t offsetType = {
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
	static mach_msg_type_t unmapType = {
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
	static mach_msg_type_t pagerCheck = {
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
	InP->protType = protType;
#else	UseStaticMsgType
	InP->protType.msgt_name = 2;
	InP->protType.msgt_size = 32;
	InP->protType.msgt_number = 1;
	InP->protType.msgt_inline = TRUE;
	InP->protType.msgt_longform = FALSE;
	InP->protType.msgt_deallocate = FALSE;
	InP->protType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->prot /* prot */ = /* prot */ prot;

#if	UseStaticMsgType
	InP->offsetType = offsetType;
#else	UseStaticMsgType
	InP->offsetType.msgt_name = 2;
	InP->offsetType.msgt_size = 32;
	InP->offsetType.msgt_number = 1;
	InP->offsetType.msgt_inline = TRUE;
	InP->offsetType.msgt_longform = FALSE;
	InP->offsetType.msgt_deallocate = FALSE;
	InP->offsetType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->offset /* offset */ = /* offset */ offset;

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
	InP->unmapType = unmapType;
#else	UseStaticMsgType
	InP->unmapType.msgt_name = 2;
	InP->unmapType.msgt_size = 32;
	InP->unmapType.msgt_number = 1;
	InP->unmapType.msgt_inline = TRUE;
	InP->unmapType.msgt_longform = FALSE;
	InP->unmapType.msgt_deallocate = FALSE;
	InP->unmapType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->unmap /* unmap */ = /* unmap */ unmap;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = device;
	InP->Head.msgh_reply_port = mig_get_reply_port();
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL | MSG_TYPE_RPC;
	InP->Head.msgh_id = 2809;

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

	if (OutP->Head.msgh_id != 2909)
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
	if (* (int *) &OutP->pagerType != * (int *) &pagerCheck)
#else	UseStaticMsgType
	if ((OutP->pagerType.msgt_inline != TRUE) ||
	    (OutP->pagerType.msgt_longform != FALSE) ||
	    (OutP->pagerType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (OutP->pagerType.msgt_number != 1) ||
	    (OutP->pagerType.msgt_size != 32))
#endif	UseStaticMsgType
		return MIG_TYPE_ERROR;
#endif	TypeCheck

	*pager /* pager */ = /* *pager */ OutP->pager;

	return OutP->RetCode;
}
