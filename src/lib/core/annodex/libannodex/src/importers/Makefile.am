## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -pedantic

INCLUDES = -I$(top_srcdir)/include $(OGGZ_CFLAGS)

libdir = $(IMPORTERS_DIR)

ANNODEXDIR = ../libannodex
ANNODEX_LIBS = $(ANNODEXDIR)/libannodex.la $(OGGZ_LIBS) -lm

if HAVE_LIBCMML
import_cmml = libanx_import_cmml.la
endif

if HAVE_OGGZ
import_ogg = libanx_import_ogg.la
endif

if HAVE_LIBSNDFILE1
import_sndfile = libanx_import_sndfile.la
endif

lib_LTLIBRARIES = \
	libanx_import_anx.la \
	$(import_cmml) \
	$(import_ogg) \
	$(import_sndfile)

libanx_import_anx_la_SOURCES = anx_import_anx.c
libanx_import_anx_la_LDFLAGS = -module -version-info $(SHARED_VERSION_INFO)
libanx_import_anx_la_LIBADD = $(ANNODEX_LIBS)

libanx_import_cmml_la_SOURCES = anx_import_cmml.c
libanx_import_cmml_la_CFLAGS = $(CMML_CFLAGS)
libanx_import_cmml_la_LDFLAGS = -module -version-info $(SHARED_VERSION_INFO)
libanx_import_cmml_la_LIBADD = $(ANNODEX_LIBS) $(CMML_LIBS)

libanx_import_ogg_la_SOURCES = anx_import_ogg.c
libanx_import_ogg_la_CFLAGS = $(OGG_CFLAGS)
libanx_import_ogg_la_LDFLAGS = -module -version-info $(SHARED_VERSION_INFO)
libanx_import_ogg_la_LIBADD = $(ANNODEX_LIBS)

libanx_import_sndfile_la_SOURCES = anx_import_sndfile.c
libanx_import_sndfile_la_CFLAGS = $(SNDFILE_CFLAGS)
libanx_import_sndfile_la_LDFLAGS = -module -version-info $(SHARED_VERSION_INFO)
libanx_import_sndfile_la_LIBADD = $(ANNODEX_LIBS) @SNDFILE_LIBS@

