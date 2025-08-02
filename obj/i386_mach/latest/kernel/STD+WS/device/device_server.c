/* Module device */

#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <ipc/ipc_port.h>

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

#define msgh_request_port	msgh_remote_port
#define MACH_MSGH_BITS_REQUEST(bits)	MACH_MSGH_BITS_REMOTE(bits)
#define msgh_reply_port		msgh_local_port
#define MACH_MSGH_BITS_REPLY(bits)	MACH_MSGH_BITS_LOCAL(bits)

#include <mach/std_types.h>
#include <kern/ipc_kobject.h>
#include <kern/ipc_tt.h>
#include <kern/ipc_host.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <kern/host.h>
#include <kern/processor.h>
#include <vm/vm_object.h>
#include <vm/vm_map.h>
#include <ipc/ipc_space.h>
#include <mach/mach_types.h>
#include <device/device_types.h>
#include <device/net_status.h>

/* Routine device_open */
mig_internal novalue _Xdevice_open
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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
		mach_port_t device;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_open
#if	(defined(__STDC__) || defined(c_plusplus))
		(mach_port_t master_port, mach_port_t reply_port, mach_msg_type_name_t reply_portPoly, dev_mode_t mode, dev_name_t name, device_t *device);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t modeCheck = {
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
	static mach_msg_type_t deviceType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 172) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->modeType != * (int *) &modeCheck)
#else	UseStaticMsgType
	if ((In0P->modeType.msgt_inline != TRUE) ||
	    (In0P->modeType.msgt_longform != FALSE) ||
	    (In0P->modeType.msgt_name != 2) ||
	    (In0P->modeType.msgt_number != 1) ||
	    (In0P->modeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->nameType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->nameType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->nameType.msgtl_name != 12) ||
	    (In0P->nameType.msgtl_number != 1) ||
	    (In0P->nameType.msgtl_size != 1024))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = ds_device_open(In0P->Head.msgh_request_port, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), In0P->mode, In0P->name, &device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->deviceType = deviceType;
#else	UseStaticMsgType
	OutP->deviceType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->deviceType.msgt_size = 32;
	OutP->deviceType.msgt_number = 1;
	OutP->deviceType.msgt_inline = TRUE;
	OutP->deviceType.msgt_longform = FALSE;
	OutP->deviceType.msgt_deallocate = FALSE;
	OutP->deviceType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->device /* convert_device_to_port device */ = /* device */ convert_device_to_port(device);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_close */
mig_internal novalue _Xdevice_close
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_close
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_close(device);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_write */
mig_internal novalue _Xdevice_write
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_write
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, mach_port_t reply_port, mach_msg_type_name_t reply_portPoly, dev_mode_t mode, recnum_t recnum, io_buf_ptr_t data, mach_msg_type_number_t dataCnt, int *bytes_written);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t modeCheck = {
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
	static mach_msg_type_t recnumCheck = {
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

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->modeType != * (int *) &modeCheck)
#else	UseStaticMsgType
	if ((In0P->modeType.msgt_inline != TRUE) ||
	    (In0P->modeType.msgt_longform != FALSE) ||
	    (In0P->modeType.msgt_name != 2) ||
	    (In0P->modeType.msgt_number != 1) ||
	    (In0P->modeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->recnumType != * (int *) &recnumCheck)
#else	UseStaticMsgType
	if ((In0P->recnumType.msgt_inline != TRUE) ||
	    (In0P->recnumType.msgt_longform != FALSE) ||
	    (In0P->recnumType.msgt_name != 2) ||
	    (In0P->recnumType.msgt_number != 1) ||
	    (In0P->recnumType.msgt_size != 32))
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

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_write(device, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), In0P->mode, In0P->recnum, In0P->data, In0P->dataType.msgtl_number, &OutP->bytes_written);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->bytes_writtenType = bytes_writtenType;
#else	UseStaticMsgType
	OutP->bytes_writtenType.msgt_name = 2;
	OutP->bytes_writtenType.msgt_size = 32;
	OutP->bytes_writtenType.msgt_number = 1;
	OutP->bytes_writtenType.msgt_inline = TRUE;
	OutP->bytes_writtenType.msgt_longform = FALSE;
	OutP->bytes_writtenType.msgt_deallocate = FALSE;
	OutP->bytes_writtenType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_write_inband */
mig_internal novalue _Xdevice_write_inband
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_write_inband
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, mach_port_t reply_port, mach_msg_type_name_t reply_portPoly, dev_mode_t mode, recnum_t recnum, io_buf_ptr_inband_t data, mach_msg_type_number_t dataCnt, int *bytes_written);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t modeCheck = {
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
	static mach_msg_type_t recnumCheck = {
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

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 44) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->modeType != * (int *) &modeCheck)
#else	UseStaticMsgType
	if ((In0P->modeType.msgt_inline != TRUE) ||
	    (In0P->modeType.msgt_longform != FALSE) ||
	    (In0P->modeType.msgt_name != 2) ||
	    (In0P->modeType.msgt_number != 1) ||
	    (In0P->modeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->recnumType != * (int *) &recnumCheck)
#else	UseStaticMsgType
	if ((In0P->recnumType.msgt_inline != TRUE) ||
	    (In0P->recnumType.msgt_longform != FALSE) ||
	    (In0P->recnumType.msgt_name != 2) ||
	    (In0P->recnumType.msgt_number != 1) ||
	    (In0P->recnumType.msgt_size != 32))
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
	if (msgh_size != 44 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_write_inband(device, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), In0P->mode, In0P->recnum, In0P->data, In0P->dataType.msgt_number, &OutP->bytes_written);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->bytes_writtenType = bytes_writtenType;
#else	UseStaticMsgType
	OutP->bytes_writtenType.msgt_name = 2;
	OutP->bytes_writtenType.msgt_size = 32;
	OutP->bytes_writtenType.msgt_number = 1;
	OutP->bytes_writtenType.msgt_inline = TRUE;
	OutP->bytes_writtenType.msgt_longform = FALSE;
	OutP->bytes_writtenType.msgt_deallocate = FALSE;
	OutP->bytes_writtenType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_read */
mig_internal novalue _Xdevice_read
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_read
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, mach_port_t reply_port, mach_msg_type_name_t reply_portPoly, dev_mode_t mode, recnum_t recnum, int bytes_wanted, io_buf_ptr_t *data, mach_msg_type_number_t *dataCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t modeCheck = {
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
	static mach_msg_type_t recnumCheck = {
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
	static mach_msg_type_t bytes_wantedCheck = {
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

	device_t device;
	mach_msg_type_number_t dataCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->modeType != * (int *) &modeCheck)
#else	UseStaticMsgType
	if ((In0P->modeType.msgt_inline != TRUE) ||
	    (In0P->modeType.msgt_longform != FALSE) ||
	    (In0P->modeType.msgt_name != 2) ||
	    (In0P->modeType.msgt_number != 1) ||
	    (In0P->modeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->recnumType != * (int *) &recnumCheck)
#else	UseStaticMsgType
	if ((In0P->recnumType.msgt_inline != TRUE) ||
	    (In0P->recnumType.msgt_longform != FALSE) ||
	    (In0P->recnumType.msgt_name != 2) ||
	    (In0P->recnumType.msgt_number != 1) ||
	    (In0P->recnumType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->bytes_wantedType != * (int *) &bytes_wantedCheck)
#else	UseStaticMsgType
	if ((In0P->bytes_wantedType.msgt_inline != TRUE) ||
	    (In0P->bytes_wantedType.msgt_longform != FALSE) ||
	    (In0P->bytes_wantedType.msgt_name != 2) ||
	    (In0P->bytes_wantedType.msgt_number != 1) ||
	    (In0P->bytes_wantedType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_read(device, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), In0P->mode, In0P->recnum, In0P->bytes_wanted, &OutP->data, &dataCnt);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->dataType = dataType;
#else	UseStaticMsgType
	OutP->dataType.msgtl_name = 9;
	OutP->dataType.msgtl_size = 8;
	OutP->dataType.msgtl_header.msgt_name = 0;
	OutP->dataType.msgtl_header.msgt_size = 0;
	OutP->dataType.msgtl_header.msgt_number = 0;
	OutP->dataType.msgtl_header.msgt_inline = FALSE;
	OutP->dataType.msgtl_header.msgt_longform = TRUE;
	OutP->dataType.msgtl_header.msgt_deallocate = FALSE;
	OutP->dataType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->dataType.msgtl_number /* dataCnt */ = /* dataType.msgtl_number */ dataCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_read_inband */
mig_internal novalue _Xdevice_read_inband
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_read_inband
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, mach_port_t reply_port, mach_msg_type_name_t reply_portPoly, dev_mode_t mode, recnum_t recnum, int bytes_wanted, io_buf_ptr_inband_t data, mach_msg_type_number_t *dataCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t modeCheck = {
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
	static mach_msg_type_t recnumCheck = {
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
	static mach_msg_type_t bytes_wantedCheck = {
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

	device_t device;
	mach_msg_type_number_t dataCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 48) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->modeType != * (int *) &modeCheck)
#else	UseStaticMsgType
	if ((In0P->modeType.msgt_inline != TRUE) ||
	    (In0P->modeType.msgt_longform != FALSE) ||
	    (In0P->modeType.msgt_name != 2) ||
	    (In0P->modeType.msgt_number != 1) ||
	    (In0P->modeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->recnumType != * (int *) &recnumCheck)
#else	UseStaticMsgType
	if ((In0P->recnumType.msgt_inline != TRUE) ||
	    (In0P->recnumType.msgt_longform != FALSE) ||
	    (In0P->recnumType.msgt_name != 2) ||
	    (In0P->recnumType.msgt_number != 1) ||
	    (In0P->recnumType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->bytes_wantedType != * (int *) &bytes_wantedCheck)
#else	UseStaticMsgType
	if ((In0P->bytes_wantedType.msgt_inline != TRUE) ||
	    (In0P->bytes_wantedType.msgt_longform != FALSE) ||
	    (In0P->bytes_wantedType.msgt_name != 2) ||
	    (In0P->bytes_wantedType.msgt_number != 1) ||
	    (In0P->bytes_wantedType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	dataCnt = 128;

	OutP->RetCode = ds_device_read_inband(device, In0P->Head.msgh_reply_port, MACH_MSGH_BITS_REPLY(In0P->Head.msgh_bits), In0P->mode, In0P->recnum, In0P->bytes_wanted, OutP->data, &dataCnt);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 36;

#if	UseStaticMsgType
	OutP->dataType = dataType;
#else	UseStaticMsgType
	OutP->dataType.msgt_name = 8;
	OutP->dataType.msgt_size = 8;
	OutP->dataType.msgt_inline = TRUE;
	OutP->dataType.msgt_longform = FALSE;
	OutP->dataType.msgt_deallocate = FALSE;
	OutP->dataType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->dataType.msgt_number /* dataCnt */ = /* dataType.msgt_number */ dataCnt;

	msgh_size_delta = (1 * dataCnt)+3 &~ 3;
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_set_status */
mig_internal novalue _Xdevice_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, int flavor, dev_status_t status, mach_msg_type_number_t statusCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 44) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->statusType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->statusType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->statusType.msgtl_name != 2) ||
	    (In0P->statusType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (4 * In0P->statusType.msgtl_number);
	if (msgh_size != 44 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_set_status(device, In0P->flavor, In0P->status, In0P->statusType.msgtl_number);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_get_status */
mig_internal novalue _Xdevice_get_status
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_get_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, int flavor, dev_status_t status, mach_msg_type_number_t *statusCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t flavorCheck = {
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

	device_t device;
	mach_msg_type_number_t statusCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->flavorType != * (int *) &flavorCheck)
#else	UseStaticMsgType
	if ((In0P->flavorType.msgt_inline != TRUE) ||
	    (In0P->flavorType.msgt_longform != FALSE) ||
	    (In0P->flavorType.msgt_name != 2) ||
	    (In0P->flavorType.msgt_number != 1) ||
	    (In0P->flavorType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	statusCnt = 1024;

	OutP->RetCode = ds_device_get_status(device, In0P->flavor, OutP->status, &statusCnt);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->statusType = statusType;
#else	UseStaticMsgType
	OutP->statusType.msgtl_name = 2;
	OutP->statusType.msgtl_size = 32;
	OutP->statusType.msgtl_header.msgt_name = 0;
	OutP->statusType.msgtl_header.msgt_size = 0;
	OutP->statusType.msgtl_header.msgt_number = 0;
	OutP->statusType.msgtl_header.msgt_inline = TRUE;
	OutP->statusType.msgtl_header.msgt_longform = TRUE;
	OutP->statusType.msgtl_header.msgt_deallocate = FALSE;
	OutP->statusType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->statusType.msgtl_number /* statusCnt */ = /* statusType.msgtl_number */ statusCnt;

	msgh_size_delta = (4 * statusCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_set_filter */
mig_internal novalue _Xdevice_set_filter
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_set_filter
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, mach_port_t receive_port, int priority, filter_array_t filter, mach_msg_type_number_t filterCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	UseStaticMsgType
	static mach_msg_type_t priorityCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 52) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->receive_portType.msgt_inline != TRUE) ||
	    (In0P->receive_portType.msgt_longform != FALSE) ||
	    (In0P->receive_portType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->receive_portType.msgt_number != 1) ||
	    (In0P->receive_portType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->priorityType != * (int *) &priorityCheck)
#else	UseStaticMsgType
	if ((In0P->priorityType.msgt_inline != TRUE) ||
	    (In0P->priorityType.msgt_longform != FALSE) ||
	    (In0P->priorityType.msgt_name != 2) ||
	    (In0P->priorityType.msgt_number != 1) ||
	    (In0P->priorityType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->filterType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->filterType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->filterType.msgtl_name != 1) ||
	    (In0P->filterType.msgtl_size != 16))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (2 * In0P->filterType.msgtl_number)+3 &~ 3;
	if (msgh_size != 52 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_set_filter(device, In0P->receive_port, In0P->priority, In0P->filter, In0P->filterType.msgtl_number);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine device_map */
mig_internal novalue _Xdevice_map
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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
		mach_port_t pager;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t ds_device_map
#if	(defined(__STDC__) || defined(c_plusplus))
		(device_t device, vm_prot_t prot, vm_offset_t offset, vm_size_t size, mach_port_t *pager, int unmap);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t protCheck = {
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

#if	UseStaticMsgType
	static mach_msg_type_t sizeCheck = {
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
	static mach_msg_type_t unmapCheck = {
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
	static mach_msg_type_t pagerType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	device_t device;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 56) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->protType != * (int *) &protCheck)
#else	UseStaticMsgType
	if ((In0P->protType.msgt_inline != TRUE) ||
	    (In0P->protType.msgt_longform != FALSE) ||
	    (In0P->protType.msgt_name != 2) ||
	    (In0P->protType.msgt_number != 1) ||
	    (In0P->protType.msgt_size != 32))
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
#if	UseStaticMsgType
	if (* (int *) &In0P->sizeType != * (int *) &sizeCheck)
#else	UseStaticMsgType
	if ((In0P->sizeType.msgt_inline != TRUE) ||
	    (In0P->sizeType.msgt_longform != FALSE) ||
	    (In0P->sizeType.msgt_name != 2) ||
	    (In0P->sizeType.msgt_number != 1) ||
	    (In0P->sizeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->unmapType != * (int *) &unmapCheck)
#else	UseStaticMsgType
	if ((In0P->unmapType.msgt_inline != TRUE) ||
	    (In0P->unmapType.msgt_longform != FALSE) ||
	    (In0P->unmapType.msgt_name != 2) ||
	    (In0P->unmapType.msgt_number != 1) ||
	    (In0P->unmapType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	device /* dev_port_lookup 0 Head.msgh_request_port */ = /* device */ dev_port_lookup(In0P->Head.msgh_request_port);

	OutP->RetCode = ds_device_map(device, In0P->prot, In0P->offset, In0P->size, &OutP->pager, In0P->unmap);
	device_deallocate(device);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->pagerType = pagerType;
#else	UseStaticMsgType
	OutP->pagerType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->pagerType.msgt_size = 32;
	OutP->pagerType.msgt_number = 1;
	OutP->pagerType.msgt_inline = TRUE;
	OutP->pagerType.msgt_longform = FALSE;
	OutP->pagerType.msgt_deallocate = FALSE;
	OutP->pagerType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

mig_external boolean_t device_server
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

	if ((InP->msgh_id > 2809) || (InP->msgh_id < 2800))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xdevice_open,
			_Xdevice_close,
			_Xdevice_write,
			_Xdevice_write_inband,
			_Xdevice_read,
			_Xdevice_read_inband,
			_Xdevice_set_status,
			_Xdevice_get_status,
			_Xdevice_set_filter,
			_Xdevice_map,
		};

		if (routines[InP->msgh_id - 2800])
			(routines[InP->msgh_id - 2800]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
