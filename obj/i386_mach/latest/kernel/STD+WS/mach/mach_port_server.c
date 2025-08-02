/* Module mach_port */

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

/* Routine mach_port_names */
mig_internal novalue _Xmach_port_names
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
		mach_port_array_t names;
		mach_msg_type_long_t typesType;
		mach_port_type_array_t types;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_names
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_array_t *names, mach_msg_type_number_t *namesCnt, mach_port_type_array_t *types, mach_msg_type_number_t *typesCnt);
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
		/* msgtl_name = */	15,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

#if	UseStaticMsgType
	static mach_msg_type_long_t typesType = {
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
	mach_msg_type_number_t namesCnt;
	mach_msg_type_number_t typesCnt;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 24) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_names(task, &OutP->names, &namesCnt, &OutP->types, &typesCnt);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 64;

#if	UseStaticMsgType
	OutP->namesType = namesType;
#else	UseStaticMsgType
	OutP->namesType.msgtl_name = 15;
	OutP->namesType.msgtl_size = 32;
	OutP->namesType.msgtl_header.msgt_name = 0;
	OutP->namesType.msgtl_header.msgt_size = 0;
	OutP->namesType.msgtl_header.msgt_number = 0;
	OutP->namesType.msgtl_header.msgt_inline = FALSE;
	OutP->namesType.msgtl_header.msgt_longform = TRUE;
	OutP->namesType.msgtl_header.msgt_deallocate = FALSE;
	OutP->namesType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->namesType.msgtl_number /* namesCnt */ = /* namesType.msgtl_number */ namesCnt;

#if	UseStaticMsgType
	OutP->typesType = typesType;
#else	UseStaticMsgType
	OutP->typesType.msgtl_name = 2;
	OutP->typesType.msgtl_size = 32;
	OutP->typesType.msgtl_header.msgt_name = 0;
	OutP->typesType.msgtl_header.msgt_size = 0;
	OutP->typesType.msgtl_header.msgt_number = 0;
	OutP->typesType.msgtl_header.msgt_inline = FALSE;
	OutP->typesType.msgtl_header.msgt_longform = TRUE;
	OutP->typesType.msgtl_header.msgt_deallocate = FALSE;
	OutP->typesType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->typesType.msgtl_number /* typesCnt */ = /* typesType.msgtl_number */ typesCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_type */
mig_internal novalue _Xmach_port_type
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
		mach_msg_type_t ptypeType;
		mach_port_type_t ptype;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_type
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_type_t *ptype);
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
	static mach_msg_type_t ptypeType = {
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

	OutP->RetCode = mach_port_type(task, In0P->name, &OutP->ptype);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->ptypeType = ptypeType;
#else	UseStaticMsgType
	OutP->ptypeType.msgt_name = 2;
	OutP->ptypeType.msgt_size = 32;
	OutP->ptypeType.msgt_number = 1;
	OutP->ptypeType.msgt_inline = TRUE;
	OutP->ptypeType.msgt_longform = FALSE;
	OutP->ptypeType.msgt_deallocate = FALSE;
	OutP->ptypeType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_rename */
mig_internal novalue _Xmach_port_rename
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_rename
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t old_name, mach_port_t new_name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t old_nameCheck = {
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
	static mach_msg_type_t new_nameCheck = {
		/* msgt_name = */		15,
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
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->old_nameType != * (int *) &old_nameCheck)
#else	UseStaticMsgType
	if ((In0P->old_nameType.msgt_inline != TRUE) ||
	    (In0P->old_nameType.msgt_longform != FALSE) ||
	    (In0P->old_nameType.msgt_name != 15) ||
	    (In0P->old_nameType.msgt_number != 1) ||
	    (In0P->old_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->new_nameType != * (int *) &new_nameCheck)
#else	UseStaticMsgType
	if ((In0P->new_nameType.msgt_inline != TRUE) ||
	    (In0P->new_nameType.msgt_longform != FALSE) ||
	    (In0P->new_nameType.msgt_name != 15) ||
	    (In0P->new_nameType.msgt_number != 1) ||
	    (In0P->new_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_rename(task, In0P->old_name, In0P->new_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_allocate_name */
mig_internal novalue _Xmach_port_allocate_name
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_allocate_name
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_right_t right, mach_port_t name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t rightCheck = {
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

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->rightType != * (int *) &rightCheck)
#else	UseStaticMsgType
	if ((In0P->rightType.msgt_inline != TRUE) ||
	    (In0P->rightType.msgt_longform != FALSE) ||
	    (In0P->rightType.msgt_name != 2) ||
	    (In0P->rightType.msgt_number != 1) ||
	    (In0P->rightType.msgt_size != 32))
#endif	UseStaticMsgType
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

	OutP->RetCode = mach_port_allocate_name(task, In0P->right, In0P->name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_allocate */
mig_internal novalue _Xmach_port_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_allocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_right_t right, mach_port_t *name);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t rightCheck = {
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

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 32) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->rightType != * (int *) &rightCheck)
#else	UseStaticMsgType
	if ((In0P->rightType.msgt_inline != TRUE) ||
	    (In0P->rightType.msgt_longform != FALSE) ||
	    (In0P->rightType.msgt_name != 2) ||
	    (In0P->rightType.msgt_number != 1) ||
	    (In0P->rightType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_allocate(task, In0P->right, &OutP->name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->nameType = nameType;
#else	UseStaticMsgType
	OutP->nameType.msgt_name = 15;
	OutP->nameType.msgt_size = 32;
	OutP->nameType.msgt_number = 1;
	OutP->nameType.msgt_inline = TRUE;
	OutP->nameType.msgt_longform = FALSE;
	OutP->nameType.msgt_deallocate = FALSE;
	OutP->nameType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_destroy */
mig_internal novalue _Xmach_port_destroy
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
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_destroy
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name);
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

	OutP->RetCode = mach_port_destroy(task, In0P->name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_deallocate */
mig_internal novalue _Xmach_port_deallocate
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
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_deallocate
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name);
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

	OutP->RetCode = mach_port_deallocate(task, In0P->name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_get_refs */
mig_internal novalue _Xmach_port_get_refs
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_get_refs
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_right_t right, mach_port_urefs_t *refs);
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
	static mach_msg_type_t rightCheck = {
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
	static mach_msg_type_t refsType = {
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
	if ((msgh_size != 40) || !msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->rightType != * (int *) &rightCheck)
#else	UseStaticMsgType
	if ((In0P->rightType.msgt_inline != TRUE) ||
	    (In0P->rightType.msgt_longform != FALSE) ||
	    (In0P->rightType.msgt_name != 2) ||
	    (In0P->rightType.msgt_number != 1) ||
	    (In0P->rightType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_get_refs(task, In0P->name, In0P->right, &OutP->refs);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->refsType = refsType;
#else	UseStaticMsgType
	OutP->refsType.msgt_name = 2;
	OutP->refsType.msgt_size = 32;
	OutP->refsType.msgt_number = 1;
	OutP->refsType.msgt_inline = TRUE;
	OutP->refsType.msgt_longform = FALSE;
	OutP->refsType.msgt_deallocate = FALSE;
	OutP->refsType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_mod_refs */
mig_internal novalue _Xmach_port_mod_refs
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_mod_refs
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_right_t right, mach_port_delta_t delta);
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
	static mach_msg_type_t rightCheck = {
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
	static mach_msg_type_t deltaCheck = {
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
	if ((msgh_size != 48) || !msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->rightType != * (int *) &rightCheck)
#else	UseStaticMsgType
	if ((In0P->rightType.msgt_inline != TRUE) ||
	    (In0P->rightType.msgt_longform != FALSE) ||
	    (In0P->rightType.msgt_name != 2) ||
	    (In0P->rightType.msgt_number != 1) ||
	    (In0P->rightType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->deltaType != * (int *) &deltaCheck)
#else	UseStaticMsgType
	if ((In0P->deltaType.msgt_inline != TRUE) ||
	    (In0P->deltaType.msgt_longform != FALSE) ||
	    (In0P->deltaType.msgt_name != 2) ||
	    (In0P->deltaType.msgt_number != 1) ||
	    (In0P->deltaType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_mod_refs(task, In0P->name, In0P->right, In0P->delta);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_get_receive_status */
mig_internal novalue _Xmach_port_get_receive_status
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
		mach_msg_type_t statusType;
		mach_port_status_t status;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_get_receive_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_status_t *status);
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
	static mach_msg_type_t statusType = {
		/* msgt_name = */		2,
		/* msgt_size = */		32,
		/* msgt_number = */		8,
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

	OutP->RetCode = mach_port_get_receive_status(task, In0P->name, &OutP->status);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 68;

#if	UseStaticMsgType
	OutP->statusType = statusType;
#else	UseStaticMsgType
	OutP->statusType.msgt_name = 2;
	OutP->statusType.msgt_size = 32;
	OutP->statusType.msgt_number = 8;
	OutP->statusType.msgt_inline = TRUE;
	OutP->statusType.msgt_longform = FALSE;
	OutP->statusType.msgt_deallocate = FALSE;
	OutP->statusType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_set_qlimit */
mig_internal novalue _Xmach_port_set_qlimit
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
		mach_msg_type_t qlimitType;
		mach_port_msgcount_t qlimit;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_set_qlimit
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_msgcount_t qlimit);
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
	static mach_msg_type_t qlimitCheck = {
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
	if ((msgh_size != 40) || !msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->qlimitType != * (int *) &qlimitCheck)
#else	UseStaticMsgType
	if ((In0P->qlimitType.msgt_inline != TRUE) ||
	    (In0P->qlimitType.msgt_longform != FALSE) ||
	    (In0P->qlimitType.msgt_name != 2) ||
	    (In0P->qlimitType.msgt_number != 1) ||
	    (In0P->qlimitType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_set_qlimit(task, In0P->name, In0P->qlimit);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_set_mscount */
mig_internal novalue _Xmach_port_set_mscount
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
		mach_msg_type_t mscountType;
		mach_port_mscount_t mscount;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_set_mscount
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_mscount_t mscount);
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
	static mach_msg_type_t mscountCheck = {
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
	if ((msgh_size != 40) || !msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->mscountType != * (int *) &mscountCheck)
#else	UseStaticMsgType
	if ((In0P->mscountType.msgt_inline != TRUE) ||
	    (In0P->mscountType.msgt_longform != FALSE) ||
	    (In0P->mscountType.msgt_name != 2) ||
	    (In0P->mscountType.msgt_number != 1) ||
	    (In0P->mscountType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_set_mscount(task, In0P->name, In0P->mscount);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_get_set_status */
mig_internal novalue _Xmach_port_get_set_status
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
		mach_msg_type_long_t membersType;
		mach_port_array_t members;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_get_set_status
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_array_t *members, mach_msg_type_number_t *membersCnt);
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
	static mach_msg_type_long_t membersType = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	15,
		/* msgtl_size = */	32,
		/* msgtl_number = */	0,
	};
#endif	UseStaticMsgType

	ipc_space_t task;
	mach_msg_type_number_t membersCnt;

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

	OutP->RetCode = mach_port_get_set_status(task, In0P->name, &OutP->members, &membersCnt);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 48;

#if	UseStaticMsgType
	OutP->membersType = membersType;
#else	UseStaticMsgType
	OutP->membersType.msgtl_name = 15;
	OutP->membersType.msgtl_size = 32;
	OutP->membersType.msgtl_header.msgt_name = 0;
	OutP->membersType.msgtl_header.msgt_size = 0;
	OutP->membersType.msgtl_header.msgt_number = 0;
	OutP->membersType.msgtl_header.msgt_inline = FALSE;
	OutP->membersType.msgtl_header.msgt_longform = TRUE;
	OutP->membersType.msgtl_header.msgt_deallocate = FALSE;
	OutP->membersType.msgtl_header.msgt_unused = 0;
#endif	UseStaticMsgType

	OutP->membersType.msgtl_number /* membersCnt */ = /* membersType.msgtl_number */ membersCnt;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_move_member */
mig_internal novalue _Xmach_port_move_member
#if	(defined(__STDC__) || defined(c_plusplus))
	(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)
#else
	(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_move_member
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t member, mach_port_t after);
#else
		();
#endif

	boolean_t msgh_simple;

	unsigned int msgh_size;

#if	UseStaticMsgType
	static mach_msg_type_t memberCheck = {
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
	static mach_msg_type_t afterCheck = {
		/* msgt_name = */		15,
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
	if ((msgh_size != 40) || !msgh_simple)
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->memberType != * (int *) &memberCheck)
#else	UseStaticMsgType
	if ((In0P->memberType.msgt_inline != TRUE) ||
	    (In0P->memberType.msgt_longform != FALSE) ||
	    (In0P->memberType.msgt_name != 15) ||
	    (In0P->memberType.msgt_number != 1) ||
	    (In0P->memberType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->afterType != * (int *) &afterCheck)
#else	UseStaticMsgType
	if ((In0P->afterType.msgt_inline != TRUE) ||
	    (In0P->afterType.msgt_longform != FALSE) ||
	    (In0P->afterType.msgt_name != 15) ||
	    (In0P->afterType.msgt_number != 1) ||
	    (In0P->afterType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_move_member(task, In0P->member, In0P->after);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_request_notification */
mig_internal novalue _Xmach_port_request_notification
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_request_notification
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_msg_id_t id, mach_port_mscount_t sync, mach_port_t notify, mach_port_t *previous);
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
	static mach_msg_type_t idCheck = {
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
	static mach_msg_type_t syncCheck = {
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
	static mach_msg_type_t previousType = {
		/* msgt_name = */		MACH_MSG_TYPE_PORT_SEND_ONCE,
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
	if ((msgh_size != 56) || msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->idType != * (int *) &idCheck)
#else	UseStaticMsgType
	if ((In0P->idType.msgt_inline != TRUE) ||
	    (In0P->idType.msgt_longform != FALSE) ||
	    (In0P->idType.msgt_name != 2) ||
	    (In0P->idType.msgt_number != 1) ||
	    (In0P->idType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->syncType != * (int *) &syncCheck)
#else	UseStaticMsgType
	if ((In0P->syncType.msgt_inline != TRUE) ||
	    (In0P->syncType.msgt_longform != FALSE) ||
	    (In0P->syncType.msgt_name != 2) ||
	    (In0P->syncType.msgt_number != 1) ||
	    (In0P->syncType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

#if	TypeCheck
	if ((In0P->notifyType.msgt_inline != TRUE) ||
	    (In0P->notifyType.msgt_longform != FALSE) ||
	    (In0P->notifyType.msgt_name != MACH_MSG_TYPE_PORT_SEND_ONCE) ||
	    (In0P->notifyType.msgt_number != 1) ||
	    (In0P->notifyType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_request_notification(task, In0P->name, In0P->id, In0P->sync, In0P->notify, &OutP->previous);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = FALSE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->previousType = previousType;
#else	UseStaticMsgType
	OutP->previousType.msgt_name = MACH_MSG_TYPE_PORT_SEND_ONCE;
	OutP->previousType.msgt_size = 32;
	OutP->previousType.msgt_number = 1;
	OutP->previousType.msgt_inline = TRUE;
	OutP->previousType.msgt_longform = FALSE;
	OutP->previousType.msgt_deallocate = FALSE;
	OutP->previousType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_insert_right */
mig_internal novalue _Xmach_port_insert_right
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
		mach_msg_type_t polyType;
		mach_port_t poly;
	} Request;

	typedef struct {
		mach_msg_header_t Head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_insert_right
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_port_t poly, mach_msg_type_name_t polyPoly);
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

	ipc_space_t task;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40))
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

#if	TypeCheck
	if ((In0P->polyType.msgt_inline != TRUE) ||
	    (In0P->polyType.msgt_longform != FALSE) ||
	    (MACH_MSG_TYPE_PORT_ANY(In0P->polyType.msgt_name) && msgh_simple) ||
	    (In0P->polyType.msgt_number != 1) ||
	    (In0P->polyType.msgt_size != 32))
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_insert_right(task, In0P->name, In0P->poly, In0P->polyType.msgt_name);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 32;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

/* Routine mach_port_extract_right */
mig_internal novalue _Xmach_port_extract_right
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

	register Request *In0P = (Request *) InHeadP;
	register Reply *OutP = (Reply *) OutHeadP;
	mig_external kern_return_t mach_port_extract_right
#if	(defined(__STDC__) || defined(c_plusplus))
		(ipc_space_t task, mach_port_t name, mach_msg_type_name_t msgt_name, mach_port_t *poly, mach_msg_type_name_t *polyPoly);
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
	static mach_msg_type_t msgt_nameCheck = {
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

	ipc_space_t task;
	mach_msg_type_name_t polyPoly;

#if	TypeCheck
	msgh_size = In0P->Head.msgh_size;
	msgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);
	if ((msgh_size != 40) || !msgh_simple)
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

#if	TypeCheck
#if	UseStaticMsgType
	if (* (int *) &In0P->msgt_nameType != * (int *) &msgt_nameCheck)
#else	UseStaticMsgType
	if ((In0P->msgt_nameType.msgt_inline != TRUE) ||
	    (In0P->msgt_nameType.msgt_longform != FALSE) ||
	    (In0P->msgt_nameType.msgt_name != 2) ||
	    (In0P->msgt_nameType.msgt_number != 1) ||
	    (In0P->msgt_nameType.msgt_size != 32))
#endif	UseStaticMsgType
		{ OutP->RetCode = MIG_BAD_ARGUMENTS; return; }
#endif	TypeCheck

	task /* convert_port_to_space 0 Head.msgh_request_port */ = /* task */ convert_port_to_space(In0P->Head.msgh_request_port);

	OutP->RetCode = mach_port_extract_right(task, In0P->name, In0P->msgt_name, &OutP->poly, &polyPoly);
	space_deallocate(task);
	if (OutP->RetCode != KERN_SUCCESS)
		return;

	msgh_simple = TRUE;
	msgh_size = 40;

#if	UseStaticMsgType
	OutP->polyType = polyType;
#else	UseStaticMsgType
	OutP->polyType.msgt_size = 32;
	OutP->polyType.msgt_number = 1;
	OutP->polyType.msgt_inline = TRUE;
	OutP->polyType.msgt_longform = FALSE;
	OutP->polyType.msgt_deallocate = FALSE;
	OutP->polyType.msgt_unused = 0;
#endif	UseStaticMsgType

	if (polyPoly == MACH_MSG_TYPE_PORT_RECEIVE)
	if (IP_VALID((ipc_port_t) InHeadP->msgh_reply_port) &&
	    IP_VALID((ipc_port_t) OutP->poly) &&
	    ipc_port_check_circularity((ipc_port_t) OutP->poly, (ipc_port_t) InHeadP->msgh_reply_port))
		OutHeadP->msgh_bits |= MACH_MSGH_BITS_CIRCULAR;

	if (MACH_MSG_TYPE_PORT_ANY(polyPoly))
		msgh_simple = FALSE;

	OutP->polyType.msgt_name /* polyPoly */ = /* polyType.msgt_name */ polyPoly;

	if (!msgh_simple)
		OutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;
	OutP->Head.msgh_size = msgh_size;
}

mig_external boolean_t mach_port_server
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

	if ((InP->msgh_id > 3216) || (InP->msgh_id < 3200))
		return FALSE;
	else {
		typedef novalue (*SERVER_STUB_PROC)
#if	(defined(__STDC__) || defined(c_plusplus))
			(mach_msg_header_t *, mach_msg_header_t *);
#else
			();
#endif
		static SERVER_STUB_PROC routines[] = {
			_Xmach_port_names,
			_Xmach_port_type,
			_Xmach_port_rename,
			_Xmach_port_allocate_name,
			_Xmach_port_allocate,
			_Xmach_port_destroy,
			_Xmach_port_deallocate,
			_Xmach_port_get_refs,
			_Xmach_port_mod_refs,
			_Xmach_port_get_receive_status,
			_Xmach_port_set_qlimit,
			_Xmach_port_set_mscount,
			_Xmach_port_get_set_status,
			_Xmach_port_move_member,
			_Xmach_port_request_notification,
			_Xmach_port_insert_right,
			_Xmach_port_extract_right,
		};

		if (routines[InP->msgh_id - 3200])
			(routines[InP->msgh_id - 3200]) (InP, &OutP->Head);
		 else
			return FALSE;
	}
	return TRUE;
}
