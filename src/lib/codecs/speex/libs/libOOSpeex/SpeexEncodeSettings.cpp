#include "StdAfx.h"
#include "SpeexEncodeSettings.h"

SpeexEncodeSettings::SpeexEncodeSettings(void)
    :   mSampleRate(0)
    ,   mNumChannels(0)
    ,   mComplexity(3)
    ,   mQuality(8)
    ,   mBitrate(-1)
    ,   mVBRMaxBitrate(-1)
    ,   mFramesPerPacket(1)
    ,   mUsingDTX(false)
    ,   mUsingDenoise(false)
    ,   mUsingAGC(false)
    ,   mUsingVAD(false)
    ,   mBitrateControlMode(SPEEX_BITRATE_CBR_QUALITY)
    ,   mEncodingMode(SPEEX_ENCODE_NO_MODE_SET)
{
}

SpeexEncodeSettings::~SpeexEncodeSettings(void)
{
}

bool SpeexEncodeSettings::setAudioParameters(unsigned long inSampleRate, unsigned long inNumChannels)
{
    if (        (inSampleRate >= 6000)
            &&  (inSampleRate <= 48000)
            &&  (inNumChannels > 0)
            &&  (inNumChannels <= 2)) {

        mSampleRate = inSampleRate;
        mNumChannels = inNumChannels;
        setMode(SPEEX_ENCODE_NO_MODE_SET);
        return true;
    }
    return false;

}
bool SpeexEncodeSettings::setMode(eSpeexEncodeMode inMode)
{
    if (inMode == SPEEX_ENCODE_NO_MODE_SET) {
        if ((mSampleRate < 6000) || (mSampleRate > 48000)) {
            return false;
        } else if (mSampleRate > 25000) {
            mEncodingMode = SPEEX_ENCODE_ULTRA_WIDEBAND;
        } else if (mSampleRate > 12500) {
            mEncodingMode = SPEEX_ENCODE_WIDEBAND;
        } else {
            mEncodingMode = SPEEX_ENCODE_NARROWBAND;
        }
        return true;
    } else {
        mEncodingMode = inMode;
        return true;
    }
    
}
bool SpeexEncodeSettings::setComplexity(long inComplexity)
{
    if ((inComplexity >= 0) && (inComplexity <= 10)) {
        mComplexity = inComplexity;
        return true;
    }
    return false;
}

bool SpeexEncodeSettings::setupVBRQualityMode(long inQuality, long inVBRMaxBitrate)
{
    if (        (inQuality >= 0)
            &&  (inQuality <= 10)) {
        mQuality = inQuality;
        mVBRMaxBitrate = inVBRMaxBitrate;
        mBitrateControlMode = SPEEX_BITRATE_VBR_QUALITY;
        return true;
    }
    return false;
}
bool SpeexEncodeSettings::setupVBRBitrateMode(long inBitrate, long inVBRMaxBitrate)
{
    if (inBitrate > 0) {
        mBitrate = inBitrate;
        mVBRMaxBitrate = inVBRMaxBitrate;
        mBitrateControlMode = SPEEX_BITRATE_VBR_BITRATE;
        return true;
    }
    return false;
}
bool SpeexEncodeSettings::setupABR(long inABRBitrate)
{
    if (inABRBitrate > 0) {
        mBitrate = inABRBitrate;
        mBitrateControlMode = SPEEX_BITRATE_ABR;
        return true;
    }
    return false;
}
bool SpeexEncodeSettings::setupCBRBitrateMode(long inCBRBitrate)
{

    if (inCBRBitrate > 0) {
        mBitrate = inCBRBitrate;
        mBitrateControlMode = SPEEX_BITRATE_CBR_BITRATE;
        return true;
    }
    return false;
}
bool SpeexEncodeSettings::setupCBRQualityMode(long inQuality)
{
    if (inQuality > 0) {
        mQuality = inQuality;
        mBitrateControlMode = SPEEX_BITRATE_CBR_QUALITY;
        return true;
    }
    return false;
}

bool SpeexEncodeSettings::setEncodingFlags(bool inUseDTX, bool inUseVAD, bool inUseAGC, bool inUseDenoise)
{
    mUsingDTX = inUseDTX;
    mUsingVAD = inUseVAD;
    mUsingAGC = inUseAGC;
    mUsingDenoise = inUseDenoise;
    return true;
}