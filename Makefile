#
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#  
# HISTORY
# $Log:	Makefile,v $
# Revision 2.8  90/10/25  14:41:25  rwd
# 	Don't make clean include area.
# 	[90/10/24            rwd]
# 
# Revision 2.7  90/09/09  14:30:48  rpd
# 	Added install : all
# 	[90/08/20            rwd]
# 
# Revision 2.6  90/06/02  14:44:48  rpd
# 	Release mig before building the include directory.
# 	[90/03/26  21:21:20  rpd]
# 
# Revision 2.5  89/12/08  19:51:53  rwd
# 	Fix thread make
# 	[89/11/30            rwd]
# 
# Revision 2.4  89/10/16  15:21:47  rwd
# 	Change include file make
# 	[89/10/05            rwd]
# 
# Revision 2.3  89/08/09  14:32:59  rwd
# 	Make clean libmach
# 	[89/08/07            rwd]
# 
# Revision 2.2  89/08/05  16:20:46  rwd
# 	First version
# 	[89/08/05  16:18:56  rwd]
# 
#

install: all

all:
	cd kernel;$(MAKE)
	cd kernel/src/mig;$(MAKE) release
	cd include;$(MAKE)
	cd user/libmach;$(MAKE) clean;$(MAKE)
	cd user/threads;$(MAKE)
	cd user;$(MAKE) install
