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


#include "SpeexEncodeFilter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Xiph.Org Speex Encoder",						// Name
	    &CLSID_SpeexEncodeFilter,					// CLSID
	    SpeexEncodeFilter::CreateInstance,			// Method to create an instance of MyComponent
        NULL,										// Initialization function
        NULL										// Set-up information (for filters)
    },
    { 
		L"Speex Encode Properties",						// Name
	    &CLSID_PropsSpeexEncoder,            // CLSID
	    PropsSpeexEncoder::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI SpeexEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	SpeexEncodeFilter *pNewObject = new SpeexEncodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

STDMETHODIMP SpeexEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) 
{
	if (riid == IID_ISpeexEncodeSettings) {
		*ppv = (ISpeexEncodeSettings*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_ISpecifyPropertyPages) {
		*ppv = (ISpecifyPropertyPages*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}
	return AbstractTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

//SpecifyPropertyPages Implementation
STDMETHODIMP SpeexEncodeFilter::GetPages(CAUUID* outPropPages) 
{
	if (outPropPages == NULL) return E_POINTER;

	const int NUM_PROP_PAGES = 1;
    outPropPages->cElems = NUM_PROP_PAGES;
    outPropPages->pElems = (GUID*)(CoTaskMemAlloc(sizeof(GUID) * NUM_PROP_PAGES));
    if (outPropPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }

	outPropPages->pElems[0] = CLSID_PropsSpeexEncoder;
    
    return S_OK;

}

SpeexEncodeFilter::SpeexEncodeFilter(void)
	:	AbstractTransformFilter(NAME("Xiph.Org Speex Encoder"), CLSID_SpeexEncodeFilter)
{
	bool locWasConstructed = ConstructPins();
}

SpeexEncodeFilter::~SpeexEncodeFilter(void)
{
}

bool SpeexEncodeFilter::ConstructPins() 
{
	//Inputs Audio / PCM / WaveFormatEx
	//Outputs Audio / Speex / Speex

	//Vector to hold our set of media types we want to accept.
	vector<CMediaType*> locAcceptableTypes;

	//Setup the media types for the output pin.
	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_Speex;
	locAcceptMediaType->formattype = FORMAT_Speex;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new SpeexEncodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	//Clear out the vector, now we've already passed it to the output pin.
	locAcceptableTypes.clear();

	//Setup the media Types for the input pin.
	locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);			//Deleted by pin

	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;

	locAcceptableTypes.push_back(locAcceptMediaType);
	
	mInputPin = new SpeexEncodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;
}

STDMETHODIMP_(SpeexEncodeSettings) SpeexEncodeFilter::getEncoderSettings()
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings;
}


STDMETHODIMP_(bool) SpeexEncodeFilter::setMode(SpeexEncodeSettings::eSpeexEncodeMode inMode)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setMode(inMode);
    
}
STDMETHODIMP_(bool) SpeexEncodeFilter::setComplexity(long inComplexity)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setComplexity(inComplexity);
}

STDMETHODIMP_(bool) SpeexEncodeFilter::setupVBRQualityMode(long inQuality, long inVBRMaxBitrate)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setupVBRQualityMode(inQuality, inVBRMaxBitrate);
}
STDMETHODIMP_(bool) SpeexEncodeFilter::setupVBRBitrateMode(long inBitrate, long inVBRMaxBitrate)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setupVBRBitrateMode(inBitrate, inVBRMaxBitrate);
}
STDMETHODIMP_(bool) SpeexEncodeFilter::setupABR(long inABRBitrate)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setupABR(inABRBitrate);
}
STDMETHODIMP_(bool) SpeexEncodeFilter::setupCBRBitrateMode(long inCBRBitrate)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setupCBRBitrateMode(inCBRBitrate);
}
STDMETHODIMP_(bool) SpeexEncodeFilter::setupCBRQualityMode(long inQuality)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setupCBRQualityMode(inQuality);
}

STDMETHODIMP_(bool) SpeexEncodeFilter::setEncodingFlags(bool inUseDTX, bool inUseVAD, bool inUseAGC, bool inUseDenoise)
{
    return ((SpeexEncodeInputPin*)mInputPin)->mEncoderSettings.setEncodingFlags(inUseDTX, inUseVAD, inUseAGC, inUseDenoise);

}
