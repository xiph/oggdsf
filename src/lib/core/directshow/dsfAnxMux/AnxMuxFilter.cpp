#include "StdAfx.h"
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
{

	mInterleaver = new AnxPageInterleaver(this, this, 2, 0);
	mInputPins.push_back(new AnxMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream()));


		//Make our delegate pin[0], the top pin... we send all out requests there.
	IMediaSeeking* locSeeker = NULL;
	mInputPins[0]->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
}

AnxMuxFilter::~AnxMuxFilter(void)
{
}
