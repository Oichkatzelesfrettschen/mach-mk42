#
# Copyright (c) 1995, 1994, 1993, 1992, 1991, 1990  
# Open Software Foundation, Inc. 
#  
# Permission to use, copy, modify, and distribute this software and 
# its documentation for any purpose and without fee is hereby granted, 
# provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation, and that the name of ("OSF") or Open Software 
# Foundation not be used in advertising or publicity pertaining to 
# distribution of the software without specific, written prior permission. 
#  
# OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
# FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL OSF BE LIABLE FOR ANY 
# SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
# ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING 
# OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE 
#
#
# OSF Research Institute MK6.1 (unencumbered) 1/31/1995

VPATH	= ${MAKETOP}mach_services/include/servers:${${TARGET_MACHINE}_VPATH}

PROGRAMS 		= bootstrap

ILIST			= bootstrap
IDIR			= ${MACH3_ROOT_SERVERS_IDIR}

USE_STATIC_LIBRARIES	=

LIBS			= -lmach -lsa_mach -lmach

CFLAGS			= ${${OBJECT_FORMAT}_EXTRA_WARNINGS} -Werror
serviceServer.o_CFLAGS	=

INCFLAGS		= -I.

MIG_DEFS		= service.defs

OFILES			= bootstrap.o service.o serviceServer.o \
			  file_io.o load.o strfcns.o \
			  ${${TARGET_MACHINE}_OFILES}

DEPENDENCIES		=

.if exists(${TARGET_MACHINE}/machdep.mk)
.include "${TARGET_MACHINE}/machdep.mk"
.endif
.include <${RULES_MK}>

.if exists(depend.mk)
.include "depend.mk"
.endif
