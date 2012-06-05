# Makefile -- Hello
# Copyright 2000, 2002, 2003 Free Software Foundation, Inc.
# Written by Stephane Carrez (stcarrez@nerim.fr)
# 
# This file is part of GEL.
# 
# GEL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GEL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GEL; see the file COPYING.  If not, write to
# the Free Software Foundation, 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#
# Host generic commands
RM=rm -f
INSTALL=cp

# 68HC11 compiler and tools
DEVC_PREFIX=m6812-elf-
CC=$(DEVC_PREFIX)gcc
SIZE=$(DEVC_PREFIX)size
OBJCOPY=$(DEVC_PREFIX)objcopy

# Compile with debugging info (-g), with size optimization (-Os)
# and 16-bit integers (-mshort)
CFLAGS=-g -Os -mshort

# Must pass -mshort to the linker, use the m68hc11elfb linker script
# which includes 'memory.x'.
LDFLAGS=$(CFLAGS) -Wl,-m,m68hc12elfb

# Flags for objcopy tool
OBJCOPY_FLAGS=--only-section=.text \
              --only-section=.rodata \
	      --only-section=.vectors \
	      --only-section=.data

# List of C sources
CSRCS=hello.c

# List of objects
OBJS=$(CSRCS:.c=.o)

PROGS=hello.elf

all::	$(PROGS) hello.s19

hello.elf:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

hello.s19:    hello.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --output-target=srec $< $@

install::	$(PROGS)

clean::
	$(RM) *.elf *.o *.s19

