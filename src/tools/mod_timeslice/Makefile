#
# UNIX Makefile for mod_timeslice
#
# You may need to tweak the following variables in order to get things building
# properly
#

builddir=.

# point these to your apache 2 share directory
top_srcdir=/usr/share/apache2
top_builddir=/usr/share/apache2
include /usr/share/apache2/build/special.mk

# default to apxs2, but fallback to apxs if apxs2 can't be found
APXS=$(shell [ -x `which apxs2` ] && echo apxs2 || \
       echo apxs )

# default to apache2ctl, but fallback to apachectl
APACHECTL=$(shell [ -x `which apache2ctl` ] && echo apache2ctl || \
	    echo apachectl )

# default to GNU C++ compiler
CXX=g++


#
# Tweakage stops here
#


#   additional defines, includes and libraries
#DEFS=-Dmy_define=my_value
#INCLUDES=-Imy/include/dir
LIBS=-lOOOgg -lOOOggSeek
CXXFLAGS=-w

#   the default target
all: local-shared-build

#   install the shared object file into Apache 
install: install-modules

#   cleanup
clean:
	-rm -f mod_timeslice.o mod_timeslice.lo mod_timeslice.slo mod_timeslice.la 

#   simple test
test: reload
	lynx -mime_header http://localhost/timeslice

#   install and activate shared object by reloading Apache to
#   force a reload of the shared object file
reload: install restart

#   the general Apache start/restart/stop
#   procedures
start:
	$(APACHECTL) start
restart:
	$(APACHECTL) restart
stop:
	$(APACHECTL) stop

