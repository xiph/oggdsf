#include "StdAfx.h"
#include ".\cmmlrawsourcefilter.h"

CFactoryTemplate g_Templates[] = 
{
    { 
		L"CMMLRawDemuxFilter",						// Name
	    &CLSID_CMMLRawSourceFilter,            // CLSID
	    CMMLRawSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


CUnknown* WINAPI CMMLRawSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	CMMLRawSourceFilter *pNewObject = new CMMLRawSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

CMMLRawSourceFilter::CMMLRawSourceFilter(void)
	:	CBaseFilter(NAME("CMMLRawSourceFilter"), NULL, m_pLock, CLSID_CMMLRawSourceFilter)
{
}

CMMLRawSourceFilter::~CMMLRawSourceFilter(void)
{
}

//BaseFilter Interface
int CMMLRawSourceFilter::GetPinCount() {
	return 1;
}
CBasePin* CMMLRawSourceFilter::GetPin(int inPinNo) {

	if (inPinNo == 0) {
		return mCMMLSourcePin;
	} else {
		return NULL;
	}
}