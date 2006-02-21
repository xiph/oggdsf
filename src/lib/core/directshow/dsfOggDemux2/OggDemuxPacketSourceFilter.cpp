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
        NULL									// Set-up information (for filters)
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
	if (riid == IID_IFileSourceFilter) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
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
{
	//Why do we do this, should the base class do it ?
	m_pLock = new CCritSec;

	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamLock = new CCritSec;

	mStreamMapper = new OggStreamMapper(this, m_pLock);

	
}

OggDemuxPacketSourceFilter::~OggDemuxPacketSourceFilter(void)
{
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
	return CBaseFilter::Run(tStart);

	

}
STDMETHODIMP OggDemuxPacketSourceFilter::Pause(void) 
{
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
STDMETHODIMP OggDemuxPacketSourceFilter::Stop(void) 
{
	CAutoLock locLock(m_pLock);
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
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndFlush();
	}

	
	//if (mSetIgnorePackets == true) {
	//	mStreamMapper->toStartOfData();
	//	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
	//		//mStreamMapper->getOggStream(i)->flush();
	//		mStreamMapper->getOggStream(i)->getPin()->DeliverEndFlush();
	//	}

	//} else {
	//
	//	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
	//		mStreamMapper->getOggStream(i)->flush();
	//		mStreamMapper->getOggStream(i)->getPin()->DeliverEndFlush();
	//	}
	//}
	//mSetIgnorePackets = false;
}
void OggDemuxPacketSourceFilter::DeliverEOS() 
{
	//mStreamMapper->toStartOfData();
	//CAutoLock locLock(m_pLock);
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndOfStream();
		
	}
	//debugLog<<"Calling reset stream from DeliverEOS"<<endl;
	resetStream();
}

void OggDemuxPacketSourceFilter::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) 
{
	
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) {
		mStreamMapper->getPinByIndex(i)->DeliverNewSegment(tStart, tStop, dRate);
	}
}

void OggDemuxPacketSourceFilter::resetStream() {
	{
		CAutoLock locDemuxLock(mDemuxLock);
		CAutoLock locSourceLock(mSourceFileLock);

		//Close up the data source
		mDataSource->clear();

		mDataSource->close();
		delete mDataSource;
		mDataSource = NULL;
		

		mOggBuffer.clearData();

		//Before opening make the interface
		mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(mFileName).c_str());

		mDataSource->open(StringHelper::toNarrowStr(mFileName).c_str());
		mDataSource->seek(0);   //Should always be zero for now.

		//TODO::: Should be doing stuff with the demux state here ? or packetiser ?>?
		
		mJustReset = true;   //TODO::: Look into this !
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
	
	unsigned short locRetryCount = 0;
	const unsigned short RETRY_THRESHOLD = 3;

	//Create and open a data source
	mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(mFileName).c_str());
	mDataSource->open(StringHelper::toNarrowStr(mFileName).c_str());
	
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
			if (mDataSource->isError() && (mDataSource->shouldRetryAt() != "") && (locRetryCount < RETRY_THRESHOLD)) {
				mOggBuffer.clearData();
				string locNewLocation = mDataSource->shouldRetryAt();
				//debugLog<<"Retrying at : "<<locNewLocation<<endl;
				delete mDataSource;
				mDataSource = DataSourceFactory::createDataSource(locNewLocation.c_str());
				mDataSource->open(locNewLocation.c_str());
				locRetryCount++;
			} else {
				//debugLog<<"Bailing out"<<endl;
				delete[] locBuff;
				return VFW_E_CANNOT_RENDER;
			}
		}
	}
	
	//mStreamMapper->setAllowDispatch(true);
	//mStreamMapper->();			//Flushes all streams and sets them to ignore the right number of headers.
	mOggBuffer.clearData();
	mDataSource->seek(0);			//TODO::: This is bad for streams.

	//debugLog<<"COMPLETED SETUP"<<endl;
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



	mFileName = inFileName;

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
		return S_OK;
	} else {
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
				return S_OK;

			case THREAD_RUN:
	
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
	}
	return S_OK;
}

void OggDemuxPacketSourceFilter::notifyPinConnected()
{
	if (mStreamMapper->allStreamsReady()) {
		//Setup the seek table.
		if (mSeekTable == NULL) {
			mSeekTable = new AutoOggChainGranuleSeekTable(StringHelper::toNarrowStr(mFileName));
			int locNumPins = GetPinCount();

			OggDemuxPacketSourcePin* locPin = NULL;
			for (int i = 0; i < locNumPins; i++) {
				locPin = (OggDemuxPacketSourcePin*)GetPin(i);
				
				
				mSeekTable->addStream(locPin->getSerialNo(), locPin->getDecoderInterface());
			}
			mSeekTable->buildTable();
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

	while(true) {
		if(CheckRequest(&locCommand) == TRUE) {
			//debugLog<<"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			delete[] locBuff;
			return S_OK;
		}
		//debugLog<<"Looping..."<<endl;
		{
			CAutoLock locSourceLock(mSourceFileLock);


			locBytesRead = mDataSource->read(locBuff, 4096);
			mJustReset = false;
		}
		//debugLog <<"DataProcessLoop : gcount = "<<locBytesRead<<endl;
		{
			CAutoLock locDemuxLock(mDemuxLock);
			//CAutoLock locStreamLock(mStreamLock);
			if (mJustReset) {		//To avoid blocking problems... restart the loop if it was just reset while waiting for lock.
				continue;
			}
			locFeedResult = mOggBuffer.feed((const unsigned char*)locBuff, locBytesRead);
			locKeepGoing = ((locFeedResult == (OggDataBuffer::FEED_OK)) || (locFeedResult == OggDataBuffer::PROCESS_DISPATCH_FALSE));;
		}
		if (!locKeepGoing) {
			//debugLog << "DataProcessLoop : Feed in data buffer said stop"<<endl;
			//debugLog<<"DataProcessLoop : Exiting. Deliver EOS"<<endl;
			DeliverEOS();
		}
		{
			CAutoLock locSourceLock(mSourceFileLock);
			locIsEOF = mDataSource->isEOF();
		}
		if (locIsEOF) {
			//debugLog << "DataProcessLoop : EOF"<<endl;
			//debugLog<<"DataProcessLoop : Exiting. Deliver EOS"<<endl;
			DeliverEOS();
		}
	}

	//debugLog<<"DataProcessLoop : Exiting. Deliver EOS"<<endl;

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


