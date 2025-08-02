SOURCE_DIR=..
IDENT=-DCMU -DMACH -DMACH_KERNEL -DAT386 -DMACH_LOAD -DCMUCS
#
# Mach Operating System
# Copyright (c) 1991,1990,1989,1988,1987,1986 Carnegie Mellon University
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
# $Log:	Makefile.template,v $
# Revision 2.12  91/02/05  17:03:12  mrt
# 	Changed to new copyright
# 	[91/01/28  14:47:11  mrt]
# 
# Revision 2.11  90/12/14  10:59:15  jsb
# 	Added rule for building dev_forward.c (for cross-node device support).
# 	[90/12/13  20:58:23  jsb]
# 
# Revision 2.10  90/12/04  14:45:43  jsb
# 	Removed workaround for libmig.a; this can be worked around entirely
# 	in Makefile.{machine}.
# 	[90/12/04  11:37:33  jsb]
# 
# Revision 2.9  90/11/25  17:48:22  jsb
# 	Added support for cross-compilation, using existing KCC technology:
# 	KCC is used to build images to be run on target machine (machine
# 	for which kernel is being built), and CC is used to build images
# 	to be run on host machine (machine upon which compilation is done).
# 	Use KCC (not CC) to build vers.o. Use CC (not KCC) to build genassym.
# 	Ignore errors from size command. Add workaround so that machine
# 	dependent makefile can choose to link kernel with LIBMIG_OBJS instead
# 	of with libmig.a, in case we don't have a working ranlib.
# 	This is all in support of building i860 kernels on i386.
# 	[90/11/25  16:46:51  jsb]
# 
# Revision 2.8  90/11/05  14:25:27  rpd
# 	Added REG_EXP.
# 	[90/10/29            rwd]
# 
# Revision 2.7  90/10/25  14:43:20  rwd
# 	Added explicit dependency for ds_routines.o.
# 	[90/10/18            rpd]
# 
# Revision 2.6  90/08/27  21:46:33  dbg
# 	Remove symbols.raw from LDDEPS dependency list.  Remove
# 	symbols.sort make rules.  Remove ioconf.c from LINTFILES.
# 	Remove BSD license - this file was written entirely at CMU,
# 	using only features from the (public domain) src/config program.
# 	[90/08/13            dbg]
# 
# Revision 2.5  90/06/19  22:57:19  rpd
# 	Replaced mach_user.c and mach_port_user.c with libmig.a.
# 	[90/06/03            rpd]
# 
# Revision 2.4  90/06/02  14:46:37  rpd
# 	New style rules for default_pager_object.defs.
# 	[90/05/03            rpd]
# 	Don't barf if xstrip fails.
# 	[90/05/02            rpd]
# 
# 	From jsb: don't rearrange symbols.
# 	Use xstrip instead of -x.
# 	[90/04/23            rpd]
# 	Added explicit rules for running mig.
# 	Moved generated dependencies to Makedep.
# 	[90/03/26  21:39:19  rpd]
# 
# Revision 2.3  90/03/14  21:10:31  rwd
# 	Add default_pager_object.defs refs
# 	[90/01/22            rwd]
# 
#
# Condensed history:
#	Changes for MACH_KERNEL (dbg).
#	Added mach_host.defs and related files (dlb).
#	Mucho hacking at Mig rules (rpd).
#	Lint hacking (mwyoung).
#	New memory_object files (mwyoung).
#	CPATH and -I hacking (mja, rpd).
#	Running Mig in the build directory (mwyoung).
#	VOLATILE hacking (rvb).
#	Created from old Makefile.vax (mja).
#	Reorganized for simplified config program (mja).
#	Introduced new make depend stuff (rvb).
#

#
#  This must be here before any rules are possibly defined by the
#  machine dependent makefile fragment so that a plain "make" command
#  always works.  The config program will emit an appropriate rule to
#  cause "all" to depend on every kernel configuration it generates.
#

default: all

#
#  In order to move the link commands out of config and back into the
#  template makefile, config has been changed to emit a dendency rule
#  of the form *vmunix.sys: *vmunix.swap for each configuration so that
#  the common commands for building each configuration can be placed
#  directly in the template in the default rule for the .sys.swap
#  dependency.
#
.SUFFIXES: .swap .sys

#
#  Make sure we don't remove this by accident if interrupted at the wrong
#  time.
#
.PRECIOUS: Makefile

#
#  Compilation components
#
CC=	cc
CPP=	${CC} -ES
C2=	/lib/c2
LD=	ld
MD=	/usr/mach/bin/md
ENCODE=	uuencode
DECODE=	uudecode
VOLATILE="-Dvolatile=" 
FORCE_VOLATILE=

KCC=	${CC}
KCPP=	${KCC} -ES

#
#  This changed in the 4.3 CPP from .n to .d
#
MD_SUFFIX=d

#
#  Pre-processor environment
#
DEPENDS = -MD
INCLUDES =
DEFINES = $(IDENT) -DKERNEL
ALLOPTS = $(INCLUDES) $(DEFINES)
LINTOPTS = ${ALLOPTS} ${VOLATILE}

#
#  Common C-compilation flags
#
COPTS_P = ${DEPENDS} ${ALLOPTS}
COPTS = ${COPTS_P}
CDEBUGFLAGS = -O
CMACHFLAGS =
CFLAGS=	$(CDEBUGFLAGS) $(CMACHFLAGS) ${COPTS}

#
#  Common assembly flags
#
SFLAGS=	${DEPENDS} ${INCLUDES} ${IDENT} -DKERNEL -DASSEMBLER


#
#  Path name of the version identification file.
#
FEATURES=$(SOURCE_DIR)/machine/FEATURES.h
VERSION_FILES= \
	$(SOURCE_DIR)/conf/version.major \
	$(SOURCE_DIR)/conf/version.minor \
	$(SOURCE_DIR)/conf/version.variant \
	$(SOURCE_DIR)/conf/version.edit \
	$(SOURCE_DIR)/conf/version.patch

#
#  LDOBJS is the set of object files which comprise the kernel.
#  It is used both in the dependency list for each *vmunix.swap
#  rule emitted by config and also in the .sys.swap rule
#  below which links the kernel from these objects.
#  
#  LDOBJS_PREFIX and LDOBJS_SUFFIX are defined in the machine
#  dependent Makefile (if necessary).
#
LDOBJS=${LDOBJS_PREFIX} ${OBJS} ${LDOBJS_SUFFIX} libmig.a

#
#  LDDEPS is the set of extra dependencies associated with each
#  *vmunix.swap rule (in addition to $LDOBJS and the swap*.o file).
#
#  LDDEPS_PREFIX is defined in the machine dependent Makefile
#  (if necessary).
#
LDDEPS=${LDDEPS_PREFIX} ${VERSION_FILES} \
	$(SOURCE_DIR)/conf/newvers.sh $(SOURCE_DIR)/conf/copyright

#
#  PRELDDEPS is another set of extra dependencies associated with each
#  *vmunix.swap rule (in addition to $LDOBJS and the swap*.o file).
#  It is defined in the machine dependent Makefile (if necessary).
#
#  The generated rule looks like
#	vmunix.sys : ${PRELDDEPS} ${LDOBJS} ${LDDEPS}
#

#
#  SWAPDEPS is the set of extra dependencies associated with each
#  swap*.o rule emitted by config (in addition to the
#  ../machine/swap*.c file).
#
#  SWAPDEPS_PREFIX is defined in the machine dependent Makefile
#  (if necessary).
#
SWAPDEPS=${SWAPDEPS_PREFIX} 

#
#  SYSDEPS is the set of extra dependencies associated with each
#  *vmunix rule (in addition to *vmunix.sys).
#
#  SYSDEPS_PREFIX is defined in the machine dependent Makefile
#  (if necessary).
#
SYSDEPS=${SYSDEPS_PREFIX}



#
#  Compilation rules to generate *vmunix from *vmunix.sys
#
SYS_RULE_1=@eval `awk 'NR==1{S=$$1;next;}\
END {\
  C = "ln $@.sys $@";\
  if (S != "") {\
    C = C "; ln $@ $@" S; \
    printf "rm -f $@%s; ", S; \
  }\
  printf "echo \"%s\"; %s\n", C, C; \
}' vers.suffix`
SYS_RULE_2=
SYS_RULE_3=
SYS_RULE_4=

#
#  Things to be done just before exiting
#
EXIT_RULE=${MD} -u Makedep -f -d `ls *.${MD_SUFFIX}`

#
# Compilation rules to generate .o from .b
#
B_RULE_1A=rm -f $*.o; ${DECODE} 
B_RULE_1B=$*.b

#
# Compilation rules to generate .o from .o
#
O_RULE_1A=rm -f $*.o; cp 
O_RULE_1B=$*.o .

#
# Compilation rules to generate .o from .s
#
S_RULE_1A=${KCPP} ${SFLAGS} 
S_RULE_1B=$*.s >$*.i 
S_RULE_2=${AS} -o $*.o $*.i
S_RULE_3=rm -f $*.i

#
# Compilation rules to generate .o from .c for normal files
#
C_RULE_1A=${KCC} -c ${CFLAGS} 
C_RULE_1B=$*.c
C_RULE_2=
C_RULE_3=
C_RULE_4=

#
# Compilation rules to generate .o from .c for driver files
#
C_RULE_1A_D=${C_RULE_1A}
C_RULE_1B_D=${C_RULE_1B}
C_RULE_2_D=${C_RULE_2}
C_RULE_3_D=${C_RULE_3}
C_RULE_4_D=${C_RULE_4}

#
# Compilation rules to generate .o from .c for profiling routine files
#
C_RULE_1A_P=${KCC} -c -S ${COPTS_P} 
C_RULE_1B_P=${C_RULE_1B}
C_RULE_2_P=ex - $*.s < ${GPROF.EX}
C_RULE_3_P=${AS} -o $*.o $*.s
C_RULE_4_P=rm -f $*.s



gprof: kgmon_on all kgmon_off

kgmon_on:
	kgmon -r -b

kgmon_off:
	kgmon -h -p
	gprof /mach >GPROF


#
#  These macros are filled in by the config program depending on the
#  current configuration.  The MACHDEP macro is replaced by the
#  contents of the machine dependent makefile template and the others
#  are replaced by the corresponding symbol definitions for the
#  configuration.
#


OBJS=db_access.o db_aout.o db_break.o db_command.o db_examine.o \
	db_expr.o db_input.o db_lex.o db_output.o db_print.o db_run.o \
	db_sym.o db_trap.o db_variables.o db_watch.o db_write_cmd.o \
	ipc_entry.o ipc_hash.o ipc_init.o ipc_kmsg.o ipc_marequest.o \
	ipc_mqueue.o ipc_notify.o ipc_object.o ipc_port.o ipc_pset.o \
	ipc_right.o ipc_space.o ipc_splay.o ipc_table.o ipc_thread.o \
	mach_debug.o mach_msg.o mach_port.o ast.o bootstrap.o debug.o \
	exception.o host.o ipc_host.o ipc_kobject.o ipc_mig.o \
	ipc_sched.o ipc_tt.o kalloc.o lock.o mach_clock.o mach_factor.o \
	machine.o printf.o priority.o processor.o queue.o sched_prim.o \
	startup.o syscall_emulation.o syscall_subr.o syscall_sw.o task.o \
	thread.o thread_swap.o time_stamp.o timer.o xpr.o zalloc.o \
	mach_host_server.o mach_port_server.o mach_server.o \
	memory_object_default_user.o memory_object_user.o \
	mach_debug_server.o memory_object.o vm_debug.o vm_fault.o \
	vm_init.o vm_kern.o vm_map.o vm_object.o vm_pageout.o \
	vm_resident.o vm_user.o blkio.o chario.o cirbuf.o dev_lookup.o \
	dev_name.o dev_pager.o device_reply_user.o device_server.o \
	device_user.o device_init.o ds_routines.o net_io.o subrs.o \
	boot_printf.o default_pager.o def_pager_setup.o file_io.o load.o \
	ioconf.o pmap.o bcopy.o bzero.o exec.o fpu.o gcc.o gdt.o \
	hardclock.o i386_init.o idt.o init.o interrupt.o io_emulate.o \
	io_map.o db_disasm.o db_interface.o db_trace.o ktss.o ldt.o \
	loose_ends.o ntoh.o pic.o pit.o pcb.o phys.o read_fault.o \
	setroot.o spl.o trap.o _setjmp.o autoconf.o blit.o c765.o com.o \
	conf.o hd.o if_3c501.o if_pc586.o if_ns8390.o iopl.o kd.o \
	kdasm.o kd_event.o kd_mouse.o kd_queue.o m765drv.o m765knl.o \
	pic_isa.o rtc.o 

CFILES=\
	$(SOURCE_DIR)/ddb/db_access.c \
	$(SOURCE_DIR)/ddb/db_aout.c \
	$(SOURCE_DIR)/ddb/db_break.c \
	$(SOURCE_DIR)/ddb/db_command.c \
	$(SOURCE_DIR)/ddb/db_examine.c \
	$(SOURCE_DIR)/ddb/db_expr.c \
	$(SOURCE_DIR)/ddb/db_input.c \
	$(SOURCE_DIR)/ddb/db_lex.c \
	$(SOURCE_DIR)/ddb/db_output.c \
	$(SOURCE_DIR)/ddb/db_print.c \
	$(SOURCE_DIR)/ddb/db_run.c \
	$(SOURCE_DIR)/ddb/db_sym.c \
	$(SOURCE_DIR)/ddb/db_trap.c \
	$(SOURCE_DIR)/ddb/db_variables.c \
	$(SOURCE_DIR)/ddb/db_watch.c \
	$(SOURCE_DIR)/ddb/db_write_cmd.c \
	$(SOURCE_DIR)/ipc/ipc_entry.c \
	$(SOURCE_DIR)/ipc/ipc_hash.c \
	$(SOURCE_DIR)/ipc/ipc_init.c \
	$(SOURCE_DIR)/ipc/ipc_kmsg.c \
	$(SOURCE_DIR)/ipc/ipc_marequest.c \
	$(SOURCE_DIR)/ipc/ipc_mqueue.c \
	$(SOURCE_DIR)/ipc/ipc_notify.c \
	$(SOURCE_DIR)/ipc/ipc_object.c \
	$(SOURCE_DIR)/ipc/ipc_port.c \
	$(SOURCE_DIR)/ipc/ipc_pset.c \
	$(SOURCE_DIR)/ipc/ipc_right.c \
	$(SOURCE_DIR)/ipc/ipc_space.c \
	$(SOURCE_DIR)/ipc/ipc_splay.c \
	$(SOURCE_DIR)/ipc/ipc_table.c \
	$(SOURCE_DIR)/ipc/ipc_thread.c \
	$(SOURCE_DIR)/ipc/mach_debug.c \
	$(SOURCE_DIR)/ipc/mach_msg.c \
	$(SOURCE_DIR)/ipc/mach_port.c \
	$(SOURCE_DIR)/kern/ast.c \
	$(SOURCE_DIR)/kern/bootstrap.c \
	$(SOURCE_DIR)/kern/debug.c \
	$(SOURCE_DIR)/kern/exception.c \
	$(SOURCE_DIR)/kern/host.c \
	$(SOURCE_DIR)/kern/ipc_host.c \
	$(SOURCE_DIR)/kern/ipc_kobject.c \
	$(SOURCE_DIR)/kern/ipc_mig.c \
	$(SOURCE_DIR)/kern/ipc_sched.c \
	$(SOURCE_DIR)/kern/ipc_tt.c \
	$(SOURCE_DIR)/kern/kalloc.c \
	$(SOURCE_DIR)/kern/lock.c \
	$(SOURCE_DIR)/kern/mach_clock.c \
	$(SOURCE_DIR)/kern/mach_factor.c \
	$(SOURCE_DIR)/kern/machine.c \
	$(SOURCE_DIR)/kern/printf.c \
	$(SOURCE_DIR)/kern/priority.c \
	$(SOURCE_DIR)/kern/processor.c \
	$(SOURCE_DIR)/kern/queue.c \
	$(SOURCE_DIR)/kern/sched_prim.c \
	$(SOURCE_DIR)/kern/startup.c \
	$(SOURCE_DIR)/kern/syscall_emulation.c \
	$(SOURCE_DIR)/kern/syscall_subr.c \
	$(SOURCE_DIR)/kern/syscall_sw.c \
	$(SOURCE_DIR)/kern/task.c \
	$(SOURCE_DIR)/kern/thread.c \
	$(SOURCE_DIR)/kern/thread_swap.c \
	$(SOURCE_DIR)/kern/time_stamp.c \
	$(SOURCE_DIR)/kern/timer.c \
	$(SOURCE_DIR)/kern/xpr.c \
	$(SOURCE_DIR)/kern/zalloc.c \
	./mach/mach_host_server.c \
	./mach/mach_port_server.c \
	./mach/mach_server.c \
	./mach/memory_object_default_user.c \
	./mach/memory_object_user.c \
	./mach_debug/mach_debug_server.c \
	$(SOURCE_DIR)/vm/memory_object.c \
	$(SOURCE_DIR)/vm/vm_debug.c \
	$(SOURCE_DIR)/vm/vm_fault.c \
	$(SOURCE_DIR)/vm/vm_init.c \
	$(SOURCE_DIR)/vm/vm_kern.c \
	$(SOURCE_DIR)/vm/vm_map.c \
	$(SOURCE_DIR)/vm/vm_object.c \
	$(SOURCE_DIR)/vm/vm_pageout.c \
	$(SOURCE_DIR)/vm/vm_resident.c \
	$(SOURCE_DIR)/vm/vm_user.c \
	$(SOURCE_DIR)/device/blkio.c \
	$(SOURCE_DIR)/device/chario.c \
	$(SOURCE_DIR)/device/cirbuf.c \
	$(SOURCE_DIR)/device/dev_lookup.c \
	$(SOURCE_DIR)/device/dev_name.c \
	$(SOURCE_DIR)/device/dev_pager.c \
	./device/device_reply_user.c \
	./device/device_server.c \
	./device/device_user.c \
	$(SOURCE_DIR)/device/device_init.c \
	$(SOURCE_DIR)/device/ds_routines.c \
	$(SOURCE_DIR)/device/net_io.c \
	$(SOURCE_DIR)/device/subrs.c \
	$(SOURCE_DIR)/boot_ufs/boot_printf.c \
	$(SOURCE_DIR)/boot_ufs/default_pager.c \
	$(SOURCE_DIR)/boot_ufs/def_pager_setup.c \
	$(SOURCE_DIR)/boot_ufs/file_io.c \
	$(SOURCE_DIR)/boot_ufs/load.c \
	./ioconf.c \
	$(SOURCE_DIR)/intel/pmap.c \
	$(SOURCE_DIR)/i386/exec.c \
	$(SOURCE_DIR)/i386/fpu.c \
	$(SOURCE_DIR)/i386/gdt.c \
	$(SOURCE_DIR)/i386/hardclock.c \
	$(SOURCE_DIR)/i386/i386_init.c \
	$(SOURCE_DIR)/i386/init.c \
	$(SOURCE_DIR)/i386/io_emulate.c \
	$(SOURCE_DIR)/i386/io_map.c \
	$(SOURCE_DIR)/i386/db_disasm.c \
	$(SOURCE_DIR)/i386/db_interface.c \
	$(SOURCE_DIR)/i386/db_trace.c \
	$(SOURCE_DIR)/i386/ktss.c \
	$(SOURCE_DIR)/i386/ldt.c \
	$(SOURCE_DIR)/i386/loose_ends.c \
	$(SOURCE_DIR)/i386/pic.c \
	$(SOURCE_DIR)/i386/pit.c \
	$(SOURCE_DIR)/i386/pcb.c \
	$(SOURCE_DIR)/i386/phys.c \
	$(SOURCE_DIR)/i386/read_fault.c \
	$(SOURCE_DIR)/i386/setroot.c \
	$(SOURCE_DIR)/i386/trap.c \
	$(SOURCE_DIR)/i386at/autoconf.c \
	$(SOURCE_DIR)/i386at/blit.c \
	$(SOURCE_DIR)/i386at/c765.c \
	$(SOURCE_DIR)/i386at/com.c \
	$(SOURCE_DIR)/i386at/conf.c \
	$(SOURCE_DIR)/i386at/hd.c \
	$(SOURCE_DIR)/i386at/if_3c501.c \
	$(SOURCE_DIR)/i386at/if_pc586.c \
	$(SOURCE_DIR)/i386at/if_ns8390.c \
	$(SOURCE_DIR)/i386at/iopl.c \
	$(SOURCE_DIR)/i386at/kd.c \
	$(SOURCE_DIR)/i386at/kd_event.c \
	$(SOURCE_DIR)/i386at/kd_mouse.c \
	$(SOURCE_DIR)/i386at/kd_queue.c \
	$(SOURCE_DIR)/i386at/m765drv.c \
	$(SOURCE_DIR)/i386at/m765knl.c \
	$(SOURCE_DIR)/i386at/pic_isa.c \
	$(SOURCE_DIR)/i386at/rtc.c 
COBJS=db_access.o db_aout.o db_break.o db_command.o db_examine.o \
	db_expr.o db_input.o db_lex.o db_output.o db_print.o db_run.o \
	db_sym.o db_trap.o db_variables.o db_watch.o db_write_cmd.o \
	ipc_entry.o ipc_hash.o ipc_init.o ipc_kmsg.o ipc_marequest.o \
	ipc_mqueue.o ipc_notify.o ipc_object.o ipc_port.o ipc_pset.o \
	ipc_right.o ipc_space.o ipc_splay.o ipc_table.o ipc_thread.o \
	mach_debug.o mach_msg.o mach_port.o ast.o bootstrap.o debug.o \
	exception.o host.o ipc_host.o ipc_kobject.o ipc_mig.o \
	ipc_sched.o ipc_tt.o kalloc.o lock.o mach_clock.o mach_factor.o \
	machine.o printf.o priority.o processor.o queue.o sched_prim.o \
	startup.o syscall_emulation.o syscall_subr.o syscall_sw.o task.o \
	thread.o thread_swap.o time_stamp.o timer.o xpr.o zalloc.o \
	mach_host_server.o mach_port_server.o mach_server.o \
	memory_object_default_user.o memory_object_user.o \
	mach_debug_server.o memory_object.o vm_debug.o vm_fault.o \
	vm_init.o vm_kern.o vm_map.o vm_object.o vm_pageout.o \
	vm_resident.o vm_user.o blkio.o chario.o cirbuf.o dev_lookup.o \
	dev_name.o dev_pager.o device_reply_user.o device_server.o \
	device_user.o device_init.o ds_routines.o net_io.o subrs.o \
	boot_printf.o default_pager.o def_pager_setup.o file_io.o load.o \
	ioconf.o pmap.o exec.o fpu.o gdt.o hardclock.o i386_init.o \
	init.o io_emulate.o io_map.o db_disasm.o db_interface.o \
	db_trace.o ktss.o ldt.o loose_ends.o pic.o pit.o pcb.o phys.o \
	read_fault.o setroot.o trap.o autoconf.o blit.o c765.o com.o \
	conf.o hd.o if_3c501.o if_pc586.o if_ns8390.o iopl.o kd.o \
	kd_event.o kd_mouse.o kd_queue.o m765drv.o m765knl.o pic_isa.o \
	rtc.o 

SFILES=\
	$(SOURCE_DIR)/i386/bcopy.s \
	$(SOURCE_DIR)/i386/bzero.s \
	$(SOURCE_DIR)/i386/gcc.s \
	$(SOURCE_DIR)/i386/idt.s \
	$(SOURCE_DIR)/i386/interrupt.s \
	$(SOURCE_DIR)/i386/ntoh.s \
	$(SOURCE_DIR)/i386/spl.s \
	$(SOURCE_DIR)/i386/_setjmp.s \
	$(SOURCE_DIR)/i386at/kdasm.s 
SOBJS=bcopy.o bzero.o gcc.o idt.o interrupt.o ntoh.o spl.o _setjmp.o \
	kdasm.o 

BFILES=
ORDERED=
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
# $Log:	Makefile.i386,v $
# Revision 2.3  91/02/05  17:02:51  mrt
# 	Changed to new copyright
# 	[91/01/28  14:46:50  mrt]
# 
# Revision 2.2  90/05/03  15:18:09  dbg
# 	Fix locore build rule - add space before '-DLOCORE'.
# 	[90/02/14            dbg]
# 
# Revision 2.6.1.5  90/02/01  13:33:48  rvb
# 	Don't play with SYS_RULE_1.  And stub out SYS_RULE{2,3,4}
# 	to @ to not to have (null command) complaint.
# 	And, the .s's files should be in $(SOURCE_DIR) not ..
# 	[90/01/23            rvb]
# 
# Revision 2.6.1.4  90/01/02  13:55:27  rvb
# 	SWAPSYS is now coff only.
# 
# Revision 2.6.1.3  89/12/21  17:11:24  rvb
# 	a.out vs coff now handled by MASTER.i386
# 
# Revision 2.6.1.2  89/11/21  12:55:09  rvb
# 	md is no longer in /usr/mach/bin
# 
# 
# Revision 2.6.1.1  89/10/22  11:29:04  rvb
# 	Indicate coff vs a.out
# 	[89/10/21            rvb]
# 
# Revision 2.6  89/09/25  12:20:10  rvb
# 	uprt.s -> start.s
# 	[89/09/23            rvb]
# 
# Revision 2.5  89/04/07  14:58:25  rvb
# 	We don't need INCLUDES any more.
# 	[89/04/07            rvb]
# 
# Revision 2.4  89/04/05  12:57:12  rvb
# 	Some changes for gcc, and locore is a .s
# 	[89/03/21            rvb]
# 
# Revision 2.3  89/02/25  17:39:18  gm0w
# 	Changes for cleanup.
# 
# Revision 2.2  89/01/23  22:15:23  af
# 	Created.
# 	[89/01/16  17:11:51  af]
# 

###############################################################################
#BEGIN	Machine dependent Makefile fragment for the i386
###############################################################################

#tmp

SYS_RULE_2=-@
SYS_RULE_3=-@
SYS_RULE_4=-@
MD=md

#LIBS_P=-lc_p

LDOBJS_PREFIX= ${ORDERED} locore.o
LOCORE_DEPS=	assym.s $(SOURCE_DIR)/machine/start.s \
		$(SOURCE_DIR)/machine/locore.s $(SOURCE_DIR)/machine/cswitch.s
LOCORE_HEADS=

locore.s: ${LOCORE_DEPS}
	cat ${LOCORE_DEPS} >locore.tmp
	mv locore.tmp locore.s
	@echo The hack below should get fixed sometime.
	cp /dev/null ioconf.c

locore.o: assym.s locore.s ${LOCORE_HEADS}
	${S_RULE_1A} -DLOCORE -I../machine ${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

###############################################################################
#END	Machine dependent Makefile fragment for the i386
###############################################################################
CC=	gcc
GAS=	gas
LDFLAGS=-x -e _pstart -T c0100000
SWAPSYS=@
VOLATILE=
FORCE_VOLATILE=-fvolatile 
C_RULE_1A=${KCC} -c ${CFLAGS} -fno-function-cse 
C_RULE_2=
C_RULE_3=
C_RULE_4=


#
#  Default rule used to build a *vmunix.sys configuration from the
#  object list and a particular *vmunix.swap module.  The *vmunix.swap
#  module is a normal object file compiled from the appropriate swap*.c
#  file and then copied to the *vmunix.swap name to trigger the full
#  kernel link using this default rule.
#

.swap.sys:
	@/bin/sh $(SOURCE_DIR)/conf/newvers.sh \
			 $(SOURCE_DIR)/conf/copyright \
			`cat ${VERSION_FILES}`
	@${KCC} -c vers.c
	@rm -f $* $*.sys
	@echo loading $*.sys
	@${LD} ${LDFLAGS} ${LDOBJS} vers.o $*.swap ${LIBS}
	@echo stripping $*.sys
	@-xstrip a.out
	@-size a.out
	@chmod 755 a.out
	@-mv a.out $*.sys



#
#  OBJSDEPS is the set of files (defined in the machine dependent
#  template if necessary) which all objects depend on (such as an
#  in-line assembler expansion filter
#

${OBJS}: ${OBJSDEPS}



#
#  This macro is replaced by three rules per configuration, plus a
#  final definition for the LOAD symbol consisting of all configuration
#  names and a dependency of "all" on this symbol (so that the
#  "default" rule above will build everything).  The three rules in
#  each configuration are:
#
#  - a *vmunix from *vmunix.sys dependency using the SYS_RULE_* rules,
#  - a *vmunix.swap from LDDEPS, swap*.o, and LDDEPS dependency that
#    simply copies the swap file to the target name, and
#  - a swap*.o from swap*.c and SWAPDEPS dependency using the C_RULE_* rules.
#

mach_kernel .ORDER: mach_kernel.sys ${SYSDEPS}
	${SYS_RULE_1}
	${SYS_RULE_2}
	${SYS_RULE_3}
	${SYS_RULE_4}

mach_kernel.sys:${P} ${PRELDDEPS} ${LDOBJS} ${LDDEPS}

mach_kernel.swap: swapgeneric.o
	@cp swapgeneric.o $@

swapgeneric.o: $(SOURCE_DIR)/machine/swapgeneric.c ${SWAPDEPS}
	${C_RULE_1A}$(SOURCE_DIR)/machine/${C_RULE_1B}
	${C_RULE_2}
	${C_RULE_3}
	${C_RULE_4}

LOAD = mach_kernel

all: ${LOAD}




#
#  Rules for components which are not part of the kernel proper or that
#  need to be built in a special manner.
#

genassym.o: $(SOURCE_DIR)/machine/genassym.c
	${CC} -c ${COPTS_P} ${VOLATILE} $(SOURCE_DIR)/machine/genassym.c

assym.s: genassym.o
	${CC} genassym.o
	./a.out > assym.s; rm -f a.out

locore.o: assym.s ${LOCORE_HEADS}

#
#  Mach IPC-based interfaces
#

#  Explicit dependencies on generated files,
#  to ensure that Mig has been run by the time
#  these files are compiled.

vm_pageout.o: mach/memory_object_user.h mach/memory_object_default.h

vm_object.o: mach/memory_object_user.h mach/memory_object_default.h

vm_fault.o: mach/memory_object_user.h

memory_object.o: mach/memory_object_user.h

exception.o: mach/exc.h

bootstrap.o: mach/mach_interface.h mach/mach_user_internal.h \
	mach/mach_port_internal.h

default_pager.o: device/device.h mach/mach_user_internal.h \
	mach/mach_port_internal.h mach/memory_object_server.c \
	mach/memory_object_default_server.c \
	mach/default_pager_object_server.c

def_pager_setup.o : mach/mach_interface.h mach/mach_user_internal.h \
	mach/mach_port_internal.h

load.o : mach/mach_interface.h mach/mach_user_internal.h \
	mach/mach_port_internal.h

sasys.o : mach/mach_interface.h mach/mach_user_internal.h device/device.h \
	mach/mach_port_internal.h

dev_pager.o : device/device_pager_server.c

boot_printf.o : mach/mach_user_internal.h mach/mach_port_internal.h

ds_routines.o : device/device_reply.h

#  The Mig-generated files go into subdirectories.
#  These macros create the subdirectories as they are needed.

MAKE_MACH = [ -d mach ] || mkdir mach
MAKE_MACH_DEBUG = [ -d mach_debug ] || mkdir mach_debug
MAKE_DEVICE = [ -d device ] || mkdir device

MIGFLAGS = -DKERNEL
MIGKSFLAGS = $(MIGFLAGS) -DKERNEL_SERVER
MIGKUFLAGS = $(MIGFLAGS) -DKERNEL_USER

MIGDEPS = ../bin/mig ../lib/migcom
MIG = PATH=../bin:$$PATH LPATH=../lib:$$LPATH mig

MACHDIR = mach/

MACH_FUNCS = thread_resume thread_create task_create task_set_special_port \
	vm_set_default_memory_manager vm_allocate vm_deallocate \
	vm_write vm_protect vm_pageable \
	memory_object_data_provided memory_object_data_unavailable \
	memory_object_data_error memory_object_set_attributes

MACH_FFILES = $(MACH_FUNCS/$(REG_EXP)/$(MACHDIR)&.c)

MACH_FILES = mach/mach_interface.h mach/mach_user_internal.h \
	mach/mach_server.c $(MACH_FFILES)

$(MACH_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/mach.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKSFLAGS) -header mach/mach_interface.h -i mach/ -server mach/mach_server.c -iheader mach/mach_user_internal.h $(SOURCE_DIR)/mach/mach.defs

MACH_PORT_FUNCS = mach_port_allocate mach_port_destroy \
	mach_port_insert_right mach_port_move_member

MACH_PORT_FFILES = $(MACH_PORT_FUNCS/$(REG_EXP)/$(MACHDIR)&.c)

MACH_PORT_FILES = mach/mach_port_interface.h mach/mach_port_internal.h \
	mach/mach_port_server.c $(MACH_PORT_FFILES)

$(MACH_PORT_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/mach_port.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKSFLAGS) -header mach/mach_port_interface.h -i mach/ -server mach/mach_port_server.c -iheader mach/mach_port_internal.h $(SOURCE_DIR)/mach/mach_port.defs

MEMORY_OBJECT_FILES = mach/memory_object_user.h \
	mach/memory_object_user.c mach/memory_object_server.c

$(MEMORY_OBJECT_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/memory_object.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKUFLAGS) -header mach/memory_object_user.h -user mach/memory_object_user.c -server mach/memory_object_server.c $(SOURCE_DIR)/mach/memory_object.defs

MEMORY_OBJECT_DEFAULT_FILES = mach/memory_object_default.h \
	mach/memory_object_default_user.c \
	mach/memory_object_default_server.c

$(MEMORY_OBJECT_DEFAULT_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/memory_object_default.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKUFLAGS) -header mach/memory_object_default.h -user mach/memory_object_default_user.c -server mach/memory_object_default_server.c $(SOURCE_DIR)/mach/memory_object_default.defs

EXC_FILES = mach/exc.h mach/exc_user.c mach/exc_server.c

$(EXC_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/exc.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKUFLAGS) -header mach/exc.h -user mach/exc_user.c -server mach/exc_server.c $(SOURCE_DIR)/mach/exc.defs

DEVKUSER_FILES = mach/dev_forward.c

$(DEVKUSER_FILES): $(MIGDEPS) $(SOURCE_DIR)/device/device.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKUFLAGS) -header mach/dev_forward.h -user mach/dev_forward.c -server /dev/null $(SOURCE_DIR)/device/dev_forward.defs

MACH_HOST_FILES = mach/mach_host_server.c

$(MACH_HOST_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/mach_host.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGKSFLAGS) -header /dev/null -user /dev/null -server mach/mach_host_server.c $(SOURCE_DIR)/mach/mach_host.defs

MACH_DEBUG_FILES = mach_debug/mach_debug_server.c

$(MACH_DEBUG_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach_debug/mach_debug.defs
	-$(MAKE_MACH_DEBUG)
	$(MIG) -MD $(MIGKSFLAGS) -header /dev/null -user /dev/null -server mach_debug/mach_debug_server.c $(SOURCE_DIR)/mach_debug/mach_debug.defs

DEVICE_FILES = device/device.h device/device_server.c device/device_user.c

$(DEVICE_FILES): $(MIGDEPS) $(SOURCE_DIR)/device/device.defs
	-$(MAKE_DEVICE)
	$(MIG) -MD $(MIGKSFLAGS) -header device/device.h -user device/device_user.c -server device/device_server.c $(SOURCE_DIR)/device/device.defs

DEVICE_REPLY_FILES = device/device_reply.h device/device_reply_user.c

$(DEVICE_REPLY_FILES): $(MIGDEPS) $(SOURCE_DIR)/device/device_reply.defs
	-$(MAKE_DEVICE)
	$(MIG) -MD $(MIGKUFLAGS) -header device/device_reply.h -user device/device_reply_user.c -server /dev/null $(SOURCE_DIR)/device/device_reply.defs

DEVICE_PAGER_FILES = device/device_pager_server.c

$(DEVICE_PAGER_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/memory_object.defs
	-$(MAKE_DEVICE)
	$(MIG) -MD $(MIGKSFLAGS) -header /dev/null -user /dev/null -server device/device_pager_server.c $(SOURCE_DIR)/mach/memory_object.defs

DEFAULT_PAGER_OBJECT_FILES = mach/default_pager_object_server.c

$(DEFAULT_PAGER_OBJECT_FILES): $(MIGDEPS) $(SOURCE_DIR)/mach/default_pager_object.defs
	-$(MAKE_MACH)
	$(MIG) -MD $(MIGFLAGS) -header /dev/null -user /dev/null -server mach/default_pager_object_server.c $(SOURCE_DIR)/mach/default_pager_object.defs

LIBMIG_OBJS = $(MACH_FUNCS/$(REG_EXP)/&.o) $(MACH_PORT_FUNCS/$(REG_EXP)/&.o)

$(LIBMIG_OBJS): mach/$$(@:r).c
	$(KCC) -c $(CFLAGS) mach/$*.c

libmig.a:	$(LIBMIG_OBJS)
	rm -f libmig.a
	ar cq libmig.a $(LIBMIG_OBJS)
	ranlib libmig.a

#  Update the Makefile when any dependency files exist rather than only
#  when they are newer than the makefile (since it often gets touched
#  during reconfigurations when dependency files have yet to be
#  integrated).  The makefile is copied before doing any potential
#  integration so that the new and old versions can be compared when
#  done (by the SYS_RULE_* rules) to prevent re-building things when no
#  dependencies change.
#
#  N.B.  There is always an empty artificial dependency file M.d
#  (created by config) which will never be integrated and simply
#  prevents the wild-card expansion below from failing.
#
#  The default exit rule now brings the Makefile up to date, so it
#  shouldn't be necessary to execute this rule manually.
#

Makedep: *.${MD_SUFFIX}
	${EXIT_RULE}

.EXIT:
	${EXIT_RULE}

depend: 
	@echo you don\'t make depend any more.



#
#  Permit manually forced checkout of modules when the dependency
#  information is not current.
#

checkout:	${USE}



#
#  Clean up the current build directory.
#

clean:
	@for f in ${LOAD}; do echo rm -f $$f $$f.* && rm -f $$f $$f.*; done
	rm -f eddep tags *.o locore.i \
		errs linterrs makelinks *.log *LOG* ${CLEAN_EXTRA}


#
#  Run "lint" on the current build directory.  This will often be done
#  with a special configuration that defines the union of all options
#  used in all configurations so that all combinations are checked.
#  The LINTFILES variable allows lint to be easily run on an optional
#  sub-set of files specified on the make command line when this is
#  desired.
#

LINTFILES=$(SOURCE_DIR)/machine/Locore.c \
	${CFILES} \
	$(SOURCE_DIR)/machine/swapgeneric.c

lint:	ALWAYS assym.s
	@lint -n -hbxn -DGENERIC ${LINTOPTS} ${LINTFILES} | \
	    egrep -v 'struct/union .* never defined' | \
	    egrep -v 'possible pointer alignment problem'

ALWAYS:

MACHINEDIR=$(SOURCE_DIR)/machine

#
#  This macro is replaced by the set of rules which build everything in
#  OBJS.  The extra dependencies for each object file are inserted
#  following this by md which uses the keys on the sentinel line
#  below.
#

db_access.o: $(SOURCE_DIR)/ddb/db_access.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_aout.o: $(SOURCE_DIR)/ddb/db_aout.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_break.o: $(SOURCE_DIR)/ddb/db_break.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_command.o: $(SOURCE_DIR)/ddb/db_command.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_examine.o: $(SOURCE_DIR)/ddb/db_examine.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_expr.o: $(SOURCE_DIR)/ddb/db_expr.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_input.o: $(SOURCE_DIR)/ddb/db_input.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_lex.o: $(SOURCE_DIR)/ddb/db_lex.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_output.o: $(SOURCE_DIR)/ddb/db_output.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_print.o: $(SOURCE_DIR)/ddb/db_print.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_run.o: $(SOURCE_DIR)/ddb/db_run.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_sym.o: $(SOURCE_DIR)/ddb/db_sym.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_trap.o: $(SOURCE_DIR)/ddb/db_trap.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_variables.o: $(SOURCE_DIR)/ddb/db_variables.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_watch.o: $(SOURCE_DIR)/ddb/db_watch.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_write_cmd.o: $(SOURCE_DIR)/ddb/db_write_cmd.c
	${C_RULE_1A}$(SOURCE_DIR)/ddb/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_entry.o: $(SOURCE_DIR)/ipc/ipc_entry.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_hash.o: $(SOURCE_DIR)/ipc/ipc_hash.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_init.o: $(SOURCE_DIR)/ipc/ipc_init.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_kmsg.o: $(SOURCE_DIR)/ipc/ipc_kmsg.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_marequest.o: $(SOURCE_DIR)/ipc/ipc_marequest.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_mqueue.o: $(SOURCE_DIR)/ipc/ipc_mqueue.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_notify.o: $(SOURCE_DIR)/ipc/ipc_notify.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_object.o: $(SOURCE_DIR)/ipc/ipc_object.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_port.o: $(SOURCE_DIR)/ipc/ipc_port.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_pset.o: $(SOURCE_DIR)/ipc/ipc_pset.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_right.o: $(SOURCE_DIR)/ipc/ipc_right.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_space.o: $(SOURCE_DIR)/ipc/ipc_space.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_splay.o: $(SOURCE_DIR)/ipc/ipc_splay.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_table.o: $(SOURCE_DIR)/ipc/ipc_table.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_thread.o: $(SOURCE_DIR)/ipc/ipc_thread.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_debug.o: $(SOURCE_DIR)/ipc/mach_debug.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_msg.o: $(SOURCE_DIR)/ipc/mach_msg.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_port.o: $(SOURCE_DIR)/ipc/mach_port.c
	${C_RULE_1A}$(SOURCE_DIR)/ipc/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ast.o: $(SOURCE_DIR)/kern/ast.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

bootstrap.o: $(SOURCE_DIR)/kern/bootstrap.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

debug.o: $(SOURCE_DIR)/kern/debug.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

exception.o: $(SOURCE_DIR)/kern/exception.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

host.o: $(SOURCE_DIR)/kern/host.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_host.o: $(SOURCE_DIR)/kern/ipc_host.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_kobject.o: $(SOURCE_DIR)/kern/ipc_kobject.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_mig.o: $(SOURCE_DIR)/kern/ipc_mig.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_sched.o: $(SOURCE_DIR)/kern/ipc_sched.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ipc_tt.o: $(SOURCE_DIR)/kern/ipc_tt.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

kalloc.o: $(SOURCE_DIR)/kern/kalloc.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

lock.o: $(SOURCE_DIR)/kern/lock.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_clock.o: $(SOURCE_DIR)/kern/mach_clock.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_factor.o: $(SOURCE_DIR)/kern/mach_factor.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

machine.o: $(SOURCE_DIR)/kern/machine.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

printf.o: $(SOURCE_DIR)/kern/printf.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

priority.o: $(SOURCE_DIR)/kern/priority.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

processor.o: $(SOURCE_DIR)/kern/processor.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

queue.o: $(SOURCE_DIR)/kern/queue.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

sched_prim.o: $(SOURCE_DIR)/kern/sched_prim.c
	${C_RULE_1A_D}${VOLATILE}$(SOURCE_DIR)/kern/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

startup.o: $(SOURCE_DIR)/kern/startup.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

syscall_emulation.o: $(SOURCE_DIR)/kern/syscall_emulation.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

syscall_subr.o: $(SOURCE_DIR)/kern/syscall_subr.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

syscall_sw.o: $(SOURCE_DIR)/kern/syscall_sw.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

task.o: $(SOURCE_DIR)/kern/task.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

thread.o: $(SOURCE_DIR)/kern/thread.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

thread_swap.o: $(SOURCE_DIR)/kern/thread_swap.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

time_stamp.o: $(SOURCE_DIR)/kern/time_stamp.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

timer.o: $(SOURCE_DIR)/kern/timer.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

xpr.o: $(SOURCE_DIR)/kern/xpr.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

zalloc.o: $(SOURCE_DIR)/kern/zalloc.c
	${C_RULE_1A}$(SOURCE_DIR)/kern/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_host_server.o: mach/mach_host_server.c
	${C_RULE_1A}mach/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_port_server.o: mach/mach_port_server.c
	${C_RULE_1A}mach/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_server.o: mach/mach_server.c
	${C_RULE_1A}mach/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

memory_object_default_user.o: mach/memory_object_default_user.c
	${C_RULE_1A}mach/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

memory_object_user.o: mach/memory_object_user.c
	${C_RULE_1A}mach/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

mach_debug_server.o: mach_debug/mach_debug_server.c
	${C_RULE_1A}mach_debug/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

memory_object.o: $(SOURCE_DIR)/vm/memory_object.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_debug.o: $(SOURCE_DIR)/vm/vm_debug.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_fault.o: $(SOURCE_DIR)/vm/vm_fault.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_init.o: $(SOURCE_DIR)/vm/vm_init.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_kern.o: $(SOURCE_DIR)/vm/vm_kern.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_map.o: $(SOURCE_DIR)/vm/vm_map.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_object.o: $(SOURCE_DIR)/vm/vm_object.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_pageout.o: $(SOURCE_DIR)/vm/vm_pageout.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_resident.o: $(SOURCE_DIR)/vm/vm_resident.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

vm_user.o: $(SOURCE_DIR)/vm/vm_user.c
	${C_RULE_1A}$(SOURCE_DIR)/vm/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

blkio.o: $(SOURCE_DIR)/device/blkio.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

chario.o: $(SOURCE_DIR)/device/chario.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

cirbuf.o: $(SOURCE_DIR)/device/cirbuf.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

dev_lookup.o: $(SOURCE_DIR)/device/dev_lookup.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

dev_name.o: $(SOURCE_DIR)/device/dev_name.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

dev_pager.o: $(SOURCE_DIR)/device/dev_pager.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

device_reply_user.o: device/device_reply_user.c
	${C_RULE_1A}device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

device_server.o: device/device_server.c
	${C_RULE_1A}device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

device_user.o: device/device_user.c
	${C_RULE_1A}device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

device_init.o: $(SOURCE_DIR)/device/device_init.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ds_routines.o: $(SOURCE_DIR)/device/ds_routines.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

net_io.o: $(SOURCE_DIR)/device/net_io.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

subrs.o: $(SOURCE_DIR)/device/subrs.c
	${C_RULE_1A}$(SOURCE_DIR)/device/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

boot_printf.o: $(SOURCE_DIR)/boot_ufs/boot_printf.c
	${C_RULE_1A}$(SOURCE_DIR)/boot_ufs/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

default_pager.o: $(SOURCE_DIR)/boot_ufs/default_pager.c
	${C_RULE_1A}$(SOURCE_DIR)/boot_ufs/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

def_pager_setup.o: $(SOURCE_DIR)/boot_ufs/def_pager_setup.c
	${C_RULE_1A}$(SOURCE_DIR)/boot_ufs/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

file_io.o: $(SOURCE_DIR)/boot_ufs/file_io.c
	${C_RULE_1A}$(SOURCE_DIR)/boot_ufs/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

load.o: $(SOURCE_DIR)/boot_ufs/load.c
	${C_RULE_1A}$(SOURCE_DIR)/boot_ufs/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ioconf.o: ioconf.c
	${C_RULE_1A}${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

pmap.o: $(SOURCE_DIR)/intel/pmap.c
	${C_RULE_1A}$(SOURCE_DIR)/intel/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

bcopy.o: $(SOURCE_DIR)/i386/bcopy.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

bzero.o: $(SOURCE_DIR)/i386/bzero.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

exec.o: $(SOURCE_DIR)/i386/exec.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

fpu.o: $(SOURCE_DIR)/i386/fpu.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

gcc.o: $(SOURCE_DIR)/i386/gcc.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

gdt.o: $(SOURCE_DIR)/i386/gdt.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

hardclock.o: $(SOURCE_DIR)/i386/hardclock.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

i386_init.o: $(SOURCE_DIR)/i386/i386_init.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

idt.o: $(SOURCE_DIR)/i386/idt.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

init.o: $(SOURCE_DIR)/i386/init.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

interrupt.o: $(SOURCE_DIR)/i386/interrupt.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

io_emulate.o: $(SOURCE_DIR)/i386/io_emulate.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

io_map.o: $(SOURCE_DIR)/i386/io_map.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_disasm.o: $(SOURCE_DIR)/i386/db_disasm.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_interface.o: $(SOURCE_DIR)/i386/db_interface.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

db_trace.o: $(SOURCE_DIR)/i386/db_trace.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ktss.o: $(SOURCE_DIR)/i386/ktss.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ldt.o: $(SOURCE_DIR)/i386/ldt.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

loose_ends.o: $(SOURCE_DIR)/i386/loose_ends.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

ntoh.o: $(SOURCE_DIR)/i386/ntoh.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

pic.o: $(SOURCE_DIR)/i386/pic.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

pit.o: $(SOURCE_DIR)/i386/pit.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

pcb.o: $(SOURCE_DIR)/i386/pcb.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

phys.o: $(SOURCE_DIR)/i386/phys.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

read_fault.o: $(SOURCE_DIR)/i386/read_fault.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

setroot.o: $(SOURCE_DIR)/i386/setroot.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

spl.o: $(SOURCE_DIR)/i386/spl.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

trap.o: $(SOURCE_DIR)/i386/trap.c
	${C_RULE_1A}$(SOURCE_DIR)/i386/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

_setjmp.o: $(SOURCE_DIR)/i386/_setjmp.s
	${S_RULE_1A}$(SOURCE_DIR)/i386/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

autoconf.o: $(SOURCE_DIR)/i386at/autoconf.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

blit.o: $(SOURCE_DIR)/i386at/blit.c
	${C_RULE_1A_D}${FORCE_VOLATILE}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

c765.o: $(SOURCE_DIR)/i386at/c765.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

com.o: $(SOURCE_DIR)/i386at/com.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

conf.o: $(SOURCE_DIR)/i386at/conf.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

hd.o: $(SOURCE_DIR)/i386at/hd.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

if_3c501.o: $(SOURCE_DIR)/i386at/if_3c501.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

if_pc586.o: $(SOURCE_DIR)/i386at/if_pc586.c
	${C_RULE_1A_D}${VOLATILE}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

if_ns8390.o: $(SOURCE_DIR)/i386at/if_ns8390.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

iopl.o: $(SOURCE_DIR)/i386at/iopl.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

kd.o: $(SOURCE_DIR)/i386at/kd.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

kdasm.o: $(SOURCE_DIR)/i386at/kdasm.s
	${S_RULE_1A}$(SOURCE_DIR)/i386at/${S_RULE_1B}; \
	${S_RULE_2}; \
	${S_RULE_3}

kd_event.o: $(SOURCE_DIR)/i386at/kd_event.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

kd_mouse.o: $(SOURCE_DIR)/i386at/kd_mouse.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

kd_queue.o: $(SOURCE_DIR)/i386at/kd_queue.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

m765drv.o: $(SOURCE_DIR)/i386at/m765drv.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

m765knl.o: $(SOURCE_DIR)/i386at/m765knl.c
	${C_RULE_1A_D}$(SOURCE_DIR)/i386at/${C_RULE_1B_D}; \
	${C_RULE_2_D}; \
	${C_RULE_3_D}; \
	${C_RULE_4_D}

pic_isa.o: $(SOURCE_DIR)/i386at/pic_isa.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}

rtc.o: $(SOURCE_DIR)/i386at/rtc.c
	${C_RULE_1A}$(SOURCE_DIR)/i386at/${C_RULE_1B}; \
	${C_RULE_2}; \
	${C_RULE_3}; \
	${C_RULE_4}


-include Makedep
