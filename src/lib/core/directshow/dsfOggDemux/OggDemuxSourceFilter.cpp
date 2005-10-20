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

#include "stdafx.h"
#include "oggdemuxsourcefilter.h"

//#define OGGCODECS_LOGGING

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
    },

	{ 
		L"illiminable About Page",				// Name
	    &CLSID_PropsAbout,						// CLSID
	    PropsAbout::CreateInstance,				// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


//COM Creator Function
CUnknown* WINAPI OggDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	OggDemuxSourceFilter *pNewObject = new OggDemuxSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

//COM Interface query function
STDMETHODIMP OggDemuxSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSourceFilter) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	/*} else if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;*/
	} else if (riid == IID_ISpecifyPropertyPages) {
		*ppv = (ISpecifyPropertyPages*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}  else if (riid == IID_IAMFilterMiscFlags) {
		*ppv = (IAMFilterMiscFlags*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IAMMediaContent) {
		//debugLog<<"Queries for IAMMediaContent///"<<endl;
		*ppv = (IAMMediaContent*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//------------------

//ANX::: This needs to be changed so these details are passed into the constructor. Or add another parametised constructo
OggDemuxSourceFilter::OggDemuxSourceFilter()
	:	CBaseFilter(NAME("OggDemuxSourceFilter"), NULL, m_pLock, CLSID_OggDemuxSourceFilter)
	
	,	mSeekTable(NULL)
	,	mDataSource(NULL)
	,	mSeekTimeBase(0)
	,	mJustReset(true)
{
	//LEAK CHECK:::Both get deleted in constructor.
	m_pLock = new CCritSec;
	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamLock = new CCritSec;
	mStreamMapper = new OggStreamMapper(this);

#ifdef OGGCODECS_LOGGING
	debugLog.open("d:\\zen\\logs\\sourcelog.log", ios_base::out);
#endif

}

//Perhaps also pass in the name field.
OggDemuxSourceFilter::OggDemuxSourceFilter(REFCLSID inFilterGUID)
	:	CBaseFilter(NAME("OggDemuxSourceFilter"), NULL, m_pLock, inFilterGUID)
	,	mSeekTable(NULL)
	,	mStreamMapper(NULL)
	,	mSeekTimeBase(0)
	,	mJustReset(true)
{
	//LEAK CHECK:::Both get deleted in constructor.
	m_pLock = new CCritSec;
	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamLock = new CCritSec;
#ifdef OGGCODECS_LOGGING
	debugLog.open("d:\\zen\\logs\\anx_base_sourcelog.log", ios_base::out);
#endif

	//When it is derived, it's up to the superclass to set this.
	//mStreamMapper = new OggStreamMapper(this);

}

OggDemuxSourceFilter::~OggDemuxSourceFilter(void)
{
	//TODO::: For some reason, you can't delete these !!

	//Clean up all our stuff...
	//delete m_pLock;
	//delete mStreamLock;
	//delete mSourceFileLock;
	//delete mDemuxLock;
	//debugLog<<"Deleting Data Source : "<<(int)mDataSource<<endl;

	//Close down the data source and delete it

	if (mDataSource != NULL) {
		mDataSource->close();
	}
	delete mDataSource;

	debugLog.close();
	
	//Selete the stream mapper
	delete mStreamMapper;
	mStreamMapper = NULL;

	//Shut down the thread
	if (ThreadExists() == TRUE) {
		//DbgLog((LOG_ERROR, 1, TEXT("******** Thread exists - closing *****")));
		Close();
	}
	//Delete the seektable
	delete mSeekTable;
	mSeekTable = NULL;
}

//IAMFilterMiscFlags Interface
ULONG OggDemuxSourceFilter::GetMiscFlags(void) 
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}
//ISpecifyPropertyPgaes Interface
STDMETHODIMP OggDemuxSourceFilter::GetPages(CAUUID* outPropPages) 
{
	//This function is to display a property page in graphedit.
	if (outPropPages == NULL) return E_POINTER;

	const int NUM_PROP_PAGES = 1;
    outPropPages->cElems = NUM_PROP_PAGES;
    outPropPages->pElems = (GUID*)(CoTaskMemAlloc(sizeof(GUID) * NUM_PROP_PAGES));
    if (outPropPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }

	outPropPages->pElems[0] = CLSID_PropsAbout;
    
    return S_OK;

}

	//IFileSource Interface
STDMETHODIMP OggDemuxSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
	//Return the filename and mediatype of the raw data
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}


STDMETHODIMP OggDemuxSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	debugLog<<"Loading : "<<StringHelper::toNarrowStr(mFileName)<<endl;

	debugLog << "Opening source file : "<<StringHelper::toNarrowStr(mFileName)<<endl;
	mSeekTable = new AutoOggSeekTable(StringHelper::toNarrowStr(mFileName));
	mSeekTable->buildTable();
	
	return SetUpPins();
}

STDMETHODIMP OggDemuxSourceFilter::GetCapabilities(DWORD* inCapabilities) 
{
	if (mSeekTable->enabled())  {
		//debugLog<<"GetCaps "<<mSeekingCap<<endl;
		*inCapabilities = mSeekingCap;
		return S_OK;
	} else {
		//debugLog<<"Get Caps failed !!!!!!!"<<endl;
		*inCapabilities = 0;
		return S_OK;;
	}
}
STDMETHODIMP OggDemuxSourceFilter::GetDuration(LONGLONG* outDuration) 
{
	if (mSeekTable->enabled())  {
		//debugLog<<"GetDuration = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*outDuration = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}

}
	 
STDMETHODIMP OggDemuxSourceFilter::CheckCapabilities(DWORD *pCapabilities)
{
	//debugLog<<"CheckCaps	: Not impl"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::IsFormatSupported(const GUID *pFormat)
{
	ASSERT(pFormat != NULL);
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		//debugLog<<"IsFormatSupported	: TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsFormatSupported	: FALSE !!!"<<endl;
		return S_FALSE;
	}
	
	
}
STDMETHODIMP OggDemuxSourceFilter::QueryPreferredFormat(GUID *pFormat){
	//debugLog<<"QueryPrefferedTimeFormat	: MEDIA TIME"<<endl;
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::SetTimeFormat(const GUID *pFormat){
	//debugLog<<"SetTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::GetTimeFormat( GUID *pFormat){
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::GetStopPosition(LONGLONG *pStop){
	if (mSeekTable->enabled())  {

		//debugLog<<"GetStopPos = " << mSeekTable->fileDuration()<<" ds units"<<endl;
		*pStop = mSeekTable->fileDuration();
		return S_OK;
	} else {
		//debugLog<<"GetStopPos NOT IMPL"<<endl;
		return E_NOTIMPL;
	}
}
STDMETHODIMP OggDemuxSourceFilter::GetCurrentPosition(LONGLONG *pCurrent)
{
	//TODO::: Implement this properly

	//debugLog<<"GetCurrentPos = NOT_IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat){
	//debugLog<<"ConvertTimeForamt : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags){


	CAutoLock locLock(m_pLock);
	//debugLog<<"Set Positions "<<*pCurrent<<" to "<<*pStop<<" with flags "<<dwCurrentFlags<<" and "<<dwStopFlags<<endl;
	if (mSeekTable->enabled())  {
		//debugLog<<"SetPos : Current = "<<*pCurrent<<" Flags = "<<dwCurrentFlags<<" Stop = "<<*pStop<<" dwStopFlags = "<<dwStopFlags<<endl;
		//debugLog<<"       : Delivering begin flush..."<<endl;

	
		CAutoLock locSourceLock(mSourceFileLock);
		mSetIgnorePackets = false;
		DeliverBeginFlush();
		//debugLog<<"       : Begin flush Delviered."<<endl;

		//Find the byte position for this time.
		OggSeekTable::tSeekPair locStartPos = mSeekTable->getStartPos(*pCurrent);
		bool locSendExcess = false;

		//FIX::: This code needs to be removed, and handle start seek case.
		//.second is the file position.
		//.first is the time in DS units
		if (locStartPos.second == mStreamMapper->startOfData()) {
			locSendExcess = true;
			//GGFF:::
			//mStreamMapper->toStartOfData();
			mSetIgnorePackets = true;
		}
		
		
		//We have to save this here now... since time can't be reverted to granule pos in all cases
		// we have to use granule pos timestamps in order for downstream codecs to work.
		// Because of this we can't factor time bases after seeking into the sample times.
		*pCurrent	= mSeekTimeBase 
					= locStartPos.first;		//Time from seek pair.

		//debugLog<<"Corrected pCurrent : "<<mSeekTimeBase<<endl;
		for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
			mStreamMapper->getOggStream(i)->setSendExcess(locSendExcess);		//Not needed
			mStreamMapper->getOggStream(i)->setLastEndGranPos(*pCurrent);
		}
		{
			//debugLog<<"       : Delivering End Flush..."<<endl;
			DeliverEndFlush();
			//debugLog<<"       : End flush Delviered."<<endl;
			DeliverNewSegment(*pCurrent, mSeekTable->fileDuration(), 1.0);
		}

		//.second is the file position.
		mDataSource->seek(locStartPos.second);
	
		//debugLog<<"       : Seek complete."<<endl;
	} else {
		//debugLog<<"Seek not IMPL"<<endl;
		return E_NOTIMPL;
	}

	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
	//debugLog<<"Getpos : Not IMPL"<<endl;
	//debugLog<<"GetPos : Current = HARDCODED 2 secs , Stop = "<<mSeekTable->fileDuration()/UNITS <<" secs."<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest){
	//debugLog<<"****GetAvailable : NOT IMPL"<<endl;
	if (mSeekTable->enabled())  {
		//debugLog<<"Get Avail ok"<<endl;
		*pEarliest = 0;
		//debugLog<<"+++++ Duration is "<<mSeekTable->fileDuration()<<endl;
		*pLatest = mSeekTable->fileDuration();
		return S_OK;
	} else {
		return E_NOTIMPL;
	}
}
STDMETHODIMP OggDemuxSourceFilter::SetRate(double dRate)
{
	//debugLog<<"Set RATE : NOT IMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::GetRate(double *dRate)
{

	*dRate = 1.0;
	return S_OK;;
}
STDMETHODIMP OggDemuxSourceFilter::GetPreroll(LONGLONG *pllPreroll)
{

	*pllPreroll = 0;
	//debugLog<<"GetPreroll : HARD CODED TO 0"<<endl;
	return S_OK;
}
STDMETHODIMP OggDemuxSourceFilter::IsUsingTimeFormat(const GUID *pFormat){
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		//debugLog<<"IsUsingTimeFormat : MEDIA TIME TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsUsingTimeFormat : MEDIA TIME FALSE !!!!"<<endl;
		return S_FALSE;
	}
}


//BaseFilter Interface
int OggDemuxSourceFilter::GetPinCount() 
{
	return mStreamMapper->numStreams();
}
CBasePin* OggDemuxSourceFilter::GetPin(int inPinNo) 
{
	//The cast in thesecond condition removes a warning C4018 signed/unsigned mismatch.
	// Since the first condition would short circuit if inPinNo was < 0, the cast is safe.
	if ((inPinNo >= 0) && ((unsigned long)inPinNo < mStreamMapper->numStreams())) {
		return mStreamMapper->getOggStream(inPinNo)->getPin();
	} else {
		return NULL;
	}
}

//CAMThread Stuff
DWORD OggDemuxSourceFilter::ThreadProc(void) {
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

//Helper methods

void OggDemuxSourceFilter::resetStream() {
	{
		debugLog<<"Reset stream pre-lock"<<endl;
		CAutoLock locDemuxLock(mDemuxLock);
		CAutoLock locSourceLock(mSourceFileLock);
		debugLog<<"RestStream post-lock"<<endl;

		//Close up the data source
		mDataSource->clear();

		debugLog<<"Pre close"<<endl;
		mDataSource->close();
		debugLog<<"Post close"<<endl;

		//After closing kill the interface
		delete mDataSource;
		mDataSource = NULL;
		

		//Clearing the data out of the demuxer
		//mOggBuffer.debugWrite("%%%%%% Clear calling from ResetStream");
		mOggBuffer.clearData();

		//Before opening make the interface
		mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(mFileName).c_str());

		debugLog<<"Pre open"<<endl;
		mDataSource->open(StringHelper::toNarrowStr(mFileName).c_str());
		debugLog<<"Post open"<<endl;
		mDataSource->seek(mStreamMapper->startOfData());   //Should always be zero for now.

		//TODO::: Should be doing stuff with the demux state here ? or packetiser ?>?
		
		mJustReset = true;   //TODO::: Look into this !
	}

	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->setSendExcess(true);	
	}
}

void OggDemuxSourceFilter::DeliverBeginFlush() 
{
	CAutoLock locLock(m_pLock);
	
	debugLog << "Delivering Begin Flush"<<endl;
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverBeginFlush();
		//mStreamMapper->getOggStream(i)->flush();
	}

	//Should this be here or endflush or neither ?
	//mOggBuffer.debugWrite("%%%%%% Reset calling from DeliverBegingFlush");
	debugLog<<"Calling reset stream from begin flush"<<endl;
	resetStream();
}

void OggDemuxSourceFilter::DeliverEndFlush() 
{
	CAutoLock locLock(m_pLock);
	debugLog << "Delivering End Flush"<<endl;
	if (mSetIgnorePackets == true) {
		mStreamMapper->toStartOfData();
		for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
			//mStreamMapper->getOggStream(i)->flush();
			mStreamMapper->getOggStream(i)->getPin()->DeliverEndFlush();
		}

	} else {
	
		for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
			mStreamMapper->getOggStream(i)->flush();
			mStreamMapper->getOggStream(i)->getPin()->DeliverEndFlush();
		}
	}
	mSetIgnorePackets = false;
}
void OggDemuxSourceFilter::DeliverEOS() 
{
	mStreamMapper->toStartOfData();
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getOggStream(i)->getPin()->DeliverEndOfStream();
		
	}
	//mOggBuffer.debugWrite("%%%%%% Reset calling from DeliverEOS");
	debugLog<<"Calling reset stream from DeliverEOS"<<endl;
	resetStream();
}

void OggDemuxSourceFilter::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) 
{
	debugLog<<"DeliverNewSegment : Delivering start = "<<tStart<<" end = "<< tStop<<"rate = "<<dRate<<endl;
	
	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
		mStreamMapper->getOggStream(i)->getPin()->DeliverNewSegment(tStart, tStop, dRate);
	}
}
HRESULT OggDemuxSourceFilter::DataProcessLoop() 
{
	//Mess with the locking mechanisms at your own risk.

	//debugLog<<"Starting DataProcessLoop :"<<endl;
	DWORD locCommand = 0;
	char* locBuff = new  char[4096];			//Deleted before function returns...
	//TODO::: Make this a member variable ^^^^^
	bool locKeepGoing = true;
	unsigned long locBytesRead = 0;
	bool locIsEOF = true;
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
			locKeepGoing = ((mOggBuffer.feed((const unsigned char*)locBuff, locBytesRead)) == (OggDataBuffer::FEED_OK));;
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


HRESULT OggDemuxSourceFilter::SetUpPins() 
{
	CAutoLock locDemuxLock(mDemuxLock);
	CAutoLock locSourceLock(mSourceFileLock);
	
	unsigned short locRetryCount = 0;
	const unsigned short RETRY_THRESHOLD = 3;
	debugLog<<"SETUP PINS"<<endl;
	//Create and open a data source
	mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(mFileName).c_str());
	mDataSource->open(StringHelper::toNarrowStr(mFileName).c_str());
	
	//Error check
	
	//Register a callback
	mOggBuffer.registerVirtualCallback(this);

	char* locBuff = new char[RAW_BUFFER_SIZE];
	unsigned long locNumRead = 0;

	//Feed the data in until we have seen all BOS pages.
	while(!mStreamMapper->isReady()) {
	
		locNumRead = mDataSource->read(locBuff, RAW_BUFFER_SIZE);
	
		if (locNumRead > 0) {
			mOggBuffer.feed((const unsigned char*)locBuff, locNumRead);
		}

		if (mDataSource->isEOF() || mDataSource->isError()) {
			if (mDataSource->isError() && (mDataSource->shouldRetryAt() != "") && (locRetryCount < RETRY_THRESHOLD)) {
				mOggBuffer.clearData();
				string locNewLocation = mDataSource->shouldRetryAt();
				debugLog<<"Retrying at : "<<locNewLocation<<endl;
				delete mDataSource;
				mDataSource = DataSourceFactory::createDataSource(locNewLocation.c_str());
				mDataSource->open(locNewLocation.c_str());
				locRetryCount++;
			} else {
				debugLog<<"Bailing out"<<endl;
				return VFW_E_CANNOT_RENDER;
			}
		}
	}
	
	mStreamMapper->setAllowDispatch(true);
	mStreamMapper->toStartOfData();			//Flushes all streams and sets them to ignore the right number of headers.
	mOggBuffer.clearData();
	mDataSource->seek(0);			//TODO::: This is bad for streams.

	//debugLog<<"COMPLETED SETUP"<<endl;
	delete[] locBuff;
	return S_OK;
}
//IOggCallback Interface

bool OggDemuxSourceFilter::acceptOggPage(OggPage* inOggPage)	//Gives away page.
{		
	return mStreamMapper->acceptOggPage(inOggPage);
}

//IMEdiaStreaming
STDMETHODIMP OggDemuxSourceFilter::Run(REFERENCE_TIME tStart) 
{
	const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	debugLog<<"Run  :  time = "<<tStart<<endl;
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP OggDemuxSourceFilter::Pause(void) 
{
	CAutoLock locLock(m_pLock);
	debugLog << "** Pause called **"<<endl;
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
STDMETHODIMP OggDemuxSourceFilter::Stop(void) 
{
	CAutoLock locLock(m_pLock);
	debugLog<<"** Stop Called ** "<<endl;
	CallWorker(THREAD_EXIT);
	Close();
	DeliverBeginFlush();
	mSetIgnorePackets = true;
	DeliverEndFlush();
	
	return CBaseFilter::Stop();
}

CCritSec* OggDemuxSourceFilter::theLock() 
{
	return m_pLock;
}


//IAMMediaContent Interface
STDMETHODIMP  OggDemuxSourceFilter::get_AuthorName(BSTR* outAuthorName) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_Title(BSTR* outTitle) { 
	//debugLog<<"Try to get_Title"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_Rating(BSTR* outRating) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_Description(BSTR* outDescription) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_Copyright(BSTR* outCopyright) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_BaseURL(BSTR* outBaseURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_LogoURL(BSTR* outLogoURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_LogoIconURL(BSTR* outLogoIconURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_WatermarkURL(BSTR* outWatermarkURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_MoreInfoURL(BSTR* outMoreInfoURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_MoreInfoBannerImage(BSTR* outMoreInfoBannerImage) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_MoreInfoBannerURL(BSTR* outMoreInfoBannerURL) { 
	return E_NOTIMPL;
}
STDMETHODIMP  OggDemuxSourceFilter::get_MoreInfoText(BSTR* outMoreInfoText) { 
	return E_NOTIMPL;
}

//IDispatch Interface
STDMETHODIMP OggDemuxSourceFilter::GetTypeInfoCount(	unsigned int FAR*  pctinfo ) {
	return E_NOTIMPL;

}
STDMETHODIMP OggDemuxSourceFilter::GetIDsOfNames(		REFIID  riid, 
								OLECHAR FAR* FAR* rgszNames, 
								unsigned int cNames, 
								LCID lcid, 
								DISPID FAR* rgDispId ) {
	return E_NOTIMPL;
								}
STDMETHODIMP OggDemuxSourceFilter::GetTypeInfo(		unsigned int iTInfo, 
								LCID lcid, 
								ITypeInfo FAR* FAR*  ppTInfo ) {
	return E_NOTIMPL;
}
STDMETHODIMP OggDemuxSourceFilter::Invoke(			DISPID  dispIdMember,
								REFIID  riid,
								LCID  lcid,
								WORD  wFlags,
								DISPPARAMS FAR*  pDispParams,  
								VARIANT FAR*  pVarResult,  
								EXCEPINFO FAR*  pExcepInfo,  
								unsigned int FAR*  puArgErr ) {
	return E_NOTIMPL;
}
