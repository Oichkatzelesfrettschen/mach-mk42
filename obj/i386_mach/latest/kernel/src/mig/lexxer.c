# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin ={stdin}, *yyout ={stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
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
 * $Log:	lexxer.l,v $
 * Revision 2.3  91/02/05  17:54:51  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:54:35  mrt]
 * 
 * Revision 2.2  90/06/02  15:04:54  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:11:34  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 27-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#include <mach/message.h>
#include "string.h"
#include "type.h"
#include "statement.h"
#include "global.h"
#include "parser.h"
#include "lexxer.h"

#ifdef	LDEBUG
#define RETURN(sym)							\
{									\
    printf("yylex: returning 'sym' (%d)\n", (sym));			\
    return (sym);							\
}
#else	LDEBUG
#define RETURN(sym)	return (sym)
#endif	LDEBUG

#define	TPRETURN(intype, outtype, tsize)				\
{									\
    yylval.symtype.innumber = (intype);					\
    yylval.symtype.instr = "intype";					\
    yylval.symtype.outnumber = (outtype);				\
    yylval.symtype.outstr = "outtype";					\
    yylval.symtype.size = (tsize);					\
    RETURN(sySymbolicType);						\
}

#define	TRETURN(type, tsize)	TPRETURN(type,type,tsize)

#define	FRETURN(val)							\
{									\
    yylval.flag = (val);						\
    RETURN(syIPCFlag);							\
}

static struct yysvf *oldYYBegin;

static void doSharp(); /* process body of # directives */
# define Normal 2
# define String 4
# define FileName 6
# define QString 8
# define SkipToEOL 10
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	RETURN(syRoutine);
break;
case 2:
RETURN(syFunction);
break;
case 3:
RETURN(syProcedure);
break;
case 4:
RETURN(sySimpleProcedure);
break;
case 5:
RETURN(sySimpleRoutine);
break;
case 6:
RETURN(syCamelotRoutine);
break;
case 7:
RETURN(syTrapRoutine);
break;
case 8:
RETURN(syTrapSimpleRoutine);
break;
case 9:
RETURN(sySubsystem);
break;
case 10:
RETURN(syMsgOption);
break;
case 11:
	RETURN(syMsgKind);
break;
case 12:
RETURN(syWaitTime);
break;
case 13:
RETURN(syNoWaitTime);
break;
case 14:
			RETURN(syIn);
break;
case 15:
			RETURN(syOut);
break;
case 16:
		RETURN(syInOut);
break;
case 17:
RETURN(syRequestPort);
break;
case 18:
	RETURN(syReplyPort);
break;
case 19:
RETURN(syUReplyPort);
break;
case 20:
RETURN(sySReplyPort);
break;
case 21:
		RETURN(syArray);
break;
case 22:
			RETURN(syOf);
break;
case 23:
		RETURN(syErrorProc);
break;
case 24:
RETURN(syServerPrefix);
break;
case 25:
RETURN(syUserPrefix);
break;
case 26:
		RETURN(syRCSId);
break;
case 27:
	RETURN(syImport);
break;
case 28:
	RETURN(syUImport);
break;
case 29:
	RETURN(sySImport);
break;
case 30:
		RETURN(syType);
break;
case 31:
	RETURN(syCamelot);
break;
case 32:
RETURN(syKernelServer);
break;
case 33:
RETURN(syKernelUser);
break;
case 34:
		RETURN(sySkip);
break;
case 35:
	RETURN(syStruct);
break;
case 36:
	RETURN(syInTran);
break;
case 37:
	RETURN(syOutTran);
break;
case 38:
RETURN(syDestructor);
break;
case 39:
			RETURN(syCType);
break;
case 40:
	RETURN(syCUserType);
break;
case 41:
RETURN(syCServerType);
break;
case 42:
	FRETURN(flLong);
break;
case 43:
FRETURN(flNotLong);
break;
case 44:
	FRETURN(flDealloc);
break;
case 45:
FRETURN(flNotDealloc);
break;
case 46:
TRETURN(MACH_MSG_TYPE_POLYMORPHIC,32);
break;
case 47:
TRETURN(MACH_MSG_TYPE_UNSTRUCTURED,0);
break;
case 48:
	TRETURN(MACH_MSG_TYPE_BIT,1);
break;
case 49:
	TRETURN(MACH_MSG_TYPE_BOOLEAN,32);
break;
case 50:
TRETURN(MACH_MSG_TYPE_INTEGER_16,16);
break;
case 51:
TRETURN(MACH_MSG_TYPE_INTEGER_32,32);
break;
case 52:
	TRETURN(MACH_MSG_TYPE_CHAR,8);
break;
case 53:
	TRETURN(MACH_MSG_TYPE_BYTE,8);
break;
case 54:
TRETURN(MACH_MSG_TYPE_INTEGER_8,8);
break;
case 55:
	TRETURN(MACH_MSG_TYPE_REAL,0);
break;
case 56:
	TRETURN(MACH_MSG_TYPE_STRING,0);
break;
case 57:
TRETURN(MACH_MSG_TYPE_STRING_C,0);
break;
case 58:
TPRETURN(MACH_MSG_TYPE_MOVE_RECEIVE,MACH_MSG_TYPE_PORT_RECEIVE,32);
break;
case 59:
TPRETURN(MACH_MSG_TYPE_COPY_SEND,MACH_MSG_TYPE_PORT_SEND,32);
break;
case 60:
TPRETURN(MACH_MSG_TYPE_MAKE_SEND,MACH_MSG_TYPE_PORT_SEND,32);
break;
case 61:
TPRETURN(MACH_MSG_TYPE_MOVE_SEND,MACH_MSG_TYPE_PORT_SEND,32);
break;
case 62:
TPRETURN(MACH_MSG_TYPE_MAKE_SEND_ONCE,MACH_MSG_TYPE_PORT_SEND_ONCE,32);
break;
case 63:
TPRETURN(MACH_MSG_TYPE_MOVE_SEND_ONCE,MACH_MSG_TYPE_PORT_SEND_ONCE,32);
break;
case 64:
TRETURN(MACH_MSG_TYPE_PORT_NAME,32);
break;
case 65:
TPRETURN(MACH_MSG_TYPE_POLYMORPHIC,MACH_MSG_TYPE_PORT_RECEIVE,32);
break;
case 66:
TPRETURN(MACH_MSG_TYPE_POLYMORPHIC,MACH_MSG_TYPE_PORT_SEND,32);
break;
case 67:
TPRETURN(MACH_MSG_TYPE_POLYMORPHIC,MACH_MSG_TYPE_PORT_SEND_ONCE,32);
break;
case 68:
TRETURN(MACH_MSG_TYPE_POLYMORPHIC,0);
break;
case 69:
	RETURN(syColon);
break;
case 70:
	RETURN(sySemi);
break;
case 71:
	RETURN(syComma);
break;
case 72:
	RETURN(syPlus);
break;
case 73:
	RETURN(syMinus);
break;
case 74:
	RETURN(syStar);
break;
case 75:
	RETURN(syDiv);
break;
case 76:
	RETURN(syLParen);
break;
case 77:
	RETURN(syRParen);
break;
case 78:
	RETURN(syEqual);
break;
case 79:
	RETURN(syCaret);
break;
case 80:
	RETURN(syTilde);
break;
case 81:
	RETURN(syLAngle);
break;
case 82:
	RETURN(syRAngle);
break;
case 83:
	RETURN(syLBrack);
break;
case 84:
	RETURN(syRBrack);
break;
case 85:
	RETURN(syBar);
break;
case 86:
	{ yylval.identifier = strmake(yytext);
			  RETURN(syIdentifier); }
break;
case 87:
{ yylval.number = atoi(yytext); RETURN(syNumber); }
break;
case 88:
{ yylval.string = strmake(yytext);
			  BEGIN Normal; RETURN(syString); }
break;
case 89:
{ yylval.string = strmake(yytext);
			  BEGIN Normal; RETURN(syFileName); }
break;
case 90:
{ yylval.string = strmake(yytext);
			  BEGIN Normal; RETURN(syQString); }
break;
case 91:
{ doSharp(yytext+1);
					  oldYYBegin = yybgin;
					  BEGIN SkipToEOL; }
break;
case 92:
			{ doSharp(yytext+1);
					  oldYYBegin = yybgin;
					  BEGIN SkipToEOL; }
break;
case 93:
				{ yyerror("illegal # directive");
					  oldYYBegin = yybgin;
					  BEGIN SkipToEOL; }
break;
case 94:
	yybgin = oldYYBegin;
break;
case 95:
	;
break;
case 96:
		;
break;
case 97:
		{ BEGIN Normal; RETURN(syError); }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

extern void
LookNormal()
{
    BEGIN Normal;
}

extern void
LookString()
{
    BEGIN String;
}

extern void
LookQString()
{
    BEGIN QString;
}

extern void
LookFileName()
{
    BEGIN FileName;
}

static void
doSharp(body)
    char *body;
{
    register char *startName;

    yylineno = atoi(body);
    startName = index(body, '"');
    if (startName != NULL)
    {
	*rindex(body, '"') = '\0';
	strfree(yyinname);
	yyinname = strmake(startName+1);
    }
}
int yyvstop[] ={
0,

97,
0,

96,
97,
0,

96,
0,

93,
97,
0,

76,
97,
0,

77,
97,
0,

74,
97,
0,

72,
97,
0,

71,
97,
0,

73,
97,
0,

75,
97,
0,

87,
97,
0,

69,
97,
0,

70,
97,
0,

81,
97,
0,

78,
97,
0,

82,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

86,
97,
0,

83,
97,
0,

84,
97,
0,

79,
97,
0,

86,
97,
0,

85,
97,
0,

80,
97,
0,

88,
97,
0,

97,
0,

97,
0,

97,
0,

95,
97,
0,

95,
96,
97,
0,

94,
96,
0,

93,
95,
97,
0,

92,
0,

87,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

14,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

22,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

88,
0,

89,
0,

90,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

15,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

91,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

34,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

30,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

21,
86,
0,

86,
0,

86,
0,

39,
86,
0,

86,
0,

86,
0,

86,
0,

23,
86,
0,

86,
0,

86,
0,

16,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

26,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

27,
86,
0,

36,
86,
0,

42,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

35,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

31,
86,
0,

86,
0,

86,
0,

44,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

11,
86,
0,

86,
0,

86,
0,

86,
0,

37,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

1,
86,
0,

86,
0,

86,
0,

86,
0,

29,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

28,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

2,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

12,
86,
0,

86,
0,

86,
0,

40,
86,
0,

86,
0,

43,
86,
0,

86,
0,

86,
0,

86,
0,

10,
86,
0,

86,
0,

86,
0,

86,
0,

3,
86,
0,

18,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

9,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

38,
86,
0,

86,
0,

33,
86,
0,

86,
0,

45,
86,
0,

13,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

20,
86,
0,

86,
0,

86,
0,

19,
86,
0,

25,
86,
0,

86,
0,

41,
86,
0,

86,
0,

86,
0,

46,
86,
0,

17,
86,
0,

86,
0,

86,
0,

86,
0,

7,
86,
0,

86,
0,

86,
0,

32,
86,
0,

86,
0,

24,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

5,
86,
0,

86,
0,

6,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

4,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

48,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

8,
86,
0,

86,
0,

53,
86,
0,

52,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

55,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

56,
86,
0,

86,
0,

49,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

57,
86,
0,

86,
0,

59,
86,
0,

86,
0,

86,
0,

54,
86,
0,

60,
86,
0,

86,
0,

61,
86,
0,

86,
0,

64,
86,
0,

86,
0,

66,
86,
0,

86,
0,

50,
86,
0,

51,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

68,
86,
0,

86,
0,

86,
0,

86,
0,

86,
0,

58,
86,
0,

86,
0,

65,
86,
0,

86,
0,

47,
86,
0,

86,
0,

86,
0,

86,
0,

62,
86,
0,

63,
86,
0,

67,
86,
0,
0};
# define YYTYPE int
struct yywork { YYTYPE verify, advance; } yycrank[] ={
0,0,	0,0,	1,13,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,14,	1,15,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,13,	
2,16,	1,13,	7,54,	8,54,	
8,16,	9,56,	10,56,	10,16,	
12,60,	0,0,	0,0,	0,0,	
0,0,	1,13,	24,64,	24,64,	
24,64,	24,64,	24,64,	24,64,	
24,64,	24,64,	24,64,	24,64,	
0,0,	0,0,	0,0,	1,13,	
7,55,	8,55,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	1,13,	
1,13,	1,13,	1,13,	3,13,	
101,0,	31,65,	63,107,	32,67,	
33,71,	34,72,	103,0,	3,14,	
3,15,	104,0,	0,0,	0,0,	
35,65,	0,0,	0,0,	35,73,	
37,77,	33,65,	50,65,	50,79,	
32,65,	32,68,	32,69,	32,70,	
0,0,	63,107,	0,0,	63,108,	
0,0,	37,65,	65,65,	0,0,	
3,13,	31,65,	3,13,	32,67,	
33,71,	34,72,	3,17,	3,18,	
3,19,	3,20,	3,21,	3,22,	
35,65,	3,23,	3,24,	35,73,	
37,77,	33,65,	50,65,	50,79,	
32,65,	32,68,	32,69,	32,70,	
3,25,	3,26,	3,27,	3,28,	
3,29,	37,65,	65,65,	3,30,	
3,31,	3,32,	3,33,	3,34,	
3,35,	3,31,	3,31,	3,36,	
3,31,	3,37,	3,31,	3,38,	
3,39,	3,40,	3,41,	3,31,	
3,42,	3,43,	3,44,	3,45,	
3,31,	3,46,	3,31,	3,31,	
4,16,	3,47,	0,0,	3,48,	
3,49,	4,17,	4,18,	4,19,	
4,20,	4,21,	4,22,	39,80,	
4,23,	66,109,	39,65,	36,74,	
36,75,	40,81,	46,99,	3,50,	
36,65,	36,76,	38,78,	4,25,	
4,26,	4,27,	4,28,	0,0,	
0,0,	40,65,	41,83,	0,0,	
40,82,	41,84,	3,51,	46,65,	
3,52,	16,61,	72,116,	38,65,	
38,79,	68,111,	0,0,	39,80,	
0,0,	66,109,	39,65,	36,74,	
36,75,	40,81,	46,99,	0,0,	
36,65,	36,76,	68,65,	77,123,	
4,47,	0,0,	4,48,	4,49,	
16,62,	40,65,	41,83,	5,53,	
40,82,	41,84,	0,0,	46,65,	
0,0,	0,0,	72,116,	38,65,	
38,79,	68,111,	4,50,	5,53,	
16,63,	16,63,	16,63,	16,63,	
16,63,	16,63,	16,63,	16,63,	
16,63,	16,63,	68,65,	77,123,	
74,118,	4,51,	74,65,	4,52,	
5,53,	5,53,	5,53,	5,53,	
5,53,	5,53,	5,53,	5,53,	
5,53,	5,53,	5,53,	5,53,	
5,53,	5,53,	5,53,	5,53,	
5,53,	5,53,	5,53,	5,53,	
5,53,	5,53,	5,53,	5,53,	
5,53,	6,16,	6,53,	61,61,	
74,118,	0,0,	74,65,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	6,53,	0,0,	
0,0,	0,0,	0,0,	61,106,	
61,106,	61,106,	61,106,	61,106,	
61,106,	61,106,	61,106,	61,106,	
61,106,	81,65,	0,0,	6,53,	
6,53,	6,53,	6,53,	6,53,	
6,53,	6,53,	6,53,	6,53,	
6,53,	6,53,	6,53,	6,53,	
6,53,	6,53,	6,53,	6,53,	
6,53,	6,53,	6,53,	6,53,	
6,53,	6,53,	6,53,	6,53,	
11,57,	0,0,	42,85,	0,0,	
42,86,	81,65,	43,88,	88,135,	
11,58,	11,59,	43,89,	44,94,	
43,90,	69,65,	42,87,	0,0,	
45,96,	42,65,	44,95,	43,91,	
69,112,	43,92,	43,93,	67,110,	
84,130,	45,97,	45,98,	84,65,	
67,65,	0,0,	70,65,	70,113,	
0,0,	11,57,	42,85,	11,57,	
42,86,	0,0,	43,88,	88,135,	
0,0,	0,0,	43,89,	44,94,	
43,90,	69,65,	42,87,	11,57,	
45,96,	42,65,	44,95,	43,91,	
69,112,	43,92,	43,93,	67,110,	
84,130,	45,97,	45,98,	84,65,	
67,65,	11,57,	70,65,	70,113,	
11,57,	11,57,	11,57,	11,57,	
11,57,	11,57,	11,57,	11,57,	
11,57,	11,57,	11,57,	11,57,	
11,57,	11,57,	11,57,	11,57,	
11,57,	11,57,	11,57,	11,57,	
11,57,	11,57,	11,57,	11,57,	
11,57,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	0,0,	
82,65,	73,117,	82,128,	92,139,	
0,0,	73,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,66,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
82,65,	73,117,	82,128,	92,139,	
30,65,	73,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,66,	
30,65,	30,65,	30,65,	30,65,	
30,65,	30,65,	30,65,	30,65,	
53,100,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	0,0,	75,119,	
85,65,	85,131,	75,65,	0,0,	
75,120,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	75,119,	
85,65,	85,131,	75,65,	53,100,	
75,120,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	53,100,	
53,100,	53,100,	53,100,	54,101,	
78,124,	71,114,	0,0,	76,121,	
83,129,	76,122,	79,125,	54,101,	
54,0,	76,65,	83,65,	80,65,	
91,138,	80,126,	0,0,	78,65,	
80,127,	79,65,	71,65,	71,115,	
86,132,	86,133,	86,65,	87,65,	
89,136,	91,65,	87,134,	111,150,	
0,0,	89,65,	120,159,	0,0,	
54,102,	71,114,	54,101,	76,121,	
83,129,	76,122,	79,125,	0,0,	
0,0,	76,65,	83,65,	80,65,	
91,138,	80,126,	54,101,	78,65,	
80,127,	79,65,	71,65,	71,115,	
86,132,	86,133,	86,65,	87,65,	
89,136,	91,65,	87,134,	111,150,	
54,101,	89,65,	120,159,	54,101,	
54,101,	54,101,	54,101,	54,101,	
54,101,	54,101,	54,101,	54,101,	
54,101,	54,101,	54,101,	54,101,	
54,101,	54,101,	54,101,	54,101,	
54,101,	54,101,	54,101,	54,101,	
54,101,	54,101,	54,101,	54,101,	
55,103,	107,107,	90,137,	0,0,	
93,140,	94,141,	97,144,	99,146,	
55,103,	55,0,	95,142,	90,65,	
95,65,	96,143,	98,145,	0,0,	
99,65,	110,149,	96,65,	97,65,	
93,65,	112,151,	94,65,	112,65,	
107,107,	114,153,	107,108,	98,65,	
0,0,	0,0,	110,65,	114,65,	
0,0,	55,103,	90,137,	55,103,	
93,140,	94,141,	97,144,	99,146,	
0,0,	0,0,	95,142,	90,65,	
95,65,	96,143,	98,145,	55,103,	
99,65,	110,149,	96,65,	97,65,	
93,65,	112,151,	94,65,	112,65,	
0,0,	114,153,	115,65,	98,65,	
115,154,	55,102,	110,65,	114,65,	
55,103,	55,103,	55,103,	55,103,	
55,103,	55,103,	55,103,	55,103,	
55,103,	55,103,	55,103,	55,103,	
55,103,	55,103,	55,103,	55,103,	
55,103,	55,103,	55,103,	55,103,	
55,103,	55,103,	55,103,	55,103,	
55,103,	56,104,	115,65,	124,163,	
115,154,	0,0,	140,65,	140,180,	
116,155,	56,104,	56,0,	116,65,	
106,107,	124,65,	109,148,	118,157,	
0,0,	117,156,	118,65,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	145,185,	0,0,	109,65,	
117,65,	0,0,	56,105,	106,107,	
56,104,	106,108,	140,65,	140,180,	
116,155,	0,0,	0,0,	116,65,	
0,0,	124,65,	109,148,	118,157,	
56,104,	117,156,	118,65,	106,106,	
106,106,	106,106,	106,106,	106,106,	
106,106,	106,106,	106,106,	106,106,	
106,106,	145,185,	56,104,	109,65,	
117,65,	56,104,	56,104,	56,104,	
56,104,	56,104,	56,104,	56,104,	
56,104,	56,104,	56,104,	56,104,	
56,104,	56,104,	56,104,	56,104,	
56,104,	56,104,	56,104,	56,104,	
56,104,	56,104,	56,104,	56,104,	
56,104,	56,104,	108,108,	0,0,	
119,65,	0,0,	113,152,	119,158,	
126,166,	121,160,	108,108,	108,108,	
121,65,	122,161,	123,162,	127,167,	
122,65,	0,0,	123,65,	113,65,	
125,164,	128,65,	126,65,	128,168,	
125,165,	133,65,	129,65,	125,65,	
133,173,	152,191,	154,193,	0,0,	
127,65,	129,169,	0,0,	108,147,	
119,65,	108,108,	113,152,	119,158,	
126,166,	121,160,	0,0,	0,0,	
121,65,	122,161,	123,162,	127,167,	
122,65,	108,108,	123,65,	113,65,	
125,164,	128,65,	126,65,	128,168,	
125,165,	133,65,	129,65,	125,65,	
133,173,	152,191,	154,193,	108,108,	
127,65,	129,169,	108,108,	108,108,	
108,108,	108,108,	108,108,	108,108,	
108,108,	108,108,	108,108,	108,108,	
108,108,	108,108,	108,108,	108,108,	
108,108,	108,108,	108,108,	108,108,	
108,108,	108,108,	108,108,	108,108,	
108,108,	108,108,	108,108,	130,170,	
131,171,	132,172,	134,65,	136,176,	
134,174,	136,65,	135,65,	132,65,	
142,182,	131,65,	135,175,	137,177,	
155,194,	137,65,	130,65,	138,178,	
139,65,	138,65,	141,181,	139,179,	
141,65,	142,65,	143,183,	144,184,	
143,65,	144,65,	146,65,	0,0,	
146,186,	0,0,	0,0,	130,170,	
131,171,	132,172,	134,65,	136,176,	
134,174,	136,65,	135,65,	132,65,	
142,182,	131,65,	135,175,	137,177,	
155,194,	137,65,	130,65,	138,178,	
139,65,	138,65,	141,181,	139,179,	
141,65,	142,65,	143,183,	144,184,	
143,65,	144,65,	146,65,	148,65,	
146,186,	149,188,	151,190,	153,192,	
150,65,	157,196,	148,187,	149,65,	
150,189,	153,65,	156,65,	158,65,	
156,195,	158,197,	0,0,	151,65,	
160,199,	161,65,	0,0,	161,200,	
160,65,	0,0,	165,204,	164,203,	
165,65,	0,0,	0,0,	168,207,	
0,0,	159,198,	0,0,	148,65,	
164,65,	149,188,	151,190,	153,192,	
150,65,	157,196,	148,187,	149,65,	
150,189,	153,65,	156,65,	158,65,	
156,195,	158,197,	159,65,	151,65,	
160,199,	161,65,	162,201,	161,200,	
160,65,	163,65,	165,204,	164,203,	
165,65,	166,205,	167,206,	168,207,	
169,208,	159,198,	170,209,	162,65,	
164,65,	169,65,	163,202,	167,65,	
173,212,	171,210,	166,65,	174,213,	
172,65,	175,214,	177,65,	170,65,	
179,218,	180,65,	159,65,	172,211,	
174,65,	173,65,	162,201,	171,65,	
180,219,	163,65,	175,65,	178,217,	
0,0,	166,205,	167,206,	179,65,	
169,208,	178,65,	170,209,	162,65,	
182,65,	169,65,	187,65,	167,65,	
173,212,	171,210,	166,65,	174,213,	
172,65,	175,214,	177,65,	170,65,	
179,218,	180,65,	190,65,	172,211,	
174,65,	173,65,	176,215,	171,65,	
180,219,	176,216,	175,65,	178,217,	
176,65,	181,220,	181,221,	179,65,	
183,222,	178,65,	184,223,	183,65,	
182,65,	185,224,	187,65,	185,65,	
184,65,	186,65,	188,226,	186,225,	
194,65,	188,65,	189,227,	191,65,	
192,229,	191,228,	190,65,	192,65,	
193,65,	197,65,	176,215,	193,230,	
195,231,	176,216,	0,0,	189,65,	
176,65,	181,220,	181,221,	0,0,	
183,222,	195,65,	184,223,	183,65,	
196,65,	185,224,	196,232,	185,65,	
184,65,	186,65,	188,226,	186,225,	
194,65,	188,65,	189,227,	191,65,	
192,229,	191,228,	198,233,	192,65,	
193,65,	197,65,	198,65,	193,230,	
195,231,	200,235,	199,234,	189,65,	
201,236,	205,240,	202,237,	200,65,	
207,242,	195,65,	201,65,	202,65,	
196,65,	199,65,	196,232,	203,238,	
204,65,	208,243,	204,239,	203,65,	
208,65,	206,65,	205,65,	206,241,	
209,244,	207,65,	198,233,	210,65,	
214,248,	211,245,	198,65,	211,65,	
0,0,	200,235,	199,234,	216,250,	
201,236,	205,240,	209,65,	200,65,	
207,242,	0,0,	201,65,	202,65,	
218,65,	199,65,	218,252,	203,238,	
204,65,	208,243,	204,239,	203,65,	
208,65,	206,65,	205,65,	206,241,	
209,244,	207,65,	213,247,	210,65,	
214,248,	211,245,	213,65,	211,65,	
212,65,	212,246,	215,249,	216,250,	
217,65,	222,256,	209,65,	219,65,	
219,253,	221,255,	220,254,	217,251,	
218,65,	220,65,	218,252,	215,65,	
223,65,	224,258,	221,65,	225,259,	
226,65,	227,261,	226,260,	223,257,	
228,65,	0,0,	213,247,	232,65,	
225,65,	229,263,	213,65,	228,262,	
212,65,	212,246,	215,249,	231,265,	
217,65,	222,256,	231,65,	219,65,	
219,253,	221,255,	220,254,	217,251,	
229,65,	220,65,	233,65,	215,65,	
223,65,	224,258,	221,65,	225,259,	
226,65,	227,261,	226,260,	223,257,	
228,65,	230,264,	234,65,	232,65,	
225,65,	229,263,	235,266,	228,262,	
243,275,	235,65,	238,270,	231,265,	
236,65,	236,267,	231,65,	236,268,	
230,65,	237,65,	237,269,	239,271,	
229,65,	240,272,	233,65,	241,65,	
238,65,	241,273,	242,274,	240,65,	
239,65,	244,65,	242,65,	0,0,	
244,276,	230,264,	234,65,	246,65,	
245,277,	246,278,	235,266,	245,65,	
243,275,	235,65,	238,270,	0,0,	
236,65,	236,267,	252,65,	236,268,	
230,65,	237,65,	247,279,	239,271,	
248,280,	240,272,	248,65,	241,65,	
238,65,	241,273,	242,274,	240,65,	
239,65,	244,65,	242,65,	247,65,	
244,276,	0,0,	260,292,	246,65,	
245,277,	246,278,	249,281,	245,65,	
249,282,	250,65,	251,284,	250,283,	
251,65,	253,65,	252,65,	253,285,	
254,65,	258,290,	247,279,	254,286,	
248,280,	255,287,	248,65,	256,65,	
263,65,	256,288,	255,65,	257,289,	
259,291,	257,65,	258,65,	247,65,	
0,0,	259,65,	260,292,	261,65,	
0,0,	261,293,	249,281,	270,65,	
249,282,	250,65,	251,284,	250,283,	
251,65,	253,65,	0,0,	253,285,	
254,65,	258,290,	262,294,	254,286,	
262,65,	255,287,	0,0,	256,65,	
263,65,	256,288,	255,65,	257,289,	
259,291,	257,65,	258,65,	267,298,	
264,65,	259,65,	264,295,	261,65,	
265,296,	261,293,	266,297,	270,65,	
265,65,	272,302,	266,65,	269,300,	
267,65,	268,65,	268,299,	272,65,	
271,301,	273,303,	262,294,	271,65,	
262,65,	274,65,	269,65,	275,304,	
276,305,	275,65,	273,65,	277,306,	
278,307,	279,65,	278,65,	267,298,	
264,65,	280,308,	264,295,	281,309,	
265,296,	283,65,	266,297,	0,0,	
265,65,	272,302,	266,65,	288,65,	
267,65,	268,65,	268,299,	272,65,	
271,301,	273,303,	286,65,	271,65,	
286,313,	274,65,	269,65,	275,304,	
276,305,	275,65,	273,65,	277,306,	
278,307,	279,65,	278,65,	282,310,	
285,312,	280,308,	282,65,	281,309,	
284,311,	283,65,	287,314,	284,65,	
287,65,	289,315,	290,316,	288,65,	
289,65,	285,65,	291,317,	294,320,	
292,318,	293,65,	286,65,	292,65,	
286,313,	296,65,	290,65,	295,321,	
293,319,	298,323,	295,65,	291,65,	
294,65,	0,0,	299,324,	282,310,	
285,312,	0,0,	282,65,	0,0,	
284,311,	0,0,	287,314,	284,65,	
287,65,	289,315,	290,316,	299,65,	
289,65,	285,65,	291,317,	294,320,	
292,318,	293,65,	297,322,	292,65,	
300,65,	296,65,	290,65,	295,321,	
293,319,	298,323,	295,65,	291,65,	
294,65,	297,65,	299,324,	301,326,	
304,329,	300,325,	302,327,	301,65,	
303,328,	302,65,	305,330,	311,336,	
308,333,	303,65,	304,65,	299,65,	
306,65,	307,332,	306,331,	313,338,	
307,65,	0,0,	297,322,	305,65,	
300,65,	308,65,	309,334,	310,65,	
313,65,	309,65,	310,335,	315,340,	
312,337,	297,65,	317,65,	301,326,	
304,329,	312,65,	302,327,	301,65,	
303,328,	302,65,	305,330,	311,336,	
308,333,	303,65,	304,65,	314,339,	
306,65,	307,332,	306,331,	313,338,	
307,65,	314,65,	320,65,	305,65,	
316,341,	308,65,	309,334,	310,65,	
313,65,	309,65,	310,335,	315,340,	
312,337,	316,65,	317,65,	318,65,	
321,344,	312,65,	318,342,	319,343,	
322,65,	319,65,	323,65,	324,346,	
326,65,	327,348,	323,345,	314,339,	
325,65,	328,349,	325,347,	330,65,	
329,350,	314,65,	320,65,	331,65,	
316,341,	332,351,	337,65,	0,0,	
327,65,	329,65,	328,65,	0,0,	
335,65,	316,65,	335,354,	318,65,	
321,344,	0,0,	318,342,	319,343,	
322,65,	319,65,	323,65,	324,346,	
326,65,	327,348,	323,345,	333,352,	
325,65,	328,349,	334,353,	330,65,	
329,350,	344,65,	336,65,	331,65,	
336,355,	332,351,	337,65,	333,65,	
327,65,	329,65,	328,65,	338,356,	
335,65,	334,65,	335,354,	338,65,	
339,357,	340,65,	341,65,	340,358,	
342,65,	346,65,	342,360,	348,65,	
341,359,	343,361,	349,65,	333,352,	
345,362,	339,65,	334,353,	350,364,	
0,0,	344,65,	336,65,	347,65,	
336,355,	355,65,	343,65,	333,65,	
357,370,	345,65,	347,363,	338,356,	
358,65,	334,65,	350,65,	338,65,	
339,357,	340,65,	341,65,	340,358,	
342,65,	346,65,	342,360,	348,65,	
341,359,	343,361,	349,65,	352,366,	
345,362,	339,65,	351,65,	350,364,	
351,365,	354,368,	353,367,	347,65,	
352,65,	355,65,	343,65,	356,369,	
357,370,	345,65,	354,65,	359,65,	
358,65,	360,371,	350,65,	353,65,	
361,65,	362,372,	364,65,	363,373,	
356,65,	363,65,	360,65,	365,65,	
367,375,	366,65,	369,65,	352,366,	
372,65,	374,65,	351,65,	366,374,	
351,365,	354,368,	353,367,	0,0,	
352,65,	368,376,	367,65,	356,369,	
371,378,	368,65,	354,65,	359,65,	
371,65,	360,371,	380,385,	353,65,	
361,65,	362,372,	364,65,	373,379,	
356,65,	363,65,	360,65,	365,65,	
367,375,	366,65,	369,65,	370,377,	
372,65,	374,65,	370,65,	366,374,	
373,65,	375,65,	376,381,	378,383,	
375,380,	368,376,	367,65,	377,65,	
371,378,	368,65,	377,382,	379,65,	
371,65,	381,65,	380,385,	376,65,	
378,65,	382,65,	383,65,	382,386,	
0,0,	0,0,	389,402,	0,0,	
379,384,	385,395,	389,65,	370,377,	
0,0,	0,0,	370,65,	0,0,	
373,65,	375,65,	376,381,	378,383,	
375,380,	0,0,	385,65,	377,65,	
0,0,	391,405,	377,382,	379,65,	
391,65,	381,65,	386,396,	376,65,	
378,65,	382,65,	383,65,	382,386,	
384,387,	384,388,	388,400,	386,65,	
387,397,	385,395,	389,65,	384,389,	
390,403,	388,401,	387,398,	384,390,	
388,65,	387,65,	384,391,	392,406,	
384,392,	384,393,	385,65,	384,394,	
387,399,	393,65,	390,404,	393,407,	
391,65,	390,65,	386,396,	394,408,	
392,65,	395,65,	396,409,	394,65,	
400,413,	0,0,	396,65,	386,65,	
397,65,	398,411,	397,410,	399,65,	
398,65,	399,412,	404,65,	407,421,	
388,65,	387,65,	404,417,	406,420,	
384,65,	400,65,	401,414,	403,416,	
401,65,	393,65,	402,65,	410,65,	
402,415,	390,65,	403,65,	405,418,	
392,65,	395,65,	396,409,	394,65,	
406,65,	405,419,	396,65,	409,423,	
397,65,	408,65,	408,422,	399,65,	
398,65,	411,424,	404,65,	407,65,	
412,425,	413,426,	414,65,	411,65,	
409,65,	400,65,	415,428,	0,0,	
401,65,	414,427,	402,65,	410,65,	
420,433,	412,65,	403,65,	416,429,	
417,430,	418,65,	420,65,	415,65,	
406,65,	405,65,	421,434,	409,423,	
418,431,	408,65,	423,65,	424,436,	
416,65,	417,65,	419,65,	421,65,	
419,432,	413,65,	414,65,	411,65,	
409,65,	422,65,	425,65,	422,435,	
424,65,	426,65,	427,65,	428,438,	
429,65,	412,65,	430,65,	431,441,	
433,65,	418,65,	420,65,	415,65,	
431,65,	432,65,	428,65,	427,437,	
435,444,	429,439,	423,65,	430,440,	
416,65,	417,65,	419,65,	421,65,	
434,443,	436,445,	432,442,	0,0,	
434,65,	422,65,	425,65,	445,457,	
424,65,	426,65,	427,65,	445,65,	
429,65,	447,459,	430,65,	438,447,	
433,65,	0,0,	436,65,	0,0,	
431,65,	432,65,	428,65,	0,0,	
435,65,	437,65,	437,446,	443,455,	
438,65,	439,65,	439,448,	440,449,	
440,450,	446,458,	441,451,	442,452,	
434,65,	441,65,	443,65,	442,453,	
442,454,	444,65,	448,460,	445,65,	
444,456,	447,65,	446,65,	451,463,	
452,464,	449,461,	436,65,	450,462,	
457,65,	453,465,	454,466,	448,65,	
463,474,	437,65,	463,65,	455,65,	
438,65,	439,65,	449,65,	440,65,	
450,65,	452,65,	453,65,	454,65,	
456,468,	441,65,	443,65,	442,65,	
455,467,	444,65,	461,472,	459,65,	
458,469,	460,471,	446,65,	451,65,	
458,65,	460,65,	462,473,	456,65,	
457,65,	0,0,	462,65,	448,65,	
459,470,	461,65,	463,65,	455,65,	
0,0,	465,476,	449,65,	464,475,	
450,65,	452,65,	453,65,	454,65,	
464,65,	466,477,	467,478,	472,485,	
468,65,	466,65,	468,479,	459,65,	
465,65,	469,480,	471,484,	473,486,	
458,65,	460,65,	478,65,	456,65,	
472,65,	467,65,	462,65,	474,487,	
470,481,	461,65,	470,482,	469,65,	
471,65,	473,65,	475,488,	470,483,	
476,489,	474,65,	477,490,	479,65,	
464,65,	480,65,	479,491,	482,493,	
468,65,	466,65,	481,492,	475,65,	
465,65,	476,65,	483,65,	485,495,	
477,65,	484,65,	478,65,	486,65,	
472,65,	467,65,	487,497,	488,65,	
485,65,	470,65,	491,500,	469,65,	
471,65,	473,65,	484,494,	487,65,	
486,496,	474,65,	490,65,	479,65,	
492,65,	480,65,	481,65,	482,65,	
493,65,	489,498,	0,0,	475,65,	
0,0,	476,65,	483,65,	490,499,	
477,65,	484,65,	489,65,	486,65,	
497,504,	504,65,	508,514,	488,65,	
485,65,	470,65,	491,65,	494,501,	
495,65,	498,65,	494,65,	487,65,	
495,502,	498,505,	490,65,	497,65,	
492,65,	508,65,	481,65,	482,65,	
493,65,	496,503,	499,506,	500,507,	
496,65,	499,65,	502,509,	501,508,	
503,510,	507,513,	489,65,	501,65,	
503,65,	504,65,	506,512,	505,511,	
500,65,	509,65,	506,65,	502,65,	
495,65,	498,65,	494,65,	507,65,	
510,515,	511,65,	512,516,	497,65,	
505,65,	508,65,	513,65,	517,65,	
514,517,	515,518,	516,519,	518,65,	
496,65,	499,65,	519,65,	510,65,	
0,0,	512,65,	0,0,	501,65,	
503,65,	514,65,	515,65,	516,65,	
500,65,	509,65,	506,65,	502,65,	
0,0,	0,0,	0,0,	507,65,	
0,0,	511,65,	0,0,	0,0,	
505,65,	0,0,	513,65,	517,65,	
0,0,	0,0,	0,0,	518,65,	
0,0,	0,0,	519,65,	510,65,	
0,0,	512,65,	0,0,	0,0,	
0,0,	514,65,	515,65,	516,65,	
0,0};
struct yysvf yysvec[] ={
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-1,	yysvec+1,	0,	
yycrank+-90,	0,		0,	
yycrank+-145,	yysvec+3,	0,	
yycrank+-207,	yysvec+1,	0,	
yycrank+-262,	yysvec+1,	0,	
yycrank+-4,	yysvec+1,	0,	
yycrank+-5,	yysvec+1,	0,	
yycrank+-7,	yysvec+1,	0,	
yycrank+-8,	yysvec+1,	0,	
yycrank+-351,	0,		0,	
yycrank+-9,	yysvec+11,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+208,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+11,
yycrank+0,	0,		yyvstop+14,
yycrank+0,	0,		yyvstop+17,
yycrank+0,	0,		yyvstop+20,
yycrank+0,	0,		yyvstop+23,
yycrank+0,	0,		yyvstop+26,
yycrank+0,	0,		yyvstop+29,
yycrank+2,	0,		yyvstop+32,
yycrank+0,	0,		yyvstop+35,
yycrank+0,	0,		yyvstop+38,
yycrank+0,	0,		yyvstop+41,
yycrank+0,	0,		yyvstop+44,
yycrank+0,	0,		yyvstop+47,
yycrank+393,	0,		yyvstop+50,
yycrank+11,	yysvec+30,	yyvstop+53,
yycrank+30,	yysvec+30,	yyvstop+56,
yycrank+27,	yysvec+30,	yyvstop+59,
yycrank+15,	yysvec+30,	yyvstop+62,
yycrank+22,	yysvec+30,	yyvstop+65,
yycrank+118,	yysvec+30,	yyvstop+68,
yycrank+39,	yysvec+30,	yyvstop+71,
yycrank+137,	yysvec+30,	yyvstop+74,
yycrank+112,	yysvec+30,	yyvstop+77,
yycrank+127,	yysvec+30,	yyvstop+80,
yycrank+131,	yysvec+30,	yyvstop+83,
yycrank+287,	yysvec+30,	yyvstop+86,
yycrank+289,	yysvec+30,	yyvstop+89,
yycrank+281,	yysvec+30,	yyvstop+92,
yycrank+295,	yysvec+30,	yyvstop+95,
yycrank+133,	yysvec+30,	yyvstop+98,
yycrank+0,	0,		yyvstop+101,
yycrank+0,	0,		yyvstop+104,
yycrank+0,	0,		yyvstop+107,
yycrank+28,	yysvec+30,	yyvstop+110,
yycrank+0,	0,		yyvstop+113,
yycrank+0,	0,		yyvstop+116,
yycrank+480,	0,		yyvstop+119,
yycrank+-602,	0,		yyvstop+122,
yycrank+-691,	0,		yyvstop+124,
yycrank+-780,	0,		yyvstop+126,
yycrank+0,	0,		yyvstop+128,
yycrank+0,	0,		yyvstop+131,
yycrank+0,	0,		yyvstop+135,
yycrank+0,	yysvec+16,	yyvstop+138,
yycrank+267,	yysvec+16,	0,	
yycrank+0,	yysvec+16,	0,	
yycrank+85,	yysvec+16,	yyvstop+142,
yycrank+0,	yysvec+24,	yyvstop+144,
yycrank+40,	yysvec+30,	yyvstop+146,
yycrank+111,	yysvec+30,	yyvstop+148,
yycrank+298,	yysvec+30,	yyvstop+150,
yycrank+152,	yysvec+30,	yyvstop+152,
yycrank+283,	yysvec+30,	yyvstop+154,
yycrank+300,	yysvec+30,	yyvstop+156,
yycrank+540,	yysvec+30,	yyvstop+158,
yycrank+136,	yysvec+30,	yyvstop+160,
yycrank+375,	yysvec+30,	yyvstop+162,
yycrank+188,	yysvec+30,	yyvstop+164,
yycrank+460,	yysvec+30,	yyvstop+166,
yycrank+531,	yysvec+30,	yyvstop+169,
yycrank+153,	yysvec+30,	yyvstop+171,
yycrank+537,	yysvec+30,	yyvstop+173,
yycrank+539,	yysvec+30,	yyvstop+175,
yycrank+533,	yysvec+30,	yyvstop+177,
yycrank+243,	yysvec+30,	yyvstop+179,
yycrank+370,	yysvec+30,	yyvstop+182,
yycrank+532,	yysvec+30,	yyvstop+184,
yycrank+297,	yysvec+30,	yyvstop+186,
yycrank+458,	yysvec+30,	yyvstop+188,
yycrank+544,	yysvec+30,	yyvstop+190,
yycrank+545,	yysvec+30,	yyvstop+192,
yycrank+277,	yysvec+30,	yyvstop+194,
yycrank+551,	yysvec+30,	yyvstop+196,
yycrank+621,	yysvec+30,	yyvstop+198,
yycrank+547,	yysvec+30,	yyvstop+200,
yycrank+373,	yysvec+30,	yyvstop+202,
yycrank+630,	yysvec+30,	yyvstop+204,
yycrank+632,	yysvec+30,	yyvstop+206,
yycrank+622,	yysvec+30,	yyvstop+208,
yycrank+628,	yysvec+30,	yyvstop+210,
yycrank+629,	yysvec+30,	yyvstop+212,
yycrank+637,	yysvec+30,	yyvstop+214,
yycrank+626,	yysvec+30,	yyvstop+216,
yycrank+0,	yysvec+53,	yyvstop+218,
yycrank+-82,	yysvec+54,	0,	
yycrank+0,	0,		yyvstop+220,
yycrank+-88,	yysvec+55,	0,	
yycrank+-91,	yysvec+56,	0,	
yycrank+0,	0,		yyvstop+222,
yycrank+783,	0,		0,	
yycrank+684,	0,		0,	
yycrank+-869,	0,		0,	
yycrank+729,	yysvec+30,	yyvstop+224,
yycrank+640,	yysvec+30,	yyvstop+226,
yycrank+549,	yysvec+30,	yyvstop+228,
yycrank+633,	yysvec+30,	yyvstop+230,
yycrank+805,	yysvec+30,	yyvstop+232,
yycrank+641,	yysvec+30,	yyvstop+234,
yycrank+668,	yysvec+30,	yyvstop+236,
yycrank+709,	yysvec+30,	yyvstop+238,
yycrank+730,	yysvec+30,	yyvstop+240,
yycrank+716,	yysvec+30,	yyvstop+242,
yycrank+790,	yysvec+30,	yyvstop+244,
yycrank+552,	yysvec+30,	yyvstop+246,
yycrank+798,	yysvec+30,	yyvstop+248,
yycrank+802,	yysvec+30,	yyvstop+250,
yycrank+804,	yysvec+30,	yyvstop+252,
yycrank+711,	yysvec+30,	yyvstop+254,
yycrank+813,	yysvec+30,	yyvstop+256,
yycrank+808,	yysvec+30,	yyvstop+258,
yycrank+818,	yysvec+30,	yyvstop+260,
yycrank+807,	yysvec+30,	yyvstop+262,
yycrank+812,	yysvec+30,	yyvstop+265,
yycrank+892,	yysvec+30,	yyvstop+267,
yycrank+887,	yysvec+30,	yyvstop+269,
yycrank+885,	yysvec+30,	yyvstop+271,
yycrank+811,	yysvec+30,	yyvstop+273,
yycrank+880,	yysvec+30,	yyvstop+275,
yycrank+884,	yysvec+30,	yyvstop+277,
yycrank+883,	yysvec+30,	yyvstop+279,
yycrank+891,	yysvec+30,	yyvstop+281,
yycrank+895,	yysvec+30,	yyvstop+283,
yycrank+894,	yysvec+30,	yyvstop+285,
yycrank+704,	yysvec+30,	yyvstop+287,
yycrank+898,	yysvec+30,	yyvstop+289,
yycrank+899,	yysvec+30,	yyvstop+291,
yycrank+902,	yysvec+30,	yyvstop+293,
yycrank+903,	yysvec+30,	yyvstop+295,
yycrank+727,	yysvec+30,	yyvstop+297,
yycrank+904,	yysvec+30,	yyvstop+299,
yycrank+0,	0,		yyvstop+301,
yycrank+937,	yysvec+30,	yyvstop+303,
yycrank+945,	yysvec+30,	yyvstop+305,
yycrank+942,	yysvec+30,	yyvstop+307,
yycrank+953,	yysvec+30,	yyvstop+309,
yycrank+815,	yysvec+30,	yyvstop+311,
yycrank+947,	yysvec+30,	yyvstop+313,
yycrank+816,	yysvec+30,	yyvstop+315,
yycrank+890,	yysvec+30,	yyvstop+317,
yycrank+948,	yysvec+30,	yyvstop+319,
yycrank+943,	yysvec+30,	yyvstop+321,
yycrank+949,	yysvec+30,	yyvstop+323,
yycrank+984,	yysvec+30,	yyvstop+325,
yycrank+958,	yysvec+30,	yyvstop+327,
yycrank+955,	yysvec+30,	yyvstop+329,
yycrank+1001,	yysvec+30,	yyvstop+331,
yycrank+991,	yysvec+30,	yyvstop+333,
yycrank+970,	yysvec+30,	yyvstop+335,
yycrank+962,	yysvec+30,	yyvstop+337,
yycrank+1008,	yysvec+30,	yyvstop+339,
yycrank+1005,	yysvec+30,	yyvstop+341,
yycrank+965,	yysvec+30,	yyvstop+343,
yycrank+1003,	yysvec+30,	yyvstop+345,
yycrank+1013,	yysvec+30,	yyvstop+347,
yycrank+1021,	yysvec+30,	yyvstop+349,
yycrank+1010,	yysvec+30,	yyvstop+351,
yycrank+1019,	yysvec+30,	yyvstop+353,
yycrank+1018,	yysvec+30,	yyvstop+355,
yycrank+1024,	yysvec+30,	yyvstop+357,
yycrank+1058,	yysvec+30,	yyvstop+359,
yycrank+1012,	yysvec+30,	yyvstop+361,
yycrank+1031,	yysvec+30,	yyvstop+364,
yycrank+1029,	yysvec+30,	yyvstop+366,
yycrank+1015,	yysvec+30,	yyvstop+368,
yycrank+1059,	yysvec+30,	yyvstop+370,
yycrank+1034,	yysvec+30,	yyvstop+372,
yycrank+1065,	yysvec+30,	yyvstop+375,
yycrank+1070,	yysvec+30,	yyvstop+377,
yycrank+1069,	yysvec+30,	yyvstop+379,
yycrank+1071,	yysvec+30,	yyvstop+381,
yycrank+1036,	yysvec+30,	yyvstop+383,
yycrank+1075,	yysvec+30,	yyvstop+386,
yycrank+1089,	yysvec+30,	yyvstop+388,
yycrank+1048,	yysvec+30,	yyvstop+390,
yycrank+1077,	yysvec+30,	yyvstop+393,
yycrank+1081,	yysvec+30,	yyvstop+395,
yycrank+1082,	yysvec+30,	yyvstop+397,
yycrank+1074,	yysvec+30,	yyvstop+399,
yycrank+1095,	yysvec+30,	yyvstop+402,
yycrank+1098,	yysvec+30,	yyvstop+404,
yycrank+1083,	yysvec+30,	yyvstop+406,
yycrank+1116,	yysvec+30,	yyvstop+409,
yycrank+1131,	yysvec+30,	yyvstop+411,
yycrank+1125,	yysvec+30,	yyvstop+413,
yycrank+1128,	yysvec+30,	yyvstop+415,
yycrank+1129,	yysvec+30,	yyvstop+417,
yycrank+1137,	yysvec+30,	yyvstop+419,
yycrank+1134,	yysvec+30,	yyvstop+421,
yycrank+1140,	yysvec+30,	yyvstop+423,
yycrank+1139,	yysvec+30,	yyvstop+425,
yycrank+1143,	yysvec+30,	yyvstop+427,
yycrank+1138,	yysvec+30,	yyvstop+429,
yycrank+1156,	yysvec+30,	yyvstop+431,
yycrank+1145,	yysvec+30,	yyvstop+433,
yycrank+1149,	yysvec+30,	yyvstop+436,
yycrank+1182,	yysvec+30,	yyvstop+438,
yycrank+1180,	yysvec+30,	yyvstop+440,
yycrank+1146,	yysvec+30,	yyvstop+442,
yycrank+1197,	yysvec+30,	yyvstop+444,
yycrank+1153,	yysvec+30,	yyvstop+446,
yycrank+1186,	yysvec+30,	yyvstop+448,
yycrank+1162,	yysvec+30,	yyvstop+450,
yycrank+1189,	yysvec+30,	yyvstop+452,
yycrank+1195,	yysvec+30,	yyvstop+454,
yycrank+1200,	yysvec+30,	yyvstop+456,
yycrank+1187,	yysvec+30,	yyvstop+458,
yycrank+1198,	yysvec+30,	yyvstop+460,
yycrank+1199,	yysvec+30,	yyvstop+462,
yycrank+1210,	yysvec+30,	yyvstop+464,
yycrank+1202,	yysvec+30,	yyvstop+466,
yycrank+1203,	yysvec+30,	yyvstop+468,
yycrank+1206,	yysvec+30,	yyvstop+470,
yycrank+1226,	yysvec+30,	yyvstop+472,
yycrank+1254,	yysvec+30,	yyvstop+474,
yycrank+1220,	yysvec+30,	yyvstop+476,
yycrank+1209,	yysvec+30,	yyvstop+478,
yycrank+1228,	yysvec+30,	yyvstop+481,
yycrank+1240,	yysvec+30,	yyvstop+484,
yycrank+1247,	yysvec+30,	yyvstop+487,
yycrank+1250,	yysvec+30,	yyvstop+489,
yycrank+1255,	yysvec+30,	yyvstop+491,
yycrank+1262,	yysvec+30,	yyvstop+493,
yycrank+1266,	yysvec+30,	yyvstop+495,
yycrank+1265,	yysvec+30,	yyvstop+497,
yycrank+1261,	yysvec+30,	yyvstop+499,
yycrank+1268,	yysvec+30,	yyvstop+501,
yycrank+1246,	yysvec+30,	yyvstop+503,
yycrank+1267,	yysvec+30,	yyvstop+505,
yycrank+1277,	yysvec+30,	yyvstop+507,
yycrank+1273,	yysvec+30,	yyvstop+509,
yycrank+1301,	yysvec+30,	yyvstop+511,
yycrank+1292,	yysvec+30,	yyvstop+513,
yycrank+1310,	yysvec+30,	yyvstop+515,
yycrank+1311,	yysvec+30,	yyvstop+517,
yycrank+1314,	yysvec+30,	yyvstop+519,
yycrank+1284,	yysvec+30,	yyvstop+521,
yycrank+1315,	yysvec+30,	yyvstop+524,
yycrank+1318,	yysvec+30,	yyvstop+526,
yycrank+1328,	yysvec+30,	yyvstop+528,
yycrank+1325,	yysvec+30,	yyvstop+530,
yycrank+1331,	yysvec+30,	yyvstop+532,
yycrank+1332,	yysvec+30,	yyvstop+534,
yycrank+1335,	yysvec+30,	yyvstop+536,
yycrank+1304,	yysvec+30,	yyvstop+538,
yycrank+1337,	yysvec+30,	yyvstop+541,
yycrank+1354,	yysvec+30,	yyvstop+543,
yycrank+1326,	yysvec+30,	yyvstop+545,
yycrank+1366,	yysvec+30,	yyvstop+548,
yycrank+1374,	yysvec+30,	yyvstop+550,
yycrank+1376,	yysvec+30,	yyvstop+552,
yycrank+1378,	yysvec+30,	yyvstop+554,
yycrank+1379,	yysvec+30,	yyvstop+556,
yycrank+1388,	yysvec+30,	yyvstop+558,
yycrank+1341,	yysvec+30,	yyvstop+560,
yycrank+1385,	yysvec+30,	yyvstop+563,
yycrank+1381,	yysvec+30,	yyvstop+565,
yycrank+1392,	yysvec+30,	yyvstop+567,
yycrank+1387,	yysvec+30,	yyvstop+569,
yycrank+1391,	yysvec+30,	yyvstop+572,
yycrank+1390,	yysvec+30,	yyvstop+574,
yycrank+1393,	yysvec+30,	yyvstop+576,
yycrank+1396,	yysvec+30,	yyvstop+578,
yycrank+1395,	yysvec+30,	yyvstop+580,
yycrank+1399,	yysvec+30,	yyvstop+583,
yycrank+1401,	yysvec+30,	yyvstop+585,
yycrank+1432,	yysvec+30,	yyvstop+587,
yycrank+1403,	yysvec+30,	yyvstop+589,
yycrank+1437,	yysvec+30,	yyvstop+592,
yycrank+1443,	yysvec+30,	yyvstop+594,
yycrank+1416,	yysvec+30,	yyvstop+596,
yycrank+1438,	yysvec+30,	yyvstop+598,
yycrank+1409,	yysvec+30,	yyvstop+600,
yycrank+1442,	yysvec+30,	yyvstop+603,
yycrank+1452,	yysvec+30,	yyvstop+605,
yycrank+1457,	yysvec+30,	yyvstop+607,
yycrank+1449,	yysvec+30,	yyvstop+609,
yycrank+1447,	yysvec+30,	yyvstop+611,
yycrank+1458,	yysvec+30,	yyvstop+613,
yycrank+1456,	yysvec+30,	yyvstop+615,
yycrank+1451,	yysvec+30,	yyvstop+617,
yycrank+1491,	yysvec+30,	yyvstop+620,
yycrank+1455,	yysvec+30,	yyvstop+622,
yycrank+1473,	yysvec+30,	yyvstop+624,
yycrank+1482,	yysvec+30,	yyvstop+626,
yycrank+1497,	yysvec+30,	yyvstop+628,
yycrank+1499,	yysvec+30,	yyvstop+630,
yycrank+1503,	yysvec+30,	yyvstop+632,
yycrank+1504,	yysvec+30,	yyvstop+634,
yycrank+1513,	yysvec+30,	yyvstop+636,
yycrank+1506,	yysvec+30,	yyvstop+638,
yycrank+1510,	yysvec+30,	yyvstop+640,
yycrank+1515,	yysvec+30,	yyvstop+642,
yycrank+1519,	yysvec+30,	yyvstop+644,
yycrank+1517,	yysvec+30,	yyvstop+646,
yycrank+1501,	yysvec+30,	yyvstop+648,
yycrank+1527,	yysvec+30,	yyvstop+650,
yycrank+1518,	yysvec+30,	yyvstop+652,
yycrank+1543,	yysvec+30,	yyvstop+654,
yycrank+1521,	yysvec+30,	yyvstop+656,
yycrank+1555,	yysvec+30,	yyvstop+658,
yycrank+1524,	yysvec+30,	yyvstop+660,
yycrank+1557,	yysvec+30,	yyvstop+663,
yycrank+1563,	yysvec+30,	yyvstop+665,
yycrank+1544,	yysvec+30,	yyvstop+667,
yycrank+1558,	yysvec+30,	yyvstop+670,
yycrank+1562,	yysvec+30,	yyvstop+672,
yycrank+1564,	yysvec+30,	yyvstop+675,
yycrank+1565,	yysvec+30,	yyvstop+677,
yycrank+1570,	yysvec+30,	yyvstop+679,
yycrank+1566,	yysvec+30,	yyvstop+681,
yycrank+1582,	yysvec+30,	yyvstop+684,
yycrank+1584,	yysvec+30,	yyvstop+686,
yycrank+1583,	yysvec+30,	yyvstop+688,
yycrank+1573,	yysvec+30,	yyvstop+690,
yycrank+1577,	yysvec+30,	yyvstop+693,
yycrank+1579,	yysvec+30,	yyvstop+696,
yycrank+1613,	yysvec+30,	yyvstop+698,
yycrank+1619,	yysvec+30,	yyvstop+700,
yycrank+1586,	yysvec+30,	yyvstop+702,
yycrank+1608,	yysvec+30,	yyvstop+704,
yycrank+1580,	yysvec+30,	yyvstop+706,
yycrank+1621,	yysvec+30,	yyvstop+709,
yycrank+1635,	yysvec+30,	yyvstop+711,
yycrank+1623,	yysvec+30,	yyvstop+713,
yycrank+1624,	yysvec+30,	yyvstop+715,
yycrank+1626,	yysvec+30,	yyvstop+717,
yycrank+1644,	yysvec+30,	yyvstop+719,
yycrank+1607,	yysvec+30,	yyvstop+721,
yycrank+1647,	yysvec+30,	yyvstop+724,
yycrank+1627,	yysvec+30,	yyvstop+726,
yycrank+1641,	yysvec+30,	yyvstop+729,
yycrank+1629,	yysvec+30,	yyvstop+731,
yycrank+1632,	yysvec+30,	yyvstop+734,
yycrank+1652,	yysvec+30,	yyvstop+737,
yycrank+1668,	yysvec+30,	yyvstop+739,
yycrank+1674,	yysvec+30,	yyvstop+741,
yycrank+1685,	yysvec+30,	yyvstop+743,
yycrank+1680,	yysvec+30,	yyvstop+745,
yycrank+1643,	yysvec+30,	yyvstop+747,
yycrank+1690,	yysvec+30,	yyvstop+750,
yycrank+1646,	yysvec+30,	yyvstop+752,
yycrank+1650,	yysvec+30,	yyvstop+754,
yycrank+1681,	yysvec+30,	yyvstop+757,
yycrank+1692,	yysvec+30,	yyvstop+760,
yycrank+1686,	yysvec+30,	yyvstop+762,
yycrank+1687,	yysvec+30,	yyvstop+765,
yycrank+1691,	yysvec+30,	yyvstop+767,
yycrank+1688,	yysvec+30,	yyvstop+769,
yycrank+1693,	yysvec+30,	yyvstop+772,
yycrank+1695,	yysvec+30,	yyvstop+775,
yycrank+1708,	yysvec+30,	yyvstop+777,
yycrank+1711,	yysvec+30,	yyvstop+779,
yycrank+1696,	yysvec+30,	yyvstop+781,
yycrank+1732,	yysvec+30,	yyvstop+784,
yycrank+1714,	yysvec+30,	yyvstop+786,
yycrank+1698,	yysvec+30,	yyvstop+788,
yycrank+1734,	yysvec+30,	yyvstop+791,
yycrank+1699,	yysvec+30,	yyvstop+793,
yycrank+1735,	yysvec+30,	yyvstop+796,
yycrank+1749,	yysvec+30,	yyvstop+798,
yycrank+1741,	yysvec+30,	yyvstop+800,
yycrank+1750,	yysvec+30,	yyvstop+802,
yycrank+1745,	yysvec+30,	yyvstop+804,
yycrank+1716,	yysvec+30,	yyvstop+806,
yycrank+1747,	yysvec+30,	yyvstop+808,
yycrank+1751,	yysvec+30,	yyvstop+811,
yycrank+1752,	yysvec+30,	yyvstop+813,
yycrank+1802,	yysvec+30,	yyvstop+816,
yycrank+1772,	yysvec+30,	yyvstop+818,
yycrank+1789,	yysvec+30,	yyvstop+820,
yycrank+1799,	yysvec+30,	yyvstop+822,
yycrank+1798,	yysvec+30,	yyvstop+824,
yycrank+1760,	yysvec+30,	yyvstop+826,
yycrank+1811,	yysvec+30,	yyvstop+828,
yycrank+1778,	yysvec+30,	yyvstop+830,
yycrank+1814,	yysvec+30,	yyvstop+832,
yycrank+1807,	yysvec+30,	yyvstop+834,
yycrank+1817,	yysvec+30,	yyvstop+836,
yycrank+1815,	yysvec+30,	yyvstop+838,
yycrank+1820,	yysvec+30,	yyvstop+841,
yycrank+1822,	yysvec+30,	yyvstop+843,
yycrank+1826,	yysvec+30,	yyvstop+845,
yycrank+1825,	yysvec+30,	yyvstop+847,
yycrank+1835,	yysvec+30,	yyvstop+849,
yycrank+1838,	yysvec+30,	yyvstop+851,
yycrank+1840,	yysvec+30,	yyvstop+853,
yycrank+1844,	yysvec+30,	yyvstop+855,
yycrank+1828,	yysvec+30,	yyvstop+857,
yycrank+1851,	yysvec+30,	yyvstop+859,
yycrank+1850,	yysvec+30,	yyvstop+861,
yycrank+1829,	yysvec+30,	yyvstop+863,
yycrank+1855,	yysvec+30,	yyvstop+865,
yycrank+1866,	yysvec+30,	yyvstop+867,
yycrank+1841,	yysvec+30,	yyvstop+869,
yycrank+1865,	yysvec+30,	yyvstop+872,
yycrank+1875,	yysvec+30,	yyvstop+874,
yycrank+1863,	yysvec+30,	yyvstop+876,
yycrank+1864,	yysvec+30,	yyvstop+878,
yycrank+1881,	yysvec+30,	yyvstop+880,
yycrank+1890,	yysvec+30,	yyvstop+882,
yycrank+1891,	yysvec+30,	yyvstop+884,
yycrank+1879,	yysvec+30,	yyvstop+886,
yycrank+1892,	yysvec+30,	yyvstop+888,
yycrank+1880,	yysvec+30,	yyvstop+890,
yycrank+1893,	yysvec+30,	yyvstop+892,
yycrank+1899,	yysvec+30,	yyvstop+894,
yycrank+1888,	yysvec+30,	yyvstop+896,
yycrank+1902,	yysvec+30,	yyvstop+899,
yycrank+1900,	yysvec+30,	yyvstop+901,
yycrank+1903,	yysvec+30,	yyvstop+904,
yycrank+1904,	yysvec+30,	yyvstop+907,
yycrank+1916,	yysvec+30,	yyvstop+909,
yycrank+1906,	yysvec+30,	yyvstop+911,
yycrank+1908,	yysvec+30,	yyvstop+913,
yycrank+1914,	yysvec+30,	yyvstop+915,
yycrank+1915,	yysvec+30,	yyvstop+917,
yycrank+1910,	yysvec+30,	yyvstop+919,
yycrank+1930,	yysvec+30,	yyvstop+922,
yycrank+1918,	yysvec+30,	yyvstop+924,
yycrank+1944,	yysvec+30,	yyvstop+926,
yycrank+1951,	yysvec+30,	yyvstop+928,
yycrank+1954,	yysvec+30,	yyvstop+930,
yycrank+1955,	yysvec+30,	yyvstop+932,
yycrank+1957,	yysvec+30,	yyvstop+934,
yycrank+1963,	yysvec+30,	yyvstop+936,
yycrank+1965,	yysvec+30,	yyvstop+938,
yycrank+1964,	yysvec+30,	yyvstop+940,
yycrank+1967,	yysvec+30,	yyvstop+942,
yycrank+1937,	yysvec+30,	yyvstop+944,
yycrank+1972,	yysvec+30,	yyvstop+946,
yycrank+1939,	yysvec+30,	yyvstop+948,
yycrank+1981,	yysvec+30,	yyvstop+950,
yycrank+1988,	yysvec+30,	yyvstop+952,
yycrank+1990,	yysvec+30,	yyvstop+954,
yycrank+1973,	yysvec+30,	yyvstop+956,
yycrank+1991,	yysvec+30,	yyvstop+958,
yycrank+1992,	yysvec+30,	yyvstop+960,
yycrank+1993,	yysvec+30,	yyvstop+962,
yycrank+1985,	yysvec+30,	yyvstop+964,
yycrank+2009,	yysvec+30,	yyvstop+967,
yycrank+1978,	yysvec+30,	yyvstop+969,
yycrank+2006,	yysvec+30,	yyvstop+972,
yycrank+2001,	yysvec+30,	yyvstop+974,
yycrank+2007,	yysvec+30,	yyvstop+976,
yycrank+2015,	yysvec+30,	yyvstop+978,
yycrank+2012,	yysvec+30,	yyvstop+980,
yycrank+1984,	yysvec+30,	yyvstop+982,
yycrank+2026,	yysvec+30,	yyvstop+984,
yycrank+2034,	yysvec+30,	yyvstop+986,
yycrank+2031,	yysvec+30,	yyvstop+988,
yycrank+2043,	yysvec+30,	yyvstop+990,
yycrank+2030,	yysvec+30,	yyvstop+992,
yycrank+2049,	yysvec+30,	yyvstop+994,
yycrank+2079,	yysvec+30,	yyvstop+996,
yycrank+2050,	yysvec+30,	yyvstop+998,
yycrank+2042,	yysvec+30,	yyvstop+1000,
yycrank+2051,	yysvec+30,	yyvstop+1002,
yycrank+2055,	yysvec+30,	yyvstop+1004,
yycrank+2065,	yysvec+30,	yyvstop+1006,
yycrank+2067,	yysvec+30,	yyvstop+1008,
yycrank+2070,	yysvec+30,	yyvstop+1010,
yycrank+2040,	yysvec+30,	yyvstop+1012,
yycrank+2057,	yysvec+30,	yyvstop+1015,
yycrank+2059,	yysvec+30,	yyvstop+1017,
yycrank+2092,	yysvec+30,	yyvstop+1020,
yycrank+2093,	yysvec+30,	yyvstop+1022,
yycrank+2068,	yysvec+30,	yyvstop+1024,
yycrank+2071,	yysvec+30,	yyvstop+1027,
yycrank+2078,	yysvec+30,	yyvstop+1030,
yycrank+2073,	yysvec+30,	yyvstop+1032,
yycrank+2085,	yysvec+30,	yyvstop+1035,
yycrank+2077,	yysvec+30,	yyvstop+1037,
yycrank+2104,	yysvec+30,	yyvstop+1040,
yycrank+2088,	yysvec+30,	yyvstop+1042,
yycrank+2080,	yysvec+30,	yyvstop+1045,
yycrank+2090,	yysvec+30,	yyvstop+1047,
yycrank+2094,	yysvec+30,	yyvstop+1050,
yycrank+2116,	yysvec+30,	yyvstop+1053,
yycrank+2114,	yysvec+30,	yyvstop+1055,
yycrank+2130,	yysvec+30,	yyvstop+1057,
yycrank+2121,	yysvec+30,	yyvstop+1059,
yycrank+2115,	yysvec+30,	yyvstop+1061,
yycrank+2131,	yysvec+30,	yyvstop+1063,
yycrank+2142,	yysvec+30,	yyvstop+1065,
yycrank+2137,	yysvec+30,	yyvstop+1067,
yycrank+2145,	yysvec+30,	yyvstop+1069,
yycrank+2138,	yysvec+30,	yyvstop+1071,
yycrank+2107,	yysvec+30,	yyvstop+1073,
yycrank+2154,	yysvec+30,	yyvstop+1076,
yycrank+2144,	yysvec+30,	yyvstop+1078,
yycrank+2149,	yysvec+30,	yyvstop+1080,
yycrank+2123,	yysvec+30,	yyvstop+1082,
yycrank+2143,	yysvec+30,	yyvstop+1084,
yycrank+2165,	yysvec+30,	yyvstop+1087,
yycrank+2151,	yysvec+30,	yyvstop+1089,
yycrank+2167,	yysvec+30,	yyvstop+1092,
yycrank+2156,	yysvec+30,	yyvstop+1094,
yycrank+2171,	yysvec+30,	yyvstop+1097,
yycrank+2172,	yysvec+30,	yyvstop+1099,
yycrank+2173,	yysvec+30,	yyvstop+1101,
yycrank+2157,	yysvec+30,	yyvstop+1103,
yycrank+2161,	yysvec+30,	yyvstop+1106,
yycrank+2164,	yysvec+30,	yyvstop+1109,
0,	0,	0};
struct yywork *yytop = yycrank+2287;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,'$' ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,'$' ,'$' ,'$' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,'>' ,01  ,
01  ,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,
'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' ,
'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'W' ,
'X' ,'Y' ,'J' ,01  ,01  ,01  ,01  ,'J' ,
01  ,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,
'H' ,'I' ,'J' ,'K' ,'L' ,'M' ,'N' ,'O' ,
'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'W' ,
'X' ,'Y' ,'J' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] ={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	ncform	4.1	83/08/11	*/

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
