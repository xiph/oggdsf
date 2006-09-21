//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "vorbisdecodefilter.h"

//Include Files
#include "stdafx.h"
#include "VorbisDecodeFilter.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Vorbis Decode Filter",						// Name
	    &CLSID_VorbisDecodeFilter,            // CLSID
	    VorbisDecodeFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
#ifdef WINCE
		&VorbisDecodeFilterReg
#else
        NULL									// Set-up information (for filters)
#endif
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

#ifdef WINCE
LPAMOVIESETUP_FILTER VorbisDecodeFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&VorbisDecodeFilterReg;	
}

HRESULT VorbisDecodeFilter::Register()
{
	return CBaseFilter::Register();
}
#endif

//*************************************************************************************************
VorbisDecodeFilter::VorbisDecodeFilter()
	:	AbstractTransformFilter(NAME("Vorbis Decoder"), CLSID_VorbisDecodeFilter)
	,	mVorbisFormatInfo(NULL)
{

	bool locWasConstructed = ConstructPins();
	//TODO::: Error check !
}

STDMETHODIMP VorbisDecodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	//if (riid == IID_IWMPTranscodePolicy) {
	//	*ppv = (IWMPTranscodePolicy*)this;
	//	((IUnknown*)*ppv)->AddRef();
	//	return NOERROR;
	//}

	return AbstractTransformFilter::NonDelegatingQueryInterface(riid, ppv); 
}

//HRESULT VorbisDecodeFilter::allowTranscode(VARIANT_BOOL* outAllowTranscode)
//{
//    *outAllowTranscode = VARIANT_TRUE;
//    return S_OK;
//}
bool VorbisDecodeFilter::ConstructPins() 
{
	//Vector to hold our set of media types we want to accept.
	vector<CMediaType*> locAcceptableTypes;

	//Setup the media types for the output pin.
	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Second one the same type... they are actually different one is the extensible format. See CreateAndFill
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new VorbisDecodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	//Clear out the vector, now we've already passed it to the output pin.
	locAcceptableTypes.clear();

	//Setup the media Types for the input pin.
	locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_OggPacketStream);			//Deleted by pin

	locAcceptMediaType->subtype = MEDIASUBTYPE_None;
	locAcceptMediaType->formattype = FORMAT_OggIdentHeader;

	locAcceptableTypes.push_back(locAcceptMediaType);
	
	mInputPin = new VorbisDecodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;
}

VorbisDecodeFilter::~VorbisDecodeFilter(void)
{
	DbgLog((LOG_TRACE,1,TEXT("Vorbis Destructor...")));
	
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
void VorbisDecodeFilter::setVorbisFormat(BYTE* inFormatBlock) 
{
	delete mVorbisFormatInfo;
	mVorbisFormatInfo = new sVorbisFormatBlock;				//Deleted in destructor.
	//*mVorbisFormatInfo = *inFormatBlock;

	mVorbisFormatInfo->vorbisVersion = iLE_Math::charArrToULong(inFormatBlock + 7);
	mVorbisFormatInfo->numChannels = inFormatBlock[11];
	mVorbisFormatInfo->samplesPerSec = iLE_Math::charArrToULong(inFormatBlock + 12);
	mVorbisFormatInfo->maxBitsPerSec = iLE_Math::charArrToULong(inFormatBlock + 16);
	mVorbisFormatInfo->avgBitsPerSec = iLE_Math::charArrToULong(inFormatBlock + 20);
	mVorbisFormatInfo->minBitsPerSec = iLE_Math::charArrToULong(inFormatBlock + 24);

}
