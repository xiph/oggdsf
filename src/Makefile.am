AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -w

bin_PROGRAMS = OOOggDump

lib_LTLIBRARIES = libOOOgg.la

INCLUDES = -I@top_srcdir@/lib/helper/libilliCore -I@top_srcdir@/lib/core/ogg/libOOOgg

libOOOgg_la_SOURCES = $(libilliCore_SOURCES) $(libOOOggCore_SOURCES)

libilliCore_SOURCES = \
	lib/helper/libilliCore/StringHelper.cpp \
	lib/helper/libilliCore/iBE_Math.cpp \
	lib/helper/libilliCore/iLE_Math.cpp

libOOOggCore_SOURCES = \
	lib/core/ogg/libOOOgg/CircularBuffer.cpp \
	lib/core/ogg/libOOOgg/OggDataBuffer.cpp \
	lib/core/ogg/libOOOgg/OggMuxStream.cpp \
	lib/core/ogg/libOOOgg/OggPacket.cpp \
	lib/core/ogg/libOOOgg/OggPacketiser.cpp \
	lib/core/ogg/libOOOgg/OggPage.cpp \
	lib/core/ogg/libOOOgg/OggPageHeader.cpp \
	lib/core/ogg/libOOOgg/OggPageInterleaver.cpp \
	lib/core/ogg/libOOOgg/OggPaginator.cpp \
	lib/core/ogg/libOOOgg/OggPaginatorSettings.cpp \
	lib/core/ogg/libOOOgg/StampedOggPacket.cpp

OOOggDump_LDADD = libOOOgg.la
OOOggDump_SOURCES = \
	tools/OOOggDump/OOOggDump.cpp

