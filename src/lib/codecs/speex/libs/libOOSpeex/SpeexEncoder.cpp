#include "StdAfx.h"
#include "SpeexEncoder.h"

SpeexEncoder::SpeexEncoder(void)
    :   mSpeexFrameSize(-1)
    ,   mSpeexLookAhead(-1)
    ,   mSampleWidth(16)
    ,   mSpeexFrameCount(0)
    ,   mLastGranulePos(0)
    ,   mSpeexWorkingFrameBuffer(NULL)
    ,   mSpeexWorkingFrameBufferUpto(0)
    ,   mSpeexState(NULL)
    ,   mSpeexPreprocessState(NULL)
{
}

SpeexEncoder::~SpeexEncoder(void)
{
}

string SpeexEncoder::getVendorString()
{
    char* locVersionBuf = NULL;
    speex_encoder_ctl(mSpeexState, SPEEX_LIB_GET_VERSION_STRING, &locVersionBuf);
    string locVersionString(locVersionBuf);
    string retString = string("Encoded with Speex ") + locVersionString;
    return retString;
}
vector<StampedOggPacket*> SpeexEncoder::setupCodec(SpeexEncodeSettings inSettings)
{

    if (inSettings.encodingMode() == SpeexEncodeSettings::SPEEX_ENCODE_NO_MODE_SET) {
        return vector<StampedOggPacket*>();
    }
    const SpeexMode* locLibSpeexMode = speex_lib_get_mode(inSettings.encodingMode() - 1);
    SpeexHeader locLibSpeexHeader;

    
    speex_init_header(&locLibSpeexHeader, inSettings.sampleRate(), inSettings.numChannels(), locLibSpeexMode);
    locLibSpeexHeader.frames_per_packet = inSettings.framesPerPacket();

    if (        (inSettings.encodingMode() == SpeexEncodeSettings::SPEEX_BITRATE_VBR_BITRATE)
           ||  (inSettings.encodingMode() == SpeexEncodeSettings::SPEEX_BITRATE_VBR_QUALITY)) {
        locLibSpeexHeader.vbr = 1;
    }

    mSpeexState = speex_encoder_init(locLibSpeexMode);


    long temp = inSettings.encodingComplexity();
    speex_encoder_ctl(mSpeexState, SPEEX_GET_FRAME_SIZE, &mSpeexFrameSize);
    speex_encoder_ctl(mSpeexState, SPEEX_SET_COMPLEXITY, &temp);
    temp = inSettings.sampleRate();
    speex_encoder_ctl(mSpeexState, SPEEX_SET_SAMPLING_RATE, &temp);

    long SPEEX_TRUE = 1;
    long SPEEX_FALSE = 1;
    switch (inSettings.bitrateControlMode()) {
        case SpeexEncodeSettings::SPEEX_BITRATE_ABR:
            temp = inSettings.targetBitrate();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_ABR, &temp);
            
            break;
        case SpeexEncodeSettings::SPEEX_BITRATE_CBR_BITRATE:
            temp = inSettings.targetBitrate();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_BITRATE, &temp);
            break;
        case SpeexEncodeSettings::SPEEX_BITRATE_CBR_QUALITY:
            temp = inSettings.quality();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_QUALITY, &temp);
            break;
        case SpeexEncodeSettings::SPEEX_BITRATE_VBR_BITRATE:
            temp = inSettings.maxVBRBitrate();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_VBR_MAX_BITRATE, &temp);
            temp = inSettings.targetBitrate();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_BITRATE, &temp);
            speex_encoder_ctl(mSpeexState, SPEEX_SET_VBR, &SPEEX_TRUE);
            break;
        case SpeexEncodeSettings::SPEEX_BITRATE_VBR_QUALITY:
            temp = inSettings.maxVBRBitrate();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_VBR_MAX_BITRATE, &temp);
            temp = inSettings.quality();
            speex_encoder_ctl(mSpeexState, SPEEX_SET_QUALITY, &temp);
            break;
        default:
            return vector<StampedOggPacket*>();
            break;

    };

    if (inSettings.isUsingVAD()) {
        speex_encoder_ctl(mSpeexState, SPEEX_SET_VAD, &SPEEX_TRUE);
    }

    if (inSettings.isUsingDTX()) {
        speex_encoder_ctl(mSpeexState, SPEEX_SET_DTX, &SPEEX_TRUE);
    }

    speex_encoder_ctl(mSpeexState, SPEEX_GET_LOOKAHEAD, &mSpeexLookAhead);

    if (inSettings.isUsingAGC() || inSettings.isUsingDenoise()) {
        //Using the speex pre-processor
        mSpeexPreprocessState = speex_preprocess_state_init(mSpeexFrameSize, inSettings.sampleRate());
        
        speex_encoder_ctl(mSpeexState, SPEEX_PREPROCESS_SET_AGC, inSettings.isUsingAGC()    ?   &SPEEX_TRUE
                                                                                            :   &SPEEX_FALSE);
        speex_encoder_ctl(mSpeexState, SPEEX_PREPROCESS_SET_DENOISE, inSettings.isUsingDenoise()    ?   &SPEEX_TRUE
                                                                                                    :   &SPEEX_FALSE);
        mSpeexLookAhead += mSpeexFrameSize;
    } else {
        //Not using pre-processing
        mSpeexPreprocessState = NULL;
    }


    mSpeexWorkingFrameBuffer = new unsigned char[mSpeexFrameSize * inSettings.numChannels() * (mSampleWidth >> 3)];
    mSpeexWorkingFrameBufferUpto = 0;


    mSettings = inSettings;

    int locHeaderPacketSize = 0;

    //Make the header packet
    vector<StampedOggPacket*> retPacketList;
    unsigned char* locSpeexHeaderBuffer = (unsigned char*)speex_header_to_packet(&locLibSpeexHeader, &locHeaderPacketSize);

    //Put it ina new'd buffer so it can be deleted properly
    unsigned char* locHeaderBuffer = new unsigned char[locHeaderPacketSize];
    memcpy((void*)locHeaderBuffer, (const void*)locSpeexHeaderBuffer, locHeaderPacketSize);
    free(locSpeexHeaderBuffer);
    locSpeexHeaderBuffer = NULL;


    StampedOggPacket* locHeaderPacket = new StampedOggPacket(locHeaderBuffer, locHeaderPacketSize, false, false, 0, 0, StampedOggPacket::OGG_BOTH);
    retPacketList.push_back(locHeaderPacket);
    locHeaderPacket = NULL;

    //Comment packet
    string locVendorString = getVendorString();
    //8 is 4 for the length of the vendor string, and 4 for the number of user comments
    // of which there will be zero.

    long locCommentHeaderSize = locVendorString.size() + 8;
    unsigned char* locCommentBuffer = new unsigned char[locCommentHeaderSize];
    *((int*)locCommentBuffer) = locVendorString.size();
    memcpy((void*)(locCommentBuffer + 4), (const void*)locVendorString.c_str(), locVendorString.size());
    *((int*)(locCommentBuffer + 4 + locVendorString.size())) = 0;
    StampedOggPacket* locCommentPacket = new StampedOggPacket(locCommentBuffer, locCommentHeaderSize, false, false, 0, 0, StampedOggPacket::OGG_BOTH);
    retPacketList.push_back(locCommentPacket);


    speex_bits_init(&mSpeexBits);
    return retPacketList;

}

vector<StampedOggPacket*> SpeexEncoder::encode(const short* const inSampleBuffer, unsigned long inNumSamplesPerChannel)
{
    assert(mSampleWidth == 16);

    vector<StampedOggPacket*> retPacketList;
    const unsigned char* locUptoSamplePointer = (const unsigned char*)inSampleBuffer;
    long locSamplesLeft = inNumSamplesPerChannel;
    long locSamplesToCopy;
    long locBytesToCopy;
    long locBytesPerSample = mSettings.numChannels() * (mSampleWidth >> 3);

    while (locSamplesLeft > 0) {
        
        locSamplesToCopy = mSpeexFrameSize - mSpeexWorkingFrameBufferUpto;
        locSamplesToCopy = (locSamplesToCopy > locSamplesLeft) ? locSamplesLeft : locSamplesToCopy;
        locBytesToCopy = locSamplesToCopy * locBytesPerSample;
        locSamplesLeft -= locSamplesToCopy;

        memcpy((void*)(mSpeexWorkingFrameBuffer + (mSpeexWorkingFrameBufferUpto * locBytesPerSample)), (const void*)locUptoSamplePointer, locBytesToCopy);
        locUptoSamplePointer += locBytesToCopy;
        mSpeexWorkingFrameBufferUpto += locSamplesToCopy;

        assert(mSpeexWorkingFrameBufferUpto <= mSpeexFrameSize);
        if (mSpeexWorkingFrameBufferUpto == mSpeexFrameSize) {
            //Got a full frame.

            if (mSettings.numChannels() == 2) {
                speex_encode_stereo_int((short*)mSpeexWorkingFrameBuffer, mSpeexFrameSize, &mSpeexBits);
            }

            if (mSpeexPreprocessState != NULL) {
                speex_preprocess(mSpeexPreprocessState, (short*)mSpeexWorkingFrameBuffer, NULL);
            }

            speex_encode_int(mSpeexState, (short*)mSpeexWorkingFrameBuffer, &mSpeexBits);

            mSpeexWorkingFrameBufferUpto = 0;

            mSpeexFrameCount++;

            if ((mSpeexFrameCount % mSettings.framesPerPacket()) != 0) {
                //Put more in this packet
                continue;
            } else {
                //This packet is ready to go
                unsigned long locPacketSize = speex_bits_nbytes(&mSpeexBits);
                unsigned char* locPacketBuffer = new unsigned char[locPacketSize];

                speex_bits_write(&mSpeexBits, (char*)locPacketBuffer, MAX_FRAME_BYTES); 
                speex_bits_reset(&mSpeexBits);
                LOOG_INT64 locGranulePos = (mSpeexFrameCount * mSpeexFrameSize) - mSpeexLookAhead;
                StampedOggPacket* locPacket = new StampedOggPacket(locPacketBuffer, locPacketSize, false, false, mLastGranulePos, locGranulePos, StampedOggPacket::OGG_BOTH);
                mLastGranulePos = locGranulePos;
                retPacketList.push_back(locPacket);
            }
        }
        
    }

    return retPacketList;
}
