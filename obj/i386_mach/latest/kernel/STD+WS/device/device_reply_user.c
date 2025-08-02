#include "device_reply.h"
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

mig_external void init_device_reply
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

/* SimpleRoutine device_open_reply */
mig_external kern_return_t ds_device_open_reply
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	mach_port_t device_port
)
#else
	(reply_port, reply_portPoly, return_code, device_port)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	mach_port_t device_port;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t device_portType;
		mach_port_t device_port;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeType = {
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
	static mach_msg_type_t device_portType = {
		/* msgt_name = */		MACH_MSG_TYPE_MAKE_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->return_codeType = return_codeType;
#else	UseStaticMsgType
	InP->return_codeType.msgt_name = 2;
	InP->return_codeType.msgt_size = 32;
	InP->return_codeType.msgt_number = 1;
	InP->return_codeType.msgt_inline = TRUE;
	InP->return_codeType.msgt_longform = FALSE;
	InP->return_codeType.msgt_deallocate = FALSE;
	InP->return_codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->return_code /* return_code */ = /* return_code */ return_code;

#if	UseStaticMsgType
	InP->device_portType = device_portType;
#else	UseStaticMsgType
	InP->device_portType.msgt_name = MACH_MSG_TYPE_MAKE_SEND;
	InP->device_portType.msgt_size = 32;
	InP->device_portType.msgt_number = 1;
	InP->device_portType.msgt_inline = TRUE;
	InP->device_portType.msgt_longform = FALSE;
	InP->device_portType.msgt_deallocate = FALSE;
	InP->device_portType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->device_port /* device_port */ = /* device_port */ device_port;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2900;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine device_write_reply */
mig_external kern_return_t ds_device_write_reply
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
)
#else
	(reply_port, reply_portPoly, return_code, bytes_written)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	int bytes_written;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeType = {
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
	static mach_msg_type_t bytes_writtenType = {
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
	InP->return_codeType = return_codeType;
#else	UseStaticMsgType
	InP->return_codeType.msgt_name = 2;
	InP->return_codeType.msgt_size = 32;
	InP->return_codeType.msgt_number = 1;
	InP->return_codeType.msgt_inline = TRUE;
	InP->return_codeType.msgt_longform = FALSE;
	InP->return_codeType.msgt_deallocate = FALSE;
	InP->return_codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->return_code /* return_code */ = /* return_code */ return_code;

#if	UseStaticMsgType
	InP->bytes_writtenType = bytes_writtenType;
#else	UseStaticMsgType
	InP->bytes_writtenType.msgt_name = 2;
	InP->bytes_writtenType.msgt_size = 32;
	InP->bytes_writtenType.msgt_number = 1;
	InP->bytes_writtenType.msgt_inline = TRUE;
	InP->bytes_writtenType.msgt_longform = FALSE;
	InP->bytes_writtenType.msgt_deallocate = FALSE;
	InP->bytes_writtenType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->bytes_written /* bytes_written */ = /* bytes_written */ bytes_written;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2902;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine device_write_reply_inband */
mig_external kern_return_t ds_device_write_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	int bytes_written
)
#else
	(reply_port, reply_portPoly, return_code, bytes_written)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	int bytes_written;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t bytes_writtenType;
		int bytes_written;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 40;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeType = {
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
	static mach_msg_type_t bytes_writtenType = {
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
	InP->return_codeType = return_codeType;
#else	UseStaticMsgType
	InP->return_codeType.msgt_name = 2;
	InP->return_codeType.msgt_size = 32;
	InP->return_codeType.msgt_number = 1;
	InP->return_codeType.msgt_inline = TRUE;
	InP->return_codeType.msgt_longform = FALSE;
	InP->return_codeType.msgt_deallocate = FALSE;
	InP->return_codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->return_code /* return_code */ = /* return_code */ return_code;

#if	UseStaticMsgType
	InP->bytes_writtenType = bytes_writtenType;
#else	UseStaticMsgType
	InP->bytes_writtenType.msgt_name = 2;
	InP->bytes_writtenType.msgt_size = 32;
	InP->bytes_writtenType.msgt_number = 1;
	InP->bytes_writtenType.msgt_inline = TRUE;
	InP->bytes_writtenType.msgt_longform = FALSE;
	InP->bytes_writtenType.msgt_deallocate = FALSE;
	InP->bytes_writtenType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->bytes_written /* bytes_written */ = /* bytes_written */ bytes_written;

	InP->Head.msgh_bits =
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2903;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine device_read_reply */
mig_external kern_return_t ds_device_read_reply
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(reply_port, reply_portPoly, return_code, data, dataCnt)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	io_buf_ptr_t data;
	mach_msg_type_number_t dataCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_long_t dataType;
		io_buf_ptr_t data;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 48;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeType = {
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
		/* msgt_deallocate = */		TRUE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	9,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	InP->return_codeType = return_codeType;
#else	UseStaticMsgType
	InP->return_codeType.msgt_name = 2;
	InP->return_codeType.msgt_size = 32;
	InP->return_codeType.msgt_number = 1;
	InP->return_codeType.msgt_inline = TRUE;
	InP->return_codeType.msgt_longform = FALSE;
	InP->return_codeType.msgt_deallocate = FALSE;
	InP->return_codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->return_code /* return_code */ = /* return_code */ return_code;

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
	InP->dataType.msgtl_header.msgt_deallocate = TRUE;
	InP->dataType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->data /* data */ = /* data */ data;

	InP->dataType.msgtl_number /* dataCnt */ = /* dataType.msgtl_number */ dataCnt;

	InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX|
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2904;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}

/* SimpleRoutine device_read_reply_inband */
mig_external kern_return_t ds_device_read_reply_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t reply_port,
	mach_msg_type_name_t reply_portPoly,
	kern_return_t return_code,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(reply_port, reply_portPoly, return_code, data, dataCnt)
	mach_port_t reply_port;
	mach_msg_type_name_t reply_portPoly;
	kern_return_t return_code;
	io_buf_ptr_inband_t data;
	mach_msg_type_number_t dataCnt;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t return_codeType;
		kern_return_t return_code;
		mach_msg_type_t dataType;
		char data[128];
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
	unsigned int msgh_size = 36;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t return_codeType = {
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
	InP->return_codeType = return_codeType;
#else	UseStaticMsgType
	InP->return_codeType.msgt_name = 2;
	InP->return_codeType.msgt_size = 32;
	InP->return_codeType.msgt_number = 1;
	InP->return_codeType.msgt_inline = TRUE;
	InP->return_codeType.msgt_longform = FALSE;
	InP->return_codeType.msgt_deallocate = FALSE;
	InP->return_codeType.msgt_unused = 0;
#endif	UseStaticMsgType

	InP->return_code /* return_code */ = /* return_code */ return_code;

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
		MACH_MSGH_BITS(reply_portPoly, 0);
	/* msgh_size passed as argument */
	InP->Head.msgh_request_port = reply_port;
	InP->Head.msgh_reply_port = MACH_PORT_NULL;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2905;

	msg_result = mach_msg_send_from_kernel(&InP->Head, msgh_size);
	return msg_result;
}
