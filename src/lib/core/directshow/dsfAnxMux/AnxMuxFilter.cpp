//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
#include ".\anxmuxfilter.h"



// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"AnxMuxFilter",						// Name
	    &CLSID_AnxMuxFilter,            // CLSID
	    AnxMuxFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



STDMETHODIMP AnxMuxFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	//ANX3::: Maybe need an interface to set the anx version here.
	return OggMuxFilter::NonDelegatingQueryInterface(riid, ppv); 
}

CUnknown* WINAPI AnxMuxFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	AnxMuxFilter *pNewObject = new AnxMuxFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 
AnxMuxFilter::AnxMuxFilter(void)
	:	OggMuxFilter(CLSID_AnxMuxFilter)
	,	mAnxVersionMajor(3)
	,	mAnxVersionMinor(0)
{

	//ANX3::: Need to have a better way to set this.
	mInterleaver = new AnxPageInterleaver(this, this, mAnxVersionMajor, mAnxVersionMinor, this);
	mInputPins.push_back(new AnxMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream(), mAnxVersionMajor, mAnxVersionMinor));


	//	//Make our delegate pin[0], the top pin... we send all out requests there.
	//IMediaSeeking* locSeeker = NULL;
	//mInputPins[0]->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	//SetDelegate(locSeeker);
	
	//To avoid a circular reference... we do this without the addref.
	// This is safe because we control the lifetime of this pin, and it won't be deleted until we are.
	IMediaSeeking* locSeeker = (IMediaSeeking*)mInputPins[0];
	SetDelegate(locSeeker);
}

AnxMuxFilter::~AnxMuxFilter(void)
{
}

HRESULT AnxMuxFilter::addAnotherPin() {
	mInputPins.push_back(new AnxMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream(), mAnxVersionMajor, mAnxVersionMinor));
	return S_OK;
}