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