#pragma once

#ifndef __IVORBISENCODESETTINGS__
#define __IVORBISENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif


DECLARE_INTERFACE_(IVorbisEncodeSettings, IUnknown) {

	//virtual STDMETHODIMP_(unsigned long) targetBitrate() PURE;
	//virtual STDMETHODIMP_(bool) setTargetBitrate(unsigned long inBitrate) PURE;
	
	virtual STDMETHODIMP_(signed char) quality() PURE;
	virtual STDMETHODIMP_(bool) setQuality(signed char inQuality) PURE;
};

#ifdef __cplusplus
}
#endif

#endif