#pragma once

//class ITheoraEncodeSettings
//{
//public:
//	DECLARE_IUNKNOWN
//	virtual unsigned long targetBitrate() = 0;
//	virtual unsigned char quality() = 0;
//	virtual unsigned long keyframeFreq() = 0;
//
//	virtual bool setTargetBitrate(unsigned long inBitrate) = 0;
//	virtual bool setQuality(unsigned char inQuality) = 0;
//	virtual bool setKeyframeFreq(unsigned long inKeyframeFreq) = 0;
//protected:
//	//unsigned long mTargetBitrate;
//
//	//unsigned char mQuality;
//
//	//unsigned long mKeyframeFreq;
//	
//};

#ifndef __ITHEORAENCODESETTINGS__
#define __ITHEORAENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif


DECLARE_INTERFACE_(ITheoraEncodeSettings, IUnknown) {

	virtual STDMETHODIMP_(unsigned long) targetBitrate() PURE;
	
	virtual STDMETHODIMP_(unsigned char) quality() PURE;
	virtual STDMETHODIMP_(unsigned long) keyframeFreq() PURE;

	virtual STDMETHODIMP_(bool) setTargetBitrate(unsigned long inBitrate) PURE;
	virtual STDMETHODIMP_(bool) setQuality(unsigned char inQuality) PURE;
	virtual STDMETHODIMP_(bool) setKeyframeFreq(unsigned long inKeyframeFreq) PURE;


};

#ifdef __cplusplus
}
#endif

#endif