#pragma once

#ifndef __ISPEEXENCODESETTINGS__
#define __ISPEEXENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif

#include "SpeexEncodeSettings.h"

DECLARE_INTERFACE_(ISpeexEncodeSettings, IUnknown) {

	//virtual STDMETHODIMP_(bool) setQuality(signed char inQuality) PURE;
	virtual STDMETHODIMP_(SpeexEncodeSettings) getEncoderSettings() PURE;
	

    virtual STDMETHODIMP_(bool) setMode(SpeexEncodeSettings::eSpeexEncodeMode inMode) PURE;
    virtual STDMETHODIMP_(bool) setComplexity(long inComplexity) PURE;

    virtual STDMETHODIMP_(bool) setupVBRQualityMode(long inQuality, long inVBRMaxBitrate) PURE;
    virtual STDMETHODIMP_(bool) setupVBRBitrateMode(long inBitrate, long inVBRMaxBitrate) PURE;
    virtual STDMETHODIMP_(bool) setupABR(long inABRBitrate) PURE;
    virtual STDMETHODIMP_(bool) setupCBRBitrateMode(long inCBRBitrate) PURE;
    virtual STDMETHODIMP_(bool) setupCBRQualityMode(long inQuality) PURE;

    virtual STDMETHODIMP_(bool) setEncodingFlags(bool inUseDTX, bool inUseVAD, bool inUseAGC, bool inUseDenoise) PURE;

    //unsigned long sampleRate()      {       return mSampleRate;     }
    //unsigned long numChannels()     {       return mNumChannels;    }

    //long encodingComplexity()       {       return mComplexity;     }
    //long quality()                  {       return mQuality;        }
    //long targetBitrate()            {       return mBitrate;        }
    //long maxVBRBitrate()            {       return mVBRMaxBitrate;  }
    //long framesPerPacket()          {       return mFramesPerPacket;}

    //eSpeexEncodeBitrateControl bitrateControlMode()      {   return mBitrateControlMode;     }
    //eSpeexEncodeMode encodingMode()                     {   return mEncodingMode;           }

    //bool isUsingDTX()               {       return mUsingDTX;       }
    //bool isUsingVAD()               {       return mUsingVAD;       }
    //bool isUsingAGC()               {       return mUsingAGC;       }
    //bool isUsingDenoise()           {       return mUsingDenoise;   }
};



#ifdef __cplusplus
}
#endif

#endif