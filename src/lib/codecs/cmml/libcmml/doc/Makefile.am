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

AUTOMAKE_OPTIONS = foreign

CLEANFILES = doxygen-build.stamp

EXTRA_DIST = Doxyfile.in \
             cmml.dtd cmml_dox.h libcmml \
             cmml-validate.1.sgml.in cmml-validate.1 \
             cmml-fix.1.sgml.in cmml-fix.1 \
             cmml-fortune.6.sgml.in cmml-fortune.6

DOXY_INPUT = ${srcdir}/../src/cmml.h \
             ${srcdir}/../tools/cmml-validate.c \
             ${srcdir}/../tools/cmml-fix.c \
             ${srcdir}/../tools/cmml-fortune.c \
             ${srcdir}/cmml_dox.h


SUBDIRS = .

man_MANS = cmml-validate.1 cmml-fix.1 cmml-fortune.6

docdir = $(datadir)/doc/$(PACKAGE)
doc_DATA = doxygen-build.stamp
dist_docdir = $(DESTDIR)$(docdir)

# avoid having to use xsltproc:
#%.1: %.1.xml
#	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/manpages/docbook.xsl $<

# another possibility is the use of docbook2man:
#%.1: %.1.sgml
#	docbook2man $< > $@
# but it creates weird stuff in line 3

if HAVE_DOCBOOKTOMAN
%.1: %.1.sgml
	docbook-to-man $< > $@
%.6: %.6.sgml
	docbook-to-man $< > $@
else
%.1: %.1.sgml
%.6: %.6.sgml
endif

all: doxygen-build.stamp

if HAVE_DOXYGEN
doxygen-build.stamp: Doxyfile ${DOXY_INPUT}
	@echo "*** Running doxygen ***"
	doxygen
	touch doxygen-build.stamp
else
doxygen-build.stamp:
	echo "*** Warning: Doxygen not found; documentation will not be built."
	touch doxygen-build.stamp
endif

# The install targets don't copy whole directories so we have to
# handle 'html/' specially:
dist-hook:
        if test -d libcmml; then \
          mkdir $(dist_docdir); \
          for dir in libcmml/*; do \
            b=`basename $$dir`; \
            if test $$b != "CVS"; then \
              if test -d $$dir; then \
                mkdir $(dist_docdir)/$$b; \
                for f in $$dir/*; do \
                  cp -p $$f $(dist_docdir)/$$b; \
                done \
              fi \
            fi \
          done \
        fi

install-data-local: doxygen-build.stamp
	$(mkinstalldirs) $(docdir)
	if test -d libcmml; then \
	  for dir in libcmml/*; do \
	    if test -d $$dir; then \
	      b=`basename $$dir`; \
	      $(mkinstalldirs) $(docdir)/$$b; \
	      for f in $$dir/*; do \
	        $(INSTALL_DATA) $$f $(docdir)/$$b; \
	      done \
	    fi \
	  done \
	fi

uninstall-local:
	if test -d $(dist_docdir); then
	rm -rf $(dist_docdir);
	fi

maintainer-clean-local:
	if test -d libcmml; then rm -rf libcmml; fi
