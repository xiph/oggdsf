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

#include "TheoraDecodeFilter.h"



//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Theora Decode Filter",					// Name
	    &CLSID_TheoraDecodeFilter,				// CLSID
	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



TheoraDecodeFilter::TheoraDecodeFilter() 
	:	CVideoTransformFilter( NAME("Theora Decode Filter"), NULL, CLSID_TheoraDecodeFilter)
{

}

TheoraDecodeFilter::~TheoraDecodeFilter() {


}

CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 


HRESULT TheoraDecodeFilter::CheckInputType(const CMediaType* inMediaType) {

}
HRESULT TheoraDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) {

}
HRESULT TheoraDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {

}
HRESULT TheoraDecodeFilter::GetMediaType(int inPosition, CMediaType* outOutputMediaType) {

}
HRESULT TheoraDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample) {

}



//---------------------------------------
//OLD IMPLOEMENTATION....
//---------------------------------------
//#include "theoradecodefilter.h"
//
//
//
////COM Factory Template
//CFactoryTemplate g_Templates[] = 
//{
//    { 
//		L"Theora Decode Filter",					// Name
//	    &CLSID_TheoraDecodeFilter,				// CLSID
//	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
//        NULL,									// Initialization function
//        NULL									// Set-up information (for filters)
//    }
//
//};
//
//// Generic way of determining the number of items in the template
//int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 
//
//
//
////Theora Decode Filter Implementation
////==================================
//TheoraDecodeFilter::TheoraDecodeFilter()
//	:	AbstractVideoDecodeFilter(NAME("Ogg Video Decoder"), CLSID_TheoraDecodeFilter, THEORA)
//	,	mTheoraFormatInfo(NULL)
//{
//
//	bool locWasConstructed = ConstructPins();
//}
//
//bool TheoraDecodeFilter::ConstructPins() 
//{
//	
//	//Output pin must be done first because it's passed to the input pin.
//	mOutputPin = new TheoraDecodeOutputPin(this, m_pLock);
//
//	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);
//	locAcceptMediaType->subtype = MEDIASUBTYPE_Theora;
//	locAcceptMediaType->formattype = FORMAT_Theora;
//	mInputPin = new TheoraDecodeInputPin(this, m_pLock, mOutputPin, locAcceptMediaType);
//	return true;
//}
//
//TheoraDecodeFilter::~TheoraDecodeFilter(void)
//{
//	//DestroyPins();
//	delete mTheoraFormatInfo;
//}
//
//CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
//{
//	//This routine is the COM implementation to create a new Filter
//	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();
//    if (pNewObject == NULL) {
//        *pHr = E_OUTOFMEMORY;
//    }
//	return pNewObject;
//} 
//
//STDMETHODIMP TheoraDecodeFilter::GetState(DWORD dw, FILTER_STATE *pState)
//{
//    CheckPointer(pState, E_POINTER);
//    *pState = m_State;
//	if (m_State == State_Paused) {
//        return VFW_S_CANT_CUE;
//	} else {
//        return S_OK;
//	}
//}
//
////QUERY::: Do we need these ? Aren't we all friedns here ??
////RESULT::: Keep them, set function must be kept... get could go... but keep for consistency
//sTheoraFormatBlock* TheoraDecodeFilter::getTheoraFormatBlock() 
//{
//	return mTheoraFormatInfo;
//}
//void TheoraDecodeFilter::setTheoraFormat(sTheoraFormatBlock* inFormatBlock) 
//{
//	delete mTheoraFormatInfo;
//	mTheoraFormatInfo = new sTheoraFormatBlock;
//	*mTheoraFormatInfo = *inFormatBlock;
//}