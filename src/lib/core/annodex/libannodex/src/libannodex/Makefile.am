## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -pedantic

if ANX_CONFIG_READ
anx_read_sources = xtag.c xtag.h
endif

if ANX_CONFIG_WRITE
anx_write_sources = fix_dl.c fix_dl.h
anx_write_includes = $(INCLTDL)
anx_write_libs = $(LIBLTDL)
endif

INCLUDES = $(anx_write_includes) -I$(top_srcdir)/include @OGGZ_CFLAGS@

EXTRA_DIST = Version_script.in

# Libraries to build
lib_LTLIBRARIES = libannodex.la

libannodex_la_SOURCES = \
	$(anx_read_sources) $(anx_write_sources) \
	anx_compat.h \
	anx_list.c \
	anx_params.c \
	anx_tree.c anx_tree.h \
	anx_private.h \
	anx_mapping.h \
	anx_bos.h \
	anx_byteswap.h \
	annodex.c \
	anx_track.c \
	anx_snprint.c anx_snprint.h \
	anx_command.c anx_command.h \
	anx_element.c \
	anx_import.c anx_import_private.h \
	anx_time.c \
	anx_read.c \
	anx_write.c \
	content_type.c content_type.h

libannodex_la_LDFLAGS = -version-info @SHARED_VERSION_INFO@ @SHLIB_VERSION_ARG@
libannodex_la_LIBADD = $(anx_write_libs) $(OGGZ_LIBS)
libannodex_la_DEPENDENCIES = $(anx_write_libs)

