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
	libCMMLTags.la \
	libCMMLParse.la \
	libOOOgg.la \
	libOOOggChef.la \
	libOOOggSeek.la \
	libVorbisComment.la \
	$(NULL)
#libOOOggChef.la

INCLUDES = \
	   -I@top_srcdir@/../src/lib/helper \
	   -I@top_srcdir@/../src/lib/codecs/cmml \
	   -I@top_srcdir@/../src/lib/core/ogg \
	   -I@top_srcdir@/../src/lib/core/ogg/libVorbisComment \
	   $(NULL)


#
# libraries
#

libCMMLTags_la_SOURCES = \
	../src/lib/codecs/cmml/libCMMLTags/C_AnchorTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_BaseTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLDoc.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLError.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLPreamble.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLRootTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLTime.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ClipTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ClipTagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_DescTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_HeadTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_HumReadCMMLTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ImageTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ImportTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ImportTagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_Int64.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_MappedTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_MappedTagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_MetaTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_MetaTagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ParamTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_ParamTagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_StreamTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_TagList.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_TextFieldTag.cpp \
	../src/lib/codecs/cmml/libCMMLTags/C_TitleTag.cpp \
	$(NULL)

libCMMLTagsdir = $(includedir)/libCMMLTags
libCMMLTags_HEADERS = \
	../src/lib/codecs/cmml/libCMMLTags/C_AnchorTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_BaseTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLDoc.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLError.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLPreamble.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLRootTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_CMMLTime.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ClipTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ClipTagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_DescTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_HeadTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_HumReadCMMLTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ImageTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_Import.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ImportTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ImportTagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_Int64.h \
	../src/lib/codecs/cmml/libCMMLTags/C_MappedTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_MappedTagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_MetaTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_MetaTagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ParamTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_ParamTagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_StreamTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_TagList.h \
	../src/lib/codecs/cmml/libCMMLTags/C_TextFieldTag.h \
	../src/lib/codecs/cmml/libCMMLTags/C_TitleTag.h \
	../src/lib/codecs/cmml/libCMMLTags/config.h \
	../src/lib/codecs/cmml/libCMMLTags/libCMMLTags.h \
	$(NULL)

libCMMLParse_la_SOURCES = \
	../src/lib/codecs/cmml/libCMMLParse/CMMLParser.cpp \
	../src/lib/codecs/cmml/libCMMLParse/ConvertUTF.c \
	../src/lib/codecs/cmml/libCMMLParse/xlist.c \
	../src/lib/codecs/cmml/libCMMLParse/xtag.c \
	$(NULL)

libCMMLParsedir = $(includedir)/libCMMLParse
libCMMLParse_HEADERS = \
	../src/lib/codecs/cmml/libCMMLParse/CMMLParser.h \
	../src/lib/codecs/cmml/libCMMLParse/ConvertUTF.h \
	../src/lib/codecs/cmml/libCMMLParse/libCMMLParse.h \
	../src/lib/codecs/cmml/libCMMLParse/stdafx.h \
	../src/lib/codecs/cmml/libCMMLParse/xlist.h \
	../src/lib/codecs/cmml/libCMMLParse/xtag.h \
	$(NULL)

libOOOggChef_la_LIBADD = libOOOgg.la libOOOggSeek.la
libOOOggChef_la_SOURCES = \
	../src/lib/helper/libOOOggChef/AnnodexRecomposer.cpp \
	../src/lib/helper/libOOOggChef/IRecomposer.cpp \
	$(NULL)

libOOOggChefdir = $(includedir)/libOOOggChef
libOOOggChef_HEADERS = \
	../src/lib/helper/libOOOggChef/AnnodexRecomposer.h \
	../src/lib/helper/libOOOggChef/IRecomposer.h \
	../src/lib/helper/libOOOggChef/libOOOggChef.h \
	../src/lib/helper/libOOOggChef/stdafx.h \
	$(NULL)

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
	../src/lib/helper/libilliCore/stdafx.h \
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
	../src/lib/core/ogg/libOOOgg/stdafx.h \
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


