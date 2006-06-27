#pragma once

#include "VorbisDecoder.h"
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
class VorbisDecWriter
	:	public IOggCallback
	,	public IStampedOggPacketSink
{
public:
	VorbisDecWriter(string inFilename);

	virtual ~VorbisDecWriter(void);

	virtual bool acceptStampedOggPacket(StampedOggPacket* inPacket);
	virtual bool acceptOggPage(OggPage* inOggPage);

private:
	OggPacketiser mPacketiser;
	VorbisDecoder mVorbisDecoder;
	short* mOutputBuffer;
	static const unsigned long OUTPUT_BUFFER_SIZE = 4096*16;

	fstream mOutputFile;
};
