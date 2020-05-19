#
#  Makefile for tools
#
#  Copyright (c) 2020 Flavio Augusto (@facmachado)
#
#  This software may be modified and distributed under the terms
#  of the MIT license. See the LICENSE file for details.
#


#
# Initial consts & vars
#

.RECIPEPREFIX=.
.DEFAULT_GOAL=$(shell uname -m)

CC=gcc
STRIP=strip
CCFLAGS=-W -Wall -static

SRC=$(wildcard *.c)
B64=$(patsubst %.c,%,$(SRC))
B32=$(patsubst %.c,%32,$(SRC))

PREFIX=/usr/local


#
# Pattern workarounds for batch compiling
#

%: %.c
. $(CC) $< $(CCFLAGS) -o $@ && $(STRIP) -s $@

%32: %.c
. $(CC) $< $(CCFLAGS) -m32 -o $@ && $(STRIP) -s $@


#
# Arch directives
#

i386: $(B32)

i486: i386

i586: i386

i686: i386

x86_64: $(B64)


#
# Setup.exe routines
#

install:
ifeq ($(.DEFAULT_GOAL), x86_64)
. (for i in $(B64); do cp $$i $(PREFIX)/bin; chmod 700 $(PREFIX)/bin/$$i; done)
else ifeq ($(.DEFAULT_GOAL), i?86)
. (for i in $(B32); do cp $$i $(PREFIX)/bin; chmod 700 $(PREFIX)/bin/$$i; done)
endif

uninstall:
. (cd $(PREFIX)/bin && rm -f $(B64) $(B32))


#
# Clean project routine
#

clean:
. rm -f $(B64) $(B32)
