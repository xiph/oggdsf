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
#include "vorbisdecodefilter.h"

//Include Files
#include "StdAfx.h"
#include "VorbisDecodeFilter.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Vorbis Decode Filter",						// Name
	    &CLSID_VorbisDecodeFilter,            // CLSID
	    VorbisDecodeFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



//*************************************************************************************************
VorbisDecodeFilter::VorbisDecodeFilter()
	:	AbstractTransformFilter(NAME("Vorbis Decoder"), CLSID_VorbisDecodeFilter)
	,	mVorbisFormatInfo(NULL)
{

	bool locWasConstructed = ConstructPins();
	//TODO::: Error check !
}

bool VorbisDecodeFilter::ConstructPins() 
{

	//TODO::: FIX THIS UP !!!
	DbgLog((LOG_TRACE,1,TEXT("Vorbis Constructor...")));

	vector<CMediaType*> locAcceptableTypes;

	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new VorbisDecodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	locAcceptableTypes.clear();
	locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);			//Deleted by pin

	locAcceptMediaType->subtype = MEDIASUBTYPE_Vorbis;
	locAcceptMediaType->formattype = FORMAT_Vorbis;
	
	



	locAcceptableTypes.push_back(locAcceptMediaType);

	
	
	
	mInputPin = new VorbisDecodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;
}

VorbisDecodeFilter::~VorbisDecodeFilter(void)
{
	DbgLog((LOG_TRACE,1,TEXT("Vorbis Destructor...")));
	//DestroyPins();
	delete mVorbisFormatInfo;
}

CUnknown* WINAPI VorbisDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{

	VorbisDecodeFilter *pNewObject = new VorbisDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

//QUERY::: Do we need these ? Aren't we all friedns here ??
//RESULT::: Keep them, set function must be kept... get could go... but keep for consistency
sVorbisFormatBlock* VorbisDecodeFilter::getVorbisFormatBlock() 
{
	return mVorbisFormatInfo;
}
void VorbisDecodeFilter::setVorbisFormat(sVorbisFormatBlock* inFormatBlock) 
{
	delete mVorbisFormatInfo;
	mVorbisFormatInfo = new sVorbisFormatBlock;				//Deleted in destructor.
	*mVorbisFormatInfo = *inFormatBlock;
}

//Old imp
//******************************************************************
//#include "StdAfx.h"
//#include "vorbisdecodefilter.h"
//
////Include Files
//#include "StdAfx.h"
//#include "VorbisDecodeFilter.h"
//
////COM Factory Template
//CFactoryTemplate g_Templates[] = 
//{
//    { 
//		L"Vorbis Decode Filter",						// Name
//	    &CLSID_VorbisDecodeFilter,            // CLSID
//	    VorbisDecodeFilter::CreateInstance,	// Method to create an instance of MyComponent
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
////*************************************************************************************************
//VorbisDecodeFilter::VorbisDecodeFilter()
//	:	AbstractAudioDecodeFilter(NAME("Vorbis Decoder"), CLSID_VorbisDecodeFilter, VORBIS)
//	,	mVorbisFormatInfo(NULL)
//{
//
//	bool locWasConstructed = ConstructPins();
//}
//
//bool VorbisDecodeFilter::ConstructPins() 
//{
//	DbgLog((LOG_TRACE,1,TEXT("Vorbis Constructor...")));
//	//Output pin must be done first because it's passed to the input pin.
//	mOutputPin = new VorbisDecodeOutputPin(this, m_pLock);			//Deleted in base class destructor
//
//	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
//	locAcceptMediaType->subtype = MEDIASUBTYPE_Vorbis;
//	locAcceptMediaType->formattype = FORMAT_Vorbis;
//	mInputPin = new VorbisDecodeInputPin(this, m_pLock, mOutputPin, locAcceptMediaType);	//Deleted in base class filter destructor.
//	return true;
//}
//
//VorbisDecodeFilter::~VorbisDecodeFilter(void)
//{
//	DbgLog((LOG_TRACE,1,TEXT("Vorbis Destructor...")));
//	//DestroyPins();
//	delete mVorbisFormatInfo;
//}
//
//CUnknown* WINAPI VorbisDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
//{
//
//	VorbisDecodeFilter *pNewObject = new VorbisDecodeFilter();
//    if (pNewObject == NULL) {
//        *pHr = E_OUTOFMEMORY;
//    }
//	return pNewObject;
//} 
//
////QUERY::: Do we need these ? Aren't we all friedns here ??
////RESULT::: Keep them, set function must be kept... get could go... but keep for consistency
//sVorbisFormatBlock* VorbisDecodeFilter::getVorbisFormatBlock() 
//{
//	return mVorbisFormatInfo;
//}
//void VorbisDecodeFilter::setVorbisFormat(sVorbisFormatBlock* inFormatBlock) 
//{
//	delete mVorbisFormatInfo;
//	mVorbisFormatInfo = new sVorbisFormatBlock;				//Deleted in destructor.
//	*mVorbisFormatInfo = *inFormatBlock;
//}