## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -pedantic -g

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src/libannodex 

ANNODEXDIR = ../libannodex
ANNODEX_LIBS = $(ANNODEXDIR)/libannodex.la $(OGGZ_LIBS) -lm

if ANX_CONFIG_WRITE
anx_write_programs = anxenc anxed
endif

if ANX_CONFIG_READ
anx_read_programs = anxinfo anxrip anxcheck
endif

# Programs to build
bin_PROGRAMS = $(anx_read_programs) $(anx_write_programs)

anxinfo_SOURCES = anxinfo.c common.c common.h
anxinfo_LDADD = $(ANNODEX_LIBS)

anxrip_SOURCES = anxrip.c common.c common.h
anxrip_LDADD = $(ANNODEX_LIBS)

anxcheck_SOURCES = anxcheck.c common.c common.h
anxcheck_LDADD = $(ANNODEX_LIBS)

anxed_SOURCES = anxed.c common.c common.h
anxed_LDADD = $(ANNODEX_LIBS)

anxenc_SOURCES = anxenc.c common.c common.h
anxenc_LDADD = $(ANNODEX_LIBS)
