#include "StdAfx.h"
#include ".\DiracDecodeSourceFilter.h"

CFactoryTemplate g_Templates[] = 
{
    { 
		L"DiracDecodeSourceFilter",						// Name
	    &CLSID_DiracDecodeSourceFilter,            // CLSID
	    DiracDecodeSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


CUnknown* WINAPI DiracDecodeSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	DiracDecodeSourceFilter *pNewObject = new DiracDecodeSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

DiracDecodeSourceFilter::DiracDecodeSourceFilter(void)
	:	CBaseFilter(NAME("DiracDecodeSourceFilter"), NULL, m_pLock, CLSID_DiracDecodeSourceFilter)
{
}

DiracDecodeSourceFilter::~DiracDecodeSourceFilter(void)
{
}

//BaseFilter Interface
int DiracDecodeSourceFilter::GetPinCount() {
	return 1;
}
CBasePin* DiracDecodeSourceFilter::GetPin(int inPinNo) {

	if (inPinNo == 0) {
		return mDiracSourcePin;
	} else {
		return NULL;
	}
}

//IAMFilterMiscFlags Interface
ULONG DiracDecodeSourceFilter::GetMiscFlags(void) {
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

	//IFileSource Interface
STDMETHODIMP DiracDecodeSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}

//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP DiracDecodeSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;


	
	return S_OK;
}

STDMETHODIMP DiracDecodeSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//IMEdiaStreaming
STDMETHODIMP DiracDecodeSourceFilter::Run(REFERENCE_TIME tStart) {
	const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	//debugLog<<"Run  :  time = "<<tStart<<endl;
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP DiracDecodeSourceFilter::Pause(void) {
	CAutoLock locLock(m_pLock);
	//debugLog << "** Pause called **"<<endl;
	if (m_State == State_Stopped) {
		//debugLog << "Was in stopped state... starting thread"<<endl;
		if (ThreadExists() == FALSE) {
			Create();
		}
		CallWorker(THREAD_RUN);
	}
	//debugLog<<"Was NOT is stopped state, not doing much at all..."<<endl;
	
	HRESULT locHR = CBaseFilter::Pause();
	
	return locHR;
	
}
STDMETHODIMP DiracDecodeSourceFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	//debugLog<<"** Stop Called ** "<<endl;
	CallWorker(THREAD_EXIT);
	Close();
	//DeliverBeginFlush();
	//DeliverEndFlush();
	return CBaseFilter::Stop();
}

HRESULT DiracDecodeSourceFilter::DataProcessLoop() {

	return S_OK;
}

//CAMThread Stuff
DWORD DiracDecodeSourceFilter::ThreadProc(void) {
	//debugLog << "Thread Proc Called..."<<endl;
	while(true) {
		DWORD locThreadCommand = GetRequest();
		//debugLog << "Command = "<<locThreadCommand<<endl;
		switch(locThreadCommand) {
			case THREAD_EXIT:
				//debugLog << "EXIT ** "<<endl;
				Reply(S_OK);
				return S_OK;

			//case THREAD_PAUSE:
			//	// we are paused already
			//	Reply(S_OK);
			//	break;

			case THREAD_RUN:
				//debugLog << "RUN ** "<<endl;
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
	
	
	}
	return S_OK;
}