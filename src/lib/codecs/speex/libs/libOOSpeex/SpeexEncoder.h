#pragma once

#include <libOOOgg/dllstuff.h>
#include <libOOOgg/StampedOggPacket.h>

#include "SpeexEncodeSettings.h"

extern "C" {
#include "speex/speex.h"
#include "speex/speex_header.h"
#include "speex/speex_callbacks.h"
#include "speex/speex_stereo.h"
#include "speex/speex_preprocess.h"
}

#include <vector>
#include <string>

#include <assert.h>

using namespace std;

class SpeexEncoder
{
public:
    SpeexEncoder(void);
    ~SpeexEncoder(void);

    vector<StampedOggPacket*> setupCodec(SpeexEncodeSettings inSettings);
    vector<StampedOggPacket*> encode(const short* const inSampleBuffer, unsigned long inNumSamplesPerChannel);
    vector<StampedOggPacket*> flush();      //Needed?

    string getVendorString();

private:
    static const unsigned long MAX_FRAME_BYTES = 2000;
    SpeexEncodeSettings mSettings;

    long mSpeexFrameSize;
    long mSpeexLookAhead;
    long mSampleWidth;
    unsigned long mSpeexFrameCount;
    LOOG_INT64 mLastGranulePos;

    unsigned char* mSpeexWorkingFrameBuffer;
    unsigned long mSpeexWorkingFrameBufferUpto;
    void* mSpeexState;
    SpeexPreprocessState* mSpeexPreprocessState;
    SpeexBits mSpeexBits;

};
