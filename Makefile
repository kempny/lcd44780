#################################################################################
# Makefile:
#	This file is part of lcdi2c - RaspberryPi I2C LCD  
#       display library
#       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
#
#################################################################################
#    lcdi2c is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    lcdi2c is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with lcdi2c.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################

VERSION=$(shell cat VERSION)
DESTDIR?=/usr
PREFIX?=/local

LDCONFIG?=ldconfig

ifneq ($V,1)
Q ?= @
endif

DYNAMIC=liblcdi2c.so.$(VERSION)

DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEBUG) $(DEFS) -Wformat=2 -Wall -Wextra -Winline $(INCLUDE) -pipe -fPIC

LIBS    = -lpthread 

###############################################################################

SRC	=	lcdi2clib.c delay.c					

HEADERS =	lcdi2c.h

OBJ	=	$(SRC:.c=.o)

all:		$(DYNAMIC)


$(DYNAMIC):	$(OBJ)
	$Q echo "[Link (Dynamic)]"
	$Q $(CC) -shared -Wl,-soname,liblcdi2c.so$(WIRINGPI_SONAME_SUFFIX) -o liblcdi2c.so.$(VERSION) $(LIBS) $(OBJ)


.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ)  *~ Makefile.bak liblcdi2c.*



.PHONY:	install
install:	$(DYNAMIC)
	$Q echo "[Install Headers]"
	$Q install -m 0755 -d						$(DESTDIR)$(PREFIX)/include
	$Q install -m 0644 $(HEADERS)					$(DESTDIR)$(PREFIX)/include
	$Q echo "[Install Lib]"
	$Q install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	$Q install -m 0755 liblcdi2c.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/liblcdi2c.so.$(VERSION)
	$Q ln -sf $(DESTDIR)$(PREFIX)/lib/liblcdi2c.so.$(VERSION)	$(DESTDIR)/lib/liblcdi2c.so
	$Q $(LDCONFIG)


.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q cd $(DESTDIR)$(PREFIX)/include/ && rm -f $(HEADERS)
	$Q cd $(DESTDIR)$(PREFIX)/lib/     && rm -f liblcdi2c.*
	$Q $(LDCONFIG)



# DO NOT DELETE
