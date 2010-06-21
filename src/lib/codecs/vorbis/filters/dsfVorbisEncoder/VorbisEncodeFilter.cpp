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
#include "vorbisencodefilter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Xiph.Org Vorbis Encoder",				// Name
	    &CLSID_VorbisEncodeFilter,              // CLSID
	    VorbisEncodeFilter::CreateInstance,	    // Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    },
    { 
		L"Vorbis Encode Properties",						// Name
	    &CLSID_PropsVorbisEncoder,            // CLSID
	    PropsVorbisEncoder::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI VorbisEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	VorbisEncodeFilter *pNewObject = new VorbisEncodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

VorbisEncodeFilter::VorbisEncodeFilter(void)
	:	AbstractTransformFilter(NAME("Xiph.Org Vorbis Encoder"), CLSID_VorbisEncodeFilter)
{
	ConstructPins();
}

VorbisEncodeFilter::~VorbisEncodeFilter(void)
{

}

STDMETHODIMP VorbisEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) 
{
	if (riid == IID_IVorbisEncodeSettings) {
		*ppv = (IVorbisEncodeSettings*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_ISpecifyPropertyPages) {
		*ppv = (ISpecifyPropertyPages*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}
	return AbstractTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

bool VorbisEncodeFilter::ConstructPins() 
{
	//Inputs Audio / PCM / WaveFormatEx
	//Outputs Audio / Vorbis / Vorbis

	//Vector to hold our set of media types we want to accept.
	vector<CMediaType*> locAcceptableTypes;

	//Setup the media types for the output pin.
	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_Vorbis;
	locAcceptMediaType->formattype = FORMAT_Vorbis;

	locAcceptableTypes.push_back(locAcceptMediaType);

	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new VorbisEncodeOutputPin(this, m_pLock, locAcceptableTypes);			//Deleted in base class destructor

	//Clear out the vector, now we've already passed it to the output pin.
	locAcceptableTypes.clear();

	//Setup the media Types for the input pin.
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Audio);			//Deleted by pin
	locAcceptMediaType->subtype = MEDIASUBTYPE_PCM;
	locAcceptMediaType->formattype = FORMAT_WaveFormatEx;
	locAcceptableTypes.push_back(locAcceptMediaType);
	
	mInputPin = new VorbisEncodeInputPin(this, m_pLock, mOutputPin, locAcceptableTypes);	//Deleted in base class filter destructor.
	return true;
}

STDMETHODIMP_(VorbisEncodeSettings) VorbisEncodeFilter::getEncoderSettings()
{
    return ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings;
}
	
STDMETHODIMP_(bool) VorbisEncodeFilter::setQuality(int inQuality)
{
    return ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings.setQuality(inQuality);  
}

STDMETHODIMP_(bool) VorbisEncodeFilter::setBitrateQualityMode(int inBitrate)
{
    return ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings.setBitrateQualityMode(inBitrate);
}

STDMETHODIMP_(bool) VorbisEncodeFilter::setManaged(int inBitrate, int inMinBitrate, int inMaxBitrate)
{
    return ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings.setManaged(inBitrate, inMinBitrate, inMaxBitrate);
}

//SpecifyPropertyPages Implementation
STDMETHODIMP VorbisEncodeFilter::GetPages(CAUUID* outPropPages) 
{
	if (outPropPages == NULL) return E_POINTER;

	const int NUM_PROP_PAGES = 1;
    outPropPages->cElems = NUM_PROP_PAGES;
    outPropPages->pElems = (GUID*)(CoTaskMemAlloc(sizeof(GUID) * NUM_PROP_PAGES));
    if (outPropPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }

	outPropPages->pElems[0] = CLSID_PropsVorbisEncoder;
    
    return S_OK;

}
//STDMETHODIMP_(signed char) VorbisEncodeFilter::quality() 
//{
//	//return (signed char)( ((VorbisEncodeInputPin*)mInputPin)->mVorbisQuality * 100 );
//    return (signed char)( ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings.mQuality );
//}
//
//
//STDMETHODIMP_(bool) VorbisEncodeFilter::setQuality(signed char inQuality) 
//{
//	
//	if ((inQuality >= 0) && (inQuality < 100)) {
//		
//		//((VorbisEncodeInputPin*)mInputPin)->mVorbisQuality = (float)inQuality/(float)100;
//        ((VorbisEncodeInputPin*)mInputPin)->mEncoderSettings.setQuality(inQuality);
//		
//		return true;
//	} else {
//		
//		return false;
//	}
//}
