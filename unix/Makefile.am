AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -w

bin_PROGRAMS = OOOggDump

lib_LTLIBRARIES = libOOOgg.la

INCLUDES = -I@top_srcdir@/../src/lib/helper/libilliCore -I@top_srcdir@/../src/lib/core/ogg/libOOOgg

libOOOgg_la_SOURCES = $(libilliCore_SOURCES) $(libOOOggCore_SOURCES)

libilliCore_SOURCES = \
	../src/lib/helper/libilliCore/StringHelper.cpp \
	../src/lib/helper/libilliCore/iBE_Math.cpp \
	../src/lib/helper/libilliCore/iLE_Math.cpp

libOOOggCore_SOURCES = \
	../src/lib/core/ogg/libOOOgg/CircularBuffer.cpp \
	../src/lib/core/ogg/libOOOgg/OggDataBuffer.cpp \
	../src/lib/core/ogg/libOOOgg/OggMuxStream.cpp \
	../src/lib/core/ogg/libOOOgg/OggPacket.cpp \
	../src/lib/core/ogg/libOOOgg/OggPacketiser.cpp \
	../src/lib/core/ogg/libOOOgg/OggPage.cpp \
	../src/lib/core/ogg/libOOOgg/OggPageHeader.cpp \
	../src/lib/core/ogg/libOOOgg/OggPageInterleaver.cpp \
	../src/lib/core/ogg/libOOOgg/OggPaginator.cpp \
	../src/lib/core/ogg/libOOOgg/OggPaginatorSettings.cpp \
	../src/lib/core/ogg/libOOOgg/StampedOggPacket.cpp

OOOggDump_LDADD = libOOOgg.la
OOOggDump_SOURCES = \
	../src/tools/OOOggDump/OOOggDump.cpp

