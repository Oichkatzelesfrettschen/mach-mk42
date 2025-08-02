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
# Revision 2.14  91/02/14  14:16:54  mrt
# 	Added new Mach copyright
# 	[91/02/13  12:45:25  mrt]
# 
# Revision 2.13  90/12/05  23:46:53  af
# 	Changed .cs->.o rule to account for GNU i386 compiler driver.
# 
# Revision 2.12  90/11/05  14:35:27  rpd
# 	SUN->SUN3.
# 	[90/10/31            rwd]
# 	Change MACHINE to TARGET_MACHINE.
# 	[90/10/29            rwd]
# 
# Revision 2.11  90/11/05  14:08:04  rpd
# 	Added start_float.s, for the Sun-3.
# 	[90/11/01            rpd]
# 	Added exit.c.
# 	[90/10/30            rpd]
# 
# Revision 2.10  90/10/25  14:51:02  rwd
# 	Add I386 fields.
# 	[90/10/24            rwd]
# 
# Revision 2.9  90/08/07  18:00:50  rpd
# 	Added mach_msg_destroy.c, mach_msg_server.c.
# 	[90/08/06            rpd]
# 
# Revision 2.8  90/06/19  23:03:33  rpd
# 	Added mach_port_allocate, mach_port_deallocate, mach_port_insert_right
# 	to the sed script nonsense.
# 	[90/06/02            rpd]
# 
# Revision 2.7  90/06/02  15:12:18  rpd
# 	Use -O2 -g3 on mips.
# 	[90/04/25            rpd]
# 	Compile with -DMACH_IPC_COMPAT=0.
# 	[90/03/26  23:25:36  rpd]
# 
# Revision 2.6  90/05/29  18:40:00  rwd
# 	New mach_syscalls.c and sed changes to mach_user.
# 	[90/04/20            rwd]
# 
# Revision 2.5  90/05/03  15:53:35  dbg
# 	Add files for AT386.
# 	[90/04/23            dbg]
# 
# Revision 2.4  90/01/19  14:36:29  rwd
# 	mips crt0 is now .s instead of .cv
# 	[90/01/18            rwd]
# 
# Revision 2.3  89/11/29  14:18:30  af
# 	Use MD switch in assembly compiles (af+mips problem).
# 	[89/11/26  10:27:18  af]
# 
# 	Added Mips files and a special rule for compiling crt0.cv
# 	which needs a little massaging of the assembly but is otherwise
# 	a C file.
# 	Also, added machine-specific flags to assembler invocation,
# 	because on Mips as invokes cpp by default but does not like
# 	to see it twice.
# 	[89/10/28  10:59:33  af]
# 
# Revision 2.2  89/08/11  17:57:18  rwd
# 	Make clean now removes crt0.c
# 	[89/08/11            rwd]
# 
#
# Makefile for stand-alone stuff (so far)

LIB=	libmach_sa.a

SRCS=	mach_traps.cs mig_support.c mig_strncpy.c \
	mach_msg_destroy.c mach_msg_server.c \
	strcmp.c strlen.c strncpy.c msg.c mach_syscalls.c machine/_setjmp.s \
	machine/bcopy.cs strcat.c strcpy.c mach_init.c exit.c

VAX_SRCS= urem.s udiv.s
SUN3_SRCS= start_float.s

VAX_CRT0= machine/crt0.cc
SUN3_CRT0= machine/crt0.s
# PMAX_CRT0= machine/crt0.cv
PMAX_CRT0 = machine/crt0.ss
AT386_CRT0 = machine/crt0.c
I386_CRT0 = machine/crt0.c

PMAX_ASFLAGS= -nocpp

OBJS=	crt0.o exit.o mach_user.o mach_traps.o mig_support.o \
	mach_msg_destroy.o  mach_msg_server.o \
	bcopy.o strcmp.o strlen.o strncpy.o msg.o exc_server.o \
	mach_host_user.o mach_port_user.o mach_syscalls.o \
	device_user.o device_request_user.o device_reply_server.o \
	_setjmp.o bzero.o strcat.o strcpy.o mach_init.o mig_strncpy.o

VAX_OBJS= urem.o udiv.o
SUN3_OBJS= start_float.o
I386_OBJS= gcc.o
AT386_OBJS= gcc.o

DEFS=	mach/exc.defs mach/memory_object.defs mach/mach.defs mach/host.defs \
	device/device.defs device/device_request.defs device/device_reply.defs

PMAX_CDEBUGFLAGS = -O2 -g3
CDEBUGFLAGS = $($(TARGET_MACHINE)_CDEBUGFLAGS?$($(TARGET_MACHINE)_CDEBUGFLAGS):-O)
DEFINES = -DMACH_IPC_COMPAT=0 -DSTANDALONE -DTypeCheck=0
CFLAGS = $(CDEBUGFLAGS) $(DEFINES) $(INCLUDES)

CPP = $(CC) -ES

MIG=mig
MIGFLAGS = $(MIGOPTS) $(DEFINES) $(INCLUDES)

.SUFFIXES: .cs .cc .ss

${LIB}:	${OBJS} ${${TARGET_MACHINE}_OBJS}
	rm -f ${LIB}
	ar crv ${LIB} ${OBJS} ${${TARGET_MACHINE}_OBJS}
	ranlib ${LIB}

crt0.o:	${${TARGET_MACHINE}_CRT0}

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $*.c

.s.o:
	${CPP} -MD ${DEFINES} ${INCLUDES} $*.s > $*.as
	${AS} ${${TARGET_MACHINE}_ASFLAGS} -o $*.o $*.as
	rm -f $*.as

.ss.o:
	${AS} ${${TARGET_MACHINE}_ASFLAGS} -o $*.o $*.ss

.cs.o:
	cp $*.cs $*.S
	${CPP} -ES -MD ${DEFINES} ${INCLUDES} $*.S > $*.as
	${AS} ${${TARGET_MACHINE}_ASFLAGS} -o $*.o $*.as
	rm -f $*.S $*.as

#
# Special '.cc' suffix for putting crt0 thru cpp twice.
#
.cc.o:
	cp $*.cc $*.c
	${CC} -S -DCRT0 $*.c
	${CPP} $*.s > $*.i
	${AS} -o $*.o $*.i

#
# Special '.cv' suffix for massaging intermediate assembly code
# for mips
#
#.cv.o:
#	cp $*.cv $*.c
#	cc -O2 -S crt0.c
#	sed	-e 's<__start:<__start: la $$28,_gp;move $$4,$$29<' \
#		-e 's<\.option<\#\.option<' \
#		crt0.s > crt0t.s && mv crt0t.s crt0.s
#	cc -O2 -c crt0.s && rm -f crt0.s
#	ld -x -r crt0.o
#	mv a.out crt0.o

mach_user.c: mach/mach.defs mach/mach_types.defs
	${MIG} ${MIGFLAGS} \
		-user mach_temp \
		-header mach_interface.h \
		-server /dev/null \
		mach/mach.defs
	sed 	-e 's/vm_allocate/mig_vm_allocate/' \
		-e 's/vm_map/mig_vm_map/' \
		-e 's/vm_deallocate/mig_vm_deallocate/' \
		-e 's/task_create/mig_task_create/' \
		-e 's/task_terminate/mig_task_terminate/' \
		-e 's/task_suspend/mig_task_suspend/' \
		-e 's/task_set_special_port/mig_task_set_special_port/' \
		< mach_temp > mach_user.c
	rm -f mach_temp

mach_port_user.c: mach/mach_port.defs mach/mach_types.defs
	${MIG} ${MIGFLAGS} \
		-user mach_port_temp \
		-header mach_port.h \
		-server /dev/null \
		mach/mach_port.defs
	sed	-e 's/mach_port_allocate/mig_mach_port_allocate/' \
		-e 's/mach_port_deallocate/mig_mach_port_deallocate/' \
		-e 's/mach_port_insert_right/mig_mach_port_insert_right/' \
		< mach_port_temp > mach_port_user.c
	rm -f mach_port_temp

exc_server.c: mach/exc.defs mach/mach_types.defs
	${MIG} ${MIGFLAGS} \
		-server exc_server.c \
		-header exc.h \
		-user /dev/null \
		mach/exc.defs

device_user.c: device/device.defs device/device_types.defs
	${MIG} ${MIGFLAGS} \
		-user device_user.c \
		-header device.h \
		-server /dev/null \
		device/device.defs

device_request_user.c: device/device_request.defs device/device_types.defs
	${MIG} ${MIGFLAGS} \
		-user device_request_user.c \
		-server /dev/null \
		device/device_request.defs

device_reply_server.c: device/device_reply.defs device/device_types.defs
	${MIG} ${MIGFLAGS} \
		-server device_reply_server.c \
		-user /dev/null \
		device/device_reply.defs

mach_host_user.c: mach/mach_host.defs mach/mach_types.defs
	${MIG} ${MIGFLAGS} \
		-user mach_host_user.c \
		-server /dev/null \
		mach/mach_host.defs

clean:
	rm -f ${OBJS} *.as *User.c *Server.c *_user.c *_server.c *.d
	rm -f mach_interface.h exc.h device.h device_request.h device_reply.h
	rm -f mach_port.h mach_host.h
	rm -f crt0.s x.s crt0.c
