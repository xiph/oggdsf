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
	AnxDemuxSourceFilter *pNewObject = new AnxDMuxFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 
AnxMuxFilter::AnxMuxFilter(void)
{
}

AnxMuxFilter::~AnxMuxFilter(void)
{
}
