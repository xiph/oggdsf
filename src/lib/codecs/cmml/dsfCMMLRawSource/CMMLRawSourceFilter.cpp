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

STDMETHODIMP CMMLRawSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSourceFilter) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}
	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}

CMMLRawSourceFilter::CMMLRawSourceFilter(void)
	:	CBaseFilter(NAME("CMMLRawSourceFilter"), NULL, m_pLock, CLSID_CMMLRawSourceFilter)
	,	mCMMLDoc(NULL)
{
	mCMMLSourcePin = new CMMLRawSourcePin(		this
											,	this->m_pLock);



}

CMMLRawSourceFilter::~CMMLRawSourceFilter(void)
{
	delete mCMMLSourcePin;
	delete mCMMLDoc;
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

	delete mCMMLDoc;
	mCMMLDoc = new C_CMMLDoc;
	bool retVal = mCMMLParser.parseDocFromFile(mFileName, mCMMLDoc);

	if (retVal) {
		return S_OK;
	} else {
		return S_FALSE;
	}


}

//CAMThread Stuff
DWORD CMMLRawSourceFilter::ThreadProc(void) {
	//debugLog << "Thread Proc Called..."<<endl;

	while(true) {
		DWORD locThreadCommand = GetRequest();
		//debugLog << "Command = "<<locThreadCommand<<endl;
		switch(locThreadCommand) {
			case THREAD_EXIT:
				//debugLog << "EXIT ** "<<endl;
				Reply(S_OK);
				return S_OK;

			case THREAD_RUN:
				//debugLog << "RUN ** "<<endl;
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
		
	}
	return S_OK;
}


HRESULT CMMLRawSourceFilter::DataProcessLoop() 
{
	DWORD locCommand = 0;
	while(true) {
		if(CheckRequest(&locCommand) == TRUE) {
			//debugLog<<"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			return S_OK;
		}
		
		if (mUptoTag == -1) {
			deliverTag(mCMMLDoc->root()->head());
		} else if (mUptoTag < mCMMLDoc->root()->clipList()->numTags()) {
			deliverTag(mCMMLDoc->root()->clipList()->getTag(mUptoTag));
		} else {
			DeliverEOS();
		}
		mUptoTag++;

	}
	return S_OK;

}
