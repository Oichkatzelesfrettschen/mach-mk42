#include "device_request.h"
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

mig_external void init_device_request
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

/* SimpleRoutine device_open_request */
mig_external kern_return_t device_open_request
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t device_server_port,
	mach_port_t reply_port,
	dev_mode_t mode,
	dev_name_t name
)
#else
	(device_server_port, reply_port, mode, name)
	mach_port_t device_server_port;
	mach_port_t reply_port;
	dev_mode_t mode;
	dev_name_t name;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t modeType;
		dev_mode_t mode;
		mach_msg_type_long_t nameType;
		dev_name_t name;
	} Request;

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
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
	InP->Head.msgh_request_port = device_server_port;
	InP->Head.msgh_reply_port = reply_port;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2800;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}

/* SimpleRoutine device_write_request */
mig_external kern_return_t device_write_request
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	mach_port_t reply_port,
	dev_mode_t mode,
	recnum_t recnum,
	io_buf_ptr_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(device, reply_port, mode, recnum, data, dataCnt)
	device_t device;
	mach_port_t reply_port;
	dev_mode_t mode;
	recnum_t recnum;
	io_buf_ptr_t data;
	mach_msg_type_number_t dataCnt;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
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
	InP->Head.msgh_reply_port = reply_port;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2802;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}

/* SimpleRoutine device_write_request_inband */
mig_external kern_return_t device_write_request_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	mach_port_t reply_port,
	dev_mode_t mode,
	recnum_t recnum,
	io_buf_ptr_inband_t data,
	mach_msg_type_number_t dataCnt
)
#else
	(device, reply_port, mode, recnum, data, dataCnt)
	device_t device;
	mach_port_t reply_port;
	dev_mode_t mode;
	recnum_t recnum;
	io_buf_ptr_inband_t data;
	mach_msg_type_number_t dataCnt;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
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
	InP->Head.msgh_reply_port = reply_port;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2803;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}

/* SimpleRoutine device_read_request */
mig_external kern_return_t device_read_request
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	mach_port_t reply_port,
	dev_mode_t mode,
	recnum_t recnum,
	int bytes_wanted
)
#else
	(device, reply_port, mode, recnum, bytes_wanted)
	device_t device;
	mach_port_t reply_port;
	dev_mode_t mode;
	recnum_t recnum;
	int bytes_wanted;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
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
	InP->Head.msgh_reply_port = reply_port;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2804;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}

/* SimpleRoutine device_read_request_inband */
mig_external kern_return_t device_read_request_inband
#if	(defined(__STDC__) || defined(c_plusplus))
(
	device_t device,
	mach_port_t reply_port,
	dev_mode_t mode,
	recnum_t recnum,
	int bytes_wanted
)
#else
	(device, reply_port, mode, recnum, bytes_wanted)
	device_t device;
	mach_port_t reply_port;
	dev_mode_t mode;
	recnum_t recnum;
	int bytes_wanted;
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

	union {
		Request In;
	} Mess;

	register Request *InP = &Mess.In;

	mach_msg_return_t msg_result;
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
	InP->Head.msgh_reply_port = reply_port;
	InP->Head.msgh_kind = MACH_MSGH_KIND_NORMAL;
	InP->Head.msgh_id = 2805;

	msg_result = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	return msg_result;
}
