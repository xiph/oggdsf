#pragma once
#include <libOOOgg/dllstuff.h>
#include <libOOOgg/StampedOggPacket.h>
#include <libOOOgg/OggPacket.h>
#include "FLAC++/decoder.h"

//Flac Stream Format Documentation http://flac.sourceforge.net/format.html#stream

using namespace FLAC::Decoder;
class FLACPushDecoder
	:	protected Stream
{
public:
	FLACPushDecoder(void);
	virtual ~FLACPushDecoder(void);

	StampedOggPacket* decodeFLAC(OggPacket* inPacket);
	bool acceptMetadata(OggPacket* inPacket);

	void initCodec();
	void flushCodec();

	unsigned long numberOfChannels()		{	return mNumChannels;	}
	unsigned long frameSize()				{	return mFrameSize;		}
	unsigned long sampleRate()				{	return mSampleRate;		}
private:
	static const int SIZE_16_BITS = 2;

	unsigned long mNumChannels;
	unsigned long mFrameSize;
	unsigned long mSampleRate;

	//Virtuals frmo FLAC decoder
	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], unsigned *bytes);
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	OggPacket* mInPacket;
	StampedOggPacket* mOutPacket;
	
	bool mGotMetaData;
	bool mBegun;
	
};
