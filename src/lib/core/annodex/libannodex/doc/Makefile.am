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

docdir = $(datadir)/doc/$(PACKAGE)

EXTRA_DIST = Doxyfile.in \
	anxenc.1.sgml anxrip.1.sgml anxinfo.1.sgml \
	anxenc.1 anxrip.1 anxinfo.1 \
	users_guide.xml \
	anx_read_dox.h

man_MANS = anxenc.1 anxrip.1 anxinfo.1

#doc_DATA = users_guide.pdf doxygen-build.stamp
doc_DATA = doxygen-build.stamp

doc: pdf ps

pdf: users_guide.pdf
ps: users_guide.ps
html: users_guide.html

%.pdf: %.xml
	docbook2pdf $<

%.ps: %.xml
	docbook2ps $<

#%.1: %.1.xml
#	xsltproc /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/manpages/docbook.xsl $<

if HAVE_DOCBOOKTOMAN
%.1: %.1.sgml
	docbook-to-man $< > $@
else
%.1: %.1.sgml
endif

if HAVE_DOCBOOK2HTML
%.html: %.sgml
	mkdir $*
	docbook2html -o $* $<
endif

if HAVE_DOXYGEN
doxygen-build.stamp: Doxyfile $(top_srcdir)/include/annodex/*.h anx_read_dox.h
	doxygen
	touch doxygen-build.stamp
else
doxygen-build.stamp:
	echo "*** Warning: Doxygen not found; documentation will not be built."
	touch doxygen-build.stamp
endif

dist_docdir = $(distdir)/libannodex

dist-hook:
	if test -d libannodex; then \
	  mkdir $(dist_docdir); \
	  for dir in libannodex/*; do \
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
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	if test -d libannodex; then \
	  for dir in libannodex/*; do \
	    if test -d $$dir; then \
	      b=`basename $$dir`; \
	      $(mkinstalldirs) $(DESTDIR)$(docdir)/$$b; \
	      for f in $$dir/*; do \
		$(INSTALL_DATA) $$f $(DESTDIR)$(docdir)/$$b; \
	      done \
	    fi \
	  done \
	fi

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)

clean-local:
	if test -d libannodex; then rm -rf libannodex; fi
	if test -f doxygen-build.stamp; then rm -f doxygen-build.stamp; fi
