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
#include "abstractvideodecodeinputpin.h"

//#include <mtype.h>
AbstractVideoDecodeInputPin::AbstractVideoDecodeInputPin (AbstractVideoDecodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoDecodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName, CMediaType* inAcceptMediaType)
	:	CBaseInputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName)
	,	mOutputPin(inOutputPin)
	,	mParentFilter(inParentFilter)
	
	,	mBegun(false)
	
	,	mHeight(0)
	,	mWidth(0)

	,	mFrameDuration(0)
	,	mFrameSize(0)
	,	mFrameCount(0)
	
	,	mStreamLock(NULL)
	,	mLastSeenStartGranPos(0)
	,	mSeekTimeBase(0)
		
{
	//debugLog.open("G:\\logs\\avdInputPin.log", ios_base::out);
	//ConstructCodec();
	mStreamLock = new CCritSec;				//Deleted in destructor
	mAcceptableMediaType = inAcceptMediaType;

	//IMediaSeeking* locSeeker = NULL;
	//this->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	//mOutputPin->SetDelegate(locSeeker);
}

AbstractVideoDecodeInputPin::~AbstractVideoDecodeInputPin(void)
{
	//debugLog.close();
	//DestroyCodec();
	delete mStreamLock;
	
}
STDMETHODIMP AbstractVideoDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	//debugLog<<"Query Interface"<<endl;
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		//debugLog<<"Queried for IMediaSeeking"<<endl;
		return NOERROR;
	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT AbstractVideoDecodeInputPin::BreakConnect() {
	CAutoLock locLock(m_pLock);
	//Need a lock ??
	ReleaseDelegate();
	return CBaseInputPin::BreakConnect();
}
HRESULT AbstractVideoDecodeInputPin::CompleteConnect (IPin *inReceivePin) {
	CAutoLock locLock(m_pLock);
	
	IMediaSeeking* locSeeker = NULL;
	inReceivePin->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	return CBaseInputPin::CompleteConnect(inReceivePin);
}
void AbstractVideoDecodeInputPin::ResetFrameCount() {
	mFrameCount = 0;
	
}
bool AbstractVideoDecodeInputPin::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(FALSE);
	return true;
}


STDMETHODIMP AbstractVideoDecodeInputPin::Receive(IMediaSample* inSample) {
	CAutoLock locLock(mStreamLock);
	//debugLog<<"Receive "<<endl;
	HRESULT locHR;
	BYTE* locBuff = NULL;
	locHR = inSample->GetPointer(&locBuff);


	if (FAILED(locHR)) {
		//debugLog<<"Receive : Get pointer failed..."<<locHR<<endl;	
		return locHR;
	} else {
		//debugLog<<"Receive : Get pointer succeeds..."<<endl;	
		//New start time hacks
		REFERENCE_TIME locStart = 0;
		REFERENCE_TIME locEnd = 0;
		inSample->GetTime(&locStart, &locEnd);
		//Error chacks needed here
		
		//More work arounds for that stupid granule pos scheme in theora!
		REFERENCE_TIME locTimeBase = 0;
		REFERENCE_TIME locDummy = 0;
		inSample->GetMediaTime(&locTimeBase, &locDummy);
		mSeekTimeBase = locTimeBase;
		//
		
		if ((mLastSeenStartGranPos != locStart) && (locStart != -1)) {
			ResetFrameCount();
			mLastSeenStartGranPos = locStart;
		}
		
		//End of additions



		AM_MEDIA_TYPE* locMediaType = NULL;
		inSample->GetMediaType(&locMediaType);
		if (locMediaType == NULL) {
			//debugLog<<"No dynamic change..."<<endl;
		} else {
			//debugLog<<"Attempting dynamic change..."<<endl;
		}
		//if (locMediaType != NULL) {
		
			//VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)locMediaType->pbFormat;
			//mHeight = 288; //locVideoHeader->bmiHeader.biHeight;
			//mWidth =  384; //locVideoHeader->bmiHeader.biWidth;
		//}

		
		long locResult = decodeData(locBuff, inSample->GetActualDataLength(), locStart, locEnd);
		if (locResult == 0) {
			//debugLog<<"Receive : Decode OK"<<endl;	
			return S_OK;
		} else {
			//debugLog<<"Receive : Decode OK"<<endl;	
			return S_FALSE;
		}
	}

	
	return S_OK;
}

HRESULT AbstractVideoDecodeInputPin::CheckMediaType(const CMediaType *inMediaType) {
	//FIX::: Clean this up !
	
	if	( (inMediaType->majortype == MEDIATYPE_Video) &&
			(inMediaType->subtype == mAcceptableMediaType->subtype) && (inMediaType->formattype == mAcceptableMediaType->formattype)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}

STDMETHODIMP AbstractVideoDecodeInputPin::EndOfStream(void) {
	CAutoLock locLock(mStreamLock);
	return mParentFilter->mOutputPin->DeliverEndOfStream();
}

STDMETHODIMP AbstractVideoDecodeInputPin::BeginFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::BeginFlush();
	return mParentFilter->mOutputPin->DeliverBeginFlush();
}
STDMETHODIMP AbstractVideoDecodeInputPin::EndFlush() {
	CAutoLock locLock(m_pLock);
	mParentFilter->mOutputPin->DeliverEndFlush();
	
	return CBaseInputPin::EndFlush();

}

STDMETHODIMP AbstractVideoDecodeInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	CAutoLock locLock(mStreamLock);
	//This breaks it for some reason... though something similar in the audio component works fine.
	CBasePin::NewSegment(tStart, tStop, dRate);
	return mParentFilter->mOutputPin->DeliverNewSegment(tStart, tStop, dRate);
}

HRESULT AbstractVideoDecodeInputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}

	switch (inPosition) {
		case 0:

			outMediaType->SetType(&MEDIATYPE_Video);
			outMediaType->SetSubtype(&(mAcceptableMediaType->subtype));
			//Don't set the format data here. That's up to our output pin/
			return S_OK;			
		default:
			return VFW_S_NO_MORE_ITEMS;
	}
}