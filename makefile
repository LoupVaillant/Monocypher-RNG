# This file is dual-licensed.  Choose whichever licence you want from
# the two licences listed below.
#
# The first licence is a regular 2-clause BSD licence.  The second licence
# is the CC-0 from Creative Commons. It is intended to release Monocypher
# to the public domain.  The BSD licence serves as a fallback option.
#
# SPDX-License-Identifier: BSD-2-Clause OR CC0-1.0
#
# ------------------------------------------------------------------------
#
# Copyright (c) 2019, Loup Vaillant
# All rights reserved.
#
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the
#    distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ------------------------------------------------------------------------
#
# Written in 2017-2019 by Loup Vaillant and Fabio Scotoni
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related neighboring rights to this software to the public domain
# worldwide.  This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software.  If not, see
# <https://creativecommons.org/publicdomain/zero/1.0/>

CC=gcc -std=c99
CFLAGS= -pedantic -Wall -Wextra -O3 -march=native
DESTDIR=
PREFIX=usr/local
LIBDIR=$(PREFIX)/lib
PKGCONFIG=$(DESTDIR)/$(LIBDIR)/pkgconfig
MAN_DIR=$(DESTDIR)/$(PREFIX)/share/man/man3
SONAME=libmonocypher-rng.so.1

VERSION=__git__

.PHONY: all library static-library dynamic-library \
        install install-doc                        \
        check test                                 \
        clean uninstall                            \
        dist

all: library

library: static-library dynamic-library
static-library : lib/libmonocypher-rng.a
dynamic-library: lib/libmonocypher-rng.so lib/$(SONAME)

clean:
	rm -rf lib/
	rm -f  *.out

lib/libmonocypher-rng.a: lib/monocypher-rng.o
	ar cr $@ $^
lib/libmonocypher-rng.so: lib/$(SONAME)
	@mkdir -p $(@D)
	ln -sf `basename $<` $@
lib/$(SONAME): lib/monocypher-rng.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,$(SONAME) -o $@ $^

lib/monocypher-rng.o: src/monocypher-rng.c src/monocypher-rng.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I src -fPIC -c -o $@ $<
