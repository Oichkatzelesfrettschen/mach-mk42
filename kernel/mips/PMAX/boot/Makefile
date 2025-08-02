#
# Mach Operating System
# Copyright (c) 1991,1990 Carnegie Mellon University
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
# Revision 2.4  91/02/05  17:38:23  mrt
# 	Changed to use new Mach copyright
# 	[91/02/02  12:03:46  mrt]
# 
# Revision 2.3  90/12/05  23:29:18  af
# 	Created.
# 	[90/12/02            af]
# 
#

INSTALL	=	/usr/cs/etc/install -q -c

all:		mkboot primary_boot 16sec.boot test

install:	mkboot.install

mkboot.install:	$(DDIR)/mkboot

$(DDIR)/mkboot:	mkboot
	cp -p mkboot $(DDIR)/mkboot
	strip $(DDIR)/mkboot
	chmod 755 $(DDIR)/mkboot


BOOTOBJS =	start.o c_misc.o asm_misc.o ufs.o primary_boot.o
OBJS	 =	mkboot.o $(BOOTOBJS)

# Base of source tree.  Look here for include files.
IDIR = ../../..

CFLAGS= -O2 $(CDEBUGFLAGS) -I$(IDIR)
ASFLAGS= -w -O2 -I$(IDIR)

mkboot:	mkboot.o
	rm -f mkboot
	$(CC) -o mkboot $(CFLAGS) mkboot.o

primary_boot:	$(BOOTOBJS)
	ld -N -T 80600000 -o primary_boot $(BOOTOBJS)

test:	start.o asm_misc.o test.o
	ld -N -T 80030000 -o test start.o asm_misc.o test.o

16sec.boot: mkboot primary_boot label
	@echo
	@echo "THE FOLLOWING SHOULD NOT BE DONE LIGHTLY"
	@echo
	@echo "To install this boot you will want to do"
	@echo "something like this:"
	@echo "	mkboot -o /dev/rrz0a -u 16sec.boot primary_boot
	@echo "Or send the output to a <file> and then"
	@echo "	dd if=<file> of=/dev/rrz0a"
	@echo
	@echo "This is the information mkboot will ask you:"
	@echo
	mkboot -o 16sec.boot -u label -s primary_boot

label: label.b
	uudecode label.b

.c.o:
	$(CC) -c -MD $(CFLAGS) $*.c
	md -u Makedep -d $*.d

.s.o:
	$(CC) -c -MD $(ASFLAGS) $*.s
	md -u Makedep -d $*.d

LIBBASE = $(MAKECONF:h)/../../../release/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/lib
BINBASE = $(MAKECONF:h)/../../../release/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/bin
OBJBASE = $(MAKECONF:h)/../../../obj/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/kernel

release:
	-if [ -f $(BINBASE)/mkboot ]; then true ; else ln -s $(OBJBASE)/bin/mkboot $(BINBASE) ; fi

-include Makedep
