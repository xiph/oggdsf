#include "StdAfx.h"
#include "SpeexDecWriter.h"

SpeexDecWriter::SpeexDecWriter(string inFilename)
{
	mPacketiser.setPacketSink(this);
	mOutputBuffer = new short[OUTPUT_BUFFER_SIZE];
	mOutputFile.open(inFilename.c_str(), ios_base::out | ios_base::binary);
}

SpeexDecWriter::~SpeexDecWriter(void)
{
	mOutputFile.close();
	delete[] mOutputBuffer;
}

bool SpeexDecWriter::acceptStampedOggPacket(StampedOggPacket* inPacket)
{
	SpeexDecoder::eSpeexResult locRes = mSpeexDecoder.decodePacket(		inPacket->packetData()
																	,	inPacket->packetSize()
																	,	mOutputBuffer
																	,	OUTPUT_BUFFER_SIZE);

	if (locRes == SpeexDecoder::SPEEX_DATA_OK) {
		mOutputFile.write((char*)mOutputBuffer, mSpeexDecoder.frameSize() * mSpeexDecoder.numChannels() * sizeof(short));
		return true;
	} else if (locRes < SpeexDecoder::SPEEX_ERROR_MIN) {
		return true;
	} else {
		return false;
	}
}
bool SpeexDecWriter::acceptOggPage(OggPage* inOggPage)
{
	return mPacketiser.acceptOggPage(inOggPage);
}