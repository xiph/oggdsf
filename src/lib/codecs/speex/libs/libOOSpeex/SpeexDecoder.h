#pragma once

#include "SpeexDecodeSettings.h"
//#include <libOOOgg/dllstuff.h>
//#include <libOOOgg/StampedOggPacket.h>
extern "C" {
//#include "speex/speex.h"
#include "speex_cdecl.h"
//#include "speex/speex_header.h"
#include "speex_header_cdecl.h"
//#include "speex/speex_callbacks.h"
#include "speex_callbacks_cdecl.h"
//#include "speex/speex_stereo.h"
#include "speex_stereo_cdecl.h"
}

class SpeexDecoder
{
public:
	SpeexDecoder(void);
	~SpeexDecoder(void);

	enum eSpeexResult {
		SPEEX_DATA_OK,
		SPEEX_HEADER_OK,
		SPEEX_COMMENT_OK,
		SPEEX_EXTRA_HEADER_OK,
		SPEEX_ERROR_MIN = 64,
		SPEEX_BAD_HEADER,
		SPEEX_CORRUPTED_BITSTREAM,
		SPEEX_CORRUPTED_UNDERFLOW,
		SPEEX_INVALID_SPEEX_VERSION,
		SPEEX_INITIALISATION_FAILED,

	};

	bool setDecodeParams(SpeexDecodeSettings inSettings);
	eSpeexResult decodePacket(		const unsigned char* inPacket
								,	unsigned long inPacketSize
								,	short* outSamples
								,	unsigned long inOutputBufferSize); 

	int frameSize()		{	return mFrameSize;		}
	int numChannels()	{	return mNumChannels;	}
	int sampleRate()	{	return mSampleRate;		}
protected:
	eSpeexResult decodeHeader(const unsigned char* inPacket, unsigned long inPacketSize);
	unsigned long mPacketCount;

	int mFrameSize;
	int mNumChannels;
	int mSampleRate;
	int mNumFrames;
	int mNumExtraHeaders;
	bool mIsVBR;

	SpeexStereoState mStereoState;
	SpeexBits mSpeexBits;
	void* mSpeexState;

	SpeexDecodeSettings mDecoderSettings;

};
