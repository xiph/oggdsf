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


#include "FLACencodefilter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"FLAC Encode Filter",						// Name
	    &CLSID_FLACEncodeFilter,            // CLSID
	    FLACEncodeFilter::CreateInstance,	// Method to create an instance of MyComponent
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

FLACEncodeFilter::FLACEncodeFilter(void)
	:	AbstractAudioEncodeFilter(NAME("FLAC Encoder"), CLSID_FLACEncodeFilter, AbstractAudioEncodeFilter::FLAC)
{
	bool locWasConstructed = ConstructPins();
}

FLACEncodeFilter::~FLACEncodeFilter(void)
{
}

bool FLACEncodeFilter::ConstructPins() 
{

	CMediaType* locOutputMediaType = new CMediaType(&MEDIATYPE_Audio);
	locOutputMediaType->subtype = MEDIASUBTYPE_FLAC;
	locOutputMediaType->formattype = FORMAT_FLAC;
	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new FLACEncodeOutputPin(this, m_pLock, locOutputMediaType);

	
	mInputPin = new FLACEncodeInputPin(this, m_pLock, mOutputPin);
	return true;
}