#include "StdAfx.h"
#include "anxdemuxsourcefilter.h"


//-------------------
// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"AnxDemuxFilter",						// Name
	    &CLSID_AnxDemuxSourceFilter,            // CLSID
	    AnxDemuxSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



CUnknown* WINAPI AnxDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	AnxDemuxSourceFilter *pNewObject = new AnxDemuxSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 
AnxDemuxSourceFilter::AnxDemuxSourceFilter(void)
{
}

AnxDemuxSourceFilter::~AnxDemuxSourceFilter(void)
{
}


//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP AnxDemuxSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	//ANX::: Needs to override ??? Or just modify the seeker.
	mSeekTable = new AutoOggSeekTable(StringHelper::toNarrowStr(mFileName));
	mSeekTable->buildTable();
	
	return SetUpPins();
}