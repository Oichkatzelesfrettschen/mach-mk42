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
# OSF Research Institute MK6.1 (unencumbered) 1/31/1995

PROGRAMS 		= default_pager

ILIST			= default_pager
IDIR			= ${MACH3_ROOT_SERVERS_IDIR}

VPATH			= mach

USE_STATIC_LIBRARIES	=

EXPINC_SUBDIRS		= mach
COMP_SUBDIRS		=

SUBDIRS			= mach

LIBS			= -lcthreads -lmach -lsa_mach -lmach

CFLAGS			= -D_ANSI_C_SOURCE \
			  ${${OBJECT_FORMAT}_EXTRA_WARNINGS}
default_pager_objectServer.o_CFLAGS =

MIG_DEFS		= default_pager_object.defs

OFILES			= default_pager.o default_pager_objectServer.o \
			  file_io.o wiring.o kalloc.o strfcns.o

DEPENDENCIES		=

.include <${RULES_MK}>

.if exists(depend.mk)
.include "depend.mk"
.endif
