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
extern YYSTYPE yylval;
