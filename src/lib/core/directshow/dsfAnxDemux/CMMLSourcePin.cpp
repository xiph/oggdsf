//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
#include "cmmlsourcepin.h"

CMMLSourcePin::CMMLSourcePin(	TCHAR* inObjectName, 
										OggDemuxSourceFilter* inParentFilter,
										CCritSec* inFilterLock,
										StreamHeaders* inHeaderSource, 
										CMediaType* inMediaType,
										wstring inPinName,
										unsigned long inNumBuffers,
										unsigned long inBufferSize) 
		:	OggDemuxSourcePin(inObjectName, inParentFilter, inFilterLock, inHeaderSource, inMediaType, inPinName, true, inNumBuffers, inBufferSize)
{
	//debugLog.open("G:\\logs\\cmml_source_pin.log", ios_base::out);
}

CMMLSourcePin::~CMMLSourcePin(void)
{
}


STDMETHODIMP CMMLSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}
STDMETHODIMP CMMLSourcePin::Render(IPin* inOutputPin, IGraphBuilder* inGraphBuilder) {
	if (inOutputPin == NULL || inGraphBuilder == NULL) {
		return E_POINTER;
	}

	
    IBaseFilter* locCMMLFilter = NULL;



    HRESULT locHR = S_OK;
	//locHR = inGraphBuilder->FindFilterByName(L"CMML Decode Filter", &locRenderer);
	if (locCMMLFilter == NULL) {
		locHR= CoCreateInstance(CLSID_CMMLDecodeFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locCMMLFilter);
		if (locHR != S_OK) {
	        return locHR;
	    }
    
	    locHR = inGraphBuilder->AddFilter(locCMMLFilter, L"CMML Decode Filter");
	    if (locHR != S_OK) {
			locCMMLFilter->Release();
			return locHR;
		}
	}

	IEnumPins* locEnumPins;
	IPin* locCMMLInputPin = NULL;
	locHR = locCMMLFilter->EnumPins(&locEnumPins);

	if (SUCCEEDED(locHR)) {
		if (S_OK != locEnumPins->Next(1, &locCMMLInputPin, 0)) {
			locHR = E_UNEXPECTED;
		}
	}

	if (SUCCEEDED(locHR)) {
		// CConnect VMR9 to the output of the theora decoder
		//CMediaType* locMediaType = new CMediaType;
		//
		//FillMediaType(locMediaType);
		////Fixes the null format block error
		//VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)locMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		//FillVideoInfoHeader(locVideoFormat);
		locHR = inGraphBuilder->ConnectDirect(inOutputPin, locCMMLInputPin, mMediaType);
		locCMMLInputPin->Release();
	}
	if (FAILED(locHR)) 
	{
		
		inGraphBuilder->RemoveFilter(locCMMLFilter);
	}
	locCMMLFilter->Release();
	
    return locHR;


}
STDMETHODIMP CMMLSourcePin::Backout(IPin* inOutputPin, IGraphBuilder* inGraphBuilder) {
   //HRESULT locHR = S_OK;
   // if (m_Connected != NULL) // Pointer to the pin we're connected to.
   // {
   //     // Find the filter that owns the pin connected to us.
   //     FILTER_INFO fi;
   //     hr = m_Connected->QueryFilterInfo(&fi);
   //     if (SUCCEEDED(hr)) 
   //     {
   //         if (fi.pFilter != NULL) 
   //         {
   //             //  Disconnect the pins.
   //             pGraph->Disconnect(m_Connected);
   //             pGraph->Disconnect(pPin);
   //             // Remove the filter from the graph.
   //             pGraph->RemoveFilter(fi.pFilter);
   //             fi.pFilter->Release();
   //         } 
   //         else 
   //         {
   //             hr = E_UNEXPECTED;
   //         }
   //     }
   // }
   // return hr;
	return S_OK;
}

bool CMMLSourcePin::deliverOggPacket(StampedOggPacket* inPacket) {

	//Modified from the base class so that the times are multiplied by a scaling factor... curently 10000.
	//TODO::: When you properly save annodex header information, you need to account for other time schemes.
	//Probably issues for -1 gran pos here too.
	CAutoLock locStreamLock(mParentFilter->mStreamLock);
	IMediaSample* locSample = NULL;
	REFERENCE_TIME locStart = inPacket->endTime() * 10000;   //CMML Changes here.
	REFERENCE_TIME locStop = inPacket->endTime() * 10000;
	//debugLog<<"Start   : "<<locStart<<endl;
	//debugLog<<"End     : "<<locStop<<endl;
	DbgLog((LOG_TRACE, 2, "Getting Buffer in Source Pin..."));
	//DbgLog((LOG_TRACE, 2, ""));

	HRESULT	locHR = GetDeliveryBuffer(&locSample, &locStart, &locStop, NULL);
	DbgLog((LOG_TRACE, 2, "* After get Buffer in Source Pin..."));
	//Error checks
	if (locHR != S_OK) {
		DbgLog((LOG_TRACE, 2, "Getting Delivery Buff FAILED"));
		//Stopping, fluching or error
		//debugLog<<"Failure... No buffer"<<endl;
		return false;
	}

	//More hacks so we can send a timebase after a seek, since granule pos in theora
	// is not convertible in both directions to time.
	
	//TIMESTAMP FIXING !
	locSample->SetTime(&locStart, &locStop);
	
	//Yes this is way dodgy !
	locSample->SetMediaTime(&mParentFilter->mSeekTimeBase, &mParentFilter->mSeekTimeBase);
	locSample->SetSyncPoint(TRUE);
	

	// Create a pointer for the samples buffer
	BYTE* locBuffer = NULL;
	locSample->GetPointer(&locBuffer);

	if (locSample->GetSize() >= inPacket->packetSize()) {

		memcpy((void*)locBuffer, (const void*)inPacket->packetData(), inPacket->packetSize());
		locSample->SetActualDataLength(inPacket->packetSize());

		locHR = mDataQueue->Receive(locSample);
		
		if (locHR != S_OK) {
			//debugLog << "Failure... Queue rejected sample..."<<endl;
			//Stopping ??
			return false;
			
		} else {
			//debugLog<<"Delivery OK"<<endl;
			return true;
		}
	} else {
		throw 0;
	}
}