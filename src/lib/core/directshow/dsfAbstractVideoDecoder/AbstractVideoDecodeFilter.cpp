//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

#include "stdafx.h"
#include "abstractVideodecodefilter.h"

AbstractVideoDecodeFilter::AbstractVideoDecodeFilter(TCHAR* inFilterName, REFCLSID inFilterGUID, unsigned short inVideoFormat )
	:	CBaseFilter(inFilterName, NULL,m_pLock, inFilterGUID),
		mVideoFormat(inVideoFormat)
{
	//debugLog.open("G:\\logs\\avdFitler.log", ios_base::out);
	m_pLock = new CCritSec;			//Deleted in destructor
	
}

AbstractVideoDecodeFilter::~AbstractVideoDecodeFilter(void)
{
	//debugLog.close();
	delete m_pLock;
	DestroyPins();
}

void AbstractVideoDecodeFilter::DestroyPins() {
	delete mOutputPin;
	delete mInputPin;
}

STDMETHODIMP AbstractVideoDecodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

CBasePin* AbstractVideoDecodeFilter::GetPin(int inPinNo) {
	//FIX::: Errors here
	if (inPinNo < 0 ) {
		return NULL;
	} else if (inPinNo == 0) {
		return mInputPin;
	} else if (inPinNo == 1) {
		return mOutputPin;
	}
}

STDMETHODIMP AbstractVideoDecodeFilter::Stop() {
	CAutoLock locLock(m_pLock);
	//debugLog<<"**** STOP ****"<<endl;
	mInputPin->ResetFrameCount();
	return CBaseFilter::Stop();
}

STDMETHODIMP AbstractVideoDecodeFilter::Pause() {
	CAutoLock locLock(m_pLock);
	//debugLog<<"**** PAUSE ****"<<endl;
	return CBaseFilter::Pause();
}

STDMETHODIMP AbstractVideoDecodeFilter::Run(REFERENCE_TIME tStart) {
	CAutoLock locLock(m_pLock);
	//debugLog<<"**** RUN ****"<<endl;
	return CBaseFilter::Run(tStart);
}
int AbstractVideoDecodeFilter::GetPinCount(void) {
	//TO DO::: Move this somewhere better
	const long NUM_PINS = 2;
	return NUM_PINS;
}	

