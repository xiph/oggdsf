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

#include "stdafx.h"
#include "Diracencodefilter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Dirac Encode Filter",						// Name
	    &CLSID_DiracEncodeFilter,            // CLSID
	    DiracEncodeFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }


};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI DiracEncodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	DiracEncodeFilter *pNewObject = new DiracEncodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 
STDMETHODIMP DiracEncodeFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv) {



	return AbstractVideoEncodeFilter::NonDelegatingQueryInterface(riid, ppv);
}

DiracEncodeFilter::DiracEncodeFilter(void)
	:	AbstractVideoEncodeFilter(NAME("Dirac Encoder"), CLSID_DiracEncodeFilter, AbstractVideoEncodeFilter::DIRAC)
{
	bool locWasConstructed = ConstructPins();
}

DiracEncodeFilter::~DiracEncodeFilter(void)
{
}

bool DiracEncodeFilter::ConstructPins() 
{

	CMediaType* locOutputMediaType = new CMediaType(&MEDIATYPE_Video);
	locOutputMediaType->subtype = MEDIASUBTYPE_Dirac;
	locOutputMediaType->formattype = FORMAT_Dirac;
	//Output pin must be done first because it's passed to the input pin.
	mOutputPin = new DiracEncodeOutputPin(this, m_pLock, locOutputMediaType);

	
	mInputPin = new DiracEncodeInputPin(this, m_pLock, mOutputPin);
	return true;
}

