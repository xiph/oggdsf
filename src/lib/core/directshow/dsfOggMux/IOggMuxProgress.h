#pragma once
#ifndef __IOGGMUXPROGRESS__
#define __IOGGMUXPROGRESS__

#ifdef __cplusplus
extern "C" {
#endif



DECLARE_INTERFACE_(IOggMuxProgress, IUnknown) {

	virtual STDMETHODIMP_(LONGLONG) getProgressTime() PURE;
	virtual STDMETHODIMP_(LONGLONG) getBytesWritten() PURE;
	

};

#ifdef __cplusplus
}
#endif

#endif