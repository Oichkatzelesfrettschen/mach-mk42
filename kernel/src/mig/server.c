/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	server.c,v $
 * Revision 2.3  91/02/05  17:55:37  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:55:30  mrt]
 * 
 * Revision 2.2  90/06/02  15:05:29  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:13:12  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 18-Oct-88  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Set the local port in the server reply message to
 *	MACH_PORT_NULL for greater efficiency and to make Camelot
 *	happy.
 *
 * 18-Apr-88  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Changed call to WriteLocalVarDecl in WriteMsgVarDecl
 *	to write out the parameters for the C++ code to a call
 *	a new routine WriteServerVarDecl which includes the *
 *	for reference variable, but uses the transType if it
 *	exists.
 *
 * 27-Feb-88  Richard Draves (rpd) at Carnegie-Mellon University
 *	Changed reply message initialization for camelot interfaces.
 *	Now we assume camelot interfaces are all camelotroutines and
 *	always initialize the dummy field & tid field.  This fixes
 *	the wrapper-server-call bug in distributed transactions.
 *
 * 23-Feb-88  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Changed the include of camelot_types.h to cam/camelot_types.h
 *
 * 19-Feb-88  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Fixed WriteDestroyArg to not call the destructor
 *	function on any in/out args.
 *
 *  4-Feb-88  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Fixed dld's code to write out parameter list to
 *	use WriteLocalVarDecl to get transType or ServType if
 *	they exist.
 *
 * 19-Jan-88  David Golub (dbg) at Carnegie-Mellon University
 *	Change variable-length inline array declarations to use
 *	maximum size specified to Mig.  Make message variable
 *	length if the last item in the message is variable-length
 *	and inline.  Use argMultipler field to convert between
 *	argument and IPC element counts.
 *
 * 18-Jan-88  David Detlefs (dld) at Carnegie-Mellon University
 *	Modified to produce C++ compatible code via #ifdefs.
 *	All changes have to do with argument declarations.
 *
 *  2-Dec-87  David Golub (dbg) at Carnegie-Mellon University
 *	Added destructor function for IN arguments to server.
 *
 * 18-Nov-87  Jeffrey Eppinger (jle) at Carnegie-Mellon University
 *	Changed to typedef "novalue" as "void" if we're using hc.
 *
 * 17-Sep-87  Bennet Yee (bsy) at Carnegie-Mellon University
 *	Added _<system>SymTab{Base|End} for use with security
 *	dispatch routine.  It is neccessary for the authorization
 *	system to know the operations by symbolic names.
 *	It is harmless to user code as it only means an extra
 *	array if it is accidentally turned on.
 *
 * 24-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Corrected the setting of retcode for CamelotRoutines.
 *
 * 21-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Added deallocflag to call to WritePackArgType.
 *
 * 14-Aug-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Moved type declarations and assignments for DummyType 
 *	and tidType to server demux routine. Automatically
 *	include camelot_types.h and msg_types.h for interfaces 
 *	containing camelotRoutines.
 *
 *  8-Jun-87  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Removed #include of sys/types.h and strings.h from WriteIncludes.
 *	Changed the KERNEL include from ../h to sys/
 *	Removed extern from WriteServer to make hi-c happy
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#include <mach/message.h>
#include "write.h"
#include "utils.h"
#include "global.h"

static void
WriteIncludes(file)
    FILE *file;
{
    fprintf(file, "#define EXPORT_BOOLEAN\n");
    fprintf(file, "#include <mach/boolean.h>\n");
    fprintf(file, "#include <mach/kern_return.h>\n");
    fprintf(file, "#include <mach/message.h>\n");
    fprintf(file, "#include <mach/mig_errors.h>\n");
    if (IsCamelot)
    {
	fprintf(file, "#include <cam/camelot_types.h>\n");
	fprintf(file, "#include <mach/msg_type.h>\n");
    }
    if (IsKernelServer)
	fprintf(file, "#include <ipc/ipc_port.h>\n");
#if	0
    if (IsKernelServer)
    {
	fprintf(file, "#include <mach/port.h>\n");
	fprintf(file, "#include <mach/kern_return.h>\n");
	fprintf(file, "#include <mach/machine/vm_types.h>\n");
	fprintf(file, "#include <vm/vm_map.h>\n");
	fprintf(file, "#include <kern/task.h>\n");
	fprintf(file, "#include <kern/thread.h>\n");
	fprintf(file, "#include <kern/kern_obj.h>\n");
	fprintf(file, "#include <kern/kern_port.h>\n");
	fprintf(file, "#include <kern/kern_set.h>\n");
	fprintf(file, "#include <kern/port_object.h>\n");
	fprintf(file, "#include <kern/ipc_cache.h>\n");
    }
#endif
    fprintf(file, "\n");
}

static void
WriteGlobalDecls(file)
    FILE *file;
{
    fprintf(file, "/* Due to pcc compiler bug, cannot use void */\n");
    fprintf(file, "#if\t%s || defined(hc)\n", NewCDecl);
    fprintf(file, "#define novalue void\n");
    fprintf(file, "#else\n");
    fprintf(file, "#define novalue int\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");

    if (RCSId != strNULL)
	WriteRCSDecl(file, strconcat(SubsystemName, "_server"), RCSId);

    /* Used for locations in the request message, *not* reply message.
       Reply message locations aren't dependent on IsKernelServer. */

    if (IsKernelServer)
    {
	fprintf(file, "#define msgh_request_port\tmsgh_remote_port\n");
	fprintf(file, "#define MACH_MSGH_BITS_REQUEST(bits)");
	fprintf(file, "\tMACH_MSGH_BITS_REMOTE(bits)\n");
	fprintf(file, "#define msgh_reply_port\t\tmsgh_local_port\n");
	fprintf(file, "#define MACH_MSGH_BITS_REPLY(bits)");
	fprintf(file, "\tMACH_MSGH_BITS_LOCAL(bits)\n");
    }
    else
    {
	fprintf(file, "#define msgh_request_port\tmsgh_local_port\n");
	fprintf(file, "#define MACH_MSGH_BITS_REQUEST(bits)");
	fprintf(file, "\tMACH_MSGH_BITS_LOCAL(bits)\n");
	fprintf(file, "#define msgh_reply_port\t\tmsgh_remote_port\n");
	fprintf(file, "#define MACH_MSGH_BITS_REPLY(bits)");
	fprintf(file, "\tMACH_MSGH_BITS_REMOTE(bits)\n");
    }
    fprintf(file, "\n");
}

static void
WriteProlog(file)
    FILE *file;
{
    fprintf(file, "/* Module %s */\n", SubsystemName);
    fprintf(file, "\n");
    
    WriteIncludes(file);
    WriteBogusDefines(file);
    WriteGlobalDecls(file);
}


static void
WriteSymTabEntries(file, stats)
    FILE *file;
    statement_t *stats;
{
    register statement_t *stat;
    register u_int current = 0;

    for (stat = stats; stat != stNULL; stat = stat->stNext)
	if (stat->stKind == skRoutine) {
	    register	num = stat->stRoutine->rtNumber;
	    char	*name = stat->stRoutine->rtName;
	    while (++current <= num)
		fprintf(file,"\t\t\t{ \"\", 0, 0 },\n");
	    fprintf(file, "\t{ \"%s\", %d, _X%s },\n",
	    	name,
		SubsystemBase + current - 1,
		name);
	}
    while (++current <= rtNumber)
	fprintf(file,"\t{ \"\", 0, 0 },\n");
}

static void
WriteArrayEntries(file, stats)
    FILE *file;
    statement_t *stats;
{
    register u_int current = 0;
    register statement_t *stat;

    for (stat = stats; stat != stNULL; stat = stat->stNext)
	if (stat->stKind == skRoutine)
	{
	    register routine_t *rt = stat->stRoutine;

	    while (current++ < rt->rtNumber)
		fprintf(file, "\t\t\t0,\n");
	    fprintf(file, "\t\t\t_X%s,\n", rt->rtName);
	}
    while (current++ < rtNumber)
	fprintf(file, "\t\t\t0,\n");
}

static void
WriteEpilog(file, stats)
    FILE *file;
    statement_t *stats;
{
    fprintf(file, "\n");

    fprintf(file, "mig_external boolean_t %s\n", ServerProcName);
    fprintf(file, "#if\t%s\n", NewCDecl);
    fprintf(file, "\t(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)\n");
    fprintf(file, "#else\n");
    fprintf(file, "\t(InHeadP, OutHeadP)\n");
    fprintf(file, "\tmach_msg_header_t *InHeadP, *OutHeadP;\n");
    fprintf(file, "#endif\n");

    fprintf(file, "{\n");
    fprintf(file, "\tregister mach_msg_header_t *InP =  InHeadP;\n");

    if (IsCamelot)
	fprintf(file, "\tregister camelot_death_pill_t *OutP = (camelot_death_pill_t *) OutHeadP;\n");
    else
	fprintf(file, "\tregister mig_reply_header_t *OutP = (mig_reply_header_t *) OutHeadP;\n");

    fprintf(file, "\n");

    WriteStaticDecl(file, itRetCodeType,
		    itRetCodeType->itDeallocate, itRetCodeType->itLongForm,
		    !IsKernelServer, "RetCodeType");
    fprintf(file, "\n");

    if (IsCamelot)
    {
	WriteStaticDecl(file, itDummyType,
			itDummyType->itDeallocate, itDummyType->itLongForm,
			!IsKernelServer, "DummyType");
	fprintf(file, "\n");
	WriteStaticDecl(file, itTidType,
			itTidType->itDeallocate, itTidType->itLongForm,
			!IsKernelServer, "TidType");
	fprintf(file, "\n");
    }

    fprintf(file, "\tOutP->Head.msgh_bits = ");
    fprintf(file, "MACH_MSGH_BITS(MACH_MSGH_BITS_REPLY(InP->msgh_bits), 0);\n");
    fprintf(file, "\tOutP->Head.msgh_size = sizeof *OutP;\n");
    fprintf(file, "\tOutP->Head.msgh_remote_port = InP->msgh_reply_port;\n");
    fprintf(file, "\tOutP->Head.msgh_local_port = MACH_PORT_NULL;\n");
    fprintf(file, "\tOutP->Head.msgh_kind = InP->msgh_kind;\n");
    fprintf(file, "\tOutP->Head.msgh_id = InP->msgh_id + 100;\n");
    fprintf(file, "\n");
    WritePackMsgType(file, itRetCodeType,
		     itRetCodeType->itDeallocate, itRetCodeType->itLongForm,
		     !IsKernelServer, "OutP->RetCodeType", "RetCodeType");
    fprintf(file, "\tOutP->RetCode = MIG_BAD_ID;\n");
    fprintf(file, "\n");

    if (IsCamelot)
    {
	WritePackMsgType(file, itDummyType,
			 itDummyType->itDeallocate, itDummyType->itLongForm,
			 !IsKernelServer, "OutP->DummyType", "DummyType");
	fprintf(file, "\t/* dummy doesn't need a value */\n");
	fprintf(file, "\n");
	WritePackMsgType(file, itTidType,
			 itTidType->itDeallocate, itTidType->itLongForm,
			 !IsKernelServer, "OutP->TidType", "TidType");
	fprintf(file, "\tOutP->Tid = ((camelot_death_pill_t *)InP)->Tid;\n");
	fprintf(file, "\n");
    }

    fprintf(file, "\tif ((InP->msgh_id > %d) || (InP->msgh_id < %d))\n",
	    SubsystemBase + rtNumber - 1, SubsystemBase);
    fprintf(file, "\t\treturn FALSE;\n");
    fprintf(file, "\telse {\n");
    fprintf(file, "\t\ttypedef novalue (*SERVER_STUB_PROC)\n");
    fprintf(file, "#if\t%s\n", NewCDecl);
    fprintf(file, "\t\t\t(mach_msg_header_t *, mach_msg_header_t *);\n");
    fprintf(file, "#else\n");
    fprintf(file, "\t\t\t();\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\t\tstatic SERVER_STUB_PROC routines[] = {\n");

    WriteArrayEntries(file, stats);

    fprintf(file, "\t\t};\n");
    fprintf(file, "\n");

    /* Call appropriate routine */
    fprintf(file, "\t\tif (routines[InP->msgh_id - %d])\n", SubsystemBase);
    fprintf(file, "\t\t\t(routines[InP->msgh_id - %d]) (InP, &OutP->Head);\n",
	    SubsystemBase);
    fprintf(file, "\t\t else\n");
    fprintf(file, "\t\t\treturn FALSE;\n");
    
    fprintf(file, "\t}\n");
    
    fprintf(file, "\treturn TRUE;\n");
    fprintf(file, "}\n");

    /* symtab */

    if (GenSymTab) {
	fprintf(file,"\nmig_symtab_t _%sSymTab[] = {\n",SubsystemName);
	WriteSymTabEntries(file,stats);
	fprintf(file,"};\n");
	fprintf(file,"int _%sSymTabBase = %d;\n",SubsystemName,SubsystemBase);
	fprintf(file,"int _%sSymTabEnd = %d;\n",SubsystemName,SubsystemBase+rtNumber);
    }
}

/*
 *  Returns the return type of the server-side work function.
 *  Suitable for "extern %s serverfunc()".
 */
static char *
ServerSideType(rt)
    routine_t *rt;
{
    if (rt->rtServerReturn == argNULL)
	return "void";
    else
	return rt->rtServerReturn->argType->itTransType;
}

static void
WriteLocalVarDecl(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    if (it->itInLine && it->itVarArray)
    {
	register ipc_type_t *btype = it->itElement;

	fprintf(file, "\t%s %s[%d]", btype->itTransType,
		arg->argVarName, it->itNumber/btype->itNumber);
    }
    else
	fprintf(file, "\t%s %s", it->itTransType, arg->argVarName);
}

static void
WriteServerVarDecl(file, arg)
    FILE *file;
    argument_t *arg;
{
    fprintf(file, "%s %s%s",
	    arg->argType->itTransType,
	    argByReferenceServer(arg) ? "*" : "",
	    arg->argVarName);
}

/*
 *  Writes the local variable declarations which are always
 *  present:  InP, OutP, the server-side work function.
 */
static void
WriteVarDecls(file, rt)
    FILE *file;
    routine_t *rt;
{
    int i;

    fprintf(file, "\tregister Request *In0P = (Request *) InHeadP;\n");
    for (i = 1; i <= rt->rtMaxRequestPos; i++)
	fprintf(file, "\tregister Request *In%dP;\n", i);
    fprintf(file, "\tregister Reply *OutP = (Reply *) OutHeadP;\n");

    fprintf(file, "\tmig_external %s %s\n",
	    ServerSideType(rt), rt->rtServerName);
    fprintf(file, "#if\t%s\n", NewCDecl);
    fprintf(file, "\t\t(");
    WriteList(file, rt->rtArgs, WriteServerVarDecl, akbServerArg, ", ", "");
    fprintf(file, ");\n");
    fprintf(file, "#else\n");
    fprintf(file, "\t\t();\n");
    fprintf(file, "#endif\n");
    fprintf(file, "\n");

    if (rt->rtOneWay)
    {
	fprintf(file, "#if\tTypeCheck\n");
	fprintf(file, "\tboolean_t msgh_simple;\n");
	fprintf(file, "#endif\tTypeCheck\n");
    }
    else
	fprintf(file, "\tboolean_t msgh_simple;\n");
    fprintf(file, "\n");

    fprintf(file, "\tunsigned int msgh_size;\n");

    /* if either request or reply is variable, we need msgh_size_delta */
    if ((rt->rtNumRequestVar > 0) ||
	(rt->rtNumReplyVar > 0))
	fprintf(file, "\tunsigned int msgh_size_delta;\n");

    fprintf(file, "\n");
}

static void
WriteMsgError(file, error)
    FILE *file;
    char *error;
{
    fprintf(file, "\t\t{ OutP->RetCode = %s; return; }\n", error);
}

static void
WriteReplyInit(file, rt)
    FILE *file;
    routine_t *rt;
{
    fprintf(file, "\n");
    fprintf(file, "\tmsgh_simple = %s;\n", strbool(rt->rtSimpleSendReply));
    fprintf(file, "\tmsgh_size = %d;\n", rt->rtReplySize);
}

static void
WriteReplyHead(file, rt)
    FILE *file;
    routine_t *rt;
{
    fprintf(file, "\n");
    if (rt->rtMaxReplyPos > 0)
	fprintf(file, "\tOutP = (Reply *) OutHeadP;\n");

    fprintf(file, "\tif (!msgh_simple)\n");
    fprintf(file, "\t\tOutP->Head.msgh_bits |= MACH_MSGH_BITS_COMPLEX;\n");
    fprintf(file, "\tOutP->Head.msgh_size = msgh_size;\n");
}

static void
WriteCheckHead(file, rt)
    FILE *file;
    routine_t *rt;
{
    fprintf(file, "#if\tTypeCheck\n");
    fprintf(file, "\tmsgh_size = In0P->Head.msgh_size;\n");
    fprintf(file, "\tmsgh_simple = !(In0P->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX);\n");

    if (rt->rtNumRequestVar > 0)
	fprintf(file, "\tif ((msgh_size < %d)",
		rt->rtRequestSize);
    else
	fprintf(file, "\tif ((msgh_size != %d)",
		rt->rtRequestSize);
    if (rt->rtSimpleCheckRequest)
	fprintf(file, " || %smsgh_simple",
		rt->rtSimpleReceiveRequest ? "!" : "");
    fprintf(file, ")\n");
    WriteMsgError(file, "MIG_BAD_ARGUMENTS");
    fprintf(file, "#endif\tTypeCheck\n");
    fprintf(file, "\n");
}

static void
WriteTypeCheck(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;
    register routine_t *rt = arg->argRoutine;

    fprintf(file, "#if\tTypeCheck\n");
    if (akCheck(arg->argKind, akbRequestQC))
    {
	fprintf(file, "#if\tUseStaticMsgType\n");
	fprintf(file, "\tif (* (int *) &In%dP->%s != * (int *) &%sCheck)\n",
		arg->argRequestPos, arg->argTTName, arg->argVarName);
	fprintf(file, "#else\tUseStaticMsgType\n");
    }
    fprintf(file, "\tif ((In%dP->%s%s.msgt_inline != %s) ||\n",
	    arg->argRequestPos, arg->argTTName,
	    arg->argLongForm ? ".msgtl_header" : "",
	    strbool(it->itInLine));
    fprintf(file, "\t    (In%dP->%s%s.msgt_longform != %s) ||\n",
	    arg->argRequestPos, arg->argTTName,
	    arg->argLongForm ? ".msgtl_header" : "",
	    strbool(arg->argLongForm));
    if (it->itOutName == MACH_MSG_TYPE_POLYMORPHIC)
    {
	if (!rt->rtSimpleCheckRequest)
	    fprintf(file, "\t    (MACH_MSG_TYPE_PORT_ANY(In%dP->%s.msgt%s_name) && msgh_simple) ||\n",
		    arg->argRequestPos, arg->argTTName,
		    arg->argLongForm ? "l" : "");
    }
    else
	fprintf(file, "\t    (In%dP->%s.msgt%s_name != %s) ||\n",
		arg->argRequestPos, arg->argTTName,
		arg->argLongForm ? "l" : "",
		it->itOutNameStr);
    if (!it->itVarArray)
	fprintf(file, "\t    (In%dP->%s.msgt%s_number != %d) ||\n",
		arg->argRequestPos, arg->argTTName,
		arg->argLongForm ? "l" : "",
		it->itNumber);
    fprintf(file, "\t    (In%dP->%s.msgt%s_size != %d))\n",
	    arg->argRequestPos, arg->argTTName,
	    arg->argLongForm ? "l" : "",
	    it->itSize);
    if (akCheck(arg->argKind, akbRequestQC))
	fprintf(file, "#endif\tUseStaticMsgType\n");
    WriteMsgError(file, "MIG_BAD_ARGUMENTS");
    fprintf(file, "#endif\tTypeCheck\n");
    fprintf(file, "\n");
}

static void
WriteCheckMsgSize(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register routine_t *rt = arg->argRoutine;
    register ipc_type_t *btype = arg->argType->itElement;
    argument_t *count = arg->argCount;
    boolean_t NoMoreArgs, LastVarArg;

    /* If there aren't any more In args after this, then
       msgh_size_delta value will only get used by TypeCheck code,
       so put the assignment under the TypeCheck conditional. */

    NoMoreArgs = arg->argRequestPos == rt->rtMaxRequestPos;

    /* If there aren't any more variable-sized arguments after this,
       then we must check for exact msg-size and we don't need
       to update msgh_size. */

    LastVarArg = arg->argRequestPos+1 == rt->rtNumRequestVar;

    if (NoMoreArgs)
	fprintf(file, "#if\tTypeCheck\n");

    /* calculate the actual size in bytes of the data field.  note
       that this quantity must be a multiple of four.  hence, if
       the base type size isn't a multiple of four, we have to
       round up.  note also that btype->itNumber must
       divide btype->itTypeSize (see itCalculateSizeInfo). */

    fprintf(file, "\tmsgh_size_delta = (%d * In%dP->%s)",
	    btype->itTypeSize/btype->itNumber,
	    arg->argRequestPos,
	    count->argMsgField);
    if (btype->itTypeSize % 4 != 0)
	fprintf(file, "+3 &~ 3");
    fprintf(file, ";\n");

    if (!NoMoreArgs)
	fprintf(file, "#if\tTypeCheck\n");

    /* Don't decrement msgh_size until we've checked it won't underflow. */

    if (LastVarArg)
	fprintf(file, "\tif (msgh_size != %d + msgh_size_delta)\n",
		rt->rtRequestSize);
    else
	fprintf(file, "\tif (msgh_size < %d + msgh_size_delta)\n",
		rt->rtRequestSize);
    WriteMsgError(file, "MIG_BAD_ARGUMENTS");

    if (!LastVarArg)
	fprintf(file, "\tmsgh_size -= msgh_size_delta;\n");

    fprintf(file, "#endif\tTypeCheck\n");
    fprintf(file, "\n");
}

static void
WriteExtractArgValue(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    if (arg->argMultiplier > 1)
	WriteCopyType(file, it, "%s /* %d %s %d */", "/* %s */ In%dP->%s / %d",
		      arg->argVarName, arg->argRequestPos,
		      arg->argMsgField, arg->argMultiplier);
    else if (it->itInTrans != strNULL)
	WriteCopyType(file, it, "%s /* %s %d %s */", "/* %s */ %s(In%dP->%s)",
		      arg->argVarName, it->itInTrans,
		      arg->argRequestPos, arg->argMsgField);
    else
	WriteCopyType(file, it, "%s /* %d %s */", "/* %s */ In%dP->%s",
		      arg->argVarName, arg->argRequestPos, arg->argMsgField);
    fprintf(file, "\n");
}

static void
WriteInitializeCount(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *ptype = arg->argParent->argType;
    register ipc_type_t *btype = ptype->itElement;

    /*
     *	Initialize 'count' argument for variable-length inline OUT parameter
     *	with maximum allowed number of elements.
     */

    fprintf(file, "\t%s = %d;\n", arg->argVarName,
	    ptype->itNumber/btype->itNumber);
    fprintf(file, "\n");
}

static void
WriteTypeCheckArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    if (akCheck(arg->argKind, akbRequest))
	WriteTypeCheck(file, arg);

    if (akCheckAll(arg->argKind, akbVariable|akbRequest))
	WriteCheckMsgSize(file, arg);

    /* This assumes that the count argument directly follows the 
       associated variable-sized argument and any other implicit
       arguments it may have. */

    if ((akIdent(arg->argKind) == akeCount) &&
	akCheck(arg->argKind, akbSendRcv) &&
	(arg->argRequestPos < arg->argRoutine->rtMaxRequestPos))
    {
	register ipc_type_t *ptype = arg->argParent->argType;

	if (ptype->itInLine && ptype->itVarArray)
	{
	    fprintf(file, "\tIn%dP = (Request *) ((char *) In%dP + msgh_size_delta - %d);\n",
		    arg->argRequestPos+1, arg->argRequestPos,
		    ptype->itTypeSize + ptype->itPadSize);
	    fprintf(file, "\n");
	}
    }
}

static void
WriteExtractArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    if (akCheckAll(arg->argKind, akbSendRcv|akbVarNeeded))
	WriteExtractArgValue(file, arg);

    if ((akIdent(arg->argKind) == akeCount) &&
	akCheck(arg->argKind, akbReturnSnd))
    {
	register ipc_type_t *ptype = arg->argParent->argType;

	if (ptype->itInLine && ptype->itVarArray)
	    WriteInitializeCount(file, arg);
    }
}

static void
WriteServerCallArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    ipc_type_t *it = arg->argType;
    boolean_t NeedClose = FALSE;

    if (argByReferenceServer(arg))
	fprintf(file, "&");

    if ((it->itInTrans != strNULL) &&
	akCheck(arg->argKind, akbSendRcv) &&
	!akCheck(arg->argKind, akbVarNeeded))
    {
	fprintf(file, "%s(", it->itInTrans);
	NeedClose = TRUE;
    }

    if (akCheck(arg->argKind, akbVarNeeded))
	fprintf(file, "%s", arg->argVarName);
    else if (akCheck(arg->argKind, akbSendRcv))
	fprintf(file, "In%dP->%s", arg->argRequestPos, arg->argMsgField);
    else
	fprintf(file, "OutP->%s", arg->argMsgField);

    if (NeedClose)
	fprintf(file, ")");

    if (!argByReferenceServer(arg) && (arg->argMultiplier > 1))
	fprintf(file, " / %d", arg->argMultiplier);
}

static void
WriteDestroyArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    if (akCheck(arg->argKind, akbVarNeeded))
	fprintf(file, "\t%s(%s);\n", it->itDestructor, arg->argVarName);
    else
	fprintf(file, "\t%s(In%dP->%s);\n", it->itDestructor,
		arg->argRequestPos, arg->argMsgField);
}

static void
WriteDestroyPortArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    /*
     *	If a translated port argument occurs in the body of a request
     *	message, and the message is successfully processed, then the
     *	port right should be deallocated.  However, the called function
     *	didn't see the port right; it saw the translation.  So we have
     *	to release the port right for it.
     */

    if ((it->itInTrans != strNULL) &&
	(it->itOutName == MACH_MSG_TYPE_PORT_SEND))
    {
	fprintf(file, "\n");
	fprintf(file, "\tif (IP_VALID((ipc_port_t) In%dP->%s))\n",
		arg->argRequestPos, arg->argMsgField);
	fprintf(file, "\t\tipc_port_release_send((ipc_port_t) In%dP->%s);\n",
		arg->argRequestPos, arg->argMsgField);
    }
}

static void
WriteServerCall(file, rt)
    FILE *file;
    routine_t *rt;
{
    boolean_t NeedClose = FALSE;

    fprintf(file, "\t");
    if (rt->rtServerReturn != argNULL)
    {
	argument_t *arg = rt->rtServerReturn;
	ipc_type_t *it = arg->argType;

	fprintf(file, "OutP->%s = ", arg->argMsgField);
	if (it->itOutTrans != strNULL)
	{
	    fprintf(file, "%s(", it->itOutTrans);
	    NeedClose = TRUE;
	}
    }
    fprintf(file, "%s(", rt->rtServerName);
    WriteList(file, rt->rtArgs, WriteServerCallArg, akbServerArg, ", ", "");
    if (NeedClose)
	fprintf(file, ")");
    fprintf(file, ");\n");
}

static void
WriteGetReturnValue(file, rt)
    FILE *file;
    register routine_t *rt;
{
    if (rt->rtServerReturn != rt->rtRetCode)
	fprintf(file, "\tOutP->%s = KERN_SUCCESS;\n",
		rt->rtRetCode->argMsgField);
}

static void
WriteCheckReturnValue(file, rt)
    FILE *file;
    register routine_t *rt;
{
    if (rt->rtServerReturn == rt->rtRetCode)
    {
	fprintf(file, "\tif (OutP->%s != KERN_SUCCESS)\n",
		rt->rtRetCode->argMsgField);
	fprintf(file, "\t\treturn;\n");
    }
}

static void
WritePackArgType(file, arg)
    FILE *file;
    register argument_t *arg;
{
    fprintf(file, "\n");

    WritePackMsgType(file, arg->argType, arg->argDeallocate, arg->argLongForm,
		     !IsKernelServer, "OutP->%s", "%s", arg->argTTName);
}

static void
WritePackArgValue(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    fprintf(file, "\n");

    if (it->itInLine && it->itVarArray)
    {
	register argument_t *count = arg->argCount;
	register ipc_type_t *btype = it->itElement;

	/* Note btype->itNumber == count->argMultiplier */

	fprintf(file, "\tbcopy((char *) %s, (char *) OutP->%s, ",
		arg->argVarName, arg->argMsgField);
	fprintf(file, "%d * %s);\n",
		btype->itTypeSize, count->argVarName);
    }
    else if (arg->argMultiplier > 1)
	WriteCopyType(file, it, "OutP->%s /* %d %s */", "/* %s */ %d * %s",
		      arg->argMsgField,
		      arg->argMultiplier,
		      arg->argVarName);
    else if (it->itOutTrans != strNULL)
	WriteCopyType(file, it, "OutP->%s /* %s %s */", "/* %s */ %s(%s)",
		      arg->argMsgField, it->itOutTrans, arg->argVarName);
    else
	WriteCopyType(file, it, "OutP->%s /* %s */", "/* %s */ %s",
		      arg->argMsgField, arg->argVarName);
}

static void
WriteCopyArgValue(file, arg)
    FILE *file;
    argument_t *arg;
{
    fprintf(file, "\n");
    WriteCopyType(file, arg->argType, "/* %d */ OutP->%s", "In%dP->%s",
		  arg->argRequestPos, arg->argMsgField);
}

static void
WriteAdjustMsgSimple(file, arg)
    FILE *file;
    register argument_t *arg;
{
    /* akbVarNeeded must be on */

    if (!arg->argRoutine->rtSimpleFixedReply)
    {
	fprintf(file, "\n");
	fprintf(file, "\tif (MACH_MSG_TYPE_PORT_ANY(%s))\n", arg->argVarName);
	fprintf(file, "\t\tmsgh_simple = FALSE;\n");
    }
}

static void
WriteAdjustMsgCircular(file, arg)
    FILE *file;
    register argument_t *arg;
{
    fprintf(file, "\n");

    if (arg->argType->itOutName == MACH_MSG_TYPE_POLYMORPHIC)
	fprintf(file, "\tif (%s == MACH_MSG_TYPE_PORT_RECEIVE)\n",
		arg->argPoly->argVarName);

    /*
     *	The carried port right can be accessed in OutP->XXXX.  Normally
     *	the server function stuffs it directly there.  If it is InOut,
     *	then it has already been copied into the reply message.
     *	If the server function deposited it into a variable (perhaps
     *	because the reply message is variable-sized) then it has already
     *	been copied into the reply message.  Note we must use InHeadP
     *	(or In0P->Head) and OutHeadP to access the message headers,
     *	because of the variable-sized messages.
     */

    fprintf(file, "\tif (IP_VALID((ipc_port_t) InHeadP->msgh_reply_port) &&\n");
    fprintf(file, "\t    IP_VALID((ipc_port_t) OutP->%s) &&\n", arg->argMsgField);
    fprintf(file, "\t    ipc_port_check_circularity((ipc_port_t) OutP->%s, (ipc_port_t) InHeadP->msgh_reply_port))\n", arg->argMsgField);
    fprintf(file, "\t\tOutHeadP->msgh_bits |= MACH_MSGH_BITS_CIRCULAR;\n");
}

static void
WriteAdjustMsgSize(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *ptype = arg->argParent->argType;
    register ipc_type_t *btype = ptype->itElement;

    fprintf(file, "\n");

    /* calculate the actual size in bytes of the data field.
       note that this quantity must be a multiple of four.
       hence, if the base type size isn't a multiple of four,
       we have to round up. */

    fprintf(file, "\tmsgh_size_delta = (%d * %s)",
	    btype->itTypeSize, arg->argVarName);
    if (btype->itTypeSize % 4 != 0)
	fprintf(file, "+3 &~ 3");
    fprintf(file, ";\n");

    fprintf(file, "\tmsgh_size += msgh_size_delta;\n");

    /* Don't bother moving OutP unless there are more Out arguments. */
    if (arg->argReplyPos < arg->argRoutine->rtMaxReplyPos)
    {
	fprintf(file, "\tOutP = (Reply *) ((char *) OutP + ");
	fprintf(file, "msgh_size_delta - %d);\n",
		ptype->itTypeSize + ptype->itPadSize);
    }
}

static void
WritePackArg(file, arg)
    FILE *file;
    argument_t *arg;
{
    if (akCheck(arg->argKind, akbReplyInit))
	WritePackArgType(file, arg);

    if ((akIdent(arg->argKind) == akePoly) &&
	akCheck(arg->argKind, akbReturnSnd))
	WriteAdjustMsgSimple(file, arg);

    if (akCheckAll(arg->argKind, akbReturnSnd|akbVarNeeded))
	WritePackArgValue(file, arg);

    if (akCheck(arg->argKind, akbReplyCopy))
	WriteCopyArgValue(file, arg);

    /*
     *	If this is a KernelServer, and the reply message contains
     *	a receive right, we must check for the possibility of a
     *	port/message circularity.  If queueing the reply message
     *	would cause a circularity, we mark the reply message
     *	with the circular bit.
     */

    if (IsKernelServer &&
	akCheck(arg->argKind, akbReturnSnd) &&
	((arg->argType->itOutName == MACH_MSG_TYPE_PORT_RECEIVE) ||
	 (arg->argType->itOutName == MACH_MSG_TYPE_POLYMORPHIC)))
	WriteAdjustMsgCircular(file, arg);

    if ((akIdent(arg->argKind) == akeCount) &&
	akCheck(arg->argKind, akbReturnSnd))
    {
	register ipc_type_t *ptype = arg->argParent->argType;

	if (ptype->itInLine && ptype->itVarArray)
	    WriteAdjustMsgSize(file, arg);
    }
}

static void
WriteFieldDecl(file, arg)
    FILE *file;
    argument_t *arg;
{
    WriteFieldDeclPrim(file, arg, FetchServerType);
}

static void
WriteRoutine(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "\n");

    fprintf(file, "/* %s %s */\n", rtRoutineKindToStr(rt->rtKind), rt->rtName);
    fprintf(file, "mig_internal novalue _X%s\n", rt->rtName);
    fprintf(file, "#if\t%s\n", NewCDecl);
    fprintf(file, "\t(mach_msg_header_t *InHeadP, mach_msg_header_t *OutHeadP)\n");
    fprintf(file, "#else\n");
    fprintf(file, "\t(InHeadP, OutHeadP)\n");
    fprintf(file, "\tmach_msg_header_t *InHeadP, *OutHeadP;\n");
    fprintf(file, "#endif\n");

    fprintf(file, "{\n");
    WriteStructDecl(file, rt->rtArgs, WriteFieldDecl, akbRequest, "Request");
    WriteStructDecl(file, rt->rtArgs, WriteFieldDecl, akbReply, "Reply");

    WriteVarDecls(file, rt);

    WriteList(file, rt->rtArgs, WriteCheckDecl, akbRequestQC, "\n", "\n");
    WriteList(file, rt->rtArgs,
	      IsKernelServer ? WriteTypeDeclOut : WriteTypeDeclIn,
	      akbReplyInit, "\n", "\n");

    WriteList(file, rt->rtArgs, WriteLocalVarDecl,
	      akbVarNeeded, ";\n", ";\n\n");

    WriteCheckHead(file, rt);

    WriteList(file, rt->rtArgs, WriteTypeCheckArg, akbNone, "", "");
    WriteList(file, rt->rtArgs, WriteExtractArg, akbNone, "", "");

    WriteServerCall(file, rt);
    WriteGetReturnValue(file, rt);

    WriteReverseList(file, rt->rtArgs, WriteDestroyArg, akbDestroy, "", "");
    WriteCheckReturnValue(file, rt);

    if (IsKernelServer)
	WriteReverseList(file, rt->rtArgs, WriteDestroyPortArg,
			 akbSendBody|akbSendRcv, "", "");

    if (!rt->rtOneWay)
    {
	WriteReplyInit(file, rt);
	WriteList(file, rt->rtArgs, WritePackArg, akbNone, "", "");
	WriteReplyHead(file, rt);
    }

    fprintf(file, "}\n");
}

static void
WriteStubArgDecl(file, arg)
    FILE *file;
    argument_t *arg;
{
    char *ref = argByReferenceServer(arg) ? "*" : "";

    fprintf(file, "\t%s %s%s", arg->argType->itServerType,
	    ref, arg->argVarName);
}

static void
WriteStubDecl(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "\n");
    fprintf(file, "/* %s %s */\n", rtRoutineKindToStr(rt->rtKind), rt->rtName);
    fprintf(file, "%s %s\n", ServerSideType(rt), rt->rtServerName);
    fprintf(file, "#if\t%s\n", NewCDecl);
    fprintf(file, "(\n");
    WriteList(file, rt->rtArgs, WriteStubArgDecl,
	      akbServerArg, ",\n", "\n");
    fprintf(file, ")\n");
    fprintf(file, "#else\n");
    fprintf(file, "\t(");
    WriteList(file, rt->rtArgs, WriteNameDecl, akbServerArg, ", ", "");
    fprintf(file, ")\n");
    WriteList(file, rt->rtArgs, WriteStubArgDecl,
	      akbServerArg, ";\n", ";\n");
    fprintf(file, "#endif\n");
    fprintf(file, "{\n");
}

static void
WriteTrapCheckServer(file, rt, type)
    FILE *file;
    register routine_t *rt;
    string_t type;
{
    /* lock sport and check it out */

    fprintf(file, "\tport_lock(sport);\n");
    fprintf(file, "\tif ((sport->port_receiver != kernel_task)");
    if (type != strNULL)
	fprintf(file, " ||\n\t    (port_object_type(sport) != %s)", type);
    fprintf(file, ") {\n");
    fprintf(file, "\t\tport_unlock(sport);\n");
    fprintf(file, "\t\tport_release(sport);\n");
    if (!rt->rtOneWay)
	fprintf(file, "\t\tport_release(rport);\n");
    fprintf(file, "\t\tgoto do_rpc;\n");
    fprintf(file, "\t}\n");
    fprintf(file, "\n");
}

static void
WriteTrapReleaseSPort(file)
    FILE *file;
{
    /* release our reference for sport.
       it should be in-use, because it has a receiver. */

    fprintf(file, "\tsport->port_references--;\n");
    fprintf(file, "\tassert(sport->port_in_use);\n");
    fprintf(file, "\tport_unlock(sport);\n");
}

static void
WriteTrapObjectConvertPort(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "    {\n");

    WriteTrapCheckServer(file, rt, strNULL);

    fprintf(file, "\tport_unlock(sport);\n");
    fprintf(file, "\tObject = (mach_port_t) sport;\n");
    fprintf(file, "    }\n");
    fprintf(file, "\n");
}

static void
WriteTrapObjectConvertTask(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "    {\n");

    WriteTrapCheckServer(file, rt, "PORT_OBJECT_TASK");

    fprintf(file, "\tObject = (task_t) port_object_get(sport);\n");
    fprintf(file, "\tassert(Object != TASK_NULL);\n");
    fprintf(file, "\ttask_reference(Object);\n");
    fprintf(file, "\n");

    WriteTrapReleaseSPort(file);
    fprintf(file, "    }\n");
    fprintf(file, "\n");
}

static void
WriteTrapObjectConvertVmTask(file, rt)
    FILE *file;
    register routine_t *rt;
{
    /* write declaration of temporary task variable */

    fprintf(file, "    {\n");
    fprintf(file, "\tregister task_t task;\n");
    fprintf(file, "\n");

    WriteTrapCheckServer(file, rt, "PORT_OBJECT_TASK");

    fprintf(file, "\ttask = (task_t) port_object_get(sport);\n");
    fprintf(file, "\tassert(task != TASK_NULL);\n");
    fprintf(file, "\tObject = task->map;\n");
    fprintf(file, "\tvm_map_reference(Object);\n");
    fprintf(file, "\n");

    WriteTrapReleaseSPort(file);
    fprintf(file, "    }\n");
    fprintf(file, "\n");
}

static void
WriteTrapObjectConvertThread(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "    {\n");

    WriteTrapCheckServer(file, rt, "PORT_OBJECT_THREAD");

    fprintf(file, "\tObject = (thread_t) port_object_get(sport);\n");
    fprintf(file, "\tassert(Object != THREAD_NULL);\n");
    fprintf(file, "\tthread_reference(Object);\n");
    fprintf(file, "\n");

    WriteTrapReleaseSPort(file);
    fprintf(file, "    }\n");
    fprintf(file, "\n");
}

static void
WriteTrapObjectConvertMemoryObject(file, rt)
    FILE *file;
    register routine_t *rt;
{
    fprintf(file, "    {\n");

    WriteTrapCheckServer(file, rt, strNULL);

    fprintf(file, "\tport_unlock(sport);\n");
    fprintf(file, "\n");

    fprintf(file, "\tObject = (vm_object_t) vm_object_lookup(sport);\n");
    fprintf(file, "\tport_release(sport);\n");
    fprintf(file, "    }\n");
    fprintf(file, "\n");
}

static void
WriteTrapObjectConvert(file, rt)
    FILE *file;
    register routine_t *rt;
{
    register argument_t *arg = rt->rtRequestPort;
    identifier_t type = arg->argType->itName;

    if (streql(type, "mach_port_t"))
	WriteTrapObjectConvertPort(file, rt);
    else if (streql(type, "task_t"))
	WriteTrapObjectConvertTask(file, rt);
    else if (streql(type, "vm_task_t"))
	WriteTrapObjectConvertVmTask(file, rt);
    else if (streql(type, "thread_t"))
	WriteTrapObjectConvertThread(file, rt);
    else if (streql(type, "memory_object_control_t"))
	WriteTrapObjectConvertMemoryObject(file, rt);
    else
	fatal("unkown request port type %s", type);
}

static void
WriteTrapObjectRelease(file, rt)
    FILE *file;
    register routine_t *rt;
{
    register argument_t *arg = rt->rtRequestPort;
    identifier_t type = arg->argType->itName;

    if (streql(type, "mach_port_t"))
	fprintf(file, "\tport_release_macro((kern_port_t) Object);\n");
    else if (streql(type, "task_t"))
	fprintf(file, "\ttask_deallocate(Object);\n");
    else if (streql(type, "vm_task_t"))
	fprintf(file, "\tvm_map_deallocate(Object);\n");
    else if (streql(type, "thread_t"))
	fprintf(file, "\tthread_deallocate(Object);\n");
    else if (streql(type, "memory_object_control_t"))
	fprintf(file, "\tvm_object_deallocate(Object);\n");
    else
	fatal("unkown request port type %s", type);
}

static void
WriteTrapTempDecl(file, arg)
    FILE *file;
    register argument_t *arg;
{
    register ipc_type_t *it = arg->argType;

    fprintf(file, "\t%s real_%s", it->itTransType, arg->argVarName);
    if (akCheck(arg->argKind, akbSend))
	fprintf(file, " = %sIn", arg->argVarName);
}

static void
WriteTrapServerArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    if (akCheck(arg->argKind, akbReturn))
	fprintf(file, "&real_%s", arg->argVarName);
    else
	fprintf(file, "%s", arg->argVarName);
}

static void
WriteTrapReturnArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    fprintf(file, "\t\t\t*%s = real_%s;\n", arg->argVarName, arg->argVarName);
}

static void
WriteTrapCopyoutArg(file, arg)
    FILE *file;
    register argument_t *arg;
{
    fprintf(file, "\t\t\t(void) copyout((char *) &real_%s,\n",
	    arg->argVarName);
    fprintf(file, "\t\t\t               %s,\n", arg->argVarName);
    fprintf(file, "\t\t\t               sizeof real_%s);\n", arg->argVarName);
}

static void
WriteTrapRoutine(file, rt)
    FILE *file;
    register routine_t *rt;
{
    routine_t *trap = rt->rtFakeTrapRoutine;
    routine_t *rpc = rt->rtFakeRpcRoutine;
    argument_t *request = trap->rtRequestPort;
    argument_t *reply = trap->rtReplyPort;
    string_t save;

    /* write declaration of kernel trap implementation */

    WriteStubDecl(file, trap);

    /* write local variables */

    fprintf(file, "\tregister kern_port_t sport;\n");
    if (!rt->rtOneWay)
	fprintf(file, "\tregister kern_port_t rport;\n");
    fprintf(file, "\t%s Object;\n", request->argType->itTransType);
    fprintf(file, "\tthread_t thself = current_thread();\n");
    fprintf(file, "\ttask_t self = thself->task;\n");
    fprintf(file, "\tboolean_t ipc_kernel = thself->ipc_kernel;\n");
    fprintf(file, "\tkern_return_t kr;\n");
    fprintf(file, "\n");

    /* write temp variables for the Out arguments */

    WriteList(file, rt->rtArgs, WriteTrapTempDecl,
	      akbServerArg|akbReturn, ";\n", ";\n\n");

    /* check the Request/Reply ports to see if they are MACH_PORT_NULL */

    if (rt->rtOneWay)
	fprintf(file, "\tif (%s == MACH_PORT_NULL)\n", request->argVarName);
    else
	fprintf(file, "\tif ((%s == MACH_PORT_NULL) || (%s == MACH_PORT_NULL))\n",
		request->argVarName, reply->argVarName);
    fprintf(file, "\t\tgoto do_rpc;\n");
    fprintf(file, "\n");

    fprintf(file, "    {\n");
    fprintf(file, "\tkern_obj_t obj;\n");
    fprintf(file, "\n");

    /* lock the task, so we can do port translations */

    fprintf(file, "\tipc_task_lock(self);\n");
    fprintf(file, "\n");

    /* copyin the Request port */

    fprintf(file, "\tobj_cache_copyin(self, %s, obj,\n", request->argVarName);
    fprintf(file, "\t\t{\n");
    fprintf(file, "\t\tcopyin_failure:\n");
    fprintf(file, "\t\t\tipc_task_unlock(self);\n");
    fprintf(file, "\t\t\treturn MACH_SEND_INVALID_DEST;\n");
    fprintf(file, "\t\t});\n");
    fprintf(file, "\tsport = (kern_port_t) obj;\n");
    fprintf(file, "\tassert(sport != KERN_PORT_NULL);\n");
    fprintf(file, "\n");

    /* copyin the Reply port */

    if (!rt->rtOneWay)
    {
	fprintf(file, "\tobj_cache_copyin(self, %s, obj,\n",
		reply->argVarName);
	fprintf(file, "\t\t{\n");
	fprintf(file, "\t\t\tport_release(sport);\n");
	fprintf(file, "\t\t\tgoto copyin_failure;\n");
	fprintf(file, "\t\t});\n");
	fprintf(file, "\trport = (kern_port_t) obj;\n");
	fprintf(file, "\tassert(rport != KERN_PORT_NULL);\n");
	fprintf(file, "\n");
    }

    /* the copyins succeeded, so we know the task is active */

    fprintf(file, "\tassert(self->ipc_active);\n");
    fprintf(file, "\tipc_task_unlock(self);\n");
    fprintf(file, "    }\n");
    fprintf(file, "\n");

    /* Okay, now we have refs for sport and rport
       that must get released at some point.
       Convert sport to Object, losting the sport ref
       and gaining an Object ref. */

    WriteTrapObjectConvert(file, rt);

    /* Now both sport and rport are cleaned up; we have an Object
       reference which must be held across the kernel call. */

    /* call the server function */

    fprintf(file, "\tkr = %s(", rt->rtServerName);
    save = rt->rtRequestPort->argVarName;
    rt->rtRequestPort->argVarName = "Object";
    WriteList(file, rt->rtArgs, WriteTrapServerArg,
	      akbServerArg, ", ", "");
    rt->rtRequestPort->argVarName = save;
    fprintf(file, ");\n");
    fprintf(file, "\n");

    /* free the Object reference */

    WriteTrapObjectRelease(file, rt);
    fprintf(file, "\n");

    if (!rt->rtOneWay)
    {
	/* Check that rport is OK (we hold receive rights, not in a set,
	   no messages currently queued). */

	fprintf(file, "    {\n");
	fprintf(file, "\tmach_msg_return_t mr;\n");
	fprintf(file, "\n");

	fprintf(file, "\tport_lock(rport);\n");
	fprintf(file, "\tif ((rport->port_receiver != self) ||\n");
	fprintf(file, "\t    (rport->port_set != KERN_SET_NULL) ||\n");
	fprintf(file, "\t    (rport->port_message_count != 0)) {\n");
	fprintf(file, "\t\tport_unlock(rport);\n");
	fprintf(file, "\t\tport_release(rport);\n");
	fprintf(file, "\t\t(void) /* send reply message */\n");
	fprintf(file, "\t\tkr = /* receive reply message */\n");
	fprintf(file, "\t\tgoto copyout_and_return;\n");
	fprintf(file, "\t}\n");
	fprintf(file, "\n");

	/* Release rport. */

	fprintf(file, "\tassert(rport->port_in_use);\n");
	fprintf(file, "\trport->port_references--;\n");
	fprintf(file, "\tport_unlock(rport);\n");
	fprintf(file, "    }\n");
	fprintf(file, "\n");
    }

    /* copyout the Out arguments and return */

    fprintf(file, "copyout_and_return:\n");
    fprintf(file, "\n");

    if (rtCheckMask(rt->rtArgs, akbServerArg|akbReturn))
    {
	fprintf(file, "\tif (kr == KERN_SUCCESS) {\n");
	fprintf(file, "\t\tif (ipc_kernel) {\n");
	WriteList(file, rt->rtArgs, WriteTrapReturnArg,
		  akbServerArg|akbReturn, "", "");
	fprintf(file, "\t\t} else {\n");
	WriteList(file, rt->rtArgs, WriteTrapCopyoutArg,
		  akbServerArg|akbReturn, "", "");
	fprintf(file, "\t\t}\n");
	fprintf(file, "\t}\n");
    }
    fprintf(file, "\treturn kr;\n");
    fprintf(file, "\n");

    /* write the label that aborted operations above jump to */

    fprintf(file, "do_rpc:\n");
    fprintf(file, "\n");

    /* prepare the thread->ipc_kernel for the RPC */

    fprintf(file, "\tthself->ipc_kernel = TRUE;\n");
    fprintf(file, "\n");

    /* call the "external" RPC function */

    fprintf(file, "\tkr = %s(", rpc->rtServerName);
    save = rpc->rtMsgOption->argVarName;
    rpc->rtMsgOption->argVarName = "MACH_SEND_INTERRUPT|MACH_RCV_INTERRUPT";
    WriteList(file, rpc->rtArgs, WriteTrapServerArg,
	      akbUserArg, ", ", "");
    rpc->rtMsgOption->argVarName = save;
    fprintf(file, ");\n");
    fprintf(file, "\n");

    /* restore thread->ipc_kernel */

    fprintf(file, "\tthself->ipc_kernel = ipc_kernel;\n");
    fprintf(file, "\n");

    fprintf(file, "\tgoto copyout_and_return;\n");
    fprintf(file, "}\n");
}

void
WriteServer(file, stats)
    FILE *file;
    statement_t *stats;
{
    register statement_t *stat;

    WriteProlog(file);
    for (stat = stats; stat != stNULL; stat = stat->stNext)
	switch (stat->stKind)
	{
	  case skRoutine:
	    WriteRoutine(file, stat->stRoutine);
	    if (IsKernelServer && stat->stRoutine->rtTrap)
		WriteTrapRoutine(file, stat->stRoutine);
	    break;
	  case skImport:
	  case skSImport:
	    WriteImport(file, stat->stFileName);
	    break;
	  case skUImport:
	    break;
	  default:
	    fatal("WriteServer(): bad statement_kind_t (%d)",
		  (int) stat->stKind);
	}
    WriteEpilog(file, stats);
}
