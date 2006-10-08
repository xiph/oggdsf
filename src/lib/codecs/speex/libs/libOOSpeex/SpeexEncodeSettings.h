#pragma once

class SpeexEncodeSettings
{
public:
    SpeexEncodeSettings(void);
    ~SpeexEncodeSettings(void);

    enum eSpeexEncodeMode {
        SPEEX_ENCODE_NO_MODE_SET,
        SPEEX_ENCODE_NARROWBAND,
        SPEEX_ENCODE_WIDEBAND,
        SPEEX_ENCODE_ULTRA_WIDEBAND,
        
    };

    enum eSpeexEncodeBitrateControl {
        SPEEX_BITRATE_NO_BITRATE_CONTROL_SET,
        SPEEX_BITRATE_VBR_QUALITY,
        SPEEX_BITRATE_VBR_BITRATE,
        SPEEX_BITRATE_ABR,
        SPEEX_BITRATE_CBR_QUALITY,
        SPEEX_BITRATE_CBR_BITRATE,
    };

    bool setAudioParameters(unsigned long inSampleRate, unsigned long inNumChannels);
    bool setMode(eSpeexEncodeMode inMode);
    bool setComplexity(long inComplexity);

    bool setupVBRQualityMode(long inQuality, long inVBRMaxBitrate);
    bool setupVBRBitrateMode(long inBitrate, long inVBRMaxBitrate);
    bool setupABR(long inABRBitrate);
    bool setupCBRBitrateMode(long inCBRBitrate);
    bool setupCBRQualityMode(long inQuality);

    bool setEncodingFlags(bool inUseDTX, bool inUseVAD, bool inUseAGC, bool inUseDenoise);

    unsigned long sampleRate()      {       return mSampleRate;     }
    unsigned long numChannels()     {       return mNumChannels;    }

    long encodingComplexity()       {       return mComplexity;     }
    long quality()                  {       return mQuality;        }
    long targetBitrate()            {       return mBitrate;        }
    long maxVBRBitrate()            {       return mVBRMaxBitrate;  }
    long framesPerPacket()          {       return mFramesPerPacket;}

    eSpeexEncodeBitrateControl bitrateControlMode()      {   return mBitrateControlMode;     }
    eSpeexEncodeMode encodingMode()                     {   return mEncodingMode;           }

    bool isUsingDTX()               {       return mUsingDTX;       }
    bool isUsingVAD()               {       return mUsingVAD;       }
    bool isUsingAGC()               {       return mUsingAGC;       }
    bool isUsingDenoise()           {       return mUsingDenoise;   }

private:
    unsigned long mSampleRate;
    unsigned long mNumChannels;

    long mComplexity;
    long mQuality;
    long mBitrate;
    long mVBRMaxBitrate;
    long mFramesPerPacket; //??Meaningful?

    eSpeexEncodeBitrateControl mBitrateControlMode;
    eSpeexEncodeMode mEncodingMode;

    bool mUsingDTX;
    bool mUsingVAD;
    bool mUsingAGC;
    bool mUsingDenoise;
    
};
