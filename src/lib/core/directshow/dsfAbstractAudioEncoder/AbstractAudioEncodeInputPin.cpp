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
#include "abstractaudioencodeinputpin.h"

//#include <mtype.h>
AbstractAudioEncodeInputPin::AbstractAudioEncodeInputPin(AbstractAudioEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractAudioEncodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBaseInputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName),
		mOutputPin(inOutputPin),
		mUptoFrame(0),
		mBegun(false),
		mParentFilter(inParentFilter)
	
{
	//ConstructCodec();
	
}

AbstractAudioEncodeInputPin::~AbstractAudioEncodeInputPin(void)
{
	//DestroyCodec();
}


void AbstractAudioEncodeInputPin::ResetFrameCount() {
	mUptoFrame = 0;
	
}
bool AbstractAudioEncodeInputPin::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(TRUE);
	return true;
}


STDMETHODIMP AbstractAudioEncodeInputPin::Receive(IMediaSample* inSample) {

	HRESULT locHR;
	BYTE* locBuff = NULL;
	locHR = inSample->GetPointer(&locBuff);

	if (FAILED(locHR)) {
		
		return locHR;
	} else {
		
		long locResult = encodeData(locBuff, inSample->GetActualDataLength());
		if (locResult == 0) {
			return S_OK;
		} else {
			return S_FALSE;
		}
	}

	
	return S_OK;
}

HRESULT AbstractAudioEncodeInputPin::CheckMediaType(const CMediaType *inMediaType) {
	//FIX::: Clean this up !
	
	if	( (inMediaType->majortype == MEDIATYPE_Audio) &&
			(inMediaType->subtype == MEDIASUBTYPE_PCM) &&
			(inMediaType->formattype == FORMAT_WaveFormatEx)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}

STDMETHODIMP AbstractAudioEncodeInputPin::EndOfStream(void) {
	return mParentFilter->mOutputPin->DeliverEndOfStream();
}

STDMETHODIMP AbstractAudioEncodeInputPin::BeginFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::BeginFlush();
	return mParentFilter->mOutputPin->DeliverBeginFlush();
}
STDMETHODIMP AbstractAudioEncodeInputPin::EndFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::EndFlush();
	return mParentFilter->mOutputPin->DeliverEndFlush();

}

STDMETHODIMP AbstractAudioEncodeInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	return mParentFilter->mOutputPin->DeliverNewSegment(tStart, tStop, dRate);
}

HRESULT AbstractAudioEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.

	if (inMediaType->subtype == MEDIASUBTYPE_PCM) {
		mWaveFormat = (WAVEFORMATEX*)inMediaType->pbFormat;
		//mParentFilter->mAudioFormat = AbstractAudioDecodeFilter::VORBIS;
	} else {
		//Failed... should never be here !
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}

HRESULT AbstractAudioEncodeInputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}

	switch (inPosition) {
		case 0:

			outMediaType->SetType(&MEDIATYPE_Audio);
			outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
			//Don't set the format data here. That's up to our output pin/
			return S_OK;			
		default:
			return VFW_S_NO_MORE_ITEMS;
	}
}