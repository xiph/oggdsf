AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -w

bin_PROGRAMS = OOOggDump OOOggValidate OOOggStat OOOggChainSplitter OOOggCommentDump OOOggSeekFileMaker OOOggSeekFileReader

lib_LTLIBRARIES = libilliCore.la libOOOgg.la libVorbisComment.la libOOOggSeek.la

INCLUDES = \
	   -I@top_srcdir@/../src/lib/helper/libilliCore \
	   -I@top_srcdir@/../src/lib/core/ogg/libOOOgg \
	   -I@top_srcdir@/../src/lib/core/ogg/libOOOggSeek \
	   -I@top_srcdir@/../src/lib/core/ogg/libVorbisComment

libOOOggSeek_la_LIBADD = libOOOgg.la
libOOOggSeek_la_SOURCES = \
	../src/lib/core/ogg/libOOOggSeek/AutoAnxSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/AutoOggSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggBinarySeeker.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggSeekPoint.cpp \
	../src/lib/core/ogg/libOOOggSeek/OggSeekTable.cpp \
	../src/lib/core/ogg/libOOOggSeek/OGGSRecogniser.cpp

libVorbisComment_la_LIBADD = libOOOgg.la
libVorbisComment_la_SOURCES = \
	../src/lib/core/ogg/libVorbisComment/FileComments.cpp \
	../src/lib/core/ogg/libVorbisComment/SingleVorbisComment.cpp \
	../src/lib/core/ogg/libVorbisComment/StreamCommentInfo.cpp \
	../src/lib/core/ogg/libVorbisComment/VorbisComments.cpp
	
libilliCore_la_SOURCES = \
	../src/lib/helper/libilliCore/StringHelper.cpp \
	../src/lib/helper/libilliCore/iBE_Math.cpp \
	../src/lib/helper/libilliCore/iLE_Math.cpp

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
	../src/lib/core/ogg/libOOOgg/StampedOggPacket.cpp

OOOggDump_LDADD = libOOOgg.la
OOOggDump_SOURCES = \
	../src/tools/OOOggDump/OOOggDump.cpp

OOOggValidate_LDADD = libOOOgg.la
OOOggValidate_SOURCES = \
	../src/tools/OOOggValidate/OOOggValidate.cpp \
	../src/tools/OOOggValidate/OggStreamValidationState.cpp \
	../src/tools/OOOggValidate/OggValidationState.cpp

OOOggStat_LDADD = libOOOgg.la
OOOggStat_SOURCES = \
	../src/tools/OOOggStat/OOOggStat.cpp

OOOggChainSplitter_LDADD = libOOOgg.la libVorbisComment.la
OOOggChainSplitter_SOURCES = \
	../src/tools/oggChainSplitter/oggChainSplitter.cpp

OOOggCommentDump_LDADD = libOOOgg.la libVorbisComment.la
OOOggCommentDump_SOURCES = \
	../src/tools/OOOggCommentDump/OOOggCommentDump.cpp

OOOggSeekFileMaker_LDADD = libOOOgg.la libOOOggSeek.la
OOOggSeekFileMaker_SOURCES = \
	../src/tools/OOOggSeekFileMaker/OOOggSeekFileMaker.cpp

OOOggSeekFileReader_LDADD = libOOOgg.la libOOOggSeek.la
OOOggSeekFileReader_SOURCES = \
	../src/tools/OOOggSeekFileReader/OOOggSeekFileReader.cpp


