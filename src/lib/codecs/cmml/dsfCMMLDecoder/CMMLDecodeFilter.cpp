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
{
	mCMMLParser = new CMMLParser;
	debugLog.open("C:\\Temp\\cmmlfilter.log", ios_base::out);
	debugLog<<"*** Log Begins ***"<<endl;
}

CMMLDecodeFilter::~CMMLDecodeFilter(void)
{
	debugLog<<"*** Log Ends ***"<<endl;
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
	const unsigned long MIN_NUM_BUFFERS = 3;
	const unsigned long DEFAULT_NUM_BUFFERS = 5;

	
	
	
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
	unsigned long locSize = inSample->GetActualDataLength();
	char* locCMML = NULL;
	BYTE* locInBuff = NULL;
	BYTE* locOutBuff = NULL;
	HRESULT locHR = S_FALSE;
	wstring locWCMML;
	char* locText = NULL;
	string locTextStr;
	unsigned long locTextSize = 0;

	debugLog<<"Transform : Input Sample Size = "<<locSize<<endl;
	if (locSize > 0) {
		locCMML = new char[locSize+1];
		locCMML[locSize] = '\0';
		
		locHR = inSample->GetPointer(&locInBuff);
		memcpy((void*)locCMML, (const void*) locInBuff, locSize);
		debugLog<<"           : Sample Text = "<<locCMML<<endl<<endl;
		locWCMML = toWStr(locCMML);
		if (mSeenHead == false) {
			debugLog << "           : Processing a head tag"<<endl;
			//Head tag... needs error checks
			mSeenHead = true;
			mHeadTag = new C_HeadTag;
			bool locParseOK = mCMMLParser->parseHeadTag(locWCMML, mHeadTag);
			if (locParseOK) {
				debugLog<<"          : Parse OK"<<endl;

				locHR = outSample->GetPointer(&locOutBuff);
				locTextSize = mHeadTag->title()->text().size();
				locTextStr = mHeadTag->title()->text();
				
				debugLog<<"            : Title Text = "<<locTextStr<<endl;
				memcpy((void*)locOutBuff, (const void*) locTextStr.c_str(), locTextSize);
				locOutBuff[locTextSize] = '\0';
				outSample->SetActualDataLength(locTextSize + 1);
			} else {
				debugLog<<"          : Parse FAILED"<<endl;
			}

		} else {
			//Clip Tag... needs error checks
			debugLog << "           : Processing a clip tag"<<endl;
			C_ClipTag locClipTag;
			bool locParseOK = mCMMLParser->parseClipTag(locWCMML, &locClipTag);
			if (locParseOK) {
				debugLog<<"          : Parse OK"<<endl;
			

				locHR = outSample->GetPointer(&locOutBuff);
				locTextSize = locClipTag.anchor()->text().size();
				locTextStr = locClipTag.anchor()->text();
				memcpy((void*)locOutBuff, (const void*) locTextStr.c_str(), locTextSize);
				
				debugLog << "               : Clip Text = "<<locTextStr<<endl;
				locOutBuff[locTextSize] = '\0';
				outSample->SetActualDataLength(locTextSize + 1);
			} else {
				debugLog<<"          : Parse FAILED"<<endl;
			}
		}

		delete locCMML;
		return S_OK;

	} else {
		//This is dumb !!
		return S_OK;
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