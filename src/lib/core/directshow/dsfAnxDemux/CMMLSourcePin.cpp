#include "StdAfx.h"
#include "cmmlsourcepin.h"

CMMLSourcePin::CMMLSourcePin(	TCHAR* inObjectName, 
										OggDemuxSourceFilter* inParentFilter,
										CCritSec* inFilterLock,
										StreamHeaders* inHeaderSource, 
										CMediaType* inMediaType,
										wstring inPinName) 
											:	OggDemuxSourcePin(inObjectName, inParentFilter, inFilterLock, inHeaderSource, inMediaType, inPinName)
{
	
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