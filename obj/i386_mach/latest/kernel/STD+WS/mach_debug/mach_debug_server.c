/* Module mach_debug */

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
#include <mach_debug/mach_debug_types.h>

/* Routine host_zone_info */
mig_internal novalue _Xhost_zone_info
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
		mach_msg_type_long_t namesType;
		zone_name_array_t names;
		mach_msg_type_long_t infoType;
		zone_info_array_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_zone_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, zone_name_array_t *names, mach_msg_type_number_t *namesCnt, zone_info_array_t *info, mach_msg_type_number_t *infoCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t namesType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t namesCnt;
	mach_msg_type_number_t infoCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_zone_info(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->names, &namesCnt, &OutP->info, &infoCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 64;

#if	UseStaticMsgType
	OutP->namesType = namesType;
#else	UseStaticMsgType
	OutP->namesType.msgtl_name = 8;
	OutP->namesType.msgtl_size = 8;
	OutP->namesType.msgtl_header.msgt_name = 0;
	OutP->namesType.msgtl_header.msgt_size = 0;
	OutP->namesType.msgtl_header.msgt_number = 0;
	OutP->namesType.msgtl_header.msgt_inline = FALSE;
	OutP->namesType.msgtl_header.msgt_longform = TRUE;
	OutP->namesType.msgtl_header.msgt_deallocate = FALSE;
	OutP->namesType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->namesType.msgtl_number /* 80 namesCnt */ = /* namesType.msgtl_number */ 80 * namesCnt;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgtl_name = 2;
	OutP->infoType.msgtl_size = 32;
	OutP->infoType.msgtl_header.msgt_name = 0;
	OutP->infoType.msgtl_header.msgt_size = 0;
	OutP->infoType.msgtl_header.msgt_number = 0;
	OutP->infoType.msgtl_header.msgt_inline = FALSE;
	OutP->infoType.msgtl_header.msgt_longform = TRUE;
	OutP->infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->infoType.msgtl_number /* 9 infoCnt */ = /* infoType.msgtl_number */ 9 * infoCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_get_srights */
mig_internal novalue _Xmach_port_get_srights
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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
		mach_msg_type_t srightsType;
		mach_port_rights_t srights;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_get_srights
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_rights_t *srights);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

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
	static mach_msg_type_t srightsType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->nameType != * (int *) &nameCheck)
#else	UseStaticMsgType
	if ((In0P->nameType.msgt_inline != TRUE) ||
	    (In0P->nameType.msgt_longform != FALSE) ||
	    (In0P->nameType.msgt_name != 15) ||
	    (In0P->nameType.msgt_number != 1) ||
	    (In0P->nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_get_srights(task, In0P->name, &OutP->srights);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->srightsType = srightsType;
#else	UseStaticMsgType
	OutP->srightsType.msgt_name = 2;
	OutP->srightsType.msgt_size = 32;
	OutP->srightsType.msgt_number = 1;
	OutP->srightsType.msgt_inline = TRUE;
	OutP->srightsType.msgt_longform = FALSE;
	OutP->srightsType.msgt_deallocate = FALSE;
	OutP->srightsType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_ipc_hash_info */
mig_internal novalue _Xhost_ipc_hash_info
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
		mach_msg_type_long_t infoType;
		hash_info_bucket_array_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_ipc_hash_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, hash_info_bucket_array_t *info, mach_msg_type_number_t *infoCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t infoCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_ipc_hash_info(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->info, &infoCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgtl_name = 2;
	OutP->infoType.msgtl_size = 32;
	OutP->infoType.msgtl_header.msgt_name = 0;
	OutP->infoType.msgtl_header.msgt_size = 0;
	OutP->infoType.msgtl_header.msgt_number = 0;
	OutP->infoType.msgtl_header.msgt_inline = FALSE;
	OutP->infoType.msgtl_header.msgt_longform = TRUE;
	OutP->infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->infoType.msgtl_number /* infoCnt */ = /* infoType.msgtl_number */ infoCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_ipc_marequest_info */
mig_internal novalue _Xhost_ipc_marequest_info
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
		mach_msg_type_t max_requestsType;
		unsigned max_requests;
		mach_msg_type_long_t infoType;
		hash_info_bucket_array_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_ipc_marequest_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, unsigned *max_requests, hash_info_bucket_array_t *info, mach_msg_type_number_t *infoCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t max_requestsType = {
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
	static mach_msg_type_long_t infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t infoCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_ipc_marequest_info(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->max_requests, &OutP->info, &infoCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 56;

#if	UseStaticMsgType
	OutP->max_requestsType = max_requestsType;
#else	UseStaticMsgType
	OutP->max_requestsType.msgt_name = 2;
	OutP->max_requestsType.msgt_size = 32;
	OutP->max_requestsType.msgt_number = 1;
	OutP->max_requestsType.msgt_inline = TRUE;
	OutP->max_requestsType.msgt_longform = FALSE;
	OutP->max_requestsType.msgt_deallocate = FALSE;
	OutP->max_requestsType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgtl_name = 2;
	OutP->infoType.msgtl_size = 32;
	OutP->infoType.msgtl_header.msgt_name = 0;
	OutP->infoType.msgtl_header.msgt_size = 0;
	OutP->infoType.msgtl_header.msgt_number = 0;
	OutP->infoType.msgtl_header.msgt_inline = FALSE;
	OutP->infoType.msgtl_header.msgt_longform = TRUE;
	OutP->infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->infoType.msgtl_number /* infoCnt */ = /* infoType.msgtl_number */ infoCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_space_info */
mig_internal novalue _Xmach_port_space_info
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
		mach_msg_type_t infoType;
		ipc_info_space_t info;
		mach_msg_type_long_t table_infoType;
		ipc_info_name_array_t table_info;
		mach_msg_type_long_t tree_infoType;
		ipc_info_tree_name_array_t tree_info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_space_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, ipc_info_space_t *info, ipc_info_name_array_t *table_info, mach_msg_type_number_t *table_infoCnt, ipc_info_tree_name_array_t *tree_info, mach_msg_type_number_t *tree_infoCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t infoType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		6,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t table_infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t tree_infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	ipc_space_t task;
	mach_msg_type_number_t table_infoCnt;
	mach_msg_type_number_t tree_infoCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_space_info(task, &OutP->info, &OutP->table_info, &table_infoCnt, &OutP->tree_info, &tree_infoCnt);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 92;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgt_name = 2;
	OutP->infoType.msgt_size = 32;
	OutP->infoType.msgt_number = 6;
	OutP->infoType.msgt_inline = TRUE;
	OutP->infoType.msgt_longform = FALSE;
	OutP->infoType.msgt_deallocate = FALSE;
	OutP->infoType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->table_infoType = table_infoType;
#else	UseStaticMsgType
	OutP->table_infoType.msgtl_name = 2;
	OutP->table_infoType.msgtl_size = 32;
	OutP->table_infoType.msgtl_header.msgt_name = 0;
	OutP->table_infoType.msgtl_header.msgt_size = 0;
	OutP->table_infoType.msgtl_header.msgt_number = 0;
	OutP->table_infoType.msgtl_header.msgt_inline = FALSE;
	OutP->table_infoType.msgtl_header.msgt_longform = TRUE;
	OutP->table_infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->table_infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->table_infoType.msgtl_number /* 9 table_infoCnt */ = /* table_infoType.msgtl_number */ 9 * table_infoCnt;

#if	UseStaticMsgType
	OutP->tree_infoType = tree_infoType;
#else	UseStaticMsgType
	OutP->tree_infoType.msgtl_name = 2;
	OutP->tree_infoType.msgtl_size = 32;
	OutP->tree_infoType.msgtl_header.msgt_name = 0;
	OutP->tree_infoType.msgtl_header.msgt_size = 0;
	OutP->tree_infoType.msgtl_header.msgt_number = 0;
	OutP->tree_infoType.msgtl_header.msgt_inline = FALSE;
	OutP->tree_infoType.msgtl_header.msgt_longform = TRUE;
	OutP->tree_infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->tree_infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->tree_infoType.msgtl_number /* 11 tree_infoCnt */ = /* tree_infoType.msgtl_number */ 11 * tree_infoCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_dnrequest_info */
mig_internal novalue _Xmach_port_dnrequest_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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
		mach_msg_type_t totalType;
		unsigned total;
		mach_msg_type_t usedType;
		unsigned used;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_dnrequest_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, unsigned *total, unsigned *used);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

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
	static mach_msg_type_t totalType = {
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
	static mach_msg_type_t usedType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->nameType != * (int *) &nameCheck)
#else	UseStaticMsgType
	if ((In0P->nameType.msgt_inline != TRUE) ||
	    (In0P->nameType.msgt_longform != FALSE) ||
	    (In0P->nameType.msgt_name != 15) ||
	    (In0P->nameType.msgt_number != 1) ||
	    (In0P->nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_dnrequest_info(task, In0P->name, &OutP->total, &OutP->used);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->totalType = totalType;
#else	UseStaticMsgType
	OutP->totalType.msgt_name = 2;
	OutP->totalType.msgt_size = 32;
	OutP->totalType.msgt_number = 1;
	OutP->totalType.msgt_inline = TRUE;
	OutP->totalType.msgt_longform = FALSE;
	OutP->totalType.msgt_deallocate = FALSE;
	OutP->totalType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->usedType = usedType;
#else	UseStaticMsgType
	OutP->usedType.msgt_name = 2;
	OutP->usedType.msgt_size = 32;
	OutP->usedType.msgt_number = 1;
	OutP->usedType.msgt_inline = TRUE;
	OutP->usedType.msgt_longform = FALSE;
	OutP->usedType.msgt_deallocate = FALSE;
	OutP->usedType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_vm_region_info */
mig_internal novalue _Xmach_vm_region_info
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
#endif
{
	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t addressType;
		vm_address_t address;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
		mach_msg_type_t regionType;
		vm_info_region_t region;
		mach_msg_type_long_t objectsType;
		vm_info_object_array_t objects;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_vm_region_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(vm_map_t task, vm_address_t address, vm_info_region_t *region, vm_info_object_array_t *objects, mach_msg_type_number_t *objectsCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t addressCheck = {
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
	static mach_msg_type_t regionType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		10,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t objectsType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	vm_map_t task;
	mach_msg_type_number_t objectsCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->addressType != * (int *) &addressCheck)
#else	UseStaticMsgType
	if ((In0P->addressType.msgt_inline != TRUE) ||
	    (In0P->addressType.msgt_longform != FALSE) ||
	    (In0P->addressType.msgt_name != 2) ||
	    (In0P->addressType.msgt_number != 1) ||
	    (In0P->addressType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_map 0 Head.msgh_request_port */ = /* task */ convert_port_to_map(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_vm_region_info(task, In0P->address, &OutP->region, &OutP->objects, &objectsCnt);
	vm_map_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 92;

#if	UseStaticMsgType
	OutP->regionType = regionType;
#else	UseStaticMsgType
	OutP->regionType.msgt_name = 2;
	OutP->regionType.msgt_size = 32;
	OutP->regionType.msgt_number = 10;
	OutP->regionType.msgt_inline = TRUE;
	OutP->regionType.msgt_longform = FALSE;
	OutP->regionType.msgt_deallocate = FALSE;
	OutP->regionType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->objectsType = objectsType;
#else	UseStaticMsgType
	OutP->objectsType.msgtl_name = 2;
	OutP->objectsType.msgtl_size = 32;
	OutP->objectsType.msgtl_header.msgt_name = 0;
	OutP->objectsType.msgtl_header.msgt_size = 0;
	OutP->objectsType.msgtl_header.msgt_number = 0;
	OutP->objectsType.msgtl_header.msgt_inline = FALSE;
	OutP->objectsType.msgtl_header.msgt_longform = TRUE;
	OutP->objectsType.msgtl_header.msgt_deallocate = FALSE;
	OutP->objectsType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->objectsType.msgtl_number /* 22 objectsCnt */ = /* objectsType.msgtl_number */ 22 * objectsCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine vm_mapped_pages_info */
mig_internal novalue _Xvm_mapped_pages_info
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
		mach_msg_type_long_t pagesType;
		page_address_array_t pages;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t vm_mapped_pages_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(task_t task, page_address_array_t *pages, mach_msg_type_number_t *pagesCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t pagesType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	task_t task;
	mach_msg_type_number_t pagesCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_task 0 Head.msgh_request_port */ = /* task */ convert_port_to_task(In0P->Head.msgh_request_port);

	OutP->RetCode = vm_mapped_pages_info(task, &OutP->pages, &pagesCnt);
	task_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->pagesType = pagesType;
#else	UseStaticMsgType
	OutP->pagesType.msgtl_name = 2;
	OutP->pagesType.msgtl_size = 32;
	OutP->pagesType.msgtl_header.msgt_name = 0;
	OutP->pagesType.msgtl_header.msgt_size = 0;
	OutP->pagesType.msgtl_header.msgt_number = 0;
	OutP->pagesType.msgtl_header.msgt_inline = FALSE;
	OutP->pagesType.msgtl_header.msgt_longform = TRUE;
	OutP->pagesType.msgtl_header.msgt_deallocate = FALSE;
	OutP->pagesType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->pagesType.msgtl_number /* pagesCnt */ = /* pagesType.msgtl_number */ pagesCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_stack_usage */
mig_internal novalue _Xhost_stack_usage
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
		mach_msg_type_t reservedType;
		vm_size_t reserved;
		mach_msg_type_t totalType;
		unsigned total;
		mach_msg_type_t spaceType;
		vm_size_t space;
		mach_msg_type_t residentType;
		vm_size_t resident;
		mach_msg_type_t maxusageType;
		vm_size_t maxusage;
		mach_msg_type_t maxstackType;
		vm_offset_t maxstack;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_stack_usage
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, vm_size_t *reserved, unsigned *total, vm_size_t *space, vm_size_t *resident, vm_size_t *maxusage, vm_offset_t *maxstack);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t reservedType = {
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
	static mach_msg_type_t totalType = {
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
	static mach_msg_type_t spaceType = {
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
	static mach_msg_type_t residentType = {
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
	static mach_msg_type_t maxusageType = {
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
	static mach_msg_type_t maxstackType = {
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
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_stack_usage(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->reserved, &OutP->total, &OutP->space, &OutP->resident, &OutP->maxusage, &OutP->maxstack);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 80;

#if	UseStaticMsgType
	OutP->reservedType = reservedType;
#else	UseStaticMsgType
	OutP->reservedType.msgt_name = 2;
	OutP->reservedType.msgt_size = 32;
	OutP->reservedType.msgt_number = 1;
	OutP->reservedType.msgt_inline = TRUE;
	OutP->reservedType.msgt_longform = FALSE;
	OutP->reservedType.msgt_deallocate = FALSE;
	OutP->reservedType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->totalType = totalType;
#else	UseStaticMsgType
	OutP->totalType.msgt_name = 2;
	OutP->totalType.msgt_size = 32;
	OutP->totalType.msgt_number = 1;
	OutP->totalType.msgt_inline = TRUE;
	OutP->totalType.msgt_longform = FALSE;
	OutP->totalType.msgt_deallocate = FALSE;
	OutP->totalType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->spaceType = spaceType;
#else	UseStaticMsgType
	OutP->spaceType.msgt_name = 2;
	OutP->spaceType.msgt_size = 32;
	OutP->spaceType.msgt_number = 1;
	OutP->spaceType.msgt_inline = TRUE;
	OutP->spaceType.msgt_longform = FALSE;
	OutP->spaceType.msgt_deallocate = FALSE;
	OutP->spaceType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->residentType = residentType;
#else	UseStaticMsgType
	OutP->residentType.msgt_name = 2;
	OutP->residentType.msgt_size = 32;
	OutP->residentType.msgt_number = 1;
	OutP->residentType.msgt_inline = TRUE;
	OutP->residentType.msgt_longform = FALSE;
	OutP->residentType.msgt_deallocate = FALSE;
	OutP->residentType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->maxusageType = maxusageType;
#else	UseStaticMsgType
	OutP->maxusageType.msgt_name = 2;
	OutP->maxusageType.msgt_size = 32;
	OutP->maxusageType.msgt_number = 1;
	OutP->maxusageType.msgt_inline = TRUE;
	OutP->maxusageType.msgt_longform = FALSE;
	OutP->maxusageType.msgt_deallocate = FALSE;
	OutP->maxusageType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->maxstackType = maxstackType;
#else	UseStaticMsgType
	OutP->maxstackType.msgt_name = 2;
	OutP->maxstackType.msgt_size = 32;
	OutP->maxstackType.msgt_number = 1;
	OutP->maxstackType.msgt_inline = TRUE;
	OutP->maxstackType.msgt_longform = FALSE;
	OutP->maxstackType.msgt_deallocate = FALSE;
	OutP->maxstackType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine processor_set_stack_usage */
mig_internal novalue _Xprocessor_set_stack_usage
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
		mach_msg_type_t totalType;
		unsigned total;
		mach_msg_type_t spaceType;
		vm_size_t space;
		mach_msg_type_t residentType;
		vm_size_t resident;
		mach_msg_type_t maxusageType;
		vm_size_t maxusage;
		mach_msg_type_t maxstackType;
		vm_offset_t maxstack;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t processor_set_stack_usage
#if	(defined(__STDC__) || defined(c_plusplus))
		(processor_set_t pset, unsigned *total, vm_size_t *space, vm_size_t *resident, vm_size_t *maxusage, vm_offset_t *maxstack);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t totalType = {
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
	static mach_msg_type_t spaceType = {
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
	static mach_msg_type_t residentType = {
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
	static mach_msg_type_t maxusageType = {
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
	static mach_msg_type_t maxstackType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	};
#endif	UseStaticMsgType

	processor_set_t pset;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	pset /* convert_port_to_pset_name 0 Head.msgh_request_port */ = /* pset */ convert_port_to_pset_name(In0P->Head.msgh_request_port);

	OutP->RetCode = processor_set_stack_usage(pset, &OutP->total, &OutP->space, &OutP->resident, &OutP->maxusage, &OutP->maxstack);
	pset_deallocate(pset);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 72;

#if	UseStaticMsgType
	OutP->totalType = totalType;
#else	UseStaticMsgType
	OutP->totalType.msgt_name = 2;
	OutP->totalType.msgt_size = 32;
	OutP->totalType.msgt_number = 1;
	OutP->totalType.msgt_inline = TRUE;
	OutP->totalType.msgt_longform = FALSE;
	OutP->totalType.msgt_deallocate = FALSE;
	OutP->totalType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->spaceType = spaceType;
#else	UseStaticMsgType
	OutP->spaceType.msgt_name = 2;
	OutP->spaceType.msgt_size = 32;
	OutP->spaceType.msgt_number = 1;
	OutP->spaceType.msgt_inline = TRUE;
	OutP->spaceType.msgt_longform = FALSE;
	OutP->spaceType.msgt_deallocate = FALSE;
	OutP->spaceType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->residentType = residentType;
#else	UseStaticMsgType
	OutP->residentType.msgt_name = 2;
	OutP->residentType.msgt_size = 32;
	OutP->residentType.msgt_number = 1;
	OutP->residentType.msgt_inline = TRUE;
	OutP->residentType.msgt_longform = FALSE;
	OutP->residentType.msgt_deallocate = FALSE;
	OutP->residentType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->maxusageType = maxusageType;
#else	UseStaticMsgType
	OutP->maxusageType.msgt_name = 2;
	OutP->maxusageType.msgt_size = 32;
	OutP->maxusageType.msgt_number = 1;
	OutP->maxusageType.msgt_inline = TRUE;
	OutP->maxusageType.msgt_longform = FALSE;
	OutP->maxusageType.msgt_deallocate = FALSE;
	OutP->maxusageType.msgt_unused = 0;
#endif	UseStaticMsgType

#if	UseStaticMsgType
	OutP->maxstackType = maxstackType;
#else	UseStaticMsgType
	OutP->maxstackType.msgt_name = 2;
	OutP->maxstackType.msgt_size = 32;
	OutP->maxstackType.msgt_number = 1;
	OutP->maxstackType.msgt_inline = TRUE;
	OutP->maxstackType.msgt_longform = FALSE;
	OutP->maxstackType.msgt_deallocate = FALSE;
	OutP->maxstackType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine host_virtual_physical_table_info */
mig_internal novalue _Xhost_virtual_physical_table_info
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
		mach_msg_type_long_t infoType;
		hash_info_bucket_array_t info;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t host_virtual_physical_table_info
#if	(defined(__STDC__) || defined(c_plusplus))
		(host_t host, hash_info_bucket_array_t *info, mach_msg_type_number_t *infoCnt);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_long_t infoType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	2,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	mach_msg_type_number_t infoCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	OutP->RetCode = host_virtual_physical_table_info(convert_port_to_host(In0P->Head.msgh_request_port), &OutP->info, &infoCnt);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->infoType = infoType;
#else	UseStaticMsgType
	OutP->infoType.msgtl_name = 2;
	OutP->infoType.msgtl_size = 32;
	OutP->infoType.msgtl_header.msgt_name = 0;
	OutP->infoType.msgtl_header.msgt_size = 0;
	OutP->infoType.msgtl_header.msgt_number = 0;
	OutP->infoType.msgtl_header.msgt_inline = FALSE;
	OutP->infoType.msgtl_header.msgt_longform = TRUE;
	OutP->infoType.msgtl_header.msgt_deallocate = FALSE;
	OutP->infoType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->infoType.msgtl_number /* infoCnt */ = /* infoType.msgtl_number */ infoCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

mig_external boolean_t mach_debug_server
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

	if ((InP->msgh_id > 3016) || (InP->msgh_id < 3000))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			0,
			0,
			0,
			0,
			0,
			_Xhost_zone_info,
			0,
			_Xmach_port_get_srights,
			_Xhost_ipc_hash_info,
			_Xhost_ipc_marequest_info,
			_Xmach_port_space_info,
			_Xmach_port_dnrequest_info,
			_Xmach_vm_region_info,
			_Xvm_mapped_pages_info,
			_Xhost_stack_usage,
			_Xprocessor_set_stack_usage,
			_Xhost_virtual_physical_table_info,
		};

		if (routines[InP->msgh_id - 3000])
			(routines[InP->msgh_id - 3000]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
