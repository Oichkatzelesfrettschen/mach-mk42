 #
 # Mach Operating System
 # Copyright (c) 1991,1990,1989 Carnegie Mellon University
 # All Rights Reserved.
 # 
 # Permission to use, copy, modify and distribute this software and its
 # documentation is hereby granted, provided that both the copyright
 # notice and this permission notice appear in all copies of the
 # software, derivative works or modified versions, and any portions
 # thereof, and that both notices appear in supporting documentation.
 # 
 # CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 # CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 # ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 # 
 # Carnegie Mellon requests users of this software to return to
 # 
 #  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 #  School of Computer Science
 #  Carnegie Mellon University
 #  Pittsburgh PA 15213-3890
 # 
 # any improvements or extensions that they make and grant Carnegie the
 # rights to redistribute these changes.
 #
 #
 # HISTORY
 # $Log:	crt0.ss,v $
 # Revision 2.5  91/02/14  14:18:29  mrt
 # 	Added new Mach copyright
 # 	[91/02/13  12:50:02  mrt]
 # 
 # Revision 2.4  90/11/05  14:35:56  rpd
 # 	Removed the definition of exit.
 # 	[90/10/30            rpd]
 # 
 # Revision 2.3  90/06/02  15:12:57  rpd
 # 	Converted to new IPC.
 # 	[90/03/26  23:28:04  rpd]
 # 
 # Revision 2.2  90/01/19  14:36:34  rwd
 # 	Version from sandro.
 # 	[90/01/18            rwd]
 # 
 #

	.verstamp	1 31
	.comm	environ 4
	.comm	mach_init_routine 4
	.comm	_cthread_init_routine 4
	.comm	_cthread_exit_routine 4
	.text	
	.align	2
	.file	2 "crt0.cv"
	.globl	__start
	.loc	2 49
 #  49	{
	.ent	__start 2
__start:
	la	$28,_gp
	move	$4,$29
	#.option	O2
	subu	$sp, 24
	sw	$31, 20($sp)
	sw	$16, 16($sp)
	.mask	0x80010000, -4
	.frame	$sp, 24, $31
	move	$5, $4
	.loc	2 53
 #  50		register unsigned argc;
 #  51		register va_list argv;
 #  52	
 #  53		argc = va_arg(sp, unsigned);
	addu	$5, $5, 7
	and	$5, $5, -4
	lw	$16, -4($5)
	.loc	2 54
 #  54		argv = sp;
	.loc	2 55
 #  55		environ = argv + (argc + 1) * sizeof(char *);
	mul	$14, $16, 4
	addu	$15, $5, $14
	addu	$24, $15, 4
	sw	$24, environ
	.loc	2 57
 #  56	
 #  57		if (mach_init_routine)
	lw	$25, mach_init_routine
	beq	$25, 0, $32
	.loc	2 58
 #  58			(*mach_init_routine)();
	sw	$5, 24($sp)
	jal	$25
	lw	$5, 24($sp)
$32:
	lw	$2, _cthread_init_routine
	.loc	2 59
 #  59		if (_cthread_init_routine)
	beq	$2, 0, $33
	.loc	2 60
 #  60			(*_cthread_init_routine)();
	sw	$5, 24($sp)
	jal	$2
	lw	$5, 24($sp)

	beq	$2, $0, $33
	subu	$2, 0x14
	move	$29,$2

$33:
	.loc	2 62
 #  61	
 #  62		argc = main(argc, argv, environ);
	move	$4, $16
	lw	$6, environ
	jal	main
	lw	$3, _cthread_exit_routine
	move	$16, $2
	.loc	2 64
 #  63	
 #  64		if (_cthread_exit_routine)
	beq	$3, 0, $34
	.loc	2 65
 #  65			(*_cthread_exit_routine)(argc);
	move	$4, $16
	jal	$3
$34:
	.loc	2 67
 #  66	
 #  67		exit(argc);
	move	$4, $16
	jal	exit
	.loc	2 68
 #  68	}
	lw	$16, 16($sp)
	lw	$31, 20($sp)
	addu	$sp, 24
	j	$31
	.end	__start
