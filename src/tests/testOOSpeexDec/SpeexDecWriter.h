#pragma once

#include "SpeexDecoder.h"
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
class SpeexDecWriter
	:	public IOggCallback
	,	public IStampedOggPacketSink
{
public:
	SpeexDecWriter(string inFilename);

	virtual ~SpeexDecWriter(void);

	virtual bool acceptStampedOggPacket(StampedOggPacket* inPacket);
	virtual bool acceptOggPage(OggPage* inOggPage);

private:
	OggPacketiser mPacketiser;
	SpeexDecoder mSpeexDecoder;
	short* mOutputBuffer;
	static const unsigned long OUTPUT_BUFFER_SIZE = 2048;

	fstream mOutputFile;
};
