#pragma once
#include "dllstuff.h"
#include "StampedOggPacket.h"
#include "OggPacket.h"
#include "FLAC++/decoder.h"
using namespace FLAC::Decoder;
class FLACPushDecoder
	:	protected Stream
{
public:
	FLACPushDecoder(void);
	virtual ~FLACPushDecoder(void);

	StampedOggPacket* decodeFLAC(OggPacket* inPacket);

	void initCodec();
	void flushCodec();
	//Probably shouldn't be public... but who cares for now.
	unsigned long mNumChannels;
	unsigned long mFrameSize;
	unsigned long mSampleRate;
protected:
	static const int SIZE_16_BITS = 2;
	//Virtuals frmo FLAC decoder
	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], unsigned *bytes);
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	OggPacket* mInPacket;
	StampedOggPacket* mOutPacket;
	bool mBegun;
	
};
