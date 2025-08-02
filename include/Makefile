#
# Mach Operating System
# Copyright (c) 1990 Carnegie-Mellon University
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#  
# HISTORY
# $Log:	Makefile,v $
# Revision 2.19  91/01/08  21:22:30  rpd
# 	Added i386at/disk.h.
# 
# Revision 2.18  91/01/08  19:18:25  rpd
# 	Added i386/fp_reg.h.
# 	[91/01/08            rpd]
# 
# Revision 2.17  91/01/08  15:08:33  rpd
# 	Added mach_debug/hash_info.h.
# 	[91/01/04            rpd]
# 
# Revision 2.16  90/12/05  23:26:49  af
# 
# 
# Revision 2.15  90/12/05  20:42:01  af
# 	Revised PMAX device exports, for new copyright free code.
# 	[90/12/04            af]
# 
# Revision 2.14  90/11/05  14:24:54  rpd
# 	Added machine/cthreads.h include file from libthreads.
# 	[90/11/01            rwd]
# 	Added REG_EXP.
# 	[90/10/29            rwd]
# 
# Revision 2.13  90/09/09  23:19:43  rpd
# 	Added if_se.h for MIPS mapped ether.
# 	[90/08/23            af]
# 
# Revision 2.12  90/08/06  15:06:31  rwd
# 	Rid ourselves of sys/types.h and sys/ioctl.h.  These are bad to
# 	have around for server compilations.
# 	[90/07/16            rwd]
# 
# Revision 2.11  90/06/02  14:44:56  rpd
# 	Fixed definition of SRCBASE so that symbolic links work
# 	when BCSBBASE isn't defined.
# 	[90/05/01            rpd]
# 
# 	Added mach_debug/vm_info.h.
# 	[90/04/20            rpd]
# 
# 	Use symbolic links instead of copying.
# 	[90/03/27            rpd]
# 	Removed mach/mach_extra.h, mach/vm_param.h.
# 	Added new host/processor header files.
# 	Added mach/notify.defs, mach/mach_port.defs.
# 	Changed mach_debug/ files.
# 	mach/mach.h comes from libmach now.
# 	[90/03/26  21:26:07  rpd]
# 
# Revision 2.10  90/05/08  16:43:31  dbg
# 	Ensure that we always find MiG.
# 	[90/05/08            dbg]
# 
# Revision 2.9  90/05/03  15:14:31  dbg
# 	Get the dependencies right.
# 	Fix list of MACH_DEBUG files.
# 	[90/05/02            dbg]
# 
# 	Add i386 support.
# 	[90/04/23            dbg]
# 
# Revision 2.8  90/03/14  21:09:52  rwd
# 	Added default_pager_object.defs and host_types.h.
# 	[90/01/22            rwd]
# 
# Revision 2.7  90/01/22  23:04:10  af
# 	Added vm_attributes.h
# 	[89/12/09  10:43:29  af]
# 
# 	Added PMAX device headers.
# 	[89/11/30            af]
# 
# Revision 2.6  89/12/08  19:46:03  rwd
# 	Added PMAX device headers.
# 	[89/11/30            af]
# 
# Revision 2.5  89/11/29  14:07:49  af
# 	Added one more mips file.
# 	[89/11/28  13:16:27  af]
# 
# 	Added varargs.h in the include list.  Copied from libmach.
# 	I think there are now three copies of varargs around...
# 	needs fixing.
# 	[89/10/28  10:52:10  af]
# 
# 	Added MIPS files.
# 	[89/10/28  09:51:53  af]
# 
# Revision 2.4  89/11/14  11:02:11  dbg
# 	Fix double make rule for 'sys' directory.
# 	[89/11/14            dbg]
# 
# Revision 2.3  89/11/13  15:26:39  dbg
# 	Add vaxuba directory and header files for QD, QV drivers.
# 	[89/11/13            dbg]
# 
# Revision 2.2  89/10/16  15:21:55  rwd
# 	Created version for $BCSBBASE/include
# 	[89/10/05            rwd]
# 
#

#
# Makefile to populate include directory from kernel, libmach, libthreads.
#
# Assume in {base}/include; sources in {base}/src
#

SRCBASE=	${BCSBBASE?$(BCSBBASE):$(MAKECONF:h)/..}
MACH_KERNEL=	${SRCBASE}/kernel
LIBMACH=	${SRCBASE}/user/libmach
LIBTHREADS=	${SRCBASE}/user/threads

MACH_INCLUDES=	mach/boolean.h mach/error.h mach/exception.h \
		mach/kern_return.h mach/mach_traps.h \
		mach/mach_types.h mach/machine.h mach/memory_object.h \
		mach/message.h mach/mig_errors.h mach/msg_type.h \
		mach/notify.h mach/port.h mach/std_types.h mach/syscall_sw.h \
		mach/task_info.h mach/task_special_ports.h mach/thread_info.h \
		mach/thread_special_ports.h mach/thread_status.h \
		mach/time_value.h mach/vm_inherit.h \
		mach/vm_prot.h mach/vm_statistics.h mach/vm_attributes.h \
		mach/host_info.h mach/mach_param.h mach/policy.h \
		mach/processor_info.h mach/thread_switch.h \
		mach/mach_types.defs mach/std_types.defs

MACH_DEFS=	mach/exc.defs mach/mach.defs \
		mach/mach_host.defs mach/mach_port.defs mach/notify.defs \
		mach/memory_object.defs mach/memory_object_default.defs \
		mach/default_pager_object.defs

MACH_HDRS=	mach/exc.h mach/mach_interface.h mach/memory_object_user.h \
		mach/mach_host.h mach/mach_port.h \
		mach/memory_object_default.h mach/default_pager_object.h

MACH_MACHINE_INCLUDES=\
		boolean.h \
		exception.h \
		kern_return.h \
		syscall_sw.h \
		thread_status.h \
		vm_param.h \
		vm_types.h

MACH_VAX	= mach/vax/
MACH_SUN3	= mach/sun3/
MACH_MIPS	= mach/mips/
MACH_I386	= mach/i386/

MACH_VAX_INCLUDES=	${MACH_MACHINE_INCLUDES/$(REG_EXP)/${MACH_VAX}&}
MACH_SUN3_INCLUDES=	${MACH_MACHINE_INCLUDES/$(REG_EXP)/${MACH_SUN3}&} \
			mach/sun3/reg.h
MACH_MIPS_INCLUDES=	${MACH_MACHINE_INCLUDES/$(REG_EXP)/${MACH_MIPS}&} \
			mach/mips/asm.h mach/mips/mips_instruction.h
MACH_I386_INCLUDES=	${MACH_MACHINE_INCLUDES/$(REG_EXP)/${MACH_I386}&} \
			i386/fp_reg.h

MACH_FILES=	${MACH_INCLUDES} ${MACH_DEFS} ${MACH_HDRS} \
		${MACH_VAX_INCLUDES} ${MACH_SUN3_INCLUDES} \
		${MACH_MIPS_INCLUDES} ${MACH_I386_INCLUDES}

MACH_DEBUG_INCLUDES=\
		mach_debug/ipc_info.h mach_debug/vm_info.h \
		mach_debug/zone_info.h mach_debug/page_info.h \
		mach_debug/hash_info.h \
		mach_debug/mach_debug_types.h \
		mach_debug/mach_debug_types.defs

MACH_DEBUG_DEFS=\
		mach_debug/mach_debug.defs
MACH_DEBUG_HDRS=\
		${MACH_DEBUG_DEFS:.defs=.h}

MACH_DEBUG_FILES=\
		${MACH_DEBUG_INCLUDES} ${MACH_DEBUG_DEFS} ${MACH_DEBUG_HDRS}

DEVICE_INCLUDES= \
		device/device_types.h device/net_status.h device/tty_status.h \
		device/device_types.defs

DEVICE_VAX_INCLUDES= \
		vaxuba/qdioctl.h vaxuba/qdreg.h vaxuba/qduser.h \
		vaxuba/qevent.h vaxuba/qvioctl.h vaxuba/qvreg.h

DEVICE_PMAX_INCLUDES= \
		mips/PMAX/screen.h mips/PMAX/mapped_scsi.h mips/PMAX/if_se.h \
		mips/PMAX/lk201.h

DEVICE_I386_INCLUDES= \
		i386at/disk.h

DEVICE_DEFS=	device/device.defs device/device_reply.defs \
		device/device_request.defs

DEVICE_HDRS=	${DEVICE_DEFS:.defs=.h}

DEVICE_FILES=	${DEVICE_INCLUDES} ${DEVICE_DEFS} ${DEVICE_HDRS} \
		${DEVICE_VAX_INCLUDES} ${DEVICE_PMAX_INCLUDES} \
		${DEVICE_I386_INCLUDES}

DIRS=	device mach mach/vax mach/sun3 mach/mips mach/i386 \
	mach_debug \
	./mips mips/PMAX sys vaxuba \
	./vax ./sun3 ./i386 ./i386at
LINKS=  mach/vax_mach mach/sun3_mach mach/pmax_mach mach/i386_mach \
	mach/machine sun3_mach i386_mach pmax_mach vax_mach machine

LIBMACH_FILES=	mach/mach.h mach_init.h setjmp.h strings.h varargs.h

LIBTHREADS_FILES=\
		cthreads.h sun3/cthreads.h mips/cthreads.h vax/cthreads.h \
		mips i386/cthreads.h

COMPAT_FILES_1=	kern/mach.h sys/kern_return.h sys/message.h
COMPAT_FILES_2=	./mach.h ./mig_errors.h ./msg_type.h

COMPAT_FILES=	${COMPAT_FILES_1} ${COMPAT_FILES_2}
COMPAT_DIRS=	kern sys

#
# Directory lists overlap - build their union.
#
ALLDIRLIST=	${DIRS} ${COMPAT_DIRS}
ALLDIRS=	${ALLDIRLIST:u}

all:	${ALLDIRS} ${LINKS} ${MACH_FILES} ${DEVICE_FILES} ${MACH_DEBUG_FILES} \
	${LIBMACH_FILES} ${LIBTHREADS_FILES} \
	${COMPAT_FILES}

${ALLDIRS}:
	-if [ ! -d $@ ]; then mkdir $@; fi

${LINKS}:
	-if [ ! -d mach/vax_mach  ]; then ln -s vax mach/vax_mach; fi
	-if [ ! -d mach/sun3_mach ]; then ln -s sun3 mach/sun3_mach; fi
	-if [ ! -d mach/pmax_mach ]; then ln -s mips mach/pmax_mach; fi
	-if [ ! -d mach/i386_mach ]; then ln -s i386 mach/i386_mach; fi
	-if [ ! -d mach/machine ]; then ln -s @sys mach/machine; fi
	-if [ ! -d vax_mach  ]; then ln -s vax vax_mach; fi
	-if [ ! -d sun3_mach ]; then ln -s sun3 sun3_mach; fi
	-if [ ! -d pmax_mach ]; then ln -s mips pmax_mach; fi
	-if [ ! -d i386_mach ]; then ln -s i386 i386_mach; fi
	-if [ ! -d machine ]; then ln -s @sys machine; fi

${MACH_INCLUDES} ${MACH_DEFS} ${MACH_VAX_INCLUDES} ${MACH_SUN3_INCLUDES} \
${MACH_MIPS_INCLUDES} ${MACH_I386_INCLUDES} \
${MACH_DEBUG_INCLUDES} ${MACH_DEBUG_DEFS}:
	ln -s ${MACH_KERNEL}/$@ $@

${DEVICE_INCLUDES} ${DEVICE_DEFS} ${DEVICE_VAX_INCLUDES} \
${DEVICE_PMAX_INCLUDES} ${DEVICE_I386_INCLUDES}:
	ln -s ${MACH_KERNEL}/$@ $@

${LIBMACH_FILES}:
	ln -s ${LIBMACH}/$(@F) $@

${LIBTHREADS_FILES}:
	ln -s ${LIBTHREADS}/$@ $@

${COMPAT_FILES_1}:
	ln -s ../mach/$(@F) $@

${COMPAT_FILES_2}:
	ln -s mach/$(@F) $@

%.h:	%.defs
	mig -I. -server /dev/null -user /dev/null -header %.h %.defs

mach/memory_object_user.h:	mach/memory_object.defs
	mig -I. -server /dev/null -user /dev/null \
		-header $@ mach/memory_object.defs

mach/mach_interface.h:	mach/mach.defs
	mig -I. -server /dev/null -user /dev/null \
		-header $@ mach/mach.defs

clean:
	rm -rf *
