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
	debugLog.open("G:\\logs\\newtheofilter.log", ios_base::out);
}

TheoraDecodeFilter::~TheoraDecodeFilter() {
	debugLog.close();

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
void TheoraDecodeFilter::FillMediaType(CMediaType* outMediaType, unsigned long inSampleSize) {
	outMediaType->SetType(&MEDIATYPE_Video);
	outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
	outMediaType->SetFormatType(&FORMAT_VideoInfo);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(inSampleSize);		

}
bool TheoraDecodeFilter::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer) {
	TheoraDecodeFilter* locFilter = this;

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / locFilter->mTheoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->mTheoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = 12;   //12 bits per pixel
	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth
	inFormatBuffer->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
	inFormatBuffer->bmiHeader.biHeight = locFilter->mTheoraFormatInfo->frameHeight;   //Not sure
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	inFormatBuffer->bmiHeader.biSizeImage = ((locFilter->mTheoraFormatInfo->frameHeight * locFilter->mTheoraFormatInfo->frameWidth) * 3)/2;    //Size in bytes of image ??
	inFormatBuffer->bmiHeader.biWidth = locFilter->mTheoraFormatInfo->frameWidth;
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->mTheoraFormatInfo->frameHeight;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->mTheoraFormatInfo->frameWidth;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->mTheoraFormatInfo->frameHeight;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->mTheoraFormatInfo->frameWidth;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}

HRESULT TheoraDecodeFilter::CheckInputType(const CMediaType* inMediaType) {
	
	if	( (inMediaType->majortype == MEDIATYPE_Video) &&
			(inMediaType->subtype == MEDIASUBTYPE_Theora) && (inMediaType->formattype == FORMAT_Theora)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}
HRESULT TheoraDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) {
	return S_OK;
}
HRESULT TheoraDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
	debugLog<<endl;	debugLog<<"DecideBufferSize :"<<endl;	//FIX::: Abstract this out properly	

	debugLog<<"Allocator is "<<(unsigned long)inAllocator<<endl;
	//Our error variable
	HRESULT locHR = S_OK;

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	debugLog<<"DecideBufferSize : Requested :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<inPropertyRequest->cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<inPropertyRequest->cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<inPropertyRequest->cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<inPropertyRequest->cBuffers<<endl;


	const unsigned long MIN_BUFFER_SIZE = 16*16;			//What should this be ????
	const unsigned long DEFAULT_BUFFER_SIZE = 1024*1024;
	const unsigned long MIN_NUM_BUFFERS = 1;
	const unsigned long DEFAULT_NUM_BUFFERS = 1;

	
	//Validate and change what we have been requested to do.
	//Allignment of data
	if (inPropertyRequest->cbAlign <= 0) {
		locReqAlloc.cbAlign = 1;
	} else {
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}

	//Size of each buffer
	if (inPropertyRequest->cbBuffer < MIN_BUFFER_SIZE) {
		locReqAlloc.cbBuffer = DEFAULT_BUFFER_SIZE;
	} else {
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}

	//How many prefeixed bytes
	if (inPropertyRequest->cbPrefix < 0) {
			locReqAlloc.cbPrefix = 0;
	} else {
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}

	//Number of buffers in the allcoator
	if (inPropertyRequest->cBuffers < MIN_NUM_BUFFERS) {
		locReqAlloc.cBuffers = DEFAULT_NUM_BUFFERS;
	} else {

		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	debugLog<<"DecideBufferSize : Modified Request :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<locReqAlloc.cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<locReqAlloc.cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<locReqAlloc.cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<locReqAlloc.cBuffers<<endl;


	//Set the properties in the allocator
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	debugLog<<"DecideBufferSize : SetProperties returns "<<locHR<<endl;
	debugLog<<"DecideBufferSize : Actual Params :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<locActualAlloc.cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<locActualAlloc.cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<locActualAlloc.cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<locActualAlloc.cBuffers<<endl;

	//Check the response
	switch (locHR) {
		case E_POINTER:
			debugLog<<"DecideBufferSize : SetProperties - NULL POINTER"<<endl;
			return locHR;
			

		case VFW_E_ALREADY_COMMITTED:
			debugLog<<"DecideBufferSize : SetProperties - Already COMMITED"<<endl;
			return locHR;
			
		case VFW_E_BADALIGN:
			debugLog<<"DecideBufferSize : SetProperties - Bad ALIGN"<<endl;
			return locHR;
			
		case VFW_E_BUFFERS_OUTSTANDING:
			debugLog<<"DecideBufferSize : SetProperties - BUFFS OUTSTANDING"<<endl;
			return locHR;
			

		case S_OK:

			break;
		default:
			debugLog<<"DecideBufferSize : SetProperties - UNKNOWN ERROR"<<endl;
			break;

	}

	
	//TO DO::: Do we commit ?
	//RESOLVED ::: Yep !
	
	locHR = inAllocator->Commit();
	debugLog<<"DecideBufferSize : Commit Returned "<<locHR<<endl;


	switch (locHR) {
		case E_FAIL:
			debugLog<<"DecideBufferSize : Commit - FAILED "<<endl;
			return locHR;
		case E_POINTER:
			debugLog<<"DecideBufferSize : Commit - NULL POINTER "<<endl;
			return locHR;
		case E_INVALIDARG:
			debugLog<<"DecideBufferSize : Commit - INVALID ARG "<<endl;
			return locHR;
		case E_NOTIMPL:
			debugLog<<"DecideBufferSize : Commit - NOT IMPL"<<endl;
			return locHR;
		case S_OK:
			debugLog<<"DecideBufferSize : Commit - ** SUCCESS **"<<endl;
			break;
		default:
			debugLog<<"DecideBufferSize : Commit - UNKNOWN ERROR "<<endl;
			return locHR;
	}


	return S_OK;
}
HRESULT TheoraDecodeFilter::GetMediaType(int inPosition, CMediaType* outOutputMediaType) {
	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if (inPosition == 0) {
		
		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		FillVideoInfoHeader(locVideoFormat);
		FillMediaType(outOutputMediaType, locVideoFormat->bmiHeader.biSizeImage);
		debugLog<<"Vid format size "<<locVideoFormat->bmiHeader.biSizeImage<<endl;
		//outMediaType->SetSampleSize(locVideoFormat->bmiHeader.biSizeImage);
		debugLog<<"Returning from GetMediaType"<<endl;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT TheoraDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample) {

	return S_OK;
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