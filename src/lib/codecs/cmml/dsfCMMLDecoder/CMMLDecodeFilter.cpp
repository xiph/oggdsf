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

#include "StdAfx.h"

#include "cmmldecodefilter.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"CMML Decode Filter",					// Name
	    &CLSID_CMMLDecodeFilter,				// CLSID
	    CMMLDecodeFilter::CreateInstance,		// Method to create an instance of Speex Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

//-----------------------------------------------------------
CMMLDecodeFilter::CMMLDecodeFilter(void)
	:	CTransformFilter(NAME("CMML Decoder"), NULL, CLSID_CMMLDecodeFilter)
	
	,	mCMMLParser(NULL)
	,	mSeenHead(false)
	,	mHeadTag(NULL)
	,	mCMMLCallbacks(NULL)
{
	debugLog.open("G:\\logs\\cmml_decode.logs", ios_base::out);
		mCMMLParser = new CMMLParser;
	//debugLog.open("C:\\Temp\\cmmlfilter.log", ios_base::out);
	//debugLog<<"*** Log Begins ***"<<endl;
}

CMMLDecodeFilter::~CMMLDecodeFilter(void)
{
	//debugLog<<"*** Log Ends ***"<<endl;
	debugLog.close();
	delete mCMMLParser;
}

CUnknown* WINAPI CMMLDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	CMMLDecodeFilter *pNewObject = new CMMLDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

STDMETHODIMP CMMLDecodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	if (riid == IID_ICMMLAppControl) {
		*ppv = (ICMMLAppControl*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CMMLDecodeFilter::CheckInputType(const CMediaType* inInputMediaType) {
	if (	(inInputMediaType->majortype == MEDIATYPE_Text)	&&
			(inInputMediaType->subtype == MEDIASUBTYPE_CMML) &&
			(inInputMediaType->formattype == FORMAT_CMML) ){

		debugLog<<"Input Type Accepted"<<endl;
		return S_OK;
	} else {
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
}
HRESULT CMMLDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) {
	if (	(inInputMediaType->majortype == MEDIATYPE_Text)	&&
			(inInputMediaType->subtype == MEDIASUBTYPE_CMML) &&
			(inInputMediaType->formattype == FORMAT_CMML) &&
			(inOutputMediaType->majortype == MEDIATYPE_Text) &&
			(inOutputMediaType->subtype == MEDIASUBTYPE_SubtitleVMR9) ){

		debugLog << "Transform Accepted"<<endl;
		return S_OK;
	} else {
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	
}
HRESULT CMMLDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
	//FIX::: Abstract this out properly	

	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	const unsigned long MIN_BUFFER_SIZE = 1024;			
	const unsigned long DEFAULT_BUFFER_SIZE = 4096;
	const unsigned long MIN_NUM_BUFFERS = 10;
	const unsigned long DEFAULT_NUM_BUFFERS = 15;

	
	
	
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
	
	locHR = inAllocator->Commit();

	return locHR;
}
HRESULT CMMLDecodeFilter::GetMediaType(int inPosition, CMediaType* outMediaType) {
	if (inPosition < 0) {
		return E_INVALIDARG;
	} else if (inPosition == 0) {
		outMediaType->majortype = MEDIATYPE_Text;
		outMediaType->subtype = MEDIASUBTYPE_SubtitleVMR9;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT CMMLDecodeFilter::Transform(IMediaSample* inSample, IMediaSample* outSample) {
	inSample->AddRef();
	debugLog<<"In sample ref count = "<<inSample->Release();

	outSample->AddRef();
	debugLog<<"Out sample ref count = "<<outSample->Release();

	unsigned long locSize = inSample->GetActualDataLength();
	char* locCMML = NULL;
	BYTE* locInBuff = NULL;
	BYTE* locOutBuff = NULL;
	HRESULT locHR = S_FALSE;
	wstring locWCMML;
	char* locText = NULL;
	string locTextStr;
	unsigned long locTextSize = 0;
	//outSample->SetMediaTime(NULL, NULL);

	LONGLONG locStart, locEnd;
	inSample->GetTime(&locStart, &locEnd);
	debugLog<<"Input Sample Time : "<<locStart<<" to "<<locEnd<<endl;
	LONGLONG locSampleTime = locStart;
	inSample->GetMediaTime(&locStart, &locEnd);
	debugLog<<"Input Sample Media Time : "<<locStart<<" to "<<locEnd<<endl;
	locSampleTime -= locStart;
	debugLog<<"Corrected Sample time = "<<locSampleTime<<endl;
	debugLog<<"Transform : Input Sample Size = "<<locSize<<endl;
	if (locSize > 0) {
		locCMML = new char[locSize+1];
		locCMML[locSize] = '\0';
		
		locHR = inSample->GetPointer(&locInBuff);
		memcpy((void*)locCMML, (const void*) locInBuff, locSize);
		//debugLog<<"           : Sample Text = "<<locCMML<<endl<<endl;
		locWCMML = toWStr(locCMML);
		if (mSeenHead == false) {
			//debugLog << "           : Processing a head tag"<<endl;
			//Head tag... needs error checks
			mSeenHead = true;
			mHeadTag = new C_HeadTag;
			bool locParseOK = mCMMLParser->parseHeadTag(locWCMML, mHeadTag);
			if (locParseOK) {
				//debugLog<<"          : Parse OK"<<endl;

				locHR = outSample->GetPointer(&locOutBuff);
				locTextSize = mHeadTag->title()->text().size();
				locTextStr = mHeadTag->title()->text();
				
				//debugLog<<"            : Title Text = "<<locTextStr<<endl;
				memcpy((void*)locOutBuff, (const void*) locTextStr.c_str(), locTextSize);
				locOutBuff[locTextSize] = '\0';
				outSample->SetActualDataLength(locTextSize + 1);
				if (mCMMLCallbacks != NULL) {
					mCMMLCallbacks->headCallback(mHeadTag->clone());
				}
			} else {
				//debugLog<<"          : Parse FAILED"<<endl;
			}

		} else {
			//Clip Tag... needs error checks
			//debugLog << "           : Processing a clip tag"<<endl;
			C_ClipTag locClipTag;
			bool locParseOK = mCMMLParser->parseClipTag(locWCMML, &locClipTag);
			if (locParseOK) {
				//debugLog<<"          : Parse OK"<<endl;
			

				locHR = outSample->GetPointer(&locOutBuff);
				locTextSize = locClipTag.anchor()->text().size();
				locTextStr = locClipTag.anchor()->text();
				memcpy((void*)locOutBuff, (const void*) locTextStr.c_str(), locTextSize);
				
				debugLog << "               : Clip Text = "<<locTextStr<<endl;
				locOutBuff[locTextSize] = '\0';
				outSample->SetActualDataLength(locTextSize + 1);
				//debugLog<<"Setting Sample time "<<locClipTag.start()<<endl;
				//LONGLONG locSampleTime = ;
				outSample->SetTime(&locSampleTime, &locSampleTime);
				outSample->SetMediaTime(NULL, NULL);
				outSample->SetSyncPoint(TRUE);
				outSample->SetDiscontinuity(FALSE);
				outSample->SetPreroll(FALSE);

				if (mCMMLCallbacks != NULL) {
					mCMMLCallbacks->clipCallback(locClipTag.clone());
				}

			} else {
				debugLog<<"          : Parse FAILED"<<endl;
				delete locCMML;
				return S_FALSE;
			}
		}

		debugLog<<"Returning... "<<endl<<endl;
		delete locCMML;
		return S_OK;

	} else {
		//Zero length Sample... Blank out... Don't send.
		debugLog<<"Zero length sample..."<<endl;
		return S_FALSE;
	}
}

wstring CMMLDecodeFilter::toWStr(string inString) {
	wstring retVal;

	//LPCWSTR retPtr = new wchar_t[retVal.length() + 1];
	for (std::string::const_iterator i = inString.begin(); i != inString.end(); i++) {
		retVal.append(1, *i);
	}
	

	return retVal;
}

//Implementation of ICMMLAppControl
STDMETHODIMP_(bool) CMMLDecodeFilter::setCallbacks(ICMMLCallbacks* inCallbacks) {
	mCMMLCallbacks = inCallbacks;
	return true;
}
STDMETHODIMP_(ICMMLCallbacks*) CMMLDecodeFilter::getCallbacks() {
	return mCMMLCallbacks;
}

