#
# $Id: vars.mk,v 1.2 2009/09/18 14:35:20 philipn Exp $
#
# Makefile variables used for compiling, installing etc
#
# Copyright (C) 2008  BBC Research, Stuart Cunningham <stuart_hc@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

# TOPLEVEL must be set by any Makefile which includes this .mk file
ifndef TOPLEVEL
  $(error TOPLEVEL variable not defined)
endif

LIBMXF_DIR = $(TOPLEVEL)/lib

# MingW cross-compile path prefix
ifdef MINGW_CC_PREFIX
BIN_PREFIX=$(MINGW_CC_PREFIX)
else
BIN_PREFIX=
endif

# Variables for compilation of libMXF client applications
CC = $(BIN_PREFIX)gcc
INCLUDES = -I. -I$(LIBMXF_DIR)/include
CFLAGS = -Wall -W -Werror -g -O2 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE $(INCLUDES)
AR = $(BIN_PREFIX)ar crv

# Get the OS name e.g. Linux, MINGW32_NT-5.0, Darwin, SunOS
ifdef MINGW_CC_PREFIX
OSNAME=MINGW_CC_OS
else
OSNAME=$(shell uname -s)
endif

# Linux specific
ifeq ($(OSNAME),Linux)
UUIDLIB = -luuid
endif

# Solaris specific
ifeq ($(OSNAME),SunOS)
UUIDLIB = -luuid
endif

# MS Windows specific when building with msys
ifeq (MINGW32_NT,$(findstring MINGW32_NT,$(OSNAME)))
UUIDLIB = -lole32
endif

# MS Windows specific when cross-compiling with MingW
ifeq ($(OSNAME),MINGW_CC_OS)
UUIDLIB = -lole32
endif

# Default install location is /usr/local
MXF_INSTALL_PREFIX ?= /usr/local
