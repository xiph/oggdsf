#include "stdafx.h"
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
	,	mUptoTag(0)
{
	m_pLock = new CCritSec;
	mCMMLSourcePin = new CMMLRawSourcePin(		this
											,	this->m_pLock);



}

CMMLRawSourceFilter::~CMMLRawSourceFilter(void)
{

	delete mCMMLSourcePin;
	delete mCMMLDoc;
	delete m_pLock;
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

	mUptoTag = -1;

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
	C_ClipTag* locClip = NULL;
	DWORD locCommand = 0;
	while(true) {
		if(CheckRequest(&locCommand) == TRUE) {
			//debugLog<<"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			return S_OK;
		}
		
		if (mUptoTag == -1) {
			//Deliver the head tag
			mCMMLSourcePin->deliverTag(mCMMLDoc->root()->head());
		} else if (mUptoTag < mCMMLDoc->root()->clipList()->numTags()) {

			locClip = mCMMLDoc->root()->clipList()->getTag(mUptoTag);

			wstring locTrackName = locClip->track();

			if (locTrackName == L"") {
				locTrackName = L"default";
			}

			//Check if we have a pending end time in this track.
			tTrackMap::iterator locIt = mTrackMap.find(locTrackName);
			if (locIt != mTrackMap.end()) {
				//There's an entry for this track in the map.
				__int64 locStartTime = StringHelper::stringToNum(StringHelper::toNarrowStr(locClip->start()));
				
				if (locStartTime <= locIt->second) {
					//The start time of this clip is before the potential end time we saved.
					// This means the end time means nothing, and we can ignore it and remove from the map.
					mTrackMap.erase(locIt);

				} else {
					//The start time of this clip is after the saved end time...
					// We send an empty clip marked with the end time.
					C_ClipTag* locEndTag = new C_ClipTag;
					locEndTag->setStart(StringHelper::toWStr(StringHelper::numToString(locIt->second)));
					locEndTag->setTrack(locTrackName);
					mCMMLSourcePin->deliverTag(locEndTag);

					//Now remove it from the map.
					mTrackMap.erase(locIt);
				}
			}


			//If this clip has an end time we can add it's end time to the map
			if (locClip->end() != L"") {
				//There's a specified end time on this clip, so hold on to it
				__int64 locEndTime = StringHelper::stringToNum(StringHelper::toNarrowStr(locClip->end()));
				mTrackMap.insert(tTrackMap::value_type(locTrackName, locEndTime));
			}
			
			mCMMLSourcePin->deliverTag(locClip);
		} else {
			mCMMLSourcePin->DeliverEndOfStream();
			return S_OK;
		}
		mUptoTag++;

	}
	return S_OK;

}
//IMEdiaStreaming
STDMETHODIMP CMMLRawSourceFilter::Run(REFERENCE_TIME tStart) {
	CAutoLock locLock(m_pLock);
	return CBaseFilter::Run(tStart);
}
STDMETHODIMP CMMLRawSourceFilter::Pause(void) {
	CAutoLock locLock(m_pLock);
	if (m_State == State_Stopped) {
		if (ThreadExists() == FALSE) {
			Create();
		}
		CallWorker(THREAD_RUN);
	}

	HRESULT locHR = CBaseFilter::Pause();
	return locHR;
	
}
STDMETHODIMP CMMLRawSourceFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	CallWorker(THREAD_EXIT);
	Close();
	//mJustSeeked = true;
	//mSeekRequest = 0;
	mUptoTag = -1;
	mCMMLSourcePin->DeliverBeginFlush();
	mCMMLSourcePin->DeliverEndFlush();
	return CBaseFilter::Stop();
}