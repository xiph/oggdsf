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
#include "abstractvideoencodeinputpin.h"

AbstractVideoEncodeInputPin::AbstractVideoEncodeInputPin(AbstractVideoEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoEncodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBaseInputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName),
		mOutputPin(inOutputPin),
		mUptoFrame(0),
		mBegun(false),
		mParentFilter(inParentFilter)
	,	mHeight(0)
	,	mWidth(0)
	
{
	//debugLog.open("C:\\temp\\aaein.log", ios_base::out);
	//ConstructCodec();
	
}

AbstractVideoEncodeInputPin::~AbstractVideoEncodeInputPin(void)
{
	//debugLog.close();
	//DestroyCodec();
}


void AbstractVideoEncodeInputPin::ResetFrameCount() {
	mUptoFrame = 0;
	
}
bool AbstractVideoEncodeInputPin::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(TRUE);
	return true;
}


STDMETHODIMP AbstractVideoEncodeInputPin::Receive(IMediaSample* inSample) {

	//debugLog <<endl<< "Received sample..."<<endl;
	HRESULT locHR;
	BYTE* locBuff = NULL;
	locHR = inSample->GetPointer(&locBuff);

	if (FAILED(locHR)) {
		//debugLog << "Failed to get pointer... bailing out"<<endl;
		return locHR;
	} else {
		
		long locResult = encodeData(locBuff, inSample->GetActualDataLength());
		if (locResult >= 0) {
			//debugLog << "Encode Data returns 0... OK"<<endl;
			return S_OK;
		} else {
			//debugLog<< "Encode Data returns "<<locResult<<" FAILURE"<<endl;
			return S_FALSE;
		}
	}

	//debugLog<<"Receive falls through... returning OK"<<endl;
	return S_OK;
}

HRESULT AbstractVideoEncodeInputPin::CheckMediaType(const CMediaType *inMediaType) {
	//FIX::: Clean this up !
	
	if	( (inMediaType->majortype == MEDIATYPE_Video) &&
			(inMediaType->subtype == MEDIASUBTYPE_YV12) &&
			(inMediaType->formattype == FORMAT_VideoInfo)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}

STDMETHODIMP AbstractVideoEncodeInputPin::EndOfStream(void) {
	return mParentFilter->mOutputPin->DeliverEndOfStream();
}

STDMETHODIMP AbstractVideoEncodeInputPin::BeginFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::BeginFlush();
	return mParentFilter->mOutputPin->DeliverBeginFlush();
}
STDMETHODIMP AbstractVideoEncodeInputPin::EndFlush() {
	CAutoLock locLock(m_pLock);
	CBaseInputPin::EndFlush();
	return mParentFilter->mOutputPin->DeliverEndFlush();

}

STDMETHODIMP AbstractVideoEncodeInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	return mParentFilter->mOutputPin->DeliverNewSegment(tStart, tStop, dRate);
}

HRESULT AbstractVideoEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.

	if (inMediaType->subtype == MEDIASUBTYPE_YV12) {
		mVideoFormat = (VIDEOINFOHEADER*)inMediaType->pbFormat;
		//mParentFilter->mAudioFormat = AbstractAudioDecodeFilter::VORBIS;
	} else {
		//Failed... should never be here !
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}

HRESULT AbstractVideoEncodeInputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}

	switch (inPosition) {
		case 0:

			outMediaType->SetType(&MEDIATYPE_Video);
			outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
			//Don't set the format data here. That's up to our output pin/
			return S_OK;			
		default:
			return VFW_S_NO_MORE_ITEMS;
	}
}