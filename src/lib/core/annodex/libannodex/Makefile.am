## Process this file with automake to produce Makefile.in

if ANX_CONFIG_WRITE
SUBDIRS = doc libltdl include src
else
SUBDIRS = doc include src
endif

EXTRA_DIST = annodex.pc.in README.win32 \
	win32/Makefile \
	win32/config.h \
	win32/dirent.c \
	win32/dirent.h \
	win32/libannodex.def \
	win32/importers.def \
	win32/libannodex.sln \
	win32/libannodex/libannodex.def \
	win32/libannodex/libannodex.vcproj \
	win32/cmmlimporter/cmml_importer.def \
	win32/cmmlimporter/cmmlimporter.vcproj \
	win32/ogg_importer/ogg_importer.def \
	win32/ogg_importer/ogg_importer.vcproj \
	win32/anxenc/anxenc.vcproj

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = annodex.pc
