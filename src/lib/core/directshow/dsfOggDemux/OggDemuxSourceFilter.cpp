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

#include "StdAfx.h"
#include "oggdemuxsourcefilter.h"

//-------------------
// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"OggDemuxFilter",						// Name
	    &CLSID_OggDemuxSourceFilter,            // CLSID
	    OggDemuxSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



CUnknown* WINAPI OggDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	OggDemuxSourceFilter *pNewObject = new OggDemuxSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

STDMETHODIMP OggDemuxSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSourceFilter) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;


	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//------------------

OggDemuxSourceFilter::OggDemuxSourceFilter()
	:	CBaseFilter(NAME("OggDemucSourceFilter"), NULL, m_pLock, CLSID_OggDemuxSourceFilter)
	,	mSeekTable(NULL)
{
	//LEAK CHECK:::Both get deleted in constructor.
	m_pLock = new CCritSec;
	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamMapper = new OggStreamMapper(this);
	debugLog.open("C:\\TEMP\\sourcelog.log", ios_base::out|ios_base::binary);
	debugLog << "**************** Starting LOg ********************"<<endl;
}

OggDemuxSourceFilter::~OggDemuxSourceFilter(void)
{
	//DbgLog((LOG_ERROR, 1, TEXT("****************** DESTRUCTOR **********************")));
	delete m_pLock;
	delete mSourceFileLock;
	delete mDemuxLock;
	delete mStreamMapper;

	
	if (ThreadExists() == TRUE) {
		//DbgLog((LOG_ERROR, 1, TEXT("******** Thread exists - closing *****")));
		Close();
	}

}


	//IFileSource Interface
STDMETHODIMP OggDemuxSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	debugLog << "Opening source file : "<<StringHelper::toNarrowStr(mFileName)<<endl;
	mSeekTable = new AutoOggSeekTable(StringHelper::toNarrowStr(mFileName));
	mSeekTable->buildTable();
	
	return SetUpPins();
}

STDMETHODIMP OggDemuxSourceFilter::GetCapabilities(DWORD* inCapabilities) {
	if (mSeekTable->enabled())  {
		debugLog<<"GetCaps "<<mSeekingCap<<endl;
		*inCapabilities = mSeekingCap;
		return S_OK;
	} else {
		*inCapabilities = 0;
		return S_OK;;
	}
}
STDMETHODIMP OggDemuxSourceFilter::GetDuration(LONGLONG* outDuration) {
	if (mSeekTable->enabled())  {
		debugLog<<"GetDuration = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*outDuration = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}

}




	 
STDMETHODIMP OggDemuxSourceFilter::CheckCapabilities(DWORD *pCapabilities){
	debugLog<<"CheckCaps	: Not imp"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::IsFormatSupported(const GUID *pFormat){
	ASSERT(pFormat != NULL);
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		debugLog<<"IsFormatSupported	: TRUE"<<endl;
		return S_OK;
	} else {
		debugLog<<"IsFormatSupported	: FALSE !!!"<<endl;
		return S_FALSE;
	}
	
	
}
STDMETHODIMP OggDemuxSourceFilter::QueryPreferredFormat(GUID *pFormat){
	debugLog<<"QueryPrefferedTimeFormat	: MEDIA TIME"<<endl;
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::SetTimeFormat(const GUID *pFormat){
	debugLog<<"SetTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::GetTimeFormat( GUID *pFormat){
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::GetStopPosition(LONGLONG *pStop){
	if (mSeekTable->enabled())  {

		debugLog<<"GetStopPos = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*pStop = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}
}
STDMETHODIMP OggDemuxSourceFilter::GetCurrentPosition(LONGLONG *pCurrent){

	debugLog <<"GetCurrPos = HARD CODED = 6 secs"<< endl;
	 *pCurrent = 6 * UNITS;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat){
	debugLog<<"ConvertTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags){
	CAutoLock locLock(m_pLock);
	if (mSeekTable->enabled())  {
		debugLog<<"SetPos : Current = "<<*pCurrent<<" Flags = "<<dwCurrentFlags<<" Stop = "<<*pStop<<" dwStopFlags = "<<dwStopFlags<<endl;
		debugLog<<"       : Delivering begin flush..."<<endl;

	
		CAutoLock locSourceLock(mSourceFileLock);
		
		DeliverBeginFlush();
		debugLog<<"       : Begin flush Delviered."<<endl;
		debugLog<<"       : Delivering new segemnt"<<endl;


		//FIX ATTEMPT::: Part of time stamping fixes.. we don't use absolute times any more... this is out !
		//IReferenceClock* locRefClock = NULL;
		//HRESULT locHR = GetSyncSource(&locRefClock);

		//LONGLONG locCurrentTime;
		//locRefClock->GetTime(&locCurrentTime);
		//DeliverNewSegment(locCurrentTime, locCurrentTime + mSeekTable->fileDuration(), 1.0);
		
		//END FIX


		debugLog<<"       : NewSegment Delviered."<<endl;
		unsigned long locStartPos = mSeekTable->getStartPos(*pCurrent);
		bool locSendExcess = false;
		if (locStartPos == mStreamMapper->startOfData()) {
			locSendExcess = true;
		}
		debugLog<<"       : Seeking to position "<<mSeekTable->getStartPos(*pCurrent)<<endl;
		{
			CAutoLock locDemuxLock(mDemuxLock);
			mOggBuffer.clearData();
		}
	
		mSourceFile.seekg(mSeekTable->getStartPos(*pCurrent), ios_base::beg);
		*pCurrent = mSeekTable->getRealStartPos();
	
	
		debugLog<<"       : Seek complete."<<endl;
		debugLog<<"       : Notifying whether to send excess... ";
		if (locSendExcess) {
			debugLog<<"YES"<<endl;
		} else {
			debugLog<<"NO"<<endl;
		}
		for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
			mStreamMapper->getOggStream(i)->setSendExcess(locSendExcess);
			mStreamMapper->getOggStream(i)->setLastEndGranPos(mSeekTable->getRealStartPos());
		}
	} else {
		return E_NOTIMPL;
	}
	

	//Run(locCurrentTime);
	debugLog<<"       : Delivering End Flush..."<<endl;
	DeliverEndFlush();
	debugLog<<"       : End flush Delviered."<<endl;
	DeliverNewSegment(*pCurrent, *pCurrent + mSeekTable->fileDuration(), 1.0);
	//DeliverBeginFlush();

	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop){
	debugLog<<"GetPos : Current = HARDCODED 2 secs , Stop = "<<mSeekTable->fileDuration()/UNITS <<" secs."<<endl;
	*pCurrent = 2 * UNITS;
	*pStop = mSeekTable->fileDuration();
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest){
	debugLog<<"GetAvailable : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::SetRate(double dRate){
	return E_NOTIMPL;
	debugLog<<"SetRate : "<<dRate<<endl;
	return S_OK;;
}
STDMETHODIMP OggDemuxSourceFilter::GetRate(double *dRate){
	
	return E_NOTIMPL;
	debugLog <<"GetRate : Hard coded to 1.0"<<endl;
	*dRate = 1.0;
	return S_OK;;
}
STDMETHODIMP OggDemuxSourceFilter::GetPreroll(LONGLONG *pllPreroll){
	return E_NOTIMPL;
	*pllPreroll = 0;
	debugLog<<"GetPreroll : HARD CODED TO 0"<<endl;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::IsUsingTimeFormat(const GUID *pFormat){
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		debugLog<<"IsUsingTimeFormat : MEDIA TIME TRUE"<<endl;
		return S_OK;
	} else {
		debugLog<<"IsUsingTimeFormat : MEDIA TIME FALSE !!!!"<<endl;
		return S_FALSE;
	}
	
}


//BaseFilter Interface
int OggDemuxSourceFilter::GetPinCount() {
	return mStreamMapper->numStreams();
}
CBasePin* OggDemuxSourceFilter::GetPin(int inPinNo) {
	//if (inPinNo >= 0 && inPinNo < mOutputPins.size()) {
	//	return mOutputPins[inPinNo];
	//} else {
	//	return NULL;
	//}

	if (inPinNo >= 0 && inPinNo < mStreamMapper->numStreams()) {
		return mStreamMapper->getOggStream(inPinNo)->getPin();
	} else {
		return NULL;
	}
}

//CAMThread Stuff
DWORD OggDemuxSourceFilter::ThreadProc(void) {
	while(true) {
		DWORD locThreadCommand = GetRequest();
		switch(locThreadCommand) {
			case THREAD_EXIT:
				Reply(S_OK);
				return S_OK;

			//case THREAD_PAUSE:
			//	// we are paused already
			//	Reply(S_OK);
			//	break;

			case THREAD_RUN:
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
	
	
	}
	return S_OK;
}

//Helper methods

void OggDemuxSourceFilter::resetStream() {
	{

		CAutoLock locSourceLock(mSourceFileLock);
		CAutoLock locDemuxLock(mDemuxLock);
		mSourceFile.clear();
		mSourceFile.close();
		mOggBuffer.clearData();
		mSourceFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::in|ios_base::binary);

		mSourceFile.seekg(mStreamMapper->startOfData(), ios_base::beg);
	}
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->setSendExcess(true);	
	}
}



void OggDemuxSourceFilter::DeliverBeginFlush() {
	CAutoLock locLock(m_pLock);
	debugLog << "Delivering Begin Flush"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverBeginFlush();
	}

	//Should this be here or endflush or neither ?
	debugLog << "Calling Reset Stream"<<endl;
	resetStream();

}

void OggDemuxSourceFilter::DeliverEndFlush() {
	CAutoLock locLock(m_pLock);
	debugLog << "Delivering End Flush"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverEndFlush();
	}
	
}
void OggDemuxSourceFilter::DeliverEOS() {
	
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverEndOfStream();
	}

	resetStream();
}

void OggDemuxSourceFilter::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) {
	debugLog<<"DeliverNewSegment : Delivering start = "<<tStart<<" end = "<< tStop<<"rate = "<<dRate<<endl;


	IReferenceClock* locRefClock = NULL;
	HRESULT locHR = GetSyncSource(&locRefClock);
	LONGLONG locCurrentTime;
	locRefClock->GetTime(&locCurrentTime);

	
	m_tStart = locCurrentTime;
	
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverNewSegment(tStart, tStop, dRate);
	}
}
HRESULT OggDemuxSourceFilter::DataProcessLoop() {
	DWORD locCommand = 0;
	char* locBuff = new  char[4096];
	bool locKeepGoing = true;
	unsigned long locBytesRead = 0;
	bool locIsEOF = true;
	{
		CAutoLock locSourceLock(mSourceFileLock);
		locIsEOF = mSourceFile.eof();
	}
	while (!locIsEOF && locKeepGoing) {
		if(CheckRequest(&locCommand) == TRUE) {
			debugLog<<"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			return S_OK;
		}

		{
			CAutoLock locSourceLock(mSourceFileLock);
			debugLog << "DataProcessLoop : Getpointer = "<<mSourceFile.tellg()<<endl;
			mSourceFile.read(locBuff, 4096);
        	locBytesRead = mSourceFile.gcount();
		}
		debugLog <<"DataProcessLoop : gcount = "<<locBytesRead<<endl;
		{
			CAutoLock locDemuxLock(mDemuxLock);
			locKeepGoing = mOggBuffer.feed(locBuff, locBytesRead);
		}
		if (!locKeepGoing) {
			debugLog << "DataProcessLoop : Feed in data buffer said stop"<<endl;
		}
		{
			CAutoLock locSourceLock(mSourceFileLock);
			locIsEOF = mSourceFile.eof();
		}
		if (locIsEOF) {
			debugLog << "DataProcessLoop : EOF"<<endl;
		}
	}

	debugLog<<"DataProcessLoop : Exiting. Deliver EOS"<<endl;
	DeliverEOS();

	//Shuold we flush ehre ?
	delete locBuff;
	//Memory leak
	//FIXED
	
	//return value ??
}
HRESULT OggDemuxSourceFilter::SetUpPins() {
	
	CAutoLock locSourceLock(mSourceFileLock);
	CAutoLock locDemuxLock(mDemuxLock);
	mSourceFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::in|ios_base::binary);
	
	//Error check
	
	//Register a callback
	mOggBuffer.registerVirtualCallback(this);

	char* locBuff = new char[RAW_BUFFER_SIZE];
	
	//Feed the data in until we have seen all BOS pages.
	while(!mStreamMapper->isReady()) {
		mSourceFile.read(locBuff, RAW_BUFFER_SIZE);
		mOggBuffer.feed(locBuff, RAW_BUFFER_SIZE);

	}
	//Memory leak
	//FIXED
	delete locBuff;
	return S_OK;
}
//IOggCallback Interface

bool OggDemuxSourceFilter::acceptOggPage(OggPage* inOggPage) {
	return mStreamMapper->acceptOggPage(inOggPage);
}

//IMEdiaStreaming
STDMETHODIMP OggDemuxSourceFilter::Run(REFERENCE_TIME tStart) {
	const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	debugLog<<"Run  :  time = "<<tStart<<endl;
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP OggDemuxSourceFilter::Pause(void) {
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
STDMETHODIMP OggDemuxSourceFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	CallWorker(THREAD_EXIT);
	Close();
	DeliverBeginFlush();
	DeliverEndFlush();
	return CBaseFilter::Stop();
}
