## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -pedantic

INCLUDES = -I$(top_srcdir)/include

ANNODEXDIR = ../libannodex
ANNODEX_LIBS = $(ANNODEXDIR)/libannodex.la $(OGGZ_LIBS) -lm

# Test programs

test: check

TESTS = paramtest

noinst_PROGRAMS = $(TESTS)
noinst_HEADERS = anx_tests.h

paramtest_SOURCES = paramtest.c
paramtest_LDADD = $(ANNODEX_LIBS)

#xTESTS = anx_list_test anx_tree_test anx_types_test anx_write_test anx_read_test

#noinst_PROGRAMS = $(xxTESTS) xtag_test

#xtag_test_SOURCES = xtag_test.c
#xtag_test_LDADD = $(ANNODEX_LIBS)
#
#anx_list_test_SOURCES = anx_list_test.c
#anx_list_test_LDADD = $(ANNODEX_LIBS)
#
##anx_tree_test_SOURCES = anx_tree_test.c
##anx_tree_test_LDADD = $(ANNODEX_LIBS)
#
#anx_types_test_SOURCES = anx_types_test.c
#anx_types_test_LDADD = $(ANNODEX_LIBS)
#
#anx_write_test_SOURCES = anx_write_test.c anx_media_test.c
#anx_write_test_LDADD = $(ANNODEX_LIBS)
#
#anx_read_test_SOURCES = anx_read_test.c
##anx_read_test_LDADD = $(ANNODEX_LIBS)


# Dodgey

#$(LIBLTDL):
#	cd $(top_builddir) && make -C libltdl

#$(ANNODEX_LIBS):
#	cd $(top_builddir) && make -C src

