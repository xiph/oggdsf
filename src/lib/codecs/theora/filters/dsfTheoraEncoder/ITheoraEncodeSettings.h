#pragma once


#ifndef __ITHEORAENCODESETTINGS__
#define __ITHEORAENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif


DECLARE_INTERFACE_(ITheoraEncodeSettings, IUnknown) {

    virtual STDMETHODIMP_(bool) canModifySettings() PURE;

	virtual STDMETHODIMP_(unsigned long) targetBitrate() PURE;
    virtual STDMETHODIMP_(unsigned long) keyFrameDataBitrate() PURE;
	virtual STDMETHODIMP_(unsigned char) quality() PURE;
    virtual STDMETHODIMP_(long) sharpness() PURE;
    virtual STDMETHODIMP_(long) noiseSensitivity() PURE;
	virtual STDMETHODIMP_(unsigned long) keyframeFreq() PURE;
    virtual STDMETHODIMP_(bool) isFixedKeyframeInterval() PURE;
    virtual STDMETHODIMP_(bool) allowDroppedFrames() PURE;
    virtual STDMETHODIMP_(bool) isUsingQualityMode() PURE;
    virtual STDMETHODIMP_(bool) isUsingQuickMode() PURE;
	virtual STDMETHODIMP_(unsigned long) keyframeFreqMin() PURE;
    virtual STDMETHODIMP_(long) keyframeAutoThreshold() PURE;

	virtual STDMETHODIMP_(bool) setTargetBitrate(unsigned long inBitrate) PURE;
    virtual STDMETHODIMP_(bool) setKeyframeDataBitrate(unsigned long inBitrate) PURE;
	virtual STDMETHODIMP_(bool) setQuality(unsigned char inQuality) PURE;
    virtual STDMETHODIMP_(bool) setSharpness(long inSharpness) PURE;
    virtual STDMETHODIMP_(bool) setNoiseSensitivity(long inNoiseSensitivity) PURE;
	virtual STDMETHODIMP_(bool) setKeyframeFreq(unsigned long inKeyframeFreq) PURE;
    virtual STDMETHODIMP_(bool) setIsFixedKeyframeInterval(bool inIsFixedKeyframeInterval) PURE;
    virtual STDMETHODIMP_(bool) setAllowDroppedFrames(bool inAllowDroppedFrames) PURE;
    virtual STDMETHODIMP_(bool) setIsUsingQualityMode(bool inIsUsingQualityMode) PURE;
    virtual STDMETHODIMP_(bool) setIsUsingQuickMode(bool inIsUsingQuickMode) PURE;
    virtual STDMETHODIMP_(bool) setKeyframeFreqMin(unsigned long inKeyframeFreqMin) PURE;
    virtual STDMETHODIMP_(bool) setKeyframeAutoThreshold(long inKeyframeAutoThreshold) PURE;

    virtual STDMETHODIMP_(void) setFlipImageVerticaly(bool flipImageVerticaly) PURE;
    virtual STDMETHODIMP_(bool) getFlipImageVerticaly() PURE;
};

#ifdef __cplusplus
}
#endif

#endif