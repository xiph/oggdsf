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
	//debugLog.open("g:\\logs\\aad.log", ios_base::out);
	mAcceptableMediaType = inAcceptMediaType;
	mStreamLock = new CCritSec;

	//This is causing a problem... since every addref on a pin automatically
	// adds a ref to the filter... we get the situation, where on shutdown
	// the output pin still hold a ref on the input pin due to this bit of code
	// So at shutdown, 
	// the input pin has a ref count of 1
	// the output pin has a ref count of 0 (it's released by the downstream filter)
	// the filter has a ref count of 1 by way of the automatic addref from the input pin
	// This means that even when everything else releases all it's refs on the filter
	// it still has a ref count of 1... and since currently the ref that the
	// output pin holds on the input pin isn't release until the output pin is
	// destroyed, and the output pin isn't destroyed until the filter is
	// we get a circular reference.
	//
	//New solution is to attach this reference (from output to input) on the
	// complete connect method of the output pin via mParentfilter
	// and to release it when on the break conncet of the output pin.
	// This means that now as soon as the downstream filter releases the output
	// pin, it will release it's ref on the input pin, leaving the pins and the filer with
	// zero ref counts... well thats the plan anyway.
	//
	//IMediaSeeking* locSeeker = NULL;
	//this->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	//mOutputPin->SetDelegate(locSeeker);
	//
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

HRESULT AbstractAudioDecodeInputPin::BreakConnect() {
	CAutoLock locLock(mFilterLock);
	//Need a lock ??
	ReleaseDelegate();
	return CBaseInputPin::BreakConnect();
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
	//debugLog.close();
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
	//
	//inSample->AddRef();
	//debugLog<<"Received Sample with refcount = "<<inSample->Release()<<endl;
	//
	//TO DO::: Fix this up...
	CAutoLock locLock(mStreamLock);
	HRESULT locHR = CheckStreaming();
	if (locHR == S_OK) {
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
			//debugLog<<"Receive : Start    = "<<locStart<<endl;
			//debugLog<<"Receive : End      = "<<locEnd<<endl;
			//debugLog<<"Receive : Timebase = "<<locTimeBase<<endl;
			
			if ((mLastSeenStartGranPos != locStart) && (locStart != -1)) {
				//debugLog<<"Receive : RESETTING FRAME COUNT !!"<<endl;
				ResetFrameCount();
			}
			//debugLog<<endl;
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
	} else {
		//debugLog<<"NOT STREAMING.... "<<endl;
		return locHR;
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
	mParentFilter->mOutputPin->DeliverEndFlush();
	
	return CBaseInputPin::EndFlush();
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