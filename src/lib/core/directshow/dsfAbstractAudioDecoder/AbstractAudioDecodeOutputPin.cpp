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
#include "abstractaudiodecodeoutputpin.h"

AbstractAudioDecodeOutputPin::AbstractAudioDecodeOutputPin(AbstractAudioDecodeFilter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBaseOutputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName)
	,	mParentFilter(inParentFilter)
	,	mFilterLock(inFilterLock)
	,	mDataQueue(NULL)
{
	
}
AbstractAudioDecodeOutputPin::~AbstractAudioDecodeOutputPin(void)
{	
	ReleaseDelegate();
	delete mDataQueue;
	mDataQueue = NULL;
}

STDMETHODIMP AbstractAudioDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT AbstractAudioDecodeOutputPin::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
		//FIX::: Abstract this out properly	

	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	const unsigned long MIN_BUFFER_SIZE = 1096;			
	const unsigned long DEFAULT_BUFFER_SIZE = 32192;
	const unsigned long MIN_NUM_BUFFERS = 10;
	const unsigned long DEFAULT_NUM_BUFFERS = 20;

	
	
	
	if (inPropertyRequest->cbAlign <= 0) {
		locReqAlloc.cbAlign = 1;
	} else {
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}

	
	if (inPropertyRequest->cbBuffer < MIN_BUFFER_SIZE) {
		locReqAlloc.cbBuffer = DEFAULT_BUFFER_SIZE;
	} else {
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}

	
	if (inPropertyRequest->cbPrefix < 0) {
			locReqAlloc.cbPrefix = 0;
	} else {
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}

	
	if (inPropertyRequest->cBuffers < MIN_NUM_BUFFERS) {
		locReqAlloc.cBuffers = DEFAULT_NUM_BUFFERS;
	} else {
		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		return locHR;
	}

	//Need to save this pointer to decommit in destructor.
	
	locHR = inAllocator->Commit();

	return locHR;
}
HRESULT AbstractAudioDecodeOutputPin::CheckMediaType(const CMediaType *inMediaType) {
	if (inMediaType->majortype == MEDIATYPE_Audio && inMediaType->subtype == MEDIASUBTYPE_PCM && inMediaType->formattype == FORMAT_WaveFormatEx) {
		return S_OK;
	} else {
		return S_FALSE;
	}
	
}

void AbstractAudioDecodeOutputPin::FillMediaType(CMediaType* outMediaType) {
	outMediaType->SetType(&MEDIATYPE_Audio);
	outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
	outMediaType->SetFormatType(&FORMAT_WaveFormatEx);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(0);

}
HRESULT AbstractAudioDecodeOutputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if (inPosition == 0) {
		FillMediaType(outMediaType);
		WAVEFORMATEX* locWaveFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
		FillWaveFormatExBuffer(locWaveFormat);
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}

HRESULT AbstractAudioDecodeOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	//TO DO::: Change the naming convention of the params
	mDataQueue->NewSegment(tStart, tStop, dRate);
	return S_OK;
}
HRESULT AbstractAudioDecodeOutputPin::DeliverEndOfStream(void) {
	//QUERY::: I think we need a lock here !
	mDataQueue->EOS();
    return S_OK;
}

HRESULT AbstractAudioDecodeOutputPin::DeliverEndFlush(void) {
	//QUERY::: Locks ??
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT AbstractAudioDecodeOutputPin::DeliverBeginFlush(void) {
	//QUERY:: Locks ???
	mDataQueue->BeginFlush();
    return S_OK;
	
}

HRESULT AbstractAudioDecodeOutputPin::CompleteConnect (IPin *inReceivePin) {
	HRESULT locHR = S_OK;

	//Here when another pin connects to us, we internally connect the seek delegate
	// from this output pin onto the input pin... and we release it on breakconnect.
	//
	IMediaSeeking* locSeeker = NULL;
	mParentFilter->mInputPin->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	//
	mDataQueue = new COutputQueue (inReceivePin, &locHR, FALSE, TRUE, 1, TRUE, 20);
	if (FAILED(locHR)) {
		locHR = locHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT AbstractAudioDecodeOutputPin::BreakConnect(void) {

	ReleaseDelegate();
	delete mDataQueue;
	mDataQueue = NULL;
	return CBaseOutputPin::BreakConnect();
}