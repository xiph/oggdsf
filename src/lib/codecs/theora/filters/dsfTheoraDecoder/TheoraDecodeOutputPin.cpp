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
#include "theoradecodeoutputpin.h"



TheoraDecodeOutputPin::TheoraDecodeOutputPin(CTransformFilter* inParentFilter, HRESULT* outHR) 
	:	CTransformOutputPin(NAME("Theora Output Pin"), inParentFilter, outHR, L"YV12 Out")
{
	//debugLog.open("G:\\logs\\theooutput.log", ios_base::out);
}
TheoraDecodeOutputPin::~TheoraDecodeOutputPin() {
	//debugLog.close();
}

STDMETHODIMP TheoraDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	//debugLog<<"Querying interface"<<endl;
	if (riid == IID_IMediaSeeking) {
		//debugLog<<"Got seekeer"<<endl;
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		
		return NOERROR;
	} else if (riid == IID_IMediaPosition) {
		//debugLog<<"Asking for OLD SEEKER"<<endl;
	}
	//debugLog<<"Trying base output pin"<<endl;
	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT TheoraDecodeOutputPin::BreakConnect() {
	CAutoLock locLock(m_pLock);
	//Need a lock ??
	ReleaseDelegate();
	//debugLog<<"Break connect"<<endl;
	return CTransformOutputPin::BreakConnect();
}
HRESULT TheoraDecodeOutputPin::CompleteConnect (IPin *inReceivePin) {
	CAutoLock locLock(m_pLock);
	//debugLog<<"Complete connect"<<endl;
	IMediaSeeking* locSeeker = NULL;

	m_pFilter->GetPin(0)->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);

	if (locSeeker == NULL) {
		//debugLog<<"Seeker was NULL"<<endl;
	}
	SetDelegate(locSeeker);
	return CTransformOutputPin::CompleteConnect(inReceivePin);
}

STDMETHODIMP TheoraDecodeOutputPin::Notify(IBaseFilter* inMessageSource, Quality inQuality) {
	return E_NOTIMPL;
}
//----------------------
//OLD IMPLEMENTATION
//----------------------
//TheoraDecodeOutputPin::TheoraDecodeOutputPin(TheoraDecodeFilter* inParentFilter, CCritSec* inFilterLock)
//	: AbstractVideoDecodeOutputPin(inParentFilter, inFilterLock,NAME("TheoraDecodeOutputPin"), L"YV12 Out")
//{
//
//		
//}
//TheoraDecodeOutputPin::~TheoraDecodeOutputPin(void)
//{
//	
//	
//}
//STDMETHODIMP TheoraDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
//{
//	if (riid == IID_IMediaSeeking) {
//		*ppv = (IMediaSeeking*)this;
//		((IUnknown*)*ppv)->AddRef();
//		return NOERROR;
//	//} else if (riid == IID_IStreamBuilder) {
//	//	*ppv = (IStreamBuilder*)this;
//	//	((IUnknown*)*ppv)->AddRef();
//	//	return NOERROR;
//	}
//
//	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
//}
//
//
//STDMETHODIMP TheoraDecodeOutputPin::Notify(IBaseFilter *pSelf, Quality q) {
//	return S_OK;
//
//}
//
//bool TheoraDecodeOutputPin::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer) {
//	TheoraDecodeFilter* locFilter = ((TheoraDecodeFilter*)m_pFilter);
//
//	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / locFilter->mTheoraFormatInfo->frameRateNumerator;
//	inFormatBuffer->dwBitRate = locFilter->mTheoraFormatInfo->targetBitrate;
//	
//	inFormatBuffer->bmiHeader.biBitCount = 12;   //12 bits per pixel
//	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
//	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth
//	inFormatBuffer->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
//	inFormatBuffer->bmiHeader.biHeight = locFilter->mTheoraFormatInfo->frameHeight;   //Not sure
//	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
//	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
//	inFormatBuffer->bmiHeader.biSizeImage = ((locFilter->mTheoraFormatInfo->frameHeight * locFilter->mTheoraFormatInfo->frameWidth) * 3)/2;    //Size in bytes of image ??
//	inFormatBuffer->bmiHeader.biWidth = locFilter->mTheoraFormatInfo->frameWidth;
//	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
//	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
//	
//	inFormatBuffer->rcSource.top = 0;
//	inFormatBuffer->rcSource.bottom = locFilter->mTheoraFormatInfo->frameHeight;
//	inFormatBuffer->rcSource.left = 0;
//	inFormatBuffer->rcSource.right = locFilter->mTheoraFormatInfo->frameWidth;
//
//	inFormatBuffer->rcTarget.top = 0;
//	inFormatBuffer->rcTarget.bottom = locFilter->mTheoraFormatInfo->frameHeight;
//	inFormatBuffer->rcTarget.left = 0;
//	inFormatBuffer->rcTarget.right = locFilter->mTheoraFormatInfo->frameWidth;
//
//	inFormatBuffer->dwBitErrorRate=0;
//	return true;
//}
//
////STDMETHODIMP TheoraDecodeOutputPin::Render(IPin* inOutputPin, IGraphBuilder* inGraphBuilder) {
////	if (inOutputPin == NULL || inGraphBuilder == NULL) {
////		return E_POINTER;
////	}
////
////	
////    IBaseFilter* locRenderer = NULL;
////
////    // Create the VMR9
////	//FIX::: Probably should check if it's already in the graph.
////	//  Also should let it connect to other VMR's
////
////
////    HRESULT locHR = S_OK;
////	locHR = inGraphBuilder->FindFilterByName(L"Video Mixing Renderer", &locRenderer);
////	/*IEnumFilters* locEnumFilters = NULL;
////	locHR = S_OK;
////	locHR = inGraphBuilder->EnumFilters(&locEnumFilters);
////	IBaseFilter* locFilter = NULL;
////	CLSID locCLSID;
////	while (locHR ==S_OK) {
////	locHR = locEnumFilters->Next(1, &locFilter, 0);
////			locFilter->GetClassID(&locCLSID);
////			if (locCLSID == CLSID_VideoMixingRenderer9) {
////				locHR=locHR;
////			}
////	}*/
////
////	if (locRenderer == NULL) {
////		locHR= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locRenderer);
////		if (locHR != S_OK) {
////	        return locHR;
////	    }
////    
////	    //Add the VMR9 to the graph
////	    locHR = inGraphBuilder->AddFilter(locRenderer, L"Video Mixing Renderer");
////	    if (locHR != S_OK) {
////			locRenderer->Release();
////			return locHR;
////		}
////		IVMRFilterConfig9* locVMRConfig = NULL;
////		locHR = locRenderer->QueryInterface(IID_IVMRFilterConfig9, (void**)&locVMRConfig);
////		if(locHR == S_OK) {
////			//locHR = locVMRConfig->SetRenderingMode(VMR9Mode_Renderless);
////		}
////	}
////
////	IEnumPins* locEnumPins;
////	IPin* locRendererInputPin = NULL;
////	locHR = locRenderer->EnumPins(&locEnumPins);
////
////	if (SUCCEEDED(locHR)) {
////		if (S_OK != locEnumPins->Next(1, &locRendererInputPin, 0)) {
////			locHR = E_UNEXPECTED;
////		}
////	}
////
////	if (SUCCEEDED(locHR)) {
////		// CConnect VMR9 to the output of the theora decoder
////		CMediaType* locMediaType = new CMediaType;
////		
////		FillMediaType(locMediaType);
////		//Fixes the null format block error
////		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)locMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
////		FillVideoInfoHeader(locVideoFormat);
////		locHR = inGraphBuilder->ConnectDirect(inOutputPin, locRendererInputPin, locMediaType);
////		locRendererInputPin->Release();
////	}
////	if (FAILED(locHR)) 
////	{
////		// Could not connect to the VMR9 renderer. Remove it from the graph.
////		inGraphBuilder->RemoveFilter(locRenderer);
////	}
////	locRenderer->Release();
////	
////    return locHR;
////
////
////}
////STDMETHODIMP TheoraDecodeOutputPin::Backout(IPin* inOutputPin, IGraphBuilder* inGraphBuilder) {
////   //HRESULT locHR = S_OK;
////   // if (m_Connected != NULL) // Pointer to the pin we're connected to.
////   // {
////   //     // Find the filter that owns the pin connected to us.
////   //     FILTER_INFO fi;
////   //     hr = m_Connected->QueryFilterInfo(&fi);
////   //     if (SUCCEEDED(hr)) 
////   //     {
////   //         if (fi.pFilter != NULL) 
////   //         {
////   //             //  Disconnect the pins.
////   //             pGraph->Disconnect(m_Connected);
////   //             pGraph->Disconnect(pPin);
////   //             // Remove the filter from the graph.
////   //             pGraph->RemoveFilter(fi.pFilter);
////   //             fi.pFilter->Release();
////   //         } 
////   //         else 
////   //         {
////   //             hr = E_UNEXPECTED;
////   //         }
////   //     }
////   // }
////   // return hr;
////	return S_OK;
////}