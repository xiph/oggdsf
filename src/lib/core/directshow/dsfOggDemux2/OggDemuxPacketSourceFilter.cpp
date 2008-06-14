//===========================================================================
//Copyright (C) 2003, 2004, 2005 Zentaro Kavanagh
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
#include "OggDemuxPacketSourceFilter.h"
#include "OggStreamMapper.h"

// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"OggDemuxFilter",						// Name
	    &CLSID_OggDemuxPacketSourceFilter,            // CLSID
	    OggDemuxPacketSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
#ifdef WINCE
		&OggDemuxPacketSourceFilterReg
#else
        NULL									// Set-up information (for filters)
#endif
    }
	
	//,

	//{ 
	//	L"illiminable About Page",				// Name
	//    &CLSID_PropsAbout,						// CLSID
	//    PropsAbout::CreateInstance,				// Method to create an instance of MyComponent
 //       NULL,									// Initialization function
 //       NULL									// Set-up information (for filters)
 //   }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


#ifdef WINCE
LPAMOVIESETUP_FILTER OggDemuxPacketSourceFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&OggDemuxPacketSourceFilterReg;	
}
#endif
//COM Creator Function
CUnknown* WINAPI OggDemuxPacketSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	OggDemuxPacketSourceFilter *pNewObject = new OggDemuxPacketSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 
//COM Interface query function
STDMETHODIMP OggDemuxPacketSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{

	//TODO::: Possibly want to add a check when someone queries for ICustomSource and then disallow IFileSource
	//			and vice versa, but that could cause a problem if applications just want to query out of
	//			curiosity but not actually use it.
	//
	//			For now, using ICustomSource is pretty much unsupported, so if you are using it, you just have to
	//			be careful you don't try and load a file twice by accident.
	if ((riid == IID_IFileSourceFilter)) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_ICustomSource) {
		*ppv = (ICustomSource*)this;
		//((IUnknown*)*ppv)->AddRef();
		return NOERROR;


	//} else if (riid == IID_IMediaSeeking) {
	//	*ppv = (IMediaSeeking*)this;
	//	((IUnknown*)*ppv)->AddRef();
	//	return NOERROR;
	/*} else if (riid == IID_ISpecifyPropertyPages) {
		*ppv = (ISpecifyPropertyPages*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	*/
	}  else if (riid == IID_IAMFilterMiscFlags) {
		*ppv = (IAMFilterMiscFlags*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	//} else if (riid == IID_IAMMediaContent) {
	//	//debugLog<<"Queries for IAMMediaContent///"<<endl;
	//	*ppv = (IAMMediaContent*)this;
	//	((IUnknown*)*ppv)->AddRef();
	//	return NOERROR;
	}  else if (riid == IID_IOggBaseTime) {
		*ppv = (IOggBaseTime*)this;
		//((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}


	

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}
OggDemuxPacketSourceFilter::OggDemuxPacketSourceFilter(void)
	:	CBaseFilter(NAME("OggDemuxPacketSourceFilter"), NULL, m_pLock, CLSID_OggDemuxPacketSourceFilter)
	,	mDataSource(NULL)
	,	mSeenAllBOSPages(false)
	,	mSeenPositiveGranulePos(false)
	,	mPendingPage(NULL)
	,	mJustReset(true)
	,	mSeekTable(NULL)
	,	mGlobalBaseTime(0)

	,	mUsingCustomSource(false)

{
    debugLog.open(L"c:\\demux.log", ios_base::out);
	debugLog<<L"Constructor"<<endl;

	//Why do we do this, should the base class do it ?
	m_pLock = new CCritSec;

	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamLock = new CCritSec;

	mStreamMapper = new OggStreamMapper(this, m_pLock);

	
}

OggDemuxPacketSourceFilter::~OggDemuxPacketSourceFilter(void)
{
	debugLog<<L"Destructor"<<endl;
	debugLog.close();
	delete mStreamMapper;
	delete mSeekTable;
	//TODO::: Delete the locks



	delete mDemuxLock;
	delete mStreamLock;
	delete mSourceFileLock;

	mDataSource->close();
	delete mDataSource;
}
//IMEdiaStreaming
STDMETHODIMP OggDemuxPacketSourceFilter::Run(REFERENCE_TIME tStart) 
{
	CAutoLock locLock(m_pLock);
    debugLog<<L"Run ------- "<<endl;
	return CBaseFilter::Run(tStart);

	

}
STDMETHODIMP OggDemuxPacketSourceFilter::Pause(void) 
{
	CAutoLock locLock(m_pLock);
    debugLog<<L"Pause post-lock"<<endl;
	if (m_State == State_Stopped) {
        debugLog<<L"Pause -- was stopped"<<endl;
		if (ThreadExists() == FALSE) {
            debugLog<<L"Pause -- CREATING THREAD"<<endl;
			Create();
		}
        debugLog<<L"Pause -- RUNNING THREAD"<<endl;
		CallWorker(THREAD_RUN);
	}
	HRESULT locHR = CBaseFilter::Pause();

    debugLog<<L"Pause ()() COMPLETE"<<endl;
	
	return locHR;
	
}
STDMETHODIMP OggDemuxPacketSourceFilter::Stop(void) 
{
	CAutoLock locLock(m_pLock);
    debugLog<<L"Stop -- KILLING!! THREAD"<<endl;
	CallWorker(THREAD_EXIT);
	Close();
	DeliverBeginFlush();
	//mSetIgnorePackets = true;
	DeliverEndFlush();
	
	return CBaseFilter::Stop();


}
void OggDemuxPacketSourceFilter::DeliverBeginFlush() 
{
	CAutoLock locLock(m_pLock);
    debugLog<<"%%% Begin Flush"<<endl;
	
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		mStreamMapper->getPinByIndex(i)->DeliverBeginFlush();
	}

	//Should this be here or endflush or neither ?
	
	//debugLog<<"Calling reset stream from begin flush"<<endl;
	resetStream();
}

void OggDemuxPacketSourceFilter::DeliverEndFlush() 
{
	CAutoLock locLock(m_pLock);
    debugLog<<L"$$$ End Flush"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndFlush();
	}

}
void OggDemuxPacketSourceFilter::DeliverEOS() 
{
	//mStreamMapper->toStartOfData();
    CAutoLock locStreamLock(mStreamLock);
    debugLog<<L"### Deliver EOS"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndOfStream();
		
	}
	//debugLog<<"Calling reset stream from DeliverEOS"<<endl;
	resetStream();
}

void OggDemuxPacketSourceFilter::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) 
{
    CAutoLock locStreamLock(mStreamLock);
	debugLog<<L"Deliver New Segment"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		mStreamMapper->getPinByIndex(i)->DeliverNewSegment(tStart, tStop, dRate);
	}
}

void OggDemuxPacketSourceFilter::resetStream() {
	{
        
		CAutoLock locDemuxLock(mDemuxLock);
		CAutoLock locSourceLock(mSourceFileLock);
        debugLog<<L"---RESET STREAM::: post locks"<<endl;

		mOggBuffer.clearData();



		//For a custom data source, we send it a clear request to reset any error state.
		//For normal source, we close down the source and re-open it.
		mDataSource->clear();

		if (!mUsingCustomSource) {
			mDataSource->close();
			delete mDataSource;
			mDataSource = NULL;

			//Before opening make the interface
			mDataSource = DataSourceFactory::createDataSource(mFileName);

			mDataSource->open(mFileName);
		}
		mDataSource->seek(0);   //Should always be zero for now.

		//TODO::: Should be doing stuff with the demux state here ? or packetiser ?>?
		
		mJustReset = true;   //TODO::: Look into this !
        debugLog<<L"---RESET STREAM::: JUST RESET = TRUE"<<endl;
	}
}
bool OggDemuxPacketSourceFilter::acceptOggPage(OggPage* inOggPage)
{
	if (!mSeenAllBOSPages) {
		if (!inOggPage->header()->isBOS()) {
			mSeenAllBOSPages = true;
			mBufferedPages.push_back(inOggPage);
			return true;
		} else {
			debugLog<<"Found BOS"<<endl;
			return mStreamMapper->acceptOggPage(inOggPage);
		}
	} else if (!mSeenPositiveGranulePos) {
		if (inOggPage->header()->GranulePos() > 0) {
			mSeenPositiveGranulePos = true;
		}
		mBufferedPages.push_back(inOggPage);
		return true;
	} else {
		//OGGCHAIN::: Here, need to check for an eos, and reset stream, else do it in strmapper
		return mStreamMapper->acceptOggPage(inOggPage);
	}
}
HRESULT OggDemuxPacketSourceFilter::SetUpPins()
{
	
	CAutoLock locDemuxLock(mDemuxLock);
	CAutoLock locSourceLock(mSourceFileLock);
	
	debugLog<<L"Setup Pins - Post lock"<<endl;
	unsigned short locRetryCount = 0;
	const unsigned short RETRY_THRESHOLD = 3;

	//For custom sources, we expect that the source will be provided open and ready
	if (!mUsingCustomSource) {
		//Create and open a data source if we are using the standard source.

		debugLog<<L"Pre data source creation"<<endl;
		//mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(mFileName).c_str());
        mDataSource = DataSourceFactory::createDataSource(mFileName);
		debugLog<<L"Post data source creation"<<endl;
		if (mDataSource == NULL) {
			return VFW_E_CANNOT_RENDER;
		}
		
		if (!mDataSource->open(mFileName)) {
			return VFW_E_CANNOT_RENDER;
		}
	} else {
		//For custom sources seek to the start, just in case
		mDataSource->seek(0);
	}
	
	//Error check
	
	//Register a callback
	mOggBuffer.registerVirtualCallback(this);

	char* locBuff = new char[SETUP_BUFFER_SIZE];
	unsigned long locNumRead = 0;

	//Feed the data in until we have seen all BOS pages.
	while(!mSeenPositiveGranulePos) {			//mStreamMapper->allStreamsReady()) {
	
		locNumRead = mDataSource->read(locBuff, SETUP_BUFFER_SIZE);
	
		if (locNumRead > 0) {
			mOggBuffer.feed((const unsigned char*)locBuff, locNumRead);
		}

		if (mDataSource->isEOF() || mDataSource->isError()) {
			if (mDataSource->isError() && (mDataSource->shouldRetryAt() != L"") && (locRetryCount < RETRY_THRESHOLD) && (!mUsingCustomSource)) {
				mOggBuffer.clearData();
				wstring locNewLocation = mDataSource->shouldRetryAt();
				//debugLog<<"Retrying at : "<<locNewLocation<<endl;
				delete mDataSource;
				mDataSource = DataSourceFactory::createDataSource(locNewLocation);
				mDataSource->open(locNewLocation);
				locRetryCount++;
			//This prevents us dying on small files, if we hit eof but we also saw a +'ve gran pos, this file is ok.
			} else if (!(mDataSource->isEOF() && mSeenPositiveGranulePos)) {
				debugLog<<L"Bailing out"<<endl;
				delete[] locBuff;
				return VFW_E_CANNOT_RENDER;
			}
		}
	}
	
	//mStreamMapper->setAllowDispatch(true);
	//mStreamMapper->();			//Flushes all streams and sets them to ignore the right number of headers.
	mOggBuffer.clearData();
	//mDataSource->clear();
	mDataSource->seek(0);			//TODO::: This is bad for streams.

	debugLog<<"COMPLETED SETUP"<<endl;
	delete[] locBuff;
	return S_OK;


}

vector<OggPage*> OggDemuxPacketSourceFilter::getMatchingBufferedPages(unsigned long inSerialNo)
{
	vector<OggPage*> locList;
	for (size_t i = 0; i < mBufferedPages.size(); i++) {
		if (mBufferedPages[i]->header()->StreamSerialNo() == inSerialNo) {
			locList.push_back(mBufferedPages[i]->clone());
		}
	}
	return locList;
}
void OggDemuxPacketSourceFilter::removeMatchingBufferedPages(unsigned long inSerialNo)
{
	vector<OggPage*> locNewList;
	int locSize = mBufferedPages.size();
	for (int i = 0; i < locSize; i++) {
		if (mBufferedPages[i]->header()->StreamSerialNo() != inSerialNo) {
			locNewList.push_back(mBufferedPages[i]);
		} else {
			delete mBufferedPages[i];
		}
	}
	mBufferedPages = locNewList;

}



int OggDemuxPacketSourceFilter::GetPinCount() 
{
	//TODO::: Implement
	return mStreamMapper->numPins();
}
CBasePin* OggDemuxPacketSourceFilter::GetPin(int inPinNo) 
{
	if (inPinNo < 0) {
		return NULL;
	}
	return mStreamMapper->getPinByIndex(inPinNo);
}

HRESULT OggDemuxPacketSourceFilter::setCustomSourceAndLoad(IFilterDataSource* inDataSource)
{
	CAutoLock locLock(m_pLock);
	mDataSource = inDataSource;
	mFileName = L"";
	mUsingCustomSource = true;

	return SetUpPins();
}
//IFileSource Interface
STDMETHODIMP OggDemuxPacketSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
	////Return the filename and mediatype of the raw data
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;

	//TODO:::
	
	return S_OK;
}


STDMETHODIMP OggDemuxPacketSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	
	////Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);

	debugLog<<L"Load - post lock"<<endl;

	
	mFileName = inFileName;

	debugLog<<L"File :"<<mFileName<<endl;

	if (mFileName.find(L"XsZZfQ__WiiPFD.anx") == mFileName.size() - 18){
		mFileName = mFileName.substr(0, mFileName.size() - 18);
		
	}

	//debugLog<<"Loading : "<<StringHelper::toNarrowStr(mFileName)<<endl;

	//debugLog << "Opening source file : "<<StringHelper::toNarrowStr(mFileName)<<endl;
	//mSeekTable = new AutoOggSeekTable(StringHelper::toNarrowStr(mFileName));
	//mSeekTable->buildTable();
	//
	HRESULT locHR = SetUpPins();

	if (locHR == S_OK) {
		//mSeekTable = new AutoOggChainGranuleSeekTable(StringHelper::toNarrowStr(mFileName));
		//int locNumPins = GetPinCount();

		//OggDemuxPacketSourcePin* locPin = NULL;
		//for (int i = 0; i < locNumPins; i++) {
		//	locPin = (OggDemuxPacketSourcePin*)GetPin(i);
		//	
		//	
		//	mSeekTable->addStream(locPin->getSerialNo(), locPin->getDecoderInterface());
		//}
		//mSeekTable->buildTable();

		debugLog<<L"Load OK"<<endl;
		return S_OK;
	} else {
		debugLog<<L"Load Fail "<<locHR<<endl;
		return locHR;
	}

	//TODO:::
	//return S_OK;
}

//IAMFilterMiscFlags Interface
ULONG OggDemuxPacketSourceFilter::GetMiscFlags(void) 
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

//CAMThread Stuff
DWORD OggDemuxPacketSourceFilter::ThreadProc(void) {
	
	while(true) {
		DWORD locThreadCommand = GetRequest();
	
		switch(locThreadCommand) {
			case THREAD_EXIT:
	
				Reply(S_OK);
                debugLog<<L"Thread Proc --- THREAD IS EXITING"<<endl;
				return S_OK;

			case THREAD_RUN:
	
				Reply(S_OK);
				DataProcessLoop();
                debugLog<<L"Thread Proc --- Data Process Loop has returnsed"<<endl;
				break;
		}
	}
	return S_OK;
}

void OggDemuxPacketSourceFilter::notifyPinConnected()
{
	debugLog<<L"Notify pin connected"<<endl;
	if (mStreamMapper->allStreamsReady()) {
		//Setup the seek table.
		if (mSeekTable == NULL) {
			//CUSTOM SOURCE:::
			if (!mUsingCustomSource) {
				debugLog<<L"Setting up seek table"<<endl;
				//ZZUNICODE:::
				//mSeekTable = new AutoOggChainGranuleSeekTable(StringHelper::toNarrowStr(mFileName));
				mSeekTable = new AutoOggChainGranuleSeekTable(mFileName);
				debugLog<<L"After Setting up seek table"<<endl;
			} else {
				mSeekTable = new CustomOggChainGranuleSeekTable(mDataSource);
			}
			int locNumPins = GetPinCount();

			OggDemuxPacketSourcePin* locPin = NULL;
			for (int i = 0; i < locNumPins; i++) {
				locPin = (OggDemuxPacketSourcePin*)GetPin(i);
				
				debugLog<<L"Adding decoder interface to sek table"<<endl;
				mSeekTable->addStream(locPin->getSerialNo(), locPin->getDecoderInterface());
			}
			debugLog<<L"Pre seek table build"<<endl;
//#ifndef WINCE
			mSeekTable->buildTable();
//#else
			//mSeekTable->disableTable();
//#endif
			debugLog<<L"Post seek table build"<<endl;
		}
	}
}
HRESULT OggDemuxPacketSourceFilter::DataProcessLoop() 
{
	//Mess with the locking mechanisms at your own risk.


	//debugLog<<"Starting DataProcessLoop :"<<endl;
	DWORD locCommand = 0;
	char* locBuff = new  char[4096];			//Deleted before function returns...
	//TODO::: Make this a member variable ^^^^^
	bool locKeepGoing = true;
	unsigned long locBytesRead = 0;
	bool locIsEOF = true;

	OggDataBuffer::eFeedResult locFeedResult;

	{
		CAutoLock locSourceLock(mSourceFileLock);
		locIsEOF = mDataSource->isEOF();
	}

	bool continueLooping = true;
	while(continueLooping) {
		if(CheckRequest(&locCommand) == TRUE) {
			debugLog<<L"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			delete[] locBuff;
			return S_OK;
		}
		//debugLog<<"Looping..."<<endl;
		{
			CAutoLock locSourceLock(mSourceFileLock);


			locBytesRead = mDataSource->read(locBuff, 4096);
			mJustReset = false;
		}

		try
		{
			//debugLog <<"DataProcessLoop : gcount = "<<locBytesRead<<endl;
			{
				CAutoLock locDemuxLock(mDemuxLock);
				//CAutoLock locStreamLock(mStreamLock);
				if (mJustReset) {		//To avoid blocking problems... restart the loop if it was just reset while waiting for lock.
					debugLog<<L"DataProcessLoop : Detected JustRest condition"<<endl;
					continue;
				}
				locFeedResult = mOggBuffer.feed((const unsigned char*)locBuff, locBytesRead);
				locKeepGoing = ((locFeedResult == (OggDataBuffer::FEED_OK)) || (locFeedResult == OggDataBuffer::PROCESS_DISPATCH_FALSE));;
				if (locFeedResult != OggDataBuffer::FEED_OK)
				{
					debugLog << L"Feed result = "<<locFeedResult<<endl;
					break;
				}
			}
			//if (!locKeepGoing) {
			//	//debugLog << "DataProcessLoop : Feed in data buffer said stop"<<endl;
	  //          CAutoLock locStreamLock(mStreamLock);
			//	debugLog<<L"DataProcessLoop : Keep going false Deliver EOS"<<endl;
	  //          debugLog<<L"Feed Result = "<<locFeedResult<<endl;
			//	DeliverEOS();
			//}
			{
				CAutoLock locSourceLock(mSourceFileLock);
				locIsEOF = mDataSource->isEOF();
			}
		}
		catch (int)
		{
			locIsEOF = true;
			continueLooping = false;
		}

		if (locIsEOF) {
			//debugLog << "DataProcessLoop : EOF"<<endl;
            CAutoLock locStreamLock(mStreamLock);
			debugLog<<L"DataProcessLoop : EOF Deliver EOS"<<endl;
			DeliverEOS();
		}
	}

	debugLog<<L"DataProcessLoop : Left loop., balinig out"<<endl;

	//Shuold we flush ehre ?
	delete[] locBuff;
	
	//return value ??
	return S_OK;
}




STDMETHODIMP OggDemuxPacketSourceFilter::GetCapabilities(DWORD* inCapabilities) 
{
	if ((mSeekTable != NULL) && (mSeekTable->enabled()))  {
		//debugLog<<"GetCaps "<<mSeekingCap<<endl;
		*inCapabilities = mSeekingCap;
		return S_OK;
	} else {
		//debugLog<<"Get Caps failed !!!!!!!"<<endl;
		*inCapabilities = 0;
		return S_OK;;
	}
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetDuration(LONGLONG* outDuration) 
{
	if ((mSeekTable != NULL) && (mSeekTable->enabled())) {
		//debugLog<<"GetDuration = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*outDuration = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}


}
	 
STDMETHODIMP OggDemuxPacketSourceFilter::CheckCapabilities(DWORD *pCapabilities)
{
	//debugLog<<"CheckCaps	: Not impl"<<endl;

	//TODO:::
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::IsFormatSupported(const GUID *pFormat)
{
	//ASSERT(pFormat != NULL);
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		//debugLog<<"IsFormatSupported	: TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsFormatSupported	: FALSE !!!"<<endl;
		return S_FALSE;
	}


	
}
STDMETHODIMP OggDemuxPacketSourceFilter::QueryPreferredFormat(GUID *pFormat){
	//debugLog<<"QueryPrefferedTimeFormat	: MEDIA TIME"<<endl;
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxPacketSourceFilter::SetTimeFormat(const GUID *pFormat){
	//debugLog<<"SetTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetTimeFormat( GUID *pFormat){
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetStopPosition(LONGLONG *pStop){
	if ((mSeekTable != NULL) && (mSeekTable->enabled()))  {

		//debugLog<<"GetStopPos = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*pStop = mSeekTable->fileDuration();
		return S_OK;
	} else {
		//debugLog<<"GetStopPos NOT IMPL"<<endl;
		return E_NOTIMPL;
	}



}
STDMETHODIMP OggDemuxPacketSourceFilter::GetCurrentPosition(LONGLONG *pCurrent)
{
	//TODO::: Implement this properly

	//debugLog<<"GetCurrentPos = NOT_IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat){
	//debugLog<<"ConvertTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags){


	CAutoLock locLock(m_pLock);
	
	if ((mSeekTable != NULL) && (mSeekTable->enabled()))  {
	
		
		CAutoLock locSourceLock(mSourceFileLock);
		DeliverBeginFlush();
		

		//Find the byte position for this time.
		if (*pCurrent > mSeekTable->fileDuration()) {
			*pCurrent = mSeekTable->fileDuration();
		} else if (*pCurrent < 0) {
			*pCurrent = 0;
		}

		OggGranuleSeekTable::tSeekPair locStartPos = mSeekTable->seekPos(*pCurrent);
		
		
		//For now, seek to the position directly, later we will discard the preroll
		//Probably don't ever want to do this. We want to record the desired time,
		//	and it will be up to the decoders to drop anything that falss before it.
		//*pCurrent = locStartPos.first;

		{
			//debugLog<<"       : Delivering End Flush..."<<endl;
			DeliverEndFlush();
			//debugLog<<"       : End flush Delviered."<<endl;
			DeliverNewSegment(*pCurrent, mSeekTable->fileDuration(), 1.0);
		}

		//.second is the file position.
		mDataSource->seek(locStartPos.second.first);
	
		return S_OK;
	} else {
		//debugLog<<"Seek not IMPL"<<endl;
		return E_NOTIMPL;
	}



}
STDMETHODIMP OggDemuxPacketSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
	//debugLog<<"Getpos : Not IMPL"<<endl;
	//debugLog<<"GetPos : Current = HARDCODED 2 secs , Stop = "<<mSeekTable->fileDuration()/UNITS <<" secs."<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest){
	//debugLog<<"****GetAvailable : NOT IMPL"<<endl;
	if ((mSeekTable != NULL) && (mSeekTable->enabled()))  {
		//debugLog<<"Get Avail ok"<<endl;
		*pEarliest = 0;
		//debugLog<<"+++++ Duration is "<<mSeekTable->fileDuration()<<endl;
		*pLatest = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}


}
STDMETHODIMP OggDemuxPacketSourceFilter::SetRate(double dRate)
{
	//debugLog<<"Set RATE : NOT IMPL"<<endl;

	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetRate(double *dRate)
{

	*dRate = 1.0;
	return S_OK;;
}
STDMETHODIMP OggDemuxPacketSourceFilter::GetPreroll(LONGLONG *pllPreroll)
{

	*pllPreroll = 0;
	//debugLog<<"GetPreroll : HARD CODED TO 0"<<endl;
	return S_OK;
}
STDMETHODIMP OggDemuxPacketSourceFilter::IsUsingTimeFormat(const GUID *pFormat) {
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		//debugLog<<"IsUsingTimeFormat : MEDIA TIME TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsUsingTimeFormat : MEDIA TIME FALSE !!!!"<<endl;
		return S_FALSE;
	}


}


//HHHH:::
bool OggDemuxPacketSourceFilter::notifyStreamBaseTime(__int64 inStreamBaseTime)
{
	if (inStreamBaseTime > mGlobalBaseTime) {
		mGlobalBaseTime = inStreamBaseTime;
	}
	return true;
}
__int64 OggDemuxPacketSourceFilter::getGlobalBaseTime()
{
	return mGlobalBaseTime;
}


