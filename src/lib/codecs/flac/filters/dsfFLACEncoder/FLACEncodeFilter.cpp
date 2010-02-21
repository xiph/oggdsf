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
#include "FLACEncodeFilter.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Xiph.Org FLAC Encoder",				// Name
	    &CLSID_FLACEncodeFilter,				// CLSID
	    FLACEncodeFilter::CreateInstance,		// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    },
    { 
		L"FLAC Encode Properties",						// Name
	    &CLSID_PropsFLACEncoder,            // CLSID
	    PropsFLACEncoder::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI FLACEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	FLACEncodeFilter *pNewObject = new FLACEncodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

STDMETHODIMP FLACEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) 
{
	if (riid == IID_IFLACEncodeSettings) {
		*ppv = (IFLACEncodeSettings*)this;
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
STDMETHODIMP FLACEncodeFilter::GetPages(CAUUID* outPropPages) 
{
	if (outPropPages == NULL) return E_POINTER;

	const int NUM_PROP_PAGES = 1;
    outPropPages->cElems = NUM_PROP_PAGES;
    outPropPages->pElems = (GUID*)(CoTaskMemAlloc(sizeof(GUID) * NUM_PROP_PAGES));
    if (outPropPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }

	outPropPages->pElems[0] = CLSID_PropsFLACEncoder;
    
    return S_OK;

}

FLACEncodeFilter::FLACEncodeFilter(void)
	:	AbstractTransformFilter(NAME("Xiph.Org FLAC Encoder"), CLSID_FLACEncodeFilter)
{
	bool locWasConstructed = ConstructPins();
}

FLACEncodeFilter::~FLACEncodeFilter(void)
{
}

bool FLACEncodeFilter::ConstructPins() 
{
	//Input		:	Audio / PCM / WaveFormatEx
	//Output	:	Audio / FLAC_1_0 / FLAC

	
	//Vector to hold our set of media types we want to accept.
	vector<CMediaType*> locAcceptableTypes;

	//Setup the media types for the output pin.
	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_OggFLAC_1_0;
	locAcceptMediaType->formattype = FORMAT_FLAC;
	
	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new FLACEncodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	//Clear out the vector, now we've already passed it to the output pin.
	locAcceptableTypes.clear();

	//Setup the media Types for the input pin.
	locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);			//Deleted by pin

	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;

	locAcceptableTypes.push_back(locAcceptMediaType);
	
	mInputPin = new FLACEncodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;

}

STDMETHODIMP_(bool) FLACEncodeFilter::canModifySettings()
{
    return true;
    
}

STDMETHODIMP_(bool) FLACEncodeFilter::setEncodingLevel(unsigned long inLevel)
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.setEncodingLevel(inLevel);
}
STDMETHODIMP_(bool) FLACEncodeFilter::setLPCOrder(unsigned long inLPCOrder)
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.setLPCOrder(inLPCOrder);
}
STDMETHODIMP_(bool) FLACEncodeFilter::setBlockSize(unsigned long inBlockSize)
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.setBlockSize(inBlockSize);
}
STDMETHODIMP_(bool) FLACEncodeFilter::useMidSideCoding(bool inUseMidSideCoding) //Only for 2 channels
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.useMidSideCoding(inUseMidSideCoding);
}
STDMETHODIMP_(bool) FLACEncodeFilter::useAdaptiveMidSideCoding(bool inUseAdaptiveMidSideCoding) //Only for 2 channels, overrides midside, is faster
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.useAdaptiveMidSideCoding(inUseAdaptiveMidSideCoding);
}
STDMETHODIMP_(bool) FLACEncodeFilter::useExhaustiveModelSearch(bool inUseExhaustiveModelSearch)
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.useExhaustiveModelSearch(inUseExhaustiveModelSearch);
}
STDMETHODIMP_(bool) FLACEncodeFilter::setRicePartitionOrder(unsigned long inMin, unsigned long inMax)
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.setRicePartitionOrder(inMin, inMax);
}

STDMETHODIMP_(long) FLACEncodeFilter::encoderLevel()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.encoderLevel();
}
STDMETHODIMP_(unsigned long) FLACEncodeFilter::LPCOrder()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.LPCOrder();
}
STDMETHODIMP_(unsigned long) FLACEncodeFilter::blockSize()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.blockSize();
}
STDMETHODIMP_(unsigned long) FLACEncodeFilter::riceMin()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.riceMin();
}
STDMETHODIMP_(unsigned long) FLACEncodeFilter::riceMax()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.riceMax();
}
STDMETHODIMP_(bool) FLACEncodeFilter::isUsingMidSideCoding()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.isUsingMidSideCoding();
}
STDMETHODIMP_(bool) FLACEncodeFilter::isUsingAdaptiveMidSideCoding()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.isUsingAdaptiveMidSideCoding();
}
STDMETHODIMP_(bool) FLACEncodeFilter::isUsingExhaustiveModel()
{
    return ((FLACEncodeInputPin*)mInputPin)->mFLACEncoderSettings.isUsingExhaustiveModel();
}