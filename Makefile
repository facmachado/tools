#
#  Makefile for tools
#
#  Copyright (c) 2020 Flavio Augusto (@facmachado)
#
#  This software may be modified and distributed under the terms
#  of the MIT license. See the LICENSE file for details.
#


.RECIPEPREFIX=.
.DEFAULT_GOAL=exe64

CC=gcc
STRIP=strip
CCFLAGS=-W -Wall -static

SRC=$(wildcard *.c)
B64=$(patsubst %.c,%,$(SRC))
B32=$(patsubst %.c,%32,$(SRC))


%: %.c
. $(CC) $< $(CCFLAGS) -o $@ && $(STRIP) -s $@

%32: %.c
. $(CC) $< $(CCFLAGS) -m32 -o $@ && $(STRIP) -s $@


exe64: $(B64)

exe32: $(B32)


install:
.

uninstall:
.


.PHONY clean

clean:
. rm -f $(B64) $(B32)
