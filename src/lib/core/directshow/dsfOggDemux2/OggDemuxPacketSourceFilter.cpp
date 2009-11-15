//===========================================================================
//Copyright (C) 2003, 2004, 2005 Zentaro Kavanagh
//Copyright (C) 2009 Cristian Adam
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
#include "ds_guids.h"
#include "common/Log.h"
#include "common/util.h"

// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
        OggDemuxPacketSourceFilter::NAME,			// Name
	    &CLSID_OggDemuxPacketSourceFilter,          // CLSID
	    OggDemuxPacketSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									    // Initialization function
        &OggDemuxPacketSourceFilter::m_filterReg    // Set-up information (for filters)
    }
};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

const wchar_t* OggDemuxPacketSourceFilter::NAME = L"Xiph.Org Ogg Demux Source";

const AMOVIESETUP_PIN OggDemuxPacketSourceFilter::m_pinReg = 
{	
    L"Ogg Packet Out",					//Name (obsoleted)
    FALSE,								//Renders from this pin ?? Not sure about this.
    TRUE,								//Is an output pin
    TRUE,								//Can have zero instances of this pin
    TRUE,								//Can have more than one instance of this pin
    &GUID_NULL,							//Connects to filter (obsoleted)
    NULL,								//Connects to pin (obsoleted)
    1,									//Only support one media type
    &m_mediaTypes	                    //Pointer to media type (Audio/Vorbis or Audio/Speex)	
};

const AMOVIESETUP_FILTER OggDemuxPacketSourceFilter::m_filterReg = 
{
    &CLSID_OggDemuxPacketSourceFilter,  // Filter CLSID.
    NAME,                               // Filter name.
    MERIT_NORMAL,                       // Merit.
    1,                                  // Number of pin types.
    &m_pinReg                           // Pointer to pin information.
};

const AMOVIESETUP_MEDIATYPE OggDemuxPacketSourceFilter::m_mediaTypes = 
{
    &MEDIATYPE_OggPacketStream,
    &MEDIASUBTYPE_None
};


#ifdef WINCE
LPAMOVIESETUP_FILTER OggDemuxPacketSourceFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&OggDemuxPacketSourceFilterReg;	
}
#endif

//COM Creator Function
CUnknown* WINAPI OggDemuxPacketSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
    util::ConfigureLogSettings();

    OggDemuxPacketSourceFilter *pNewObject = new (std::nothrow) OggDemuxPacketSourceFilter();
    if (pNewObject == NULL) 
    {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

OggDemuxPacketSourceFilter::OggDemuxPacketSourceFilter()
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
	LOG(logDEBUG) << L"Creating OggDemuxPacketSourceFilter object";

	//Why do we do this, should the base class do it ?
	m_pLock = new CCritSec;

	mSourceFileLock = new CCritSec;
	mDemuxLock = new CCritSec;
	mStreamLock = new CCritSec;

	mStreamMapper = new OggStreamMapper(this, m_pLock);
}

OggDemuxPacketSourceFilter::~OggDemuxPacketSourceFilter()
{
	LOG(logDEBUG) << L"Destroying OggDemuxPacketSourceFilter";

    delete mStreamMapper;
	delete mSeekTable;
	//TODO::: Delete the locks

	delete mDemuxLock;
	delete mStreamLock;
	delete mSourceFileLock;

	if (mDataSource)
	{
		mDataSource->close();
		delete mDataSource;
	}
}

STDMETHODIMP OggDemuxPacketSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    LOG(logDEBUG) << L"NonDelegatingQueryInterface: " << riid;

    //TODO::: Possibly want to add a check when someone queries for ICustomSource and then disallow IFileSource
    //			and vice versa, but that could cause a problem if applications just want to query out of
    //			curiosity but not actually use it.
    //
    //			For now, using ICustomSource is pretty much unsupported, so if you are using it, you just have to
    //			be careful you don't try and load a file twice by accident.

    if (riid == IID_IFileSourceFilter) 
    {
        return GetInterface((IFileSourceFilter*)this, ppv);
    } 
    else if (riid == IID_ICustomSource) 
    {
        return GetInterface((ICustomSource*)this, ppv);
    }  
    else if (riid == IID_IAMFilterMiscFlags) 
    {
        return GetInterface((IAMFilterMiscFlags*)this, ppv);
    }  
    else if (riid == IID_IOggBaseTime) 
    {
        return GetInterface((IOggBaseTime*)this, ppv);
    }

    return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}

//IMEdiaStreaming
STDMETHODIMP OggDemuxPacketSourceFilter::Run(REFERENCE_TIME tStart) 
{
	CAutoLock locLock(m_pLock);
    LOG(logDEBUG) << "Run: " << ReferenceTime(tStart);
	
    return CBaseFilter::Run(tStart);
}

STDMETHODIMP OggDemuxPacketSourceFilter::Pause() 
{
	CAutoLock locLock(m_pLock);
    LOG(logDEBUG) << L"Pause";

	if (m_State == State_Stopped) 
    {
        LOG(logDEBUG) <<L"Pause -- was stopped";

		if (ThreadExists() == FALSE) 
        {
            LOG(logDEBUG) << L"Pause -- CREATING THREAD";
			Create();
		}

        LOG(logDEBUG) << L"Pause -- RUNNING THREAD";
		CallWorker(THREAD_RUN);
	}

	HRESULT hr = CBaseFilter::Pause();

    LOG(logDEBUG) << L"Pause() COMPLETE. Base class returned: 0x" << std::hex << hr;
	
	return hr;	
}

STDMETHODIMP OggDemuxPacketSourceFilter::Stop(void) 
{
	CAutoLock locLock(m_pLock);
    LOG(logDEBUG) << L"Stop -- Closing worker THREAD";

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
    LOG(logDEBUG) << L"DeliverBeginFlush";
	
	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) 
    {
		mStreamMapper->getPinByIndex(i)->DeliverBeginFlush();
	}

	//Should this be here or endflush or neither ?
	resetStream();
}

void OggDemuxPacketSourceFilter::DeliverEndFlush() 
{
	CAutoLock locLock(m_pLock);
    LOG(logDEBUG) << L"DeliverEndFlush";

	for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) 
    {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndFlush();
	}
}

void OggDemuxPacketSourceFilter::DeliverEOS() 
{
	//mStreamMapper->toStartOfData();
    CAutoLock locStreamLock(mStreamLock);
    LOG(logDEBUG) <<L"Deliver EOS";
	
    for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) 
    {
		//mStreamMapper->getOggStream(i)->flush();
		mStreamMapper->getPinByIndex(i)->DeliverEndOfStream();
	}

    resetStream();
}

void OggDemuxPacketSourceFilter::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) 
{
    CAutoLock locStreamLock(mStreamLock);
	LOG(logDEBUG) << L"DeliverNewSegment";
	
    for (unsigned long i = 0; i < mStreamMapper->numPins(); i++) 
    {
		mStreamMapper->getPinByIndex(i)->DeliverNewSegment(tStart, tStop, dRate);
	}
}

void OggDemuxPacketSourceFilter::resetStream() 
{
	{   
		CAutoLock locDemuxLock(mDemuxLock);
		CAutoLock locSourceLock(mSourceFileLock);
        LOG(logDEBUG) << L"---RESET STREAM::: post locks";

		mOggBuffer.clearData();

		//For a custom data source, we send it a clear request to reset any error state.
		//For normal source, we close down the source and re-open it.
		mDataSource->clear();

		if (!mUsingCustomSource) 
        {
			mDataSource->close();
			delete mDataSource;
			mDataSource = NULL;

			//Before opening make the interface
			mDataSource = DataSourceFactory::createDataSource(m_fileName);

			mDataSource->open(m_fileName);
		}
		
        mDataSource->seek(0);   //Should always be zero for now.

		//TODO::: Should be doing stuff with the demux state here ? or packetiser ?>?
		
		mJustReset = true;   //TODO::: Look into this !
        LOG(logDEBUG) << L"---RESET STREAM::: JUST RESET = TRUE";
	}
}

bool OggDemuxPacketSourceFilter::acceptOggPage(OggPage* inOggPage)
{
	if (!mSeenAllBOSPages) 
    {
		if (!inOggPage->header()->isBOS()) 
        {
			mSeenAllBOSPages = true;
			mBufferedPages.push_back(inOggPage);
			return true;
		} 
        else 
        {
			LOG(logDEBUG) << "Found BOS";
			return mStreamMapper->acceptOggPage(inOggPage);
		}
	} 
    else if (!mSeenPositiveGranulePos) 
    {
		if (inOggPage->header()->GranulePos() > 0) 
        {
			mSeenPositiveGranulePos = true;
		}
		
        mBufferedPages.push_back(inOggPage);
		return true;
	} 
    else 
    {
		//OGGCHAIN::: Here, need to check for an eos, and reset stream, else do it in strmapper
		return mStreamMapper->acceptOggPage(inOggPage);
	}
}

HRESULT OggDemuxPacketSourceFilter::SetUpPins()
{
	CAutoLock locDemuxLock(mDemuxLock);
	CAutoLock locSourceLock(mSourceFileLock);
	
	LOG(logDEBUG) << L"Setup Pins - Post lock";

	unsigned short locRetryCount = 0;
	const unsigned short RETRY_THRESHOLD = 3;

	//For custom sources, we expect that the source will be provided open and ready
	if (!mUsingCustomSource) 
    {
		//Create and open a data source if we are using the standard source.

		LOG(logDEBUG) << L"Pre data source creation";

		//mDataSource = DataSourceFactory::createDataSource(StringHelper::toNarrowStr(m_fileName).c_str());
        mDataSource = DataSourceFactory::createDataSource(m_fileName);

		LOG(logDEBUG) << L"Post data source creation";

		if (mDataSource == NULL) 
        {
			return VFW_E_CANNOT_RENDER;
		}
		
		if (!mDataSource->open(m_fileName)) 
        {
			return VFW_E_CANNOT_RENDER;
		}
	} 
    else 
    {
		//For custom sources seek to the start, just in case
		mDataSource->seek(0);
	}
	
	//Error check
	
	//Register a callback
	mOggBuffer.registerVirtualCallback(this);

	char* locBuff = new char[SETUP_BUFFER_SIZE];
	unsigned long locNumRead = 0;

	//Feed the data in until we have seen all BOS pages.
	while (!mSeenPositiveGranulePos) 
    {			
        //mStreamMapper->allStreamsReady()) {
	
		locNumRead = mDataSource->read(locBuff, SETUP_BUFFER_SIZE);
	
		if (locNumRead > 0) 
        {
			mOggBuffer.feed((const unsigned char*)locBuff, locNumRead);
		}

		if (mDataSource->isEOF() || mDataSource->isError()) 
        {
			if (mDataSource->isError() && (mDataSource->shouldRetryAt() != L"") && (locRetryCount < RETRY_THRESHOLD) && !mUsingCustomSource) 
            {
				mOggBuffer.clearData();
				wstring locNewLocation = mDataSource->shouldRetryAt();

				//debugLog<<"Retrying at : "<<locNewLocation<<endl;
				delete mDataSource;
				mDataSource = DataSourceFactory::createDataSource(locNewLocation);
				mDataSource->open(locNewLocation);
				locRetryCount++;
			    //This prevents us dying on small files, if we hit eof but we also saw a +'ve gran pos, this file is ok.
			} 
            else if (!(mDataSource->isEOF() && mSeenPositiveGranulePos)) 
            {
				LOG(logDEBUG) << L"Bailing out";

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

	LOG(logDEBUG) << "COMPLETED SETUP";

	delete[] locBuff;
	
    return S_OK;
}

vector<OggPage*> OggDemuxPacketSourceFilter::getMatchingBufferedPages(unsigned long inSerialNo)
{
	vector<OggPage*> locList;
	for (size_t i = 0; i < mBufferedPages.size(); i++) 
    {
		if (mBufferedPages[i]->header()->StreamSerialNo() == inSerialNo) 
        {
			locList.push_back(mBufferedPages[i]->clone());
		}
	}
	
    return locList;
}

void OggDemuxPacketSourceFilter::removeMatchingBufferedPages(unsigned long inSerialNo)
{
	vector<OggPage*> locNewList;
	int locSize = mBufferedPages.size();
	
    for (int i = 0; i < locSize; i++) 
    {
		if (mBufferedPages[i]->header()->StreamSerialNo() != inSerialNo) 
        {
			locNewList.push_back(mBufferedPages[i]);
		} 
        else 
        {
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
	if (inPinNo < 0) 
    {
		return NULL;
	}
	return mStreamMapper->getPinByIndex(inPinNo);
}

HRESULT OggDemuxPacketSourceFilter::setCustomSourceAndLoad(IFilterDataSource* inDataSource)
{
	CAutoLock locLock(m_pLock);
	mDataSource = inDataSource;
	m_fileName = L"";
	mUsingCustomSource = true;

	return SetUpPins();
}

//IFileSource Interface
STDMETHODIMP OggDemuxPacketSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
    CheckPointer(outFileName, E_POINTER);
    *outFileName = NULL;

    if (!m_fileName.empty()) 
    {
    	unsigned int size  = sizeof(WCHAR) * (m_fileName.size() + 1);

        *outFileName = (LPOLESTR) CoTaskMemAlloc(size);
        if (*outFileName != NULL) 
        {
            CopyMemory(*outFileName, m_fileName.c_str(), size);
        }
    }
	
	return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	CAutoLock locLock(m_pLock);
	
	m_fileName = inFileName;

    LOG(logINFO) << L"OggDemuxPacketSourceFilter::Load(" << m_fileName << L")";

	if (m_fileName.find(L"XsZZfQ__WiiPFD.anx") == m_fileName.size() - 18)
    {
		m_fileName = m_fileName.substr(0, m_fileName.size() - 18);	
	}

	HRESULT hr = SetUpPins();
	if (FAILED(hr)) 
    {
        LOG(logDEBUG) << L"Load failed, error: 0x" << std::hex << hr;
	}

    return hr;
}

//IAMFilterMiscFlags Interface
ULONG OggDemuxPacketSourceFilter::GetMiscFlags(void) 
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

//CAMThread Stuff
DWORD OggDemuxPacketSourceFilter::ThreadProc(void) 
{	
	while(true) 
    {
		DWORD locThreadCommand = GetRequest();
	
		switch(locThreadCommand) 
        {
			case THREAD_EXIT:
	
				Reply(S_OK);
                LOG(logDEBUG) << L"Thread Proc --- THREAD IS EXITING";
				return S_OK;

			case THREAD_RUN:
	
				Reply(S_OK);
				DataProcessLoop();
                LOG(logDEBUG) << L"Thread Proc --- Data Process Loop has returned";
				break;
		}
	}

	return S_OK;
}

void OggDemuxPacketSourceFilter::notifyPinConnected()
{
	LOG(logDEBUG) << L"Notify pin connected";
	if (mStreamMapper->allStreamsReady()) 
    {
		//Setup the seek table.
		if (mSeekTable == NULL) 
        {
			//CUSTOM SOURCE:::
			if (!mUsingCustomSource) 
            {
				LOG(logDEBUG) << L"Setting up seek table";
				//ZZUNICODE:::
				//mSeekTable = new AutoOggChainGranuleSeekTable(StringHelper::toNarrowStr(m_fileName));
				mSeekTable = new AutoOggChainGranuleSeekTable(m_fileName);
				LOG(logDEBUG) << L"After Setting up seek table";
			} 
            else 
            {
				mSeekTable = new CustomOggChainGranuleSeekTable(mDataSource);
			}
			int locNumPins = GetPinCount();

			OggDemuxPacketSourcePin* locPin = NULL;
			for (int i = 0; i < locNumPins; i++) 
            {
				locPin = (OggDemuxPacketSourcePin*)GetPin(i);
				
				LOG(logDEBUG) << L"Adding decoder interface to seek table";
				mSeekTable->addStream(locPin->getSerialNo(), locPin->getDecoderInterface());
			}
			LOG(logDEBUG) << L"Pre seek table build";
			mSeekTable->buildTable();
			
            LOG(logDEBUG) << L"Post seek table build";
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
	while (continueLooping) 
    {
		if (CheckRequest(&locCommand) == TRUE) 
        {
			LOG(logDEBUG) << L"DataProcessLoop : Thread Command issued... leaving loop.";

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
                //To avoid blocking problems... restart the loop if it was just reset while waiting for lock.
				if (mJustReset) 
                {		
                    LOG(logDEBUG) << L"DataProcessLoop : Detected JustRest condition";
					continue;
				}
				locFeedResult = mOggBuffer.feed((const unsigned char*)locBuff, locBytesRead);
				locKeepGoing = ((locFeedResult == (OggDataBuffer::FEED_OK)) || (locFeedResult == OggDataBuffer::PROCESS_DISPATCH_FALSE));;
				if (locFeedResult != OggDataBuffer::FEED_OK)
				{
					LOG(logDEBUG) << L"Feed result = "<<locFeedResult;
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

		if (locIsEOF) 
        {
			//debugLog << "DataProcessLoop : EOF"<<endl;
            CAutoLock locStreamLock(mStreamLock);
			LOG(logDEBUG) << L"DataProcessLoop : EOF Deliver EOS";
			DeliverEOS();
		}
	}

	LOG(logDEBUG) << L"DataProcessLoop : Left loop., baling out";

	//should we flush here ?
	delete[] locBuff;
	
	//return value ??
	return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetCapabilities(DWORD* inCapabilities) 
{
	if (mSeekTable == NULL || !mSeekTable->enabled())  
    {
        *inCapabilities = 0;
        return S_OK;;
    }

	*inCapabilities = mSeekingCap;
    return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetDuration(LONGLONG* pDuration) 
{
	if (mSeekTable == NULL || !mSeekTable->enabled()) 
    {
        return E_NOTIMPL;
    }

	*pDuration = mSeekTable->fileDuration();

    LOG(logDEBUG) << "IMediaSeeking::GetDuration([out] " << ToString(*pDuration) << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}
	 
STDMETHODIMP OggDemuxPacketSourceFilter::CheckCapabilities(DWORD *pCapabilities)
{
    HRESULT result = S_OK;

    DWORD dwActual;
    GetCapabilities(&dwActual);
    if (*pCapabilities & (~dwActual))
    {
        result = S_FALSE;
    }

    LOG(logDEBUG) << "IMediaSeeking::CheckCapabilities([out]  " << *pCapabilities << ") -> 0x" << std::hex << result;

    return result;
}

STDMETHODIMP OggDemuxPacketSourceFilter::IsFormatSupported(const GUID *pFormat)
{
    HRESULT result = S_FALSE;

    if (*pFormat == TIME_FORMAT_MEDIA_TIME) 
    {
        result = S_OK;
    } 

    LOG(logDEBUG) << "IMediaSeeking::IsFormatSupported([in] " << ToString(*pFormat) << ") -> 0x" << std::hex << result;

    return result;
}

STDMETHODIMP OggDemuxPacketSourceFilter::QueryPreferredFormat(GUID *pFormat)
{
	*pFormat = TIME_FORMAT_MEDIA_TIME;

    LOG(logDEBUG) << "IMediaSeeking::QueryPreferredFormat([out] " << ToString(*pFormat) << ") -> 0x" << std::hex << S_OK; 

	return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::SetTimeFormat(const GUID *pFormat)
{
    LOG(logDEBUG) << "IMediaSeeking::SetTimeFormat([in] " << ToString(pFormat) << ") -> 0x" << std::hex << E_NOTIMPL; 
	
    return E_NOTIMPL;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetTimeFormat( GUID *pFormat)
{
	*pFormat = TIME_FORMAT_MEDIA_TIME;

    LOG(logDEBUG) << "IMediaSeeking::GetTimeFormat([out] " << ToString(*pFormat) << ") -> 0x" << std::hex << S_OK; 

    return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetStopPosition(LONGLONG *pStop)
{
	if (mSeekTable == NULL || !mSeekTable->enabled())  
    {
        return E_NOTIMPL;
    }

    *pStop = mSeekTable->fileDuration();

    LOG(logDEBUG) << "IMediaSeeking::GetStopPosition([out] " << ToString(*pStop) << ") -> 0x" << std::hex << S_OK;

	return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetCurrentPosition(LONGLONG *pCurrent)
{
	return E_NOTIMPL;
}

STDMETHODIMP OggDemuxPacketSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat)
{
    LOG(logDEBUG) << "IMediaSeeking::ConvertTimeFormat([out] " << ToString(pTarget) 
        << ", [in] " << ToString(pTargetFormat) << ", [in] " << ToString(Source)
        << ", [in] " << ToString(pSourceFormat) << ") -> 0x" << std::hex << E_NOTIMPL;

    return E_NOTIMPL;
}

STDMETHODIMP OggDemuxPacketSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags)
{
    CAutoLock locLock(m_pLock);

    LOG(logDEBUG) << "IMediaSeeking::SetPositions([in, out] " << ToString(pCurrent) << ", [in] " << dwCurrentFlags
        << ", [in, out] " << ToString(pStop) << ", [in] " << dwStopFlags << ") -> 0x" << std::hex << S_OK;

    if (mSeekTable == NULL || !mSeekTable->enabled())  
    {
        return E_NOTIMPL;
    }
	
	CAutoLock locSourceLock(mSourceFileLock);
	DeliverBeginFlush();
	
	//Find the byte position for this time.
	if (*pCurrent > mSeekTable->fileDuration()) 
    {
		*pCurrent = mSeekTable->fileDuration();
	} 
    else if (*pCurrent < 0) 
    {
		*pCurrent = 0;
	}

	OggGranuleSeekTable::tSeekPair locStartPos = mSeekTable->seekPos(*pCurrent);
		
	//For now, seek to the position directly, later we will discard the preroll
	//Probably don't ever want to do this. We want to record the desired time,
	//	and it will be up to the decoders to drop anything that falls before it.
	
    DeliverEndFlush();
	DeliverNewSegment(*pCurrent, mSeekTable->fileDuration(), 1.0);

	//.second is the file position.
	mDataSource->seek(locStartPos.second.first);

	return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
	return E_NOTIMPL;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest)
{
    if (mSeekTable == NULL || !mSeekTable->enabled())  
    {
        return E_NOTIMPL;
    }

    *pEarliest = 0;
    *pLatest = mSeekTable->fileDuration();

    LOG(logDEBUG) << "IMediaSeeking::GetAvailable([out] " << ToString(*pEarliest) << ", [out] " << ToString(*pLatest)
        << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::SetRate(double dRate)
{
    HRESULT result = VFW_E_UNSUPPORTED_AUDIO;

    if (dRate == 1.00f)
    {
        result = S_OK;
    }
    else if (dRate <= 0.00f)
    {
        result = E_INVALIDARG;
    }

    LOG(logDEBUG) << "IMediaSeeking::SetRate([in] " << std::setprecision(3) << std::showpoint
        << dRate << ") -> 0x" << std::hex << result;

    return result;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetRate(double *dRate)
{
    *dRate = 1.0;

    LOG(logDEBUG) << "IMediaSeeking::GetRate([out] " << std::setprecision(3) << std::showpoint
        << *dRate << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::GetPreroll(LONGLONG *pllPreroll)
{
    *pllPreroll = 0;

    LOG(logDEBUG) << "IMediaSeeking::GetPreroll([out] " << ToString(*pllPreroll) << ") -> 0x" << std::hex << S_OK;

    return S_OK;
}

STDMETHODIMP OggDemuxPacketSourceFilter::IsUsingTimeFormat(const GUID *pFormat) 
{
    HRESULT result = S_FALSE;

    if (*pFormat == TIME_FORMAT_MEDIA_TIME) 
    {
        result = S_OK;
    }

    LOG(logDEBUG) << "IMediaSeeking::IsUsingTimeFormat([in] " << ToString(*pFormat) << ") -> 0x" << std::hex << result;

    return result;
}

bool OggDemuxPacketSourceFilter::notifyStreamBaseTime(__int64 inStreamBaseTime)
{
	if (inStreamBaseTime > mGlobalBaseTime) 
    {
		mGlobalBaseTime = inStreamBaseTime;
	}
	
    return true;
}

__int64 OggDemuxPacketSourceFilter::getGlobalBaseTime()
{
	return mGlobalBaseTime;
}

CCritSec* OggDemuxPacketSourceFilter::streamLock()
{
    return mStreamLock;
}
