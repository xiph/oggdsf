AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -w

NULL =

bin_PROGRAMS = \
	OOOggDump \
	OOOggValidate \
	OOOggStat \
	OOOggChainSplitter \
	OOOggCommentDump \
	OOOggSeekFileMaker \
	OOOggSeekFileReader \
	AnxCutter \
	$(NULL)

lib_LTLIBRARIES = \
	libilliCore.la \
	libOOOgg.la \
	libOOOggSeek.la \
	libVorbisComment.la \
	$(NULL)

INCLUDES = \
	   -I@top_srcdir@/../src/lib/helper/libilliCore \
	   -I@top_srcdir@/../src/lib/core/ogg/libOOOgg \
	   -I@top_srcdir@/../src/lib/core/ogg/libOOOggSeek \
	   -I@top_srcdir@/../src/lib/core/ogg/libVorbisComment \
	   $(NULL)


#
# libraries
#

libOOOggSeek_la_LIBADD = libOOOgg.la
libOOOggSeek_la_SOURCES = \
	../src/lib/core/ogg/libOOOggSeek/AutoAnxSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/AutoOggSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggBinarySeeker.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggSeekPoint.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/OGGSRecogniser.cpp \
	$(NULL)

libOOOggSeekdir = $(includedir)/libOOOggSeek
libOOOggSeek_HEADERS = \
	../src/lib/core/ogg/libOOOggSeek/AutoAnxSeekTable.h \
	../src/lib/core/ogg/libOOOggSeek/AutoOggSeekTable.h \
	../src/lib/core/ogg/libOOOggSeek/IOggSeeker.h \
	../src/lib/core/ogg/libOOOggSeek/libOOOggSeek.h \
	../src/lib/core/ogg/libOOOggSeek/OggBinarySeeker.h \
	../src/lib/core/ogg/libOOOggSeek/OggSeekPoint.h \
	../src/lib/core/ogg/libOOOggSeek/OggSeekTable.h \
	../src/lib/core/ogg/libOOOggSeek/OGGSRecogniser.h \
	../src/lib/core/ogg/libOOOggSeek/stdafx.h \
	$(NULL)

libVorbisComment_la_LIBADD = libOOOgg.la
libVorbisComment_la_SOURCES = \
	../src/lib/core/ogg/libVorbisComment/FileComments.cpp \
	../src/lib/core/ogg/libVorbisComment/SingleVorbisComment.cpp \
	../src/lib/core/ogg/libVorbisComment/StreamCommentInfo.cpp \
	../src/lib/core/ogg/libVorbisComment/VorbisComments.cpp \
	$(NULL)
	
libilliCore_la_SOURCES = \
	../src/lib/helper/libilliCore/StringHelper.cpp \
	../src/lib/helper/libilliCore/iBE_Math.cpp \
	../src/lib/helper/libilliCore/iLE_Math.cpp \
	$(NULL)
libilliCoredir = $(includedir)/libilliCore
libilliCore_HEADERS = \
	../src/lib/helper/libilliCore/config.h \
	../src/lib/helper/libilliCore/iBE_Math.h \
	../src/lib/helper/libilliCore/iLE_Math.h \
	../src/lib/helper/libilliCore/illicoreconfig.h \
	../src/lib/helper/libilliCore/libilliCore.h \
	../src/lib/helper/libilliCore/StringHelper.h \
	$(NULL)

libOOOgg_la_LIBADD = libilliCore.la
libOOOgg_la_SOURCES = \
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
	../src/lib/core/ogg/libOOOgg/StampedOggPacket.cpp \
	$(NULL)
libOOOggdir = $(includedir)/libOOOgg
libOOOgg_HEADERS = \
	../src/lib/core/ogg/libOOOgg/CircularBuffer.h \
	../src/lib/core/ogg/libOOOgg/dllstuff.h \
	../src/lib/core/ogg/libOOOgg/IDataSource.h \
	../src/lib/core/ogg/libOOOgg/IFIFOBuffer.h \
	../src/lib/core/ogg/libOOOgg/INotifyArrival.h \
	../src/lib/core/ogg/libOOOgg/INotifyComplete.h \
	../src/lib/core/ogg/libOOOgg/IOggCallback.h \
	../src/lib/core/ogg/libOOOgg/IOggPackSource.h \
	../src/lib/core/ogg/libOOOgg/IStampedOggPacketSink.h \
	../src/lib/core/ogg/libOOOgg/libOOOgg.h \
	../src/lib/core/ogg/libOOOgg/OggDataBuffer.h \
	../src/lib/core/ogg/libOOOgg/OggMuxStream.h \
	../src/lib/core/ogg/libOOOgg/OggPacket.h \
	../src/lib/core/ogg/libOOOgg/OggPacketiser.h \
	../src/lib/core/ogg/libOOOgg/OggPage.h \
	../src/lib/core/ogg/libOOOgg/OggPageHeader.h \
	../src/lib/core/ogg/libOOOgg/OggPageInterleaver.h \
	../src/lib/core/ogg/libOOOgg/OggPaginator.h \
	../src/lib/core/ogg/libOOOgg/OggPaginatorSettings.h \
	../src/lib/core/ogg/libOOOgg/OggTypeDefs.h \
	../src/lib/core/ogg/libOOOgg/StampedOggPacket.h \
	$(NULL)


#
# tools
#

OOOggDump_LDADD = libOOOgg.la
OOOggDump_SOURCES = \
	../src/tools/OOOggDump/OOOggDump.cpp \
	$(NULL)

OOOggValidate_LDADD = libOOOgg.la
OOOggValidate_SOURCES = \
	../src/tools/OOOggValidate/OOOggValidate.cpp \
	../src/tools/OOOggValidate/OggStreamValidationState.cpp \
	../src/tools/OOOggValidate/OggValidationState.cpp \
	$(NULL)

OOOggStat_LDADD = libOOOgg.la
OOOggStat_SOURCES = \
	../src/tools/OOOggStat/OOOggStat.cpp \
	$(NULL)

OOOggChainSplitter_LDADD = libOOOgg.la libVorbisComment.la
OOOggChainSplitter_SOURCES = \
	../src/tools/oggChainSplitter/oggChainSplitter.cpp \
	$(NULL)

OOOggCommentDump_LDADD = libOOOgg.la libVorbisComment.la
OOOggCommentDump_SOURCES = \
	../src/tools/OOOggCommentDump/OOOggCommentDump.cpp \
	$(NULL)

OOOggSeekFileMaker_LDADD = libOOOgg.la libOOOggSeek.la
OOOggSeekFileMaker_SOURCES = \
	../src/tools/OOOggSeekFileMaker/OOOggSeekFileMaker.cpp \
	$(NULL)

OOOggSeekFileReader_LDADD = libOOOgg.la libOOOggSeek.la
OOOggSeekFileReader_SOURCES = \
	../src/tools/OOOggSeekFileReader/OOOggSeekFileReader.cpp \
	$(NULL)

AnxCutter_LDADD = libOOOgg.la libOOOggSeek.la
AnxCutter_SOURCES = \
	../src/tools/AnxCutter/AnxCutter.cpp \
	$(NULL)


