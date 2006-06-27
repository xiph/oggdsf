#pragma once

#include "TremorDecoder.h"
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
class TremorDecWriter
	:	public IOggCallback
	,	public IStampedOggPacketSink
{
public:
	TremorDecWriter(string inFilename);

	virtual ~TremorDecWriter(void);

	virtual bool acceptStampedOggPacket(StampedOggPacket* inPacket);
	virtual bool acceptOggPage(OggPage* inOggPage);

private:
	OggPacketiser mPacketiser;
	TremorDecoder mTremorDecoder;
	short* mOutputBuffer;
	static const unsigned long OUTPUT_BUFFER_SIZE = 4096*16;

	fstream mOutputFile;
};
