/* Module mach_host */

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

/* Routine host_processors */
mig_internal novalue _Xhost_processors
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
		mach_msg_type_long_t processor_listType;
		processor_array_t processor_list;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_processors
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, processor_array_t *processor_list, mach_msg_type_number_t *processor_listCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t processor_listType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t processor_listCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_processors(convert_port_to_host_priv(In0P->Head.msgh_request_port), &OutP->processor_list, &processor_listCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->processor_listType = processor_listType;
#else	UseStaticMsgType
	OutP->processor_listType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->processor_listType.msgtl_size = 32;
	OutP->processor_listType.msgtl_header.msgt_name = 0;
	OutP->processor_listType.msgtl_header.msgt_size = 0;
	OutP->processor_listType.msgtl_header.msgt_number = 0;
	OutP->processor_listType.msgtl_header.msgt_inline = FALSE;
	OutP->processor_listType.msgtl_header.msgt_longform = TRUE;
	OutP->processor_listType.msgtl_header.msgt_deallocate = FALSE;
	OutP->processor_listType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->processor_listType.msgtl_number /* processor_listCnt */ = /* processor_listType.msgtl_number */ processor_listCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_info */
mig_internal novalue _Xhost_info
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
		mach_msg_type_long_t host_info_outType;
		int host_info_out[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, int flavor, host_info_t host_info_out, mach_msg_type_number_t *host_info_outCnt);
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
	static mach_msg_type_long_t host_info_outType = {
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

	mach_msg_type_number_t host_info_outCnt;

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

	host_info_outCnt = 1024;

	OutP->RetCode = host_info(convert_port_to_host(In0P->Head.msgh_request_port), In0P->flavor, OutP->host_info_out, &host_info_outCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->host_info_outType = host_info_outType;
#else	UseStaticMsgType
	OutP->host_info_outType.msgtl_name = 2;
	OutP->host_info_outType.msgtl_size = 32;
	OutP->host_info_outType.msgtl_header.msgt_name = 0;
	OutP->host_info_outType.msgtl_header.msgt_size = 0;
	OutP->host_info_outType.msgtl_header.msgt_number = 0;
	OutP->host_info_outType.msgtl_header.msgt_inline = TRUE;
	OutP->host_info_outType.msgtl_header.msgt_longform = TRUE;
	OutP->host_info_outType.msgtl_header.msgt_deallocate = FALSE;
	OutP->host_info_outType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->host_info_outType.msgtl_number /* host_info_outCnt */ = /* host_info_outType.msgtl_number */ host_info_outCnt;

	msgh_size_delta = (4 * host_info_outCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_info */
mig_internal novalue _Xprocessor_info
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
		mach_msg_type_t hostType;
		mach_port_t host;
		mach_msg_type_long_t processor_info_outType;
		int processor_info_out[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor, int flavor, host_t *host, processor_info_t processor_info_out, mach_msg_type_number_t *processor_info_outCnt);
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
	static mach_msg_type_t hostType = {
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
	static mach_msg_type_long_t processor_info_outType = {
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

	host_t host;
	mach_msg_type_number_t processor_info_outCnt;

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

	processor_info_outCnt = 1024;

	OutP->RetCode = processor_info(convert_port_to_processor(In0P->Head.msgh_request_port), In0P->flavor, &host, OutP->processor_info_out, &processor_info_outCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 52;

#if	UseStaticMsgType
	OutP->hostType = hostType;
#else	UseStaticMsgType
	OutP->hostType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->hostType.msgt_size = 32;
	OutP->hostType.msgt_number = 1;
	OutP->hostType.msgt_inline = TRUE;
	OutP->hostType.msgt_longform = FALSE;
	OutP->hostType.msgt_deallocate = FALSE;
	OutP->hostType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->host /* convert_host_to_port host */ = /* host */ convert_host_to_port(host);

#if	UseStaticMsgType
	OutP->processor_info_outType = processor_info_outType;
#else	UseStaticMsgType
	OutP->processor_info_outType.msgtl_name = 2;
	OutP->processor_info_outType.msgtl_size = 32;
	OutP->processor_info_outType.msgtl_header.msgt_name = 0;
	OutP->processor_info_outType.msgtl_header.msgt_size = 0;
	OutP->processor_info_outType.msgtl_header.msgt_number = 0;
	OutP->processor_info_outType.msgtl_header.msgt_inline = TRUE;
	OutP->processor_info_outType.msgtl_header.msgt_longform = TRUE;
	OutP->processor_info_outType.msgtl_header.msgt_deallocate = FALSE;
	OutP->processor_info_outType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->processor_info_outType.msgtl_number /* processor_info_outCnt */ = /* processor_info_outType.msgtl_number */ processor_info_outCnt;

	msgh_size_delta = (4 * processor_info_outCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_start */
mig_internal novalue _Xprocessor_start
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
	mig_external kern_return_t processor_start
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_start(convert_port_to_processor(In0P->Head.msgh_request_port));
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_exit */
mig_internal novalue _Xprocessor_exit
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
	mig_external kern_return_t processor_exit
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_exit(convert_port_to_processor(In0P->Head.msgh_request_port));
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_control */
mig_internal novalue _Xprocessor_control
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_long_t processor_cmdType;
		int processor_cmd[1024];
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_control
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor, processor_info_t processor_cmd, mach_msg_type_number_t processor_cmdCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;
	unsigned int msgh_size_delta;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size < 36) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->processor_cmdType.msgtl_header.msgt_inline != TRUE) ||
	    (In0P->processor_cmdType.msgtl_header.msgt_longform != TRUE) ||
	    (In0P->processor_cmdType.msgtl_name != 2) ||
	    (In0P->processor_cmdType.msgtl_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	msgh_size_delta = (4 * In0P->processor_cmdType.msgtl_number);
	if (msgh_size != 36 + msgh_size_delta)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_control(convert_port_to_processor(In0P->Head.msgh_request_port), In0P->processor_cmd, In0P->processor_cmdType.msgtl_number);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_default */
mig_internal novalue _Xprocessor_set_default
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
		mach_msg_type_t default_setType;
		mach_port_t default_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_default
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, processor_set_t *default_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t default_setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t default_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_set_default(convert_port_to_host(In0P->Head.msgh_request_port), &default_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->default_setType = default_setType;
#else	UseStaticMsgType
	OutP->default_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->default_setType.msgt_size = 32;
	OutP->default_setType.msgt_number = 1;
	OutP->default_setType.msgt_inline = TRUE;
	OutP->default_setType.msgt_longform = FALSE;
	OutP->default_setType.msgt_deallocate = FALSE;
	OutP->default_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->default_set /* convert_pset_name_to_port default_set */ = /* default_set */ convert_pset_name_to_port(default_set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine xxx_processor_set_default_priv */
mig_internal novalue _Xxxx_processor_set_default_priv
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
		mach_msg_type_t default_setType;
		mach_port_t default_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t xxx_processor_set_default_priv
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, processor_set_t *default_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t default_setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t default_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = xxx_processor_set_default_priv(convert_port_to_host_priv(In0P->Head.msgh_request_port), &default_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->default_setType = default_setType;
#else	UseStaticMsgType
	OutP->default_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->default_setType.msgt_size = 32;
	OutP->default_setType.msgt_number = 1;
	OutP->default_setType.msgt_inline = TRUE;
	OutP->default_setType.msgt_longform = FALSE;
	OutP->default_setType.msgt_deallocate = FALSE;
	OutP->default_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->default_set /* convert_pset_to_port default_set */ = /* default_set */ convert_pset_to_port(default_set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_create */
mig_internal novalue _Xprocessor_set_create
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
		mach_msg_type_t new_setType;
		mach_port_t new_set;
		mach_msg_type_t new_nameType;
		mach_port_t new_name;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_create
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, processor_set_t *new_set, processor_set_t *new_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t new_setType = {
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
	static mach_msg_type_t new_nameType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t new_set;
	processor_set_t new_name;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_set_create(convert_port_to_host(In0P->Head.msgh_request_port), &new_set, &new_name);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->new_setType = new_setType;
#else	UseStaticMsgType
	OutP->new_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->new_setType.msgt_size = 32;
	OutP->new_setType.msgt_number = 1;
	OutP->new_setType.msgt_inline = TRUE;
	OutP->new_setType.msgt_longform = FALSE;
	OutP->new_setType.msgt_deallocate = FALSE;
	OutP->new_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->new_set /* convert_pset_to_port new_set */ = /* new_set */ convert_pset_to_port(new_set);

#if	UseStaticMsgType
	OutP->new_nameType = new_nameType;
#else	UseStaticMsgType
	OutP->new_nameType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->new_nameType.msgt_size = 32;
	OutP->new_nameType.msgt_number = 1;
	OutP->new_nameType.msgt_inline = TRUE;
	OutP->new_nameType.msgt_longform = FALSE;
	OutP->new_nameType.msgt_deallocate = FALSE;
	OutP->new_nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->new_name /* convert_pset_name_to_port new_name */ = /* new_name */ convert_pset_name_to_port(new_name);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_destroy */
mig_internal novalue _Xprocessor_set_destroy
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
	mig_external kern_return_t processor_set_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	processor_set_t set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_destroy(set);
	pset_deallocate(set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_info */
mig_internal novalue _Xprocessor_set_info
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
		mach_msg_type_t hostType;
		mach_port_t host;
		mach_msg_type_long_t info_outType;
		int info_out[1024];
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t set_name, int flavor, host_t *host, processor_set_info_t info_out, mach_msg_type_number_t *info_outCnt);
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
	static mach_msg_type_t hostType = {
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
	static mach_msg_type_long_t info_outType = {
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

	processor_set_t set_name;
	host_t host;
	mach_msg_type_number_t info_outCnt;

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

	set_name /* convert_port_to_pset_name 0 Head.msgh_request_port */ = /* set_name */ convert_port_to_pset_name(In0P->Head.msgh_request_port);

	info_outCnt = 1024;

	OutP->RetCode = processor_set_info(set_name, In0P->flavor, &host, OutP->info_out, &info_outCnt);
	pset_deallocate(set_name);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 52;

#if	UseStaticMsgType
	OutP->hostType = hostType;
#else	UseStaticMsgType
	OutP->hostType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->hostType.msgt_size = 32;
	OutP->hostType.msgt_number = 1;
	OutP->hostType.msgt_inline = TRUE;
	OutP->hostType.msgt_longform = FALSE;
	OutP->hostType.msgt_deallocate = FALSE;
	OutP->hostType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->host /* convert_host_to_port host */ = /* host */ convert_host_to_port(host);

#if	UseStaticMsgType
	OutP->info_outType = info_outType;
#else	UseStaticMsgType
	OutP->info_outType.msgtl_name = 2;
	OutP->info_outType.msgtl_size = 32;
	OutP->info_outType.msgtl_header.msgt_name = 0;
	OutP->info_outType.msgtl_header.msgt_size = 0;
	OutP->info_outType.msgtl_header.msgt_number = 0;
	OutP->info_outType.msgtl_header.msgt_inline = TRUE;
	OutP->info_outType.msgtl_header.msgt_longform = TRUE;
	OutP->info_outType.msgtl_header.msgt_deallocate = FALSE;
	OutP->info_outType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->info_outType.msgtl_number /* info_outCnt */ = /* info_outType.msgtl_number */ info_outCnt;

	msgh_size_delta = (4 * info_outCnt);
	msgh_size += msgh_size_delta;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_assign */
mig_internal novalue _Xprocessor_assign
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_setType;
		mach_port_t new_set;
		mach_msg_type_t waitType;
		boolean_t wait;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_assign
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor, processor_set_t new_set, boolean_t wait);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t waitCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t new_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->new_setType.msgt_inline != TRUE) ||
	    (In0P->new_setType.msgt_longform != FALSE) ||
	    (In0P->new_setType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->new_setType.msgt_number != 1) ||
	    (In0P->new_setType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->waitType != * (int *) &waitCheck)
#else	UseStaticMsgType
	if ((In0P->waitType.msgt_inline != TRUE) ||
	    (In0P->waitType.msgt_longform != FALSE) ||
	    (In0P->waitType.msgt_name != 0) ||
	    (In0P->waitType.msgt_number != 1) ||
	    (In0P->waitType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	new_set /* convert_port_to_pset 0 new_set */ = /* new_set */ convert_port_to_pset(In0P->new_set);

	OutP->RetCode = processor_assign(convert_port_to_processor(In0P->Head.msgh_request_port), new_set, In0P->wait);
	pset_deallocate(new_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->new_set))
		ipc_port_release_send((ipc_port_t) In0P->new_set);

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_get_assignment */
mig_internal novalue _Xprocessor_get_assignment
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
		mach_msg_type_t assigned_setType;
		mach_port_t assigned_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_get_assignment
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_t processor, processor_set_t *assigned_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t assigned_setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t assigned_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = processor_get_assignment(convert_port_to_processor(In0P->Head.msgh_request_port), &assigned_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->assigned_setType = assigned_setType;
#else	UseStaticMsgType
	OutP->assigned_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->assigned_setType.msgt_size = 32;
	OutP->assigned_setType.msgt_number = 1;
	OutP->assigned_setType.msgt_inline = TRUE;
	OutP->assigned_setType.msgt_longform = FALSE;
	OutP->assigned_setType.msgt_deallocate = FALSE;
	OutP->assigned_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->assigned_set /* convert_pset_name_to_port assigned_set */ = /* assigned_set */ convert_pset_name_to_port(assigned_set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_assign */
mig_internal novalue _Xthread_assign
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_setType;
		mach_port_t new_set;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_assign
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, processor_set_t new_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t thread;
	processor_set_t new_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->new_setType.msgt_inline != TRUE) ||
	    (In0P->new_setType.msgt_longform != FALSE) ||
	    (In0P->new_setType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->new_setType.msgt_number != 1) ||
	    (In0P->new_setType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	new_set /* convert_port_to_pset 0 new_set */ = /* new_set */ convert_port_to_pset(In0P->new_set);

	OutP->RetCode = thread_assign(thread, new_set);
	pset_deallocate(new_set);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->new_set))
		ipc_port_release_send((ipc_port_t) In0P->new_set);

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_assign_default */
mig_internal novalue _Xthread_assign_default
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
	mig_external kern_return_t thread_assign_default
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

	thread_t thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_assign_default(thread);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_get_assignment */
mig_internal novalue _Xthread_get_assignment
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
		mach_msg_type_t assigned_setType;
		mach_port_t assigned_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_get_assignment
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, processor_set_t *assigned_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t assigned_setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;
	processor_set_t assigned_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_get_assignment(thread, &assigned_set);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->assigned_setType = assigned_setType;
#else	UseStaticMsgType
	OutP->assigned_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->assigned_setType.msgt_size = 32;
	OutP->assigned_setType.msgt_number = 1;
	OutP->assigned_setType.msgt_inline = TRUE;
	OutP->assigned_setType.msgt_longform = FALSE;
	OutP->assigned_setType.msgt_deallocate = FALSE;
	OutP->assigned_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->assigned_set /* convert_pset_name_to_port assigned_set */ = /* assigned_set */ convert_pset_name_to_port(assigned_set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_assign */
mig_internal novalue _Xtask_assign
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_setType;
		mach_port_t new_set;
		mach_msg_type_t assign_threadsType;
		boolean_t assign_threads;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_assign
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, processor_set_t new_set, boolean_t assign_threads);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t assign_threadsCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;
	processor_set_t new_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->new_setType.msgt_inline != TRUE) ||
	    (In0P->new_setType.msgt_longform != FALSE) ||
	    (In0P->new_setType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->new_setType.msgt_number != 1) ||
	    (In0P->new_setType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->assign_threadsType != * (int *) &assign_threadsCheck)
#else	UseStaticMsgType
	if ((In0P->assign_threadsType.msgt_inline != TRUE) ||
	    (In0P->assign_threadsType.msgt_longform != FALSE) ||
	    (In0P->assign_threadsType.msgt_name != 0) ||
	    (In0P->assign_threadsType.msgt_number != 1) ||
	    (In0P->assign_threadsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	new_set /* convert_port_to_pset 0 new_set */ = /* new_set */ convert_port_to_pset(In0P->new_set);

	OutP->RetCode = task_assign(task, new_set, In0P->assign_threads);
	pset_deallocate(new_set);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->new_set))
		ipc_port_release_send((ipc_port_t) In0P->new_set);

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_assign_default */
mig_internal novalue _Xtask_assign_default
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t assign_threadsType;
		boolean_t assign_threads;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_assign_default
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, boolean_t assign_threads);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t assign_threadsCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->assign_threadsType != * (int *) &assign_threadsCheck)
#else	UseStaticMsgType
	if ((In0P->assign_threadsType.msgt_inline != TRUE) ||
	    (In0P->assign_threadsType.msgt_longform != FALSE) ||
	    (In0P->assign_threadsType.msgt_name != 0) ||
	    (In0P->assign_threadsType.msgt_number != 1) ||
	    (In0P->assign_threadsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_assign_default(task, In0P->assign_threads);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_get_assignment */
mig_internal novalue _Xtask_get_assignment
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
		mach_msg_type_t assigned_setType;
		mach_port_t assigned_set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_get_assignment
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, processor_set_t *assigned_set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t assigned_setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;
	processor_set_t assigned_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_get_assignment(task, &assigned_set);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->assigned_setType = assigned_setType;
#else	UseStaticMsgType
	OutP->assigned_setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->assigned_setType.msgt_size = 32;
	OutP->assigned_setType.msgt_number = 1;
	OutP->assigned_setType.msgt_inline = TRUE;
	OutP->assigned_setType.msgt_longform = FALSE;
	OutP->assigned_setType.msgt_deallocate = FALSE;
	OutP->assigned_setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->assigned_set /* convert_pset_name_to_port assigned_set */ = /* assigned_set */ convert_pset_name_to_port(assigned_set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_kernel_version */
mig_internal novalue _Xhost_kernel_version
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
		mach_msg_type_long_t kernel_versionType;
		kernel_version_t kernel_version;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_kernel_version
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, kernel_version_t kernel_version);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t kernel_versionType = {
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
		/* msgtl_size = */	4096,
		/* msgtl_number = */	1,
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_kernel_version(convert_port_to_host(In0P->Head.msgh_request_port), OutP->kernel_version);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 556;

#if	UseStaticMsgType
	OutP->kernel_versionType = kernel_versionType;
#else	UseStaticMsgType
	OutP->kernel_versionType.msgtl_name = 12;
	OutP->kernel_versionType.msgtl_size = 4096;
	OutP->kernel_versionType.msgtl_number = 1;
	OutP->kernel_versionType.msgtl_header.msgt_name = 0;
	OutP->kernel_versionType.msgtl_header.msgt_size = 0;
	OutP->kernel_versionType.msgtl_header.msgt_number = 0;
	OutP->kernel_versionType.msgtl_header.msgt_inline = TRUE;
	OutP->kernel_versionType.msgtl_header.msgt_longform = TRUE;
	OutP->kernel_versionType.msgtl_header.msgt_deallocate = FALSE;
	OutP->kernel_versionType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_priority */
mig_internal novalue _Xthread_priority
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t priorityType;
		int priority;
		mach_msg_type_t set_maxType;
		boolean_t set_max;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_priority
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, int priority, boolean_t set_max);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

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

#if	UseStaticMsgType
	static mach_msg_type_t set_maxCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
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
#if	UseStaticMsgType
	if (* (int *) &In0P->set_maxType != * (int *) &set_maxCheck)
#else	UseStaticMsgType
	if ((In0P->set_maxType.msgt_inline != TRUE) ||
	    (In0P->set_maxType.msgt_longform != FALSE) ||
	    (In0P->set_maxType.msgt_name != 0) ||
	    (In0P->set_maxType.msgt_number != 1) ||
	    (In0P->set_maxType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_priority(thread, In0P->priority, In0P->set_max);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_max_priority */
mig_internal novalue _Xthread_max_priority
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t processor_setType;
		mach_port_t processor_set;
		mach_msg_type_t max_priorityType;
		int max_priority;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_max_priority
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, processor_set_t processor_set, int max_priority);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t max_priorityCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;
	processor_set_t processor_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->processor_setType.msgt_inline != TRUE) ||
	    (In0P->processor_setType.msgt_longform != FALSE) ||
	    (In0P->processor_setType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->processor_setType.msgt_number != 1) ||
	    (In0P->processor_setType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->max_priorityType != * (int *) &max_priorityCheck)
#else	UseStaticMsgType
	if ((In0P->max_priorityType.msgt_inline != TRUE) ||
	    (In0P->max_priorityType.msgt_longform != FALSE) ||
	    (In0P->max_priorityType.msgt_name != 2) ||
	    (In0P->max_priorityType.msgt_number != 1) ||
	    (In0P->max_priorityType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	processor_set /* convert_port_to_pset 0 processor_set */ = /* processor_set */ convert_port_to_pset(In0P->processor_set);

	OutP->RetCode = thread_max_priority(thread, processor_set, In0P->max_priority);
	pset_deallocate(processor_set);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->processor_set))
		ipc_port_release_send((ipc_port_t) In0P->processor_set);

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine task_priority */
mig_internal novalue _Xtask_priority
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t priorityType;
		int priority;
		mach_msg_type_t change_threadsType;
		boolean_t change_threads;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t task_priority
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, int priority, boolean_t change_threads);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

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

#if	UseStaticMsgType
	static mach_msg_type_t change_threadsCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	task_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
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
#if	UseStaticMsgType
	if (* (int *) &In0P->change_threadsType != * (int *) &change_threadsCheck)
#else	UseStaticMsgType
	if ((In0P->change_threadsType.msgt_inline != TRUE) ||
	    (In0P->change_threadsType.msgt_longform != FALSE) ||
	    (In0P->change_threadsType.msgt_name != 0) ||
	    (In0P->change_threadsType.msgt_number != 1) ||
	    (In0P->change_threadsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = task_priority(task, In0P->priority, In0P->change_threads);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_max_priority */
mig_internal novalue _Xprocessor_set_max_priority
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t max_priorityType;
		int max_priority;
		mach_msg_type_t change_threadsType;
		boolean_t change_threads;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_max_priority
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t processor_set, int max_priority, boolean_t change_threads);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t max_priorityCheck = {
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
	static mach_msg_type_t change_threadsCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t processor_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->max_priorityType != * (int *) &max_priorityCheck)
#else	UseStaticMsgType
	if ((In0P->max_priorityType.msgt_inline != TRUE) ||
	    (In0P->max_priorityType.msgt_longform != FALSE) ||
	    (In0P->max_priorityType.msgt_name != 2) ||
	    (In0P->max_priorityType.msgt_number != 1) ||
	    (In0P->max_priorityType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->change_threadsType != * (int *) &change_threadsCheck)
#else	UseStaticMsgType
	if ((In0P->change_threadsType.msgt_inline != TRUE) ||
	    (In0P->change_threadsType.msgt_longform != FALSE) ||
	    (In0P->change_threadsType.msgt_name != 0) ||
	    (In0P->change_threadsType.msgt_number != 1) ||
	    (In0P->change_threadsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	processor_set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* processor_set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_max_priority(processor_set, In0P->max_priority, In0P->change_threads);
	pset_deallocate(processor_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine thread_policy */
mig_internal novalue _Xthread_policy
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t policyType;
		int policy;
		mach_msg_type_t dataType;
		int data;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t thread_policy
#if	(defined(__STDC__) || defined(c_plusplus))
		(thread_t thread, int policy, int data);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t policyCheck = {
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
	static mach_msg_type_t dataCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	thread_t thread;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->policyType != * (int *) &policyCheck)
#else	UseStaticMsgType
	if ((In0P->policyType.msgt_inline != TRUE) ||
	    (In0P->policyType.msgt_longform != FALSE) ||
	    (In0P->policyType.msgt_name != 2) ||
	    (In0P->policyType.msgt_number != 1) ||
	    (In0P->policyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->dataType != * (int *) &dataCheck)
#else	UseStaticMsgType
	if ((In0P->dataType.msgt_inline != TRUE) ||
	    (In0P->dataType.msgt_longform != FALSE) ||
	    (In0P->dataType.msgt_name != 2) ||
	    (In0P->dataType.msgt_number != 1) ||
	    (In0P->dataType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	thread /* convert_port_to_thread 0 Head.msgh_request_port */ = /* thread */ convert_port_to_thread(In0P->Head.msgh_request_port);

	OutP->RetCode = thread_policy(thread, In0P->policy, In0P->data);
	thread_deallocate(thread);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_policy_enable */
mig_internal novalue _Xprocessor_set_policy_enable
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t policyType;
		int policy;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_policy_enable
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t processor_set, int policy);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t policyCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t processor_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->policyType != * (int *) &policyCheck)
#else	UseStaticMsgType
	if ((In0P->policyType.msgt_inline != TRUE) ||
	    (In0P->policyType.msgt_longform != FALSE) ||
	    (In0P->policyType.msgt_name != 2) ||
	    (In0P->policyType.msgt_number != 1) ||
	    (In0P->policyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	processor_set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* processor_set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_policy_enable(processor_set, In0P->policy);
	pset_deallocate(processor_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_policy_disable */
mig_internal novalue _Xprocessor_set_policy_disable
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t policyType;
		int policy;
		mach_msg_type_t change_threadsType;
		boolean_t change_threads;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_policy_disable
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t processor_set, int policy, boolean_t change_threads);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t policyCheck = {
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
	static mach_msg_type_t change_threadsCheck = {
		/* msgt_name = */		0,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t processor_set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->policyType != * (int *) &policyCheck)
#else	UseStaticMsgType
	if ((In0P->policyType.msgt_inline != TRUE) ||
	    (In0P->policyType.msgt_longform != FALSE) ||
	    (In0P->policyType.msgt_name != 2) ||
	    (In0P->policyType.msgt_number != 1) ||
	    (In0P->policyType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->change_threadsType != * (int *) &change_threadsCheck)
#else	UseStaticMsgType
	if ((In0P->change_threadsType.msgt_inline != TRUE) ||
	    (In0P->change_threadsType.msgt_longform != FALSE) ||
	    (In0P->change_threadsType.msgt_name != 0) ||
	    (In0P->change_threadsType.msgt_number != 1) ||
	    (In0P->change_threadsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	processor_set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* processor_set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_policy_disable(processor_set, In0P->policy, In0P->change_threads);
	pset_deallocate(processor_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_tasks */
mig_internal novalue _Xprocessor_set_tasks
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
		mach_msg_type_long_t task_listType;
		task_array_t task_list;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_tasks
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t processor_set, task_array_t *task_list, mach_msg_type_number_t *task_listCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t task_listType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	processor_set_t processor_set;
	mach_msg_type_number_t task_listCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	processor_set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* processor_set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_tasks(processor_set, &OutP->task_list, &task_listCnt);
	pset_deallocate(processor_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->task_listType = task_listType;
#else	UseStaticMsgType
	OutP->task_listType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->task_listType.msgtl_size = 32;
	OutP->task_listType.msgtl_header.msgt_name = 0;
	OutP->task_listType.msgtl_header.msgt_size = 0;
	OutP->task_listType.msgtl_header.msgt_number = 0;
	OutP->task_listType.msgtl_header.msgt_inline = FALSE;
	OutP->task_listType.msgtl_header.msgt_longform = TRUE;
	OutP->task_listType.msgtl_header.msgt_deallocate = FALSE;
	OutP->task_listType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->task_listType.msgtl_number /* task_listCnt */ = /* task_listType.msgtl_number */ task_listCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_threads */
mig_internal novalue _Xprocessor_set_threads
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
		mach_msg_type_long_t thread_listType;
		thread_array_t thread_list;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_threads
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t processor_set, thread_array_t *thread_list, mach_msg_type_number_t *thread_listCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t thread_listType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	processor_set_t processor_set;
	mach_msg_type_number_t thread_listCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	processor_set /* convert_port_to_pset 0 Head.msgh_request_port */ = /* processor_set */ convert_port_to_pset(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_threads(processor_set, &OutP->thread_list, &thread_listCnt);
	pset_deallocate(processor_set);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->thread_listType = thread_listType;
#else	UseStaticMsgType
	OutP->thread_listType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->thread_listType.msgtl_size = 32;
	OutP->thread_listType.msgtl_header.msgt_name = 0;
	OutP->thread_listType.msgtl_header.msgt_size = 0;
	OutP->thread_listType.msgtl_header.msgt_number = 0;
	OutP->thread_listType.msgtl_header.msgt_inline = FALSE;
	OutP->thread_listType.msgtl_header.msgt_longform = TRUE;
	OutP->thread_listType.msgtl_header.msgt_deallocate = FALSE;
	OutP->thread_listType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->thread_listType.msgtl_number /* thread_listCnt */ = /* thread_listType.msgtl_number */ thread_listCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_processor_sets */
mig_internal novalue _Xhost_processor_sets
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
		mach_msg_type_long_t processor_setsType;
		processor_set_name_array_t processor_sets;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_processor_sets
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, processor_set_name_array_t *processor_sets, mach_msg_type_number_t *processor_setsCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t processor_setsType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	MACH_MSG_TYPE_PORT_SEND,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t processor_setsCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_processor_sets(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->processor_sets, &processor_setsCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->processor_setsType = processor_setsType;
#else	UseStaticMsgType
	OutP->processor_setsType.msgtl_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->processor_setsType.msgtl_size = 32;
	OutP->processor_setsType.msgtl_header.msgt_name = 0;
	OutP->processor_setsType.msgtl_header.msgt_size = 0;
	OutP->processor_setsType.msgtl_header.msgt_number = 0;
	OutP->processor_setsType.msgtl_header.msgt_inline = FALSE;
	OutP->processor_setsType.msgtl_header.msgt_longform = TRUE;
	OutP->processor_setsType.msgtl_header.msgt_deallocate = FALSE;
	OutP->processor_setsType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->processor_setsType.msgtl_number /* processor_setsCnt */ = /* processor_setsType.msgtl_number */ processor_setsCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_processor_set_priv */
mig_internal novalue _Xhost_processor_set_priv
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t set_nameType;
		mach_port_t set_name;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t setType;
		mach_port_t set;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_processor_set_priv
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, processor_set_t set_name, processor_set_t *set);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t setType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t set_name;
	processor_set_t set;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->set_nameType.msgt_inline != TRUE) ||
	    (In0P->set_nameType.msgt_longform != FALSE) ||
	    (In0P->set_nameType.msgt_name != MACH_MSG_TYPE_PORT_SEND) ||
	    (In0P->set_nameType.msgt_number != 1) ||
	    (In0P->set_nameType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	set_name /* convert_port_to_pset_name 0 set_name */ = /* set_name */ convert_port_to_pset_name(In0P->set_name);

	OutP->RetCode = host_processor_set_priv(convert_port_to_host_priv(In0P->Head.msgh_request_port), set_name, &set);
	pset_deallocate(set_name);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	if (IP_VALID((ipc_port_t) In0P->set_name))
		ipc_port_release_send((ipc_port_t) In0P->set_name);

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->setType = setType;
#else	UseStaticMsgType
	OutP->setType.msgt_name = MACH_MSG_TYPE_PORT_SEND;
	OutP->setType.msgt_size = 32;
	OutP->setType.msgt_number = 1;
	OutP->setType.msgt_inline = TRUE;
	OutP->setType.msgt_longform = FALSE;
	OutP->setType.msgt_deallocate = FALSE;
	OutP->setType.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->set /* convert_pset_to_port set */ = /* set */ convert_pset_to_port(set);

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_set_time */
mig_internal novalue _Xhost_set_time
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_timeType;
		time_value_t new_time;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_set_time
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, time_value_t new_time);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t new_timeCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 36) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_timeType != * (int *) &new_timeCheck)
#else	UseStaticMsgType
	if ((In0P->new_timeType.msgt_inline != TRUE) ||
	    (In0P->new_timeType.msgt_longform != FALSE) ||
	    (In0P->new_timeType.msgt_name != 2) ||
	    (In0P->new_timeType.msgt_number != 2) ||
	    (In0P->new_timeType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_set_time(convert_port_to_host_priv(In0P->Head.msgh_request_port), In0P->new_time);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_adjust_time */
mig_internal novalue _Xhost_adjust_time
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t new_adjustmentType;
		time_value_t new_adjustment;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t old_adjustmentType;
		time_value_t old_adjustment;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_adjust_time
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, time_value_t new_adjustment, time_value_t *old_adjustment);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t new_adjustmentCheck = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_t old_adjustmentType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 36) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_adjustmentType != * (int *) &new_adjustmentCheck)
#else	UseStaticMsgType
	if ((In0P->new_adjustmentType.msgt_inline != TRUE) ||
	    (In0P->new_adjustmentType.msgt_longform != FALSE) ||
	    (In0P->new_adjustmentType.msgt_name != 2) ||
	    (In0P->new_adjustmentType.msgt_number != 2) ||
	    (In0P->new_adjustmentType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_adjust_time(convert_port_to_host_priv(In0P->Head.msgh_request_port), In0P->new_adjustment, &OutP->old_adjustment);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->old_adjustmentType = old_adjustmentType;
#else	UseStaticMsgType
	OutP->old_adjustmentType.msgt_name = 2;
	OutP->old_adjustmentType.msgt_size = 32;
	OutP->old_adjustmentType.msgt_number = 2;
	OutP->old_adjustmentType.msgt_inline = TRUE;
	OutP->old_adjustmentType.msgt_longform = FALSE;
	OutP->old_adjustmentType.msgt_deallocate = FALSE;
	OutP->old_adjustmentType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_get_time */
mig_internal novalue _Xhost_get_time
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
		mach_msg_type_t current_timeType;
		time_value_t current_time;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_get_time
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, time_value_t *current_time);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t current_timeType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		2,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_get_time(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->current_time);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 44;

#if	UseStaticMsgType
	OutP->current_timeType = current_timeType;
#else	UseStaticMsgType
	OutP->current_timeType.msgt_name = 2;
	OutP->current_timeType.msgt_size = 32;
	OutP->current_timeType.msgt_number = 2;
	OutP->current_timeType.msgt_inline = TRUE;
	OutP->current_timeType.msgt_longform = FALSE;
	OutP->current_timeType.msgt_deallocate = FALSE;
	OutP->current_timeType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_reboot */
mig_internal novalue _Xhost_reboot
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t optionsType;
		int options;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_reboot
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host_priv, int options);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t optionsCheck = {
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
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->optionsType != * (int *) &optionsCheck)
#else	UseStaticMsgType
	if ((In0P->optionsType.msgt_inline != TRUE) ||
	    (In0P->optionsType.msgt_longform != FALSE) ||
	    (In0P->optionsType.msgt_name != 2) ||
	    (In0P->optionsType.msgt_number != 1) ||
	    (In0P->optionsType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_reboot(convert_port_to_host_priv(In0P->Head.msgh_request_port), In0P->options);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

mig_external boolean_t mach_host_server
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

	if ((InP->msgh_id > 2635) || (InP->msgh_id < 2600))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xhost_processors,
			_Xhost_info,
			_Xprocessor_info,
			_Xprocessor_start,
			_Xprocessor_exit,
			_Xprocessor_control,
			_Xprocessor_set_default,
			_Xxxx_processor_set_default_priv,
			_Xprocessor_set_create,
			_Xprocessor_set_destroy,
			_Xprocessor_set_info,
			_Xprocessor_assign,
			_Xprocessor_get_assignment,
			_Xthread_assign,
			_Xthread_assign_default,
			_Xthread_get_assignment,
			_Xtask_assign,
			_Xtask_assign_default,
			_Xtask_get_assignment,
			_Xhost_kernel_version,
			_Xthread_priority,
			_Xthread_max_priority,
			_Xtask_priority,
			_Xprocessor_set_max_priority,
			_Xthread_policy,
			_Xprocessor_set_policy_enable,
			_Xprocessor_set_policy_disable,
			_Xprocessor_set_tasks,
			_Xprocessor_set_threads,
			_Xhost_processor_sets,
			_Xhost_processor_set_priv,
			0,
			_Xhost_set_time,
			_Xhost_adjust_time,
			_Xhost_get_time,
			_Xhost_reboot,
		};

		if (routines[InP->msgh_id - 2600])
			(routines[InP->msgh_id - 2600]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
