#include "StdAfx.h"
#include "TremorDecWriter.h"

TremorDecWriter::TremorDecWriter(string inFilename)
{
	mPacketiser.setPacketSink(this);
	mOutputBuffer = new short[OUTPUT_BUFFER_SIZE];
	mOutputFile.open(inFilename.c_str(), ios_base::out | ios_base::binary);
}

TremorDecWriter::~TremorDecWriter(void)
{
	mOutputFile.close();
	delete[] mOutputBuffer;
}

bool TremorDecWriter::acceptStampedOggPacket(StampedOggPacket* inPacket)
{
	unsigned long locSampleCount = 0;
	TremorDecoder::eVorbisResult locRes = mTremorDecoder.decodePacket(		inPacket->packetData()
																	,	inPacket->packetSize()
																	,	mOutputBuffer
																	,	OUTPUT_BUFFER_SIZE
																	,	&locSampleCount);

	if (locRes == TremorDecoder::VORBIS_DATA_OK) {
		mOutputFile.write((char*)mOutputBuffer, locSampleCount * mTremorDecoder.numChannels() * sizeof(short));
		return true;
	} else if (locRes < TremorDecoder::VORBIS_ERROR_MIN) {
		return true;
	} else {
		return false;
	}
}
bool TremorDecWriter::acceptOggPage(OggPage* inOggPage)
{
	return mPacketiser.acceptOggPage(inOggPage);
}