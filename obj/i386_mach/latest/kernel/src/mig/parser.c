# define sySkip 257
# define syRoutine 258
# define sySimpleRoutine 259
# define syCamelotRoutine 260
# define sySimpleProcedure 261
# define syProcedure 262
# define syFunction 263
# define syTrapRoutine 264
# define syTrapSimpleRoutine 265
# define sySubsystem 266
# define syCamelot 267
# define syKernelUser 268
# define syKernelServer 269
# define syMsgOption 270
# define syMsgKind 271
# define syWaitTime 272
# define syNoWaitTime 273
# define syErrorProc 274
# define syServerPrefix 275
# define syUserPrefix 276
# define syRCSId 277
# define syImport 278
# define syUImport 279
# define sySImport 280
# define syIn 281
# define syOut 282
# define syInOut 283
# define syRequestPort 284
# define syReplyPort 285
# define sySReplyPort 286
# define syUReplyPort 287
# define syType 288
# define syArray 289
# define syStruct 290
# define syOf 291
# define syInTran 292
# define syOutTran 293
# define syDestructor 294
# define syCType 295
# define syCUserType 296
# define syCServerType 297
# define syColon 298
# define sySemi 299
# define syComma 300
# define syPlus 301
# define syMinus 302
# define syStar 303
# define syDiv 304
# define syLParen 305
# define syRParen 306
# define syEqual 307
# define syCaret 308
# define syTilde 309
# define syLAngle 310
# define syRAngle 311
# define syLBrack 312
# define syRBrack 313
# define syBar 314
# define syError 315
# define syNumber 316
# define sySymbolicType 317
# define syIdentifier 318
# define syString 319
# define syQString 320
# define syFileName 321
# define syIPCFlag 322

# line 145 "../../../../../../src/30/kernel/src/mig/parser.y"

#include "lexxer.h"
#include "string.h"
#include "type.h"
#include "routine.h"
#include "statement.h"
#include "global.h"

static char *import_name();


# line 157 "../../../../../../src/30/kernel/src/mig/parser.y"
typedef union 
{
    u_int number;
    identifier_t identifier;
    string_t string;
    statement_kind_t statement_kind;
    ipc_type_t *type;
    struct
    {
	u_int innumber;		/* msgt_name value, when sending */
	string_t instr;
	u_int outnumber;	/* msgt_name value, when receiving */
	string_t outstr;
	u_int size;		/* 0 means there is no default size */
    } symtype;
    routine_t *routine;
    arg_kind_t direction;
    argument_t *argument;
    ipc_flags_t flag;
} YYSTYPE;
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 745 "../../../../../../src/30/kernel/src/mig/parser.y"


yyerror(s)
    char *s;
{
    error(s);
}

static char *
import_name(sk)
    statement_kind_t sk;
{
    switch (sk)
    {
      case skImport:
	return "Import";
      case skSImport:
	return "SImport";
      case skUImport:
	return "UImport";
      default:
	fatal("import_name(%d): not import statement", (int) sk);
	/*NOTREACHED*/
    }
}
short yyexca[] ={
-1, 1,
	0, -1,
	270, 110,
	271, 110,
	272, 110,
	277, 112,
	278, 111,
	279, 111,
	280, 111,
	-2, 0,
	};
# define YYNPROD 113
# define YYLAST 358
short yyact[]={

 112, 113, 212,  88,  81,  82,  83,  89, 164,  86,
 167,  85,  84, 221, 216, 214, 110, 213, 163, 107,
 208, 166, 194, 148, 192, 191, 177, 115, 116, 111,
 176, 175, 174, 173, 172, 115, 116, 147, 161, 160,
 152, 151, 133, 134, 135, 136, 137, 138,  77,  76,
  75,  74, 140,  73,  72,  80,  71,  70,  64,  62,
  61, 139,  60, 184, 185, 186, 187, 100, 167, 184,
 185, 186, 187, 146, 178, 211, 129, 130, 128, 166,
 182, 189, 184, 185, 186, 187, 220, 121, 122, 123,
 124, 125, 126, 127, 183, 181,  16,  12,  35,  36,
  37,  39,  38,  40,  41,  42,  34,  87, 184, 185,
 186, 187, 117,  19,  20,  21,  22, 184, 185, 186,
 187, 205, 203, 219, 218, 215, 149, 210,  23, 186,
 187, 205, 207, 206, 193,  91, 162, 150,  55,  15,
  54,  53,  52,  51,  50,  49,  48,  47,  46,  45,
  44,  43, 209, 132, 159, 158, 157, 156, 155, 154,
 129, 130, 128, 217, 204, 198, 196, 180,  69, 165,
 190, 121, 122, 123, 124, 125, 126, 127,  66,  67,
  68,  57,  58,  59, 131, 114,  63, 118, 102, 109,
  90,  33,  32,  18,  79,  99,  78,  56,  17,  14,
  13,  10,   9,   8,   7,   6,   5,   4,   3,   2,
   1, 119, 120,  31,  30,  29,  28,  27,  26,  25,
  24,  11, 104, 103, 101, 108, 106, 105,  65,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  92,  93,  94,  95,  96,  97,  98,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0, 141, 142, 143, 144,   0,   0,
 145,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0, 168,   0, 153,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 179,   0, 169,   0, 171, 188, 170,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 195,   0, 197,   0, 199, 200, 201, 202 };
short yypact[]={

-1000,-160,-1000,-148,-149,-150,-151,-152,-153,-154,
-155,-156,-157,-158,-159,-1000,-161,-1000, -89,-1000,
-256,-258,-259,-260,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-100,-109,-1000,-261,-262,-264,-265,-267,
-268,-269,-270,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-263,-307,-308,-310,
-1000,-1000,-1000,-1000,-200,-318,-1000,-1000,-1000,-313,
-170,-170,-170,-170,-170,-170,-170,-170,-249,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-289,-1000,-1000,
-1000,-194,-1000,-1000,-1000,-1000,-145,-1000,-1000,-1000,
-1000,-250,-1000,-1000,-1000,-289,-289,-289,-289,-1000,
-281,-1000,-239,-275,-291,-1000,-1000,-1000,-180,-162,
-277,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-278,-139,-140,-141,-142,-143,-144,-279,
-280,-1000,-1000,-1000,-1000,-164,-295,-237,-281,-1000,
-110,-145,-200,-1000,-284,-285,-286,-287,-288,-292,
-236,-1000,-237,-124,-218,-219,-1000,-237,-232,-1000,
-1000,-1000,-293,-294,-171,-1000,-1000,-1000,-296,-193,
-1000,-125,-237,-126,-237,-237,-237,-237,-184,-127,
-169,-172,-173,-298,-146,-179,-1000,-238,-1000,-174,
-174,-1000,-1000,-1000,-1000,-320,-301,-303,-181,-304,
-1000,-128,-1000,-182,-183,-1000,-223,-1000,-1000,-1000,
-305,-1000 };
short yypgo[]={

   0, 228, 227, 226, 225, 169, 186, 224, 188, 223,
 222, 184, 185, 189, 221, 220, 219, 218, 217, 216,
 215, 214, 213, 212, 211, 190, 187, 170, 210, 209,
 208, 207, 206, 205, 204, 203, 202, 201, 200, 199,
 198, 197, 196, 195, 194, 193, 192, 191 };
short yyr1[]={

   0,  28,  28,  29,  29,  29,  29,  29,  29,  29,
  29,  29,  29,  29,  29,  29,  29,  30,  40,  41,
  41,  44,  44,  44,  42,  43,  32,  33,  31,  31,
  34,  35,  36,  38,   1,   1,   1,  39,  37,   6,
   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
   8,   8,   8,   8,   8,   8,   9,   9,  27,  27,
  12,  12,  13,  13,  10,   2,   2,   2,   3,   4,
   5,   5,   5,   5,   5,   5,  14,  14,  14,  14,
  14,  14,  14,  14,  15,  16,  17,  18,  19,  20,
  21,  22,  25,  25,  26,  26,  24,  23,  23,  23,
  23,  23,  23,  23,  23,  23,  23,  23,  11,  11,
  45,  46,  47 };
short yyr2[]={

   0,   0,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   1,   2,   4,   1,   0,
   2,   1,   1,   1,   1,   1,   3,   3,   3,   1,
   2,   2,   2,   3,   1,   1,   1,   3,   2,   3,
   1,   8,   8,   7,   4,   4,   4,   7,   9,   3,
   1,   1,   2,   2,   2,   2,   1,   6,   0,   3,
   1,   1,   1,   3,   1,   4,   5,   7,   5,   5,
   3,   3,   3,   3,   1,   3,   1,   1,   1,   1,
   1,   1,   1,   1,   3,   3,   3,   3,   3,   4,
   3,   3,   2,   3,   1,   3,   4,   0,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,
   0,   0,   0 };
short yychk[]={

-1000, -28, -29, -30, -31, -32, -33, -34, -35, -36,
 -37, -14, 257, -38, -39, 299, 256, -40, -45, 273,
 274, 275, 276, 288, -15, -16, -17, -18, -19, -20,
 -21, -22, -46, -47, 266, 258, 259, 260, 262, 261,
 263, 264, 265, 299, 299, 299, 299, 299, 299, 299,
 299, 299, 299, 299, 299, 299, -41, 270, 271, 272,
 318, 318, 318,  -6, 318,  -1, 278, 279, 280, 277,
 318, 318, 318, 318, 318, 318, 318, 318, -42, -44,
 318, 267, 268, 269, 319, 319, 319, 307, 321, 320,
 -25, 305, -25, -25, -25, -25, -25, -25, -25, -43,
 316,  -7,  -8,  -9, -10,  -2,  -3, 308,  -4, -13,
 305, 318, 289, 290, -12, 316, 317, 306, -26, -24,
 -23, 281, 282, 283, 284, 285, 286, 287, 272, 270,
 271, -11, 298, 292, 293, 294, 295, 296, 297, 311,
 302,  -8,  -8,  -8,  -8, -13, 312, 312, 314, 306,
 299, 318, 318,  -6, 298, 298, 298, 298, 298, 298,
 318, 318, 300, 313, 303,  -5, 316, 305,  -5, -12,
 -26, -11, 318, 318, 318, 318, 318, 318, 310,  -5,
 291, 313, 298, 313, 301, 302, 303, 304,  -5, 313,
 -27, 318, 318, 305, 318, -27, 291,  -5, 291,  -5,
  -5,  -5,  -5, 306, 291, 300, 305, 305, 318, 298,
 306, 313, 322, 318, 318, 306, 318, 291, 306, 306,
 309, 318 };
short yydef[]={

   1,  -2,   2,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  15,   0,  19,   0,  29,
   0,   0,   0,   0,  76,  77,  78,  79,  80,  81,
  82,  83,   0,   0,  18,   0,   0,   0,   0,   0,
   0,   0,   0,   3,   4,   5,   6,   7,   8,   9,
  10,  11,  12,  13,  14,  16,   0,   0,   0,   0,
  30,  31,  32,  38,   0,   0,  34,  35,  36,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  20,
  24,  21,  22,  23,  26,  27,  28,   0,  33,  37,
  84,  97,  85,  86,  87,  88,   0,  90,  91,  17,
  25,  39,  40,  50,  51,   0,   0,   0,   0,  56,
   0,  64,   0,   0,  62,  60,  61,  92,   0,  94,
   0,  98,  99, 100, 101, 102, 103, 104, 105, 106,
 107,  89,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  52,  53,  54,  55,   0,   0,   0,   0,  93,
  97,   0, 108, 109,   0,   0,   0,   0,   0,   0,
   0,  49,   0,   0,   0,   0,  74,   0,   0,  63,
  95,  58,   0,   0,   0,  44,  45,  46,   0,  58,
  65,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  96,   0,   0,   0,   0,   0,  66,   0,  68,  70,
  71,  72,  73,  75,  69,   0,   0,   0,   0,   0,
  57,   0,  59,   0,   0,  43,  47,  67,  41,  42,
   0,  48 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 11:
# line 193 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    register statement_t *st = stAlloc();

    st->stKind = skRoutine;
    st->stRoutine = yypvt[-1].routine;
    rtCheckRoutine(yypvt[-1].routine);
    if (BeVerbose)
	rtPrintRoutine(yypvt[-1].routine);
} break;
case 12:
# line 203 "../../../../../../src/30/kernel/src/mig/parser.y"
{ rtSkip(); } break;
case 16:
# line 208 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyerrok; } break;
case 17:
# line 213 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (BeVerbose)
    {
	printf("Subsystem %s: base = %u%s%s%s\n\n",
	       SubsystemName, SubsystemBase,
	       IsCamelot ? ", Camelot" : "",
	       IsKernelUser ? ", KernelUser" : "",
	       IsKernelServer ? ", KernelServer" : "");
    }
} break;
case 18:
# line 226 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (SubsystemName != strNULL)
    {
	warn("previous Subsystem decl (of %s) will be ignored", SubsystemName);
	IsCamelot = FALSE;
	IsKernelUser = FALSE;
	IsKernelServer = FALSE;
	strfree(SubsystemName);
    }
} break;
case 21:
# line 243 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (IsCamelot)
	warn("duplicate Camelot keyword");
    IsCamelot = TRUE;
} break;
case 22:
# line 249 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (IsKernelUser)
	warn("duplicate KernelUser keyword");
    IsKernelUser = TRUE;
} break;
case 23:
# line 255 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (IsKernelServer)
	warn("duplicate KernelServer keyword");
    IsKernelServer = TRUE;
} break;
case 24:
# line 262 "../../../../../../src/30/kernel/src/mig/parser.y"
{ SubsystemName = yypvt[-0].identifier; } break;
case 25:
# line 265 "../../../../../../src/30/kernel/src/mig/parser.y"
{ SubsystemBase = yypvt[-0].number; } break;
case 26:
# line 269 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (streql(yypvt[-0].string, "MACH_MSG_OPTION_NONE"))
    {
	MsgOption = strNULL;
	if (BeVerbose)
	    printf("MsgOption: canceled\n\n");
    }
    else
    {
	MsgOption = yypvt[-0].string;
	if (BeVerbose)
	    printf("MsgOption %s\n\n",yypvt[-0].string);
    }
} break;
case 27:
# line 286 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (streql(yypvt[-0].string, "MACH_MSGH_KIND_NORMAL"))
    {
	MsgKind = strNULL;
	if (BeVerbose)
	    printf("MsgKind: canceled\n\n");
    }
    else
    {
	MsgKind = yypvt[-0].string;
	if (BeVerbose)
	    printf("MsgKind %s\n\n",yypvt[-0].string);
    }
} break;
case 28:
# line 303 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    WaitTime = yypvt[-0].string;
    if (BeVerbose)
	printf("WaitTime %s\n\n", WaitTime);
} break;
case 29:
# line 309 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    WaitTime = strNULL;
    if (BeVerbose)
	printf("NoWaitTime\n\n");
} break;
case 30:
# line 317 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    ErrorProc = yypvt[-0].identifier;
    if (BeVerbose)
	printf("ErrorProc %s\n\n", ErrorProc);
} break;
case 31:
# line 325 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    ServerPrefix = yypvt[-0].identifier;
    if (BeVerbose)
	printf("ServerPrefix %s\n\n", ServerPrefix);
} break;
case 32:
# line 333 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    UserPrefix = yypvt[-0].identifier;
    if (BeVerbose)
	printf("UserPrefix %s\n\n", UserPrefix);
} break;
case 33:
# line 341 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    register statement_t *st = stAlloc();
    st->stKind = yypvt[-1].statement_kind;
    st->stFileName = yypvt[-0].string;

    if (BeVerbose)
	printf("%s %s\n\n", import_name(yypvt[-1].statement_kind), yypvt[-0].string);
} break;
case 34:
# line 351 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.statement_kind = skImport; } break;
case 35:
# line 352 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.statement_kind = skUImport; } break;
case 36:
# line 353 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.statement_kind = skSImport; } break;
case 37:
# line 357 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (RCSId != strNULL)
	warn("previous RCS decl will be ignored");
    if (BeVerbose)
	printf("RCSId %s\n\n", yypvt[-0].string);
    RCSId = yypvt[-0].string;
} break;
case 38:
# line 367 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    register identifier_t name = yypvt[-0].type->itName;

    if (itLookUp(name) != itNULL)
	warn("overriding previous definition of %s", name);
    itInsert(name, yypvt[-0].type);
} break;
case 39:
# line 377 "../../../../../../src/30/kernel/src/mig/parser.y"
{ itTypeDecl(yypvt[-2].identifier, yyval.type = yypvt[-0].type); } break;
case 40:
# line 381 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itResetType(yypvt[-0].type); } break;
case 41:
# line 384 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-7].type;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yypvt[-4].identifier))
	warn("conflicting translation types (%s, %s)",
	     yyval.type->itTransType, yypvt[-4].identifier);
    yyval.type->itTransType = yypvt[-4].identifier;

    if ((yyval.type->itInTrans != strNULL) && !streql(yyval.type->itInTrans, yypvt[-3].identifier))
	warn("conflicting in-translation functions (%s, %s)",
	     yyval.type->itInTrans, yypvt[-3].identifier);
    yyval.type->itInTrans = yypvt[-3].identifier;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yypvt[-1].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yypvt[-1].identifier);
    yyval.type->itServerType = yypvt[-1].identifier;
} break;
case 42:
# line 404 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-7].type;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yypvt[-4].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yypvt[-4].identifier);
    yyval.type->itServerType = yypvt[-4].identifier;

    if ((yyval.type->itOutTrans != strNULL) && !streql(yyval.type->itOutTrans, yypvt[-3].identifier))
	warn("conflicting out-translation functions (%s, %s)",
	     yyval.type->itOutTrans, yypvt[-3].identifier);
    yyval.type->itOutTrans = yypvt[-3].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yypvt[-1].identifier))
	warn("conflicting translation types (%s, %s)",
	     yyval.type->itTransType, yypvt[-1].identifier);
    yyval.type->itTransType = yypvt[-1].identifier;
} break;
case 43:
# line 424 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-6].type;

    if ((yyval.type->itDestructor != strNULL) && !streql(yyval.type->itDestructor, yypvt[-3].identifier))
	warn("conflicting destructor functions (%s, %s)",
	     yyval.type->itDestructor, yypvt[-3].identifier);
    yyval.type->itDestructor = yypvt[-3].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yypvt[-1].identifier))
	warn("conflicting translation types (%s, %s)",
	     yyval.type->itTransType, yypvt[-1].identifier);
    yyval.type->itTransType = yypvt[-1].identifier;
} break;
case 44:
# line 438 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-3].type;

    if ((yyval.type->itUserType != strNULL) && !streql(yyval.type->itUserType, yypvt[-0].identifier))
	warn("conflicting user types (%s, %s)",
	     yyval.type->itUserType, yypvt[-0].identifier);
    yyval.type->itUserType = yypvt[-0].identifier;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yypvt[-0].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yypvt[-0].identifier);
    yyval.type->itServerType = yypvt[-0].identifier;
} break;
case 45:
# line 452 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-3].type;

    if ((yyval.type->itUserType != strNULL) && !streql(yyval.type->itUserType, yypvt[-0].identifier))
	warn("conflicting user types (%s, %s)",
	     yyval.type->itUserType, yypvt[-0].identifier);
    yyval.type->itUserType = yypvt[-0].identifier;
} break;
case 46:
# line 462 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = yypvt[-3].type;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yypvt[-0].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yypvt[-0].identifier);
    yyval.type->itServerType = yypvt[-0].identifier;
} break;
case 47:
# line 472 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    warn("obsolete translation spec");
    yyval.type = yypvt[-6].type;

    if ((yyval.type->itInTrans != strNULL) && !streql(yyval.type->itInTrans, yypvt[-4].identifier))
	warn("conflicting in-translation functions (%s, %s)",
	     yyval.type->itInTrans, yypvt[-4].identifier);
    yyval.type->itInTrans = yypvt[-4].identifier;

    if ((yyval.type->itOutTrans != strNULL) && !streql(yyval.type->itOutTrans, yypvt[-2].identifier))
	warn("conflicting out-translation functions (%s, %s)",
	     yyval.type->itOutTrans, yypvt[-2].identifier);
    yyval.type->itOutTrans = yypvt[-2].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yypvt[-0].identifier))
	warn("conflicting translation types (%s, %s)",
	     yyval.type->itTransType, yypvt[-0].identifier);
    yyval.type->itTransType = yypvt[-0].identifier;
} break;
case 48:
# line 494 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    warn("obsolete translation spec");
    yyval.type = yypvt[-8].type;

    if ((yyval.type->itInTrans != strNULL) && !streql(yyval.type->itInTrans, yypvt[-6].identifier))
	warn("conflicting in-translation functions (%s, %s)",
	     yyval.type->itInTrans, yypvt[-6].identifier);
    yyval.type->itInTrans = yypvt[-6].identifier;

    if ((yyval.type->itOutTrans != strNULL) && !streql(yyval.type->itOutTrans, yypvt[-4].identifier))
	warn("conflicting out-translation functions (%s, %s)",
	     yyval.type->itOutTrans, yypvt[-4].identifier);
    yyval.type->itOutTrans = yypvt[-4].identifier;

    if ((yyval.type->itTransType != strNULL) && !streql(yyval.type->itTransType, yypvt[-2].identifier))
	warn("conflicting translation types (%s, %s)",
	     yyval.type->itTransType, yypvt[-2].identifier);
    yyval.type->itTransType = yypvt[-2].identifier;

    if ((yyval.type->itServerType != strNULL) && !streql(yyval.type->itServerType, yypvt[-0].identifier))
	warn("conflicting server types (%s, %s)",
	     yyval.type->itServerType, yypvt[-0].identifier);
    yyval.type->itServerType = yypvt[-0].identifier;
} break;
case 49:
# line 519 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    warn("obsolete destructor spec");
    yyval.type = yypvt[-2].type;

    if ((yyval.type->itDestructor != strNULL) && !streql(yyval.type->itDestructor, yypvt[-0].identifier))
	warn("conflicting destructor functions (%s, %s)",
	     yyval.type->itDestructor, yypvt[-0].identifier);
    yyval.type->itDestructor = yypvt[-0].identifier;
} break;
case 50:
# line 531 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = yypvt[-0].type; } break;
case 51:
# line 533 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = yypvt[-0].type; } break;
case 52:
# line 535 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itVarArrayDecl(yypvt[-1].number, yypvt[-0].type); } break;
case 53:
# line 537 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itArrayDecl(yypvt[-1].number, yypvt[-0].type); } break;
case 54:
# line 539 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itPtrDecl(yypvt[-0].type); } break;
case 55:
# line 541 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itStructDecl(yypvt[-1].number, yypvt[-0].type); } break;
case 56:
# line 545 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = itShortDecl(yypvt[-0].symtype.innumber, yypvt[-0].symtype.instr,
		     yypvt[-0].symtype.outnumber, yypvt[-0].symtype.outstr,
		     yypvt[-0].symtype.size);
} break;
case 57:
# line 552 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = itLongDecl(yypvt[-4].symtype.innumber, yypvt[-4].symtype.instr,
		    yypvt[-4].symtype.outnumber, yypvt[-4].symtype.outstr,
		    yypvt[-4].symtype.size, yypvt[-2].number, yypvt[-1].flag);
} break;
case 58:
# line 560 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.flag = flNone; } break;
case 59:
# line 562 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (yypvt[-2].flag & yypvt[-0].flag)
	warn("redundant IPC flag ignored");
    else
	yyval.flag = yypvt[-2].flag | yypvt[-0].flag;
} break;
case 60:
# line 571 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.symtype.innumber = yyval.symtype.outnumber = yypvt[-0].number;
    yyval.symtype.instr = yyval.symtype.outstr = strNULL;
    yyval.symtype.size = 0;
} break;
case 61:
# line 577 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.symtype = yypvt[-0].symtype; } break;
case 62:
# line 581 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.symtype = yypvt[-0].symtype; } break;
case 63:
# line 583 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    if (yypvt[-2].symtype.size != yypvt[-0].symtype.size)
    {
	if (yypvt[-2].symtype.size == 0)
	    yyval.symtype.size = yypvt[-0].symtype.size;
	else if (yypvt[-0].symtype.size == 0)
	    yyval.symtype.size = yypvt[-2].symtype.size;
	else
	{
	    error("sizes in IPCTypes (%d, %d) aren't equal",
		  yypvt[-2].symtype.size, yypvt[-0].symtype.size);
	    yyval.symtype.size = 0;
	}
    }
    else
	yyval.symtype.size = yypvt[-2].symtype.size;
    yyval.symtype.innumber = yypvt[-2].symtype.innumber;
    yyval.symtype.instr = yypvt[-2].symtype.instr;
    yyval.symtype.outnumber = yypvt[-0].symtype.outnumber;
    yyval.symtype.outstr = yypvt[-0].symtype.outstr;
} break;
case 64:
# line 607 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = itPrevDecl(yypvt[-0].identifier); } break;
case 65:
# line 611 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = 0; } break;
case 66:
# line 613 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = 0; } break;
case 67:
# line 616 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number; } break;
case 68:
# line 620 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number; } break;
case 69:
# line 624 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number; } break;
case 70:
# line 628 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number + yypvt[-0].number;	} break;
case 71:
# line 630 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number - yypvt[-0].number;	} break;
case 72:
# line 632 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number * yypvt[-0].number;	} break;
case 73:
# line 634 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-2].number / yypvt[-0].number;	} break;
case 74:
# line 636 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-0].number;	} break;
case 75:
# line 638 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.number = yypvt[-1].number;	} break;
case 76:
# line 642 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 77:
# line 643 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 78:
# line 644 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 79:
# line 645 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 80:
# line 646 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 81:
# line 647 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 82:
# line 648 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 83:
# line 649 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = yypvt[-0].routine; } break;
case 84:
# line 653 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeRoutine(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 85:
# line 657 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeSimpleRoutine(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 86:
# line 661 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeCamelotRoutine(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 87:
# line 665 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeProcedure(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 88:
# line 669 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeSimpleProcedure(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 89:
# line 673 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeFunction(yypvt[-2].identifier, yypvt[-1].argument, yypvt[-0].type); } break;
case 90:
# line 677 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeTrapRoutine(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 91:
# line 681 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.routine = rtMakeTrapSimpleRoutine(yypvt[-1].identifier, yypvt[-0].argument); } break;
case 92:
# line 685 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.argument = argNULL; } break;
case 93:
# line 687 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.argument = yypvt[-1].argument; } break;
case 94:
# line 692 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.argument = yypvt[-0].argument; } break;
case 95:
# line 694 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.argument = yypvt[-2].argument;
    yyval.argument->argNext = yypvt[-0].argument;
} break;
case 96:
# line 701 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.argument = argAlloc();
    yyval.argument->argKind = yypvt[-3].direction;
    yyval.argument->argName = yypvt[-2].identifier;
    yyval.argument->argType = yypvt[-1].type;
    yyval.argument->argFlags = yypvt[-0].flag;
} break;
case 97:
# line 710 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akNone; } break;
case 98:
# line 711 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akIn; } break;
case 99:
# line 712 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akOut; } break;
case 100:
# line 713 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akInOut; } break;
case 101:
# line 714 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akRequestPort; } break;
case 102:
# line 715 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akReplyPort; } break;
case 103:
# line 716 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akSReplyPort; } break;
case 104:
# line 717 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akUReplyPort; } break;
case 105:
# line 718 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akWaitTime; } break;
case 106:
# line 719 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akMsgOption; } break;
case 107:
# line 720 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.direction = akMsgKind; } break;
case 108:
# line 724 "../../../../../../src/30/kernel/src/mig/parser.y"
{
    yyval.type = itLookUp(yypvt[-0].identifier);
    if (yyval.type == itNULL)
	error("type '%s' not defined", yypvt[-0].identifier);
} break;
case 109:
# line 730 "../../../../../../src/30/kernel/src/mig/parser.y"
{ yyval.type = yypvt[-0].type; } break;
case 110:
# line 734 "../../../../../../src/30/kernel/src/mig/parser.y"
{ LookString(); } break;
case 111:
# line 738 "../../../../../../src/30/kernel/src/mig/parser.y"
{ LookFileName(); } break;
case 112:
# line 742 "../../../../../../src/30/kernel/src/mig/parser.y"
{ LookQString(); } break;
		}
		goto yystack;  /* stack new state and value */

	}
