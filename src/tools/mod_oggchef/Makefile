#
# UNIX Makefile for mod_oggchef
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

# force GNU libtool to link with C++ compiler (see Apache 2's build/rules.mk
# file for where I got these lines from)
LINK     = $(LIBTOOL) --mode=link $(CXX_COMPILE) $(LT_LDFLAGS) $(ALL_LDFLAGS) -o $@
SH_LINK  = $(SH_LIBTOOL) --mode=link $(CXX_COMPILE) $(LT_LDFLAGS) $(ALL_LDFLAGS) $(SH_LDFLAGS) $(CORE_IMPLIB) $(SH_LIBS) -o $@
MOD_LINK = $(LIBTOOL) --mode=link $(CXX_COMPILE) -module $(LT_LDFLAGS) $(ALL_LDFLAGS) -o $@



#
# Tweakage stops here
#


#   additional defines, includes and libraries
#DEFS=-Dmy_define=my_value
#INCLUDES=-Imy/include/dir
LIBS=-lOOOgg -lOOOggSeek
CXXFLAGS=-w
CFLAGS=-w

#   the default target
all: local-shared-build

#   install the shared object file into Apache 
install: install-modules

#   cleanup
clean:
	-rm -f mod_oggchef.o mod_oggchef.lo mod_oggchef.slo mod_oggchef.la 

#   simple test
test: reload
	lynx -mime_header http://localhost/oggchef

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

