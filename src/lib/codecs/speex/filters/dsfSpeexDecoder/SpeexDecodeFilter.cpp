//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
//Copyright (C) 2009 Cristian Adam
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

//Include Files
#include "stdafx.h"
#include "SpeexDecodeOutputPin.h"
#include "SpeexDecodeInputPin.h"
#include "SpeexDecodeFilter.h"
#include "common/util.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
        SpeexDecodeFilter::NAME,				// Name
	    &CLSID_SpeexDecodeFilter,				// CLSID
	    SpeexDecodeFilter::CreateInstance,		// Method to create an instance of Speex Decoder
        NULL,									// Initialization function
        &SpeexDecodeFilter::m_filterReg         // Set-up information (for filters)      
    }
};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

const wchar_t* SpeexDecodeFilter::NAME = L"Xiph.Org Speex Decoder";

const AMOVIESETUP_MEDIATYPE SpeexDecodeFilter::m_outputMediaTypes = 
{
    &MEDIATYPE_Audio,
    &MEDIASUBTYPE_PCM
};

const AMOVIESETUP_MEDIATYPE SpeexDecodeFilter::m_inputMediaTypes = 
{
    &MEDIATYPE_OggPacketStream,
    &MEDIASUBTYPE_None
};

const AMOVIESETUP_PIN SpeexDecodeFilter::m_pinReg[] = 
{
    {
        L"Speex Input",						//Name (obsoleted)
        FALSE,								//Renders from this pin ?? Not sure about this.
        FALSE,								//Not an output pin
        FALSE,								//Cannot have zero instances of this pin
        FALSE,								//Cannot have more than one instance of this pin
        &GUID_NULL,							//Connects to filter (obsoleted)
        NULL,								//Connects to pin (obsoleted)
        1,									//Support two media type
        &m_inputMediaTypes				    //Pointer to media type (Audio/Vorbis or Audio/Speex)
    } ,

    {
        L"PCM Output",						//Name (obsoleted)
        FALSE,								//Renders from this pin ?? Not sure about this.
        TRUE,								//Is an output pin
        FALSE,								//Cannot have zero instances of this pin
        FALSE,								//Cannot have more than one instance of this pin
        &GUID_NULL,							//Connects to filter (obsoleted)
        NULL,								//Connects to pin (obsoleted)
        1,									//Only support one media type
        &m_outputMediaTypes					//Pointer to media type (Audio/PCM)
     }
};

const AMOVIESETUP_FILTER SpeexDecodeFilter::m_filterReg = 
{
    &CLSID_SpeexDecodeFilter,
    NAME,
    MERIT_NORMAL,
    2,
    m_pinReg
};


#ifdef WINCE
LPAMOVIESETUP_FILTER SpeexDecodeFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&m_filterReg;	
}
#endif


SpeexDecodeFilter::SpeexDecodeFilter()
	:	AbstractTransformFilter(NAME, CLSID_SpeexDecodeFilter)
	,	mSpeexFormatInfo(NULL)
{
	bool locWasConstructed = ConstructPins();

    LOG(logDEBUG) << "Created SpeexDecodeFilter, ConstructPins returned " <<
        std::boolalpha << locWasConstructed << std::endl;
}

bool SpeexDecodeFilter::ConstructPins() 
{

	//Vector to hold our set of media types we want to accept.
	vector<CMediaType*> locAcceptableTypes;

	//Setup the media types for the output pin.
	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new SpeexDecodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	//Clear out the vector, now we've already passed it to the output pin.
	locAcceptableTypes.clear();

	//Setup the media Types for the input pin.
	locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_OggPacketStream);			//Deleted by pin

	locAcceptMediaType->subtype = MEDIASUBTYPE_None;
	locAcceptMediaType->formattype = FORMAT_OggIdentHeader;

	locAcceptableTypes.push_back(locAcceptMediaType);
	
	mInputPin = new SpeexDecodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;

}

SpeexDecodeFilter::~SpeexDecodeFilter(void)
{
    LOG(logINFO) << L"SpeexDecodeFilter destroyed!" << std::endl;

    delete mSpeexFormatInfo;
}

CUnknown* WINAPI SpeexDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
    util::ConfigureLogSettings();

    //This routine is the COM implementation to create a new Filter
    SpeexDecodeFilter *pNewObject = new (std::nothrow) SpeexDecodeFilter();
    if (pNewObject == NULL) 
    {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

STDMETHODIMP SpeexDecodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    //if (riid == IID_IWMPTranscodePolicy) {
    //	*ppv = (IWMPTranscodePolicy*)this;
    //	((IUnknown*)*ppv)->AddRef();
    //	return NOERROR;
    //}

    return AbstractTransformFilter::NonDelegatingQueryInterface(riid, ppv); 
}

//QUERY::: Do we need these ? Aren't we all friedns here ??
//RESULT::: Keep them, set function must be kept... get could go... but keep for consistency
SPEEXFORMAT* SpeexDecodeFilter::getSpeexFormatBlock() 
{
	return mSpeexFormatInfo;
}
void SpeexDecodeFilter::setSpeexFormat(BYTE* inFormatBlock) 
{
	delete mSpeexFormatInfo;
	mSpeexFormatInfo = new SPEEXFORMAT;

	mSpeexFormatInfo->speexVersion = iLE_Math::charArrToULong(inFormatBlock + 28);
	mSpeexFormatInfo->numChannels = iLE_Math::charArrToULong(inFormatBlock + 48);
	mSpeexFormatInfo->samplesPerSec = iLE_Math::charArrToULong(inFormatBlock + 36);
	mSpeexFormatInfo->avgBitsPerSec = 0;
	mSpeexFormatInfo->maxBitsPerSec = 0;
	mSpeexFormatInfo->minBitsPerSec = 0;

    LOG(logINFO) << "Speex Version: " << mSpeexFormatInfo->speexVersion
        << " Channels: " << mSpeexFormatInfo->numChannels
        << " SamplesPerSec: " << mSpeexFormatInfo->samplesPerSec
        << " MaxBitsPerSec: " << mSpeexFormatInfo->maxBitsPerSec
        << " AvgBitsPerSec: " << mSpeexFormatInfo->avgBitsPerSec
        << " MinBitsPerSec: " << mSpeexFormatInfo->minBitsPerSec;
}
