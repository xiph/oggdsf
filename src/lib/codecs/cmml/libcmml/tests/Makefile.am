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

#TOOLSDIR = ${top_srcdir}/tools
TESTSDIR = ${top_srcdir}/tests

SPC_CMML = special1.cmml special2.cmml write.cmml

SRC_CMML = simple.cmml i18n.cmml error1.cmml error2.cmml error3.cmml \
           error4.cmml error5.cmml  error6.cmml error7.cmml error8.cmml \
	   error9.cmml error10.cmml error11.cmml error12.cmml error13.cmml \
	   error14.cmml error15.cmml

RST_CMML = simple.cmml.correct i18n.cmml.correct \
           special1.cmml.scientist special2.cmml.title special2.cmml.offset \
	   error1.cmml.correct error2.cmml.correct \
	   error3.cmml.correct error4.cmml.correct \
	   error5.cmml.correct error6.cmml.correct \
	   error7.cmml.correct error8.cmml.correct \
	   error9.cmml.correct error10.cmml.correct \
	   error11.cmml.correct error12.cmml.correct \
	   error13.cmml.correct error14.cmml.correct \
	   error15.cmml.correct

EXTRA_DIST = ${SPC_CMML} ${SRC_CMML} ${RST_CMML} libcmml_new.sh


check: specific_tests ${SRC_CMML}
# general tests
	@echo "---------------------------------------------------------------"
	@echo "    Generic tests of correct files and error conditions"
	@echo "---------------------------------------------------------------"; \
	for i in $(SRC_CMML) ; do \
	if test ${TESTSDIR}/$$i ; then \
	../tools/cmml-validate -b ${TESTSDIR}/$$i 2>&1 | diff -b - ${TESTSDIR}/$${i}.correct; \
	if test $$? -ne 0 ; then \
		echo "---------------------------------------------------------------"; \
		echo "  libcmml: error in $$i"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  $${i} ... passed"; \
	fi; \
	fi; \
	done; \
	echo "---------------------------------------------------------------"; \
	echo "  libcmml passed tests."; \
	echo "---------------------------------------------------------------"

specific_tests:
	@echo "---------------------------------------------------------------"
	@echo "    Specific tests with examples programs"
	@echo "---------------------------------------------------------------"
# parsing and extracting title tag; also checks for parsing of stdin
	@../examples/cmml-parse ${TESTSDIR}/special2.cmml | diff -b - ${TESTSDIR}/special2.cmml.title; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-parse special2.cmml"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-parse ... passed"; \
	fi

# writing of CML file
	@../examples/cmml-write | diff -b - ${TESTSDIR}/write.cmml; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-write"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-write ... passed"; \
	fi

# seeking to the scientist clip offset
	@../examples/cmml-seek-clip ${TESTSDIR}/special1.cmml scientist | diff -b - ${TESTSDIR}/special1.cmml.scientist; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-seek-clip special1.cmml scientist"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-seek-clip ... passed"; \
	fi

# seeking to time offset in seconds
	@../examples/cmml-seek-offset ${TESTSDIR}/special2.cmml 160 | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-seek-offset special2.cmml 160"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-seek-offset ... passed"; \
	fi

# seeking to utc offset
	@../examples/cmml-seek-utc ${TESTSDIR}/special2.cmml 20021222T221105Z | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-seek-utc special2.cmml 20021222T221105Z"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-seek-utc ... passed"; \
	fi

# uri file opening id
	@../examples/cmml-uri-file file://${TESTSDIR}/special2.cmml\#Question_Storm_Water | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-uri-file special2.cmml\#Question_Storm_Water"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-uri-file clip ... passed"; \
	fi

# uri file opening npt
	@../examples/cmml-uri-file file://${TESTSDIR}/special2.cmml\#npt:160 | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-uri-file special2.cmml\#npt:160"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-uri-file npt ... passed"; \
	fi

# uri file opening smpte
	@../examples/cmml-uri-file file://${TESTSDIR}/special2.cmml\#smpte-25:00:02:40:00 | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-uri-file special2.cmml\#smpte-25:00:02:40:00"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-uri-file smpte ... passed"; \
	fi

# uri file opening utc
	@../examples/cmml-uri-file file://${TESTSDIR}/special2.cmml\#clock:20021222T221105Z | diff -b - ${TESTSDIR}/special2.cmml.offset; \
	if test $$? -ne 0 ; then \
		echo "  libcmml: error on cmml-uri-file special2.cmml\#clock:20021222T221105Z"; \
		echo "---------------------------------------------------------------"; \
		exit 1; \
	else \
		echo "  cmml-uri-file clock ... passed"; \
	fi

new: ./libcmml_new.sh
	./libcmml_new.sh
