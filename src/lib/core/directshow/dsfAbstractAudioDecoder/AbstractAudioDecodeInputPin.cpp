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

#include "StdAfx.h"
#include "abstractaudiodecodeinputpin.h"

//#include <mtype.h>
AbstractAudioDecodeInputPin::AbstractAudioDecodeInputPin(AbstractAudioDecodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractAudioDecodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName, CMediaType* inAcceptMediaType)
	:	CBaseInputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName)
	,	mOutputPin(inOutputPin)
	,	mUptoFrame(0)
	,	mBegun(false)
	,	mParentFilter(inParentFilter)
	,	mFrameSize(0)
	,	mNumChannels(0)
	,	mSampleRate(0)
	,	mFilterLock(inFilterLock)
	,	mLastSeenStartGranPos(0)
	,	mSeekTimeBase(0)
{
	//ConstructCodec();
	//aadDebug.open("c:\\aadec.log", ios_base::out);
	mAcceptableMediaType = inAcceptMediaType;
	mStreamLock = new CCritSec;

	IMediaSeeking* locSeeker = NULL;
	this->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	mOutputPin->SetDelegate(locSeeker);
}

STDMETHODIMP AbstractAudioDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}
HRESULT AbstractAudioDecodeInputPin::CompleteConnect (IPin *inReceivePin) {
	CAutoLock locLock(mFilterLock);
	
	IMediaSeeking* locSeeker = NULL;
	inReceivePin->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	return CBaseInputPin::CompleteConnect(inReceivePin);
}
AbstractAudioDecodeInputPin::~AbstractAudioDecodeInputPin(void)
{
	//DestroyCodec();
	//aadDebug.close();
	delete mStreamLock;
}


void AbstractAudioDecodeInputPin::ResetFrameCount() {
	mUptoFrame = 0;
	
}
void AbstractAudioDecodeInputPin::ResetTimeBases() {
	mLastSeenStartGranPos = 0;
}
bool AbstractAudioDecodeInputPin::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(TRUE);
	return true;
}


STDMETHODIMP AbstractAudioDecodeInputPin::Receive(IMediaSample* inSample) 
{
	//TO DO::: Fix this up...
	CAutoLock locLock(mStreamLock);
	HRESULT locHR;
	BYTE* locBuff = NULL;
	locHR = inSample->GetPointer(&locBuff);

	if (FAILED(locHR)) {
		
		return locHR;
	} else {
		//New start time hacks
		REFERENCE_TIME locStart = 0;
		REFERENCE_TIME locEnd = 0;

		//More work arounds for that stupid granule pos scheme in theora!
		REFERENCE_TIME locTimeBase = 0;
		REFERENCE_TIME locDummy = 0;
		inSample->GetMediaTime(&locTimeBase, &locDummy);
		mSeekTimeBase = locTimeBase;
		//

		inSample->GetTime(&locStart, &locEnd);
		//Error chacks needed here
		//aadDebug<<"Receive : Start = "<<locStart<<endl;
		if ((mLastSeenStartGranPos != locStart) && (locStart != -1)) {
			ResetFrameCount();
		}
		mLastSeenStartGranPos = locStart;
		//End of additions
		
		long locResult = decodeData(locBuff, inSample->GetActualDataLength());
		if (locResult == 0) {

			//aadDebug<<"Receive Decode : OK"<<endl;
			return S_OK;
		} else {
			//aadDebug<<"Receive Decode : *** FAILED *** "<<locResult<<endl;
			return S_FALSE;
		}
	}

	
	return S_OK;
}

HRESULT AbstractAudioDecodeInputPin::CheckMediaType(const CMediaType *inMediaType) {
	//TO DO::: Neaten this up.	
	if	( (inMediaType->majortype == MEDIATYPE_Audio) &&
			(inMediaType->subtype == mAcceptableMediaType->subtype) && (inMediaType->formattype == mAcceptableMediaType->formattype)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}

STDMETHODIMP AbstractAudioDecodeInputPin::EndOfStream(void) {
	CAutoLock locLock(mStreamLock);
	
	return mParentFilter->mOutputPin->DeliverEndOfStream();
}

STDMETHODIMP AbstractAudioDecodeInputPin::BeginFlush() {
	CAutoLock locLock(mFilterLock);
	CBaseInputPin::BeginFlush();
	return mParentFilter->mOutputPin->DeliverBeginFlush();
}
STDMETHODIMP AbstractAudioDecodeInputPin::EndFlush() {
	CAutoLock locLock(mFilterLock);
	CBaseInputPin::EndFlush();
	return mParentFilter->mOutputPin->DeliverEndFlush();
}

STDMETHODIMP AbstractAudioDecodeInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	CAutoLock locLock(mStreamLock);
	ResetFrameCount();
	mParentFilter->SetStartToNow();
	CBasePin::NewSegment(tStart, tStop, dRate);
	return mParentFilter->mOutputPin->DeliverNewSegment(tStart, tStop, dRate);
}

HRESULT AbstractAudioDecodeInputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}

	switch (inPosition) {
		case 0:

			outMediaType->SetType(&MEDIATYPE_Audio);
			outMediaType->SetSubtype(&(mAcceptableMediaType->subtype));
			//Don't set the format data here. That's up to our output pin/
			return S_OK;			
		default:
			return VFW_S_NO_MORE_ITEMS;
	}
}