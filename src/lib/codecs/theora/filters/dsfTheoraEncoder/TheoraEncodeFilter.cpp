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
#include "Theoraencodefilter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Theora Encode Filter",						// Name
	    &CLSID_TheoraEncodeFilter,            // CLSID
	    TheoraEncodeFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    },
    { 
		L"Theora Encode Properties",						// Name
	    &CLSID_PropsTheoraEncoder,            // CLSID
	    PropsTheoraEncoder::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }


};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI TheoraEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	TheoraEncodeFilter *pNewObject = new TheoraEncodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 
STDMETHODIMP TheoraEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	if (riid == IID_ITheoraEncodeSettings) {
		*ppv = (ITheoraEncodeSettings*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_ISpecifyPropertyPages) {
		*ppv = (ISpecifyPropertyPages*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}


	return AbstractVideoEncodeFilter::NonDelegatingQueryInterface(riid, ppv);
}

TheoraEncodeFilter::TheoraEncodeFilter(void)
	:	AbstractVideoEncodeFilter(NAME("Theora Encoder"), CLSID_TheoraEncodeFilter, AbstractVideoEncodeFilter::THEORA)
{
	bool locWasConstructed = ConstructPins();
}

TheoraEncodeFilter::~TheoraEncodeFilter(void)
{
}

bool TheoraEncodeFilter::ConstructPins() 
{

	CMediaType* locOutputMediaType = new CMediaType(&MEDIATYPE_Video);
	locOutputMediaType->subtype = MEDIASUBTYPE_Theora;
	locOutputMediaType->formattype = FORMAT_Theora;
	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new TheoraEncodeOutputPin(this, m_pLock, locOutputMediaType);

	
	mInputPin = new TheoraEncodeInputPin(this, m_pLock, mOutputPin);
	return true;
}

//Implementation of ITheoraEncodeSEttings
unsigned long TheoraEncodeFilter::targetBitrate() {
	return ((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->target_bitrate;
}
unsigned char TheoraEncodeFilter::quality() {
	return ((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->quality;
}
unsigned long TheoraEncodeFilter::keyframeFreq() {
	return ((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->keyframe_frequency;
}

bool TheoraEncodeFilter::setTargetBitrate(unsigned long inBitrate) {
	//Needs error checking
	((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->target_bitrate = inBitrate;
	((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->keyframe_data_target_bitrate = (inBitrate * 3)/2;
	return true;

}
bool TheoraEncodeFilter::setQuality(unsigned char inQuality) {
	//Needs error checking
	((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->quality = inQuality;
	return true;
}
bool TheoraEncodeFilter::setKeyframeFreq(unsigned long inKeyframeFreq) {
	//Needs error checking
	((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->keyframe_frequency = inKeyframeFreq;
	((TheoraEncodeInputPin*)mInputPin)->theoraInfo()->keyframe_frequency_force = inKeyframeFreq;
	return true;
}

//SpecifyPropertyPages Implementation
STDMETHODIMP TheoraEncodeFilter::GetPages(CAUUID* outPropPages) {
	if (outPropPages == NULL) return E_POINTER;

	const int NUM_PROP_PAGES = 1;
    outPropPages->cElems = NUM_PROP_PAGES;
    outPropPages->pElems = (GUID*)(CoTaskMemAlloc(sizeof(GUID) * NUM_PROP_PAGES));
    if (outPropPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }

	outPropPages->pElems[0] = CLSID_PropsTheoraEncoder;
    
    return S_OK;

}