# Copyright (C) 2003 CSIRO Australia
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  
#  - Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#  
#  - Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.
#  
#  - Neither the name of the CSIRO nor the names of its
#  contributors may be used to endorse or promote products derived from
#  this software without specific prior written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#  PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Process this file with automake to produce Makefile.in

EXTRA_DIST = cmml.pc.in README.win32 \
	Win32/Makefile \
	Win32/libcmml.def \
	Win32/ReadMe.txt \
	Win32/config.h \
	Win32/config.h.in \
	Win32/attgetopt.c \
	Win32/getopt.h \
	Win32/libcmml.dsw \
	Win32/libcmml.dsp \
	Win32/cmml_validate.dsp \
	Win32/cmml_fix.dsp \
	Win32/libcmml.sln \
	Win32/libcmml.vcproj \
	Win32/cmml-validate.vcproj \
	Win32/cmml-fix.vcproj \
	Win32/cmmlbin_installer.vdproj

SUBDIRS = src tools examples tests doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cmml.pc

test:
	make check