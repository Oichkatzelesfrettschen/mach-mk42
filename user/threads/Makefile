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
# $Log:	Makefile,v $
# Revision 2.13  91/02/14  14:19:15  mrt
# 	Added new Mach copyright
# 	[91/02/13  12:41:48  mrt]
# 
# Revision 2.12  90/11/05  14:36:32  rpd
# 	Change MACHINE to TARGET_MACHINE.
# 	[90/10/29            rwd]
# 
# Revision 2.11  90/09/09  14:34:39  rpd
# 	Remove call.o
# 	[90/08/24            rwd]
# 
# Revision 2.10  90/06/02  15:13:30  rpd
# 	Compile with -DMACH_IPC_COMPAT=0.
# 	[90/03/26  23:29:39  rpd]
# 
# Revision 2.9  90/05/29  18:40:08  rwd
# 	Fix PMAX_ASFLAGS.
# 	[90/05/22            rwd]
# 
# Revision 2.8  90/05/03  15:54:28  dbg
# 	Use -ES switch to preprocess assembler files.
# 	Not all machines' 'cc' can directly assemble .s files.
# 	[90/04/23            dbg]
# 
# Revision 2.7  90/01/19  14:36:46  rwd
# 	Added call.o
# 	[90/01/03            rwd]
# 
# Revision 2.6  89/12/08  19:53:01  rwd
# 	Use ASFLAGS for assembler, the mips one wants to know which
# 	optimization level the C code was compiled at.
# 	[89/12/06            af]
# 
# 	Changed CFLAGS to remove -g and add -O
# 	[89/10/24            rwd]
# 	Worked over for new merged coroutine-thread version.  Removed
# 	task version.
# 	[89/10/24            rwd]
# 
#

LIB = libthreads.a

COMMONFLAGS = -I. -DMACH_IPC_COMPAT=0
COPTS = -MD $(COMMONFLAGS) $(PROFILE)

CPPFLAGS = ${COPTS}
CFLAGS = -O ${COPTS}

PMAX_ASFLAGS= -O -nocpp
ASFLAGS = ${${TARGET_MACHINE}_ASFLAGS}

I = -c -d -v

# Use the following definition to enable inline expansion.
INLINE = awk -f cthread_inline.awk
# Use the following definition to disable inline expansion.
# INLINE = cat

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -S $*.c
# The Sun assembler requires an input file.
	$(MV) $*.s $*.S
	$(INLINE) $*.S > $*.s
	$(AS) $(ASFLAGS) -o $@ $*.s
	$(RM) $(RMFLAGS) $*.s $*.S

SRCS = cprocs.c cthreads.c malloc.c \
       mig_support.c stack.c sync.c \
       machine/thread.c

OBJS = $(LIB)(cprocs.o) $(LIB)(cthreads.o) $(LIB)(malloc.o) \
       $(LIB)(mig_support.o) $(LIB)(stack.o) $(LIB)(sync.o) \
       $(LIB)(thread.o) $(LIB)(lock.o) $(LIB)(csw.o)

lib: $(LIB)

include: machine ckdir
	mach_install $I cthreads.h $(DSTBASE)/include

machine:
	-if [ -d machine ]; then true; else ln -s $(machine) machine; fi

clean:
	$(RM) $(RMFLAGS) machine lib*.a Makedep* a.out core errs \
			 *.d *.s *.S *.o *.BAK *.CKP */*.BAK */*.CKP

#
# Get inline expansion file before we do anything else.
#
.INIT:	cthread_inline.awk

cthread_inline.awk: machine/cthread_inline.awk
	-if [ -f $@ ]; then chmod 644 $@; else true; fi
	cp -p machine/$@ $@

# The following are the "generic" rules,
# parameterized by LIB, IMPL, and PROFILE.

$(LIB): $(OBJS)
	$(AR) r$(ARFLAGS) $(LIB) $?
	$(RANLIB) $(LIB)
	$(RM) $(RMFLAGS) $?
# Avoid complaints from md in case Makedep doesn't exist yet.
	touch Makedep
	md -m Makedep -d *.d
# Massage dependency file to reflect object files within library.
	cp Makedep Makedep.BAK
	sed 's/^[^ #$$]*\.o/$$(LIB)(&)/' < Makedep.BAK > Makedep
	rm Makedep.BAK

$(LIB)(thread.o): machine/thread.c
	$(CC) $(CFLAGS) -c machine/thread.c

$(LIB)(malloc.o): malloc.c
	-if [ "$(TARGET_MACHINE)" = IBMRT -a "$(CC)" != /usr/cs/bin/cc ]; then \
		$(MAKE) $(MFLAGS) CC=/usr/cs/bin/cc malloc.o ; \
	else \
		$(CC) $(CFLAGS) -S malloc.c ; \
		$(MV) malloc.s malloc.S ; \
		$(INLINE) malloc.S > malloc.s ; \
		$(AS) $(ASFLAGS) -o malloc.o malloc.s ; \
		$(RM) $(RMFLAGS) malloc.s malloc.S ; \
	fi

$(LIB)(lock.o): machine/lock.s
	$(CC) -ES $(CPPFLAGS) machine/lock.s > $*.as
	$(AS) $(ASFLAGS) -o $@ $*.as
	$(RM) $(RMFLAGS) $*.as

$(LIB)(csw.o): machine/csw.s
	$(CC) -ES $(CPPFLAGS) machine/csw.s > $*.as
	$(AS) $(ASFLAGS) -o $@ $*.as
	$(RM) $(RMFLAGS) $*.as

-include Makedep
