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

//IAMFilterMiscFlags Interface
ULONG CMMLRawSourceFilter::GetMiscFlags(void) {
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

	//IFileSource Interface
STDMETHODIMP CMMLRawSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}

//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP CMMLRawSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;


	
	return S_OK;
}

STDMETHODIMP CMMLRawSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}