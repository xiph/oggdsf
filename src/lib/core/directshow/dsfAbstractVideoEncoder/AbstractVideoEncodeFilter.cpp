#include "StdAfx.h"
#include "abstractvideoencodefilter.h"

AbstractVideoEncodeFilter::AbstractVideoEncodeFilter(TCHAR* inFilterName, REFCLSID inFilterGUID, unsigned short inVideoFormat )
	:	CBaseFilter(inFilterName, NULL,m_pLock, inFilterGUID),
		mVideoFormat(inVideoFormat)
{

	m_pLock = new CCritSec;
	
}

AbstractVideoEncodeFilter::~AbstractVideoEncodeFilter(void)
{
	delete m_pLock;
	DestroyPins();
}

void AbstractVideoEncodeFilter::DestroyPins() {
	delete mOutputPin;
	delete mInputPin;
}

STDMETHODIMP AbstractVideoEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

CBasePin* AbstractVideoEncodeFilter::GetPin(int inPinNo) {
	//FIX::: Errors here
	if (inPinNo < 0 ) {
		return NULL;
	} else if (inPinNo == 0) {
		return mInputPin;
	} else if (inPinNo == 1) {
		return mOutputPin;
	}
}

STDMETHODIMP AbstractVideoEncodeFilter::Stop() {
	CAutoLock locLock(m_pLock);
	mInputPin->ResetFrameCount();
	return CBaseFilter::Stop();
}
int AbstractVideoEncodeFilter::GetPinCount(void) {
	const long NUM_PINS = 2;
	return NUM_PINS;
}	

