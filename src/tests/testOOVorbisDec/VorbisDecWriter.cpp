#include "StdAfx.h"
#include "VorbisDecWriter.h"

VorbisDecWriter::VorbisDecWriter(string inFilename)
{
	mPacketiser.setPacketSink(this);
	mOutputBuffer = new short[OUTPUT_BUFFER_SIZE];
	mOutputFile.open(inFilename.c_str(), ios_base::out | ios_base::binary);
}

VorbisDecWriter::~VorbisDecWriter(void)
{
	mOutputFile.close();
	delete[] mOutputBuffer;
}

bool VorbisDecWriter::acceptStampedOggPacket(StampedOggPacket* inPacket)
{
	unsigned long locSampleCount = 0;
	VorbisDecoder::eVorbisResult locRes = mVorbisDecoder.decodePacket(		inPacket->packetData()
																	,	inPacket->packetSize()
																	,	mOutputBuffer
																	,	OUTPUT_BUFFER_SIZE
																	,	&locSampleCount);

	if (locRes == VorbisDecoder::VORBIS_DATA_OK) {
		mOutputFile.write((char*)mOutputBuffer, locSampleCount * mVorbisDecoder.numChannels() * sizeof(short));
		return true;
	} else if (locRes < VorbisDecoder::VORBIS_ERROR_MIN) {
		return true;
	} else {
		return false;
	}
}
bool VorbisDecWriter::acceptOggPage(OggPage* inOggPage)
{
	return mPacketiser.acceptOggPage(inOggPage);
}