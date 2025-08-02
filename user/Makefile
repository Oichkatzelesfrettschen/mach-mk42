#
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#  
# HISTORY
# $Log:	Makefile,v $
# Revision 2.2  89/10/16  15:22:43  rwd
# 	Created
# 	[89/10/06            rwd]
# 
#

LIBBASE = $(MAKECONF:h)/../../../release/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/lib
OBJBASE = $(MAKECONF:h)/../../../obj/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/user

install: 
	-if [ -f $(LIBBASE)/libmach_sa.a ]; then true ; else ln -s $(OBJBASE)/libmach/libmach_sa.a $(LIBBASE) ; fi
	-if [ -f $(LIBBASE)/libthreads.a ]; then true ; else ln -s $(OBJBASE)/threads/libthreads.a $(LIBBASE) ; fi
