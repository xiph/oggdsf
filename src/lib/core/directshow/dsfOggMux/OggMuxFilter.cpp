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
#include "oggmuxfilter.h"


//+++++++++++++++++++++++++++++++++
//-------------------
// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"OggMuxFilter",						// Name
	    &CLSID_OggMuxFilter,            // CLSID
	    OggMuxFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



CUnknown* WINAPI OggMuxFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	OggMuxFilter *pNewObject = new OggMuxFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

void OggMuxFilter::NotifyComplete() {
	HRESULT locHR = NotifyEvent(EC_COMPLETE, S_OK, NULL);

}

STDMETHODIMP OggMuxFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSinkFilter) {
		*ppv = (IFileSinkFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IAMFilterMiscFlags) {
		*ppv = (IAMFilterMiscFlags*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}

ULONG OggMuxFilter::GetMiscFlags(void) {
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}

//------------------

OggMuxFilter::OggMuxFilter()
	:	CBaseFilter(NAME("OggMuxFilter"), NULL, m_pLock, CLSID_OggMuxFilter)
	,	mInterleaver(NULL)
{
	mInterleaver = new OggPageInterleaver(this, this);
	//LEAK CHECK:::Both get deleted in constructor.

	m_pLock = new CCritSec;
	mInputPins.push_back(new OggMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream()));
	//debugLog.open("C:\\temp\\muxer.log", ios_base::out);

	//Make our delegate pin[0], the top pin... we send all out requests there.
	IMediaSeeking* locSeeker = NULL;
	mInputPins[0]->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	
}

OggMuxFilter::~OggMuxFilter(void)
{
	//debugLog.close();
	//DbgLog((LOG_ERROR, 1, TEXT("****************** DESTRUCTOR **********************")));
	delete m_pLock;
	
	//Need to delete the pins !!

	
	//if (ThreadExists() == TRUE) {
	//	//DbgLog((LOG_ERROR, 1, TEXT("******** Thread exists - closing *****")));
	//	Close();
	//}

}

STDMETHODIMP OggMuxFilter::addAnotherPin() {
	mInputPins.push_back(new OggMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream()));
	return S_OK;
}

	//IFileSinkFilter Implementation
HRESULT OggMuxFilter::SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	SetupOutput();
	return S_OK;
}
HRESULT OggMuxFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}

bool OggMuxFilter::acceptOggPage(OggPage* inOggPage) {
	//debugLog<<"Page accepted... writing..."<<endl;
	unsigned char* locPageData = inOggPage->createRawPageData();
	mOutputFile.write((char*)locPageData, inOggPage->pageSize());

	delete locPageData;
	return true;
}
bool OggMuxFilter::SetupOutput() {
	mOutputFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::out | ios_base::binary);
	return mOutputFile.is_open();
}
bool OggMuxFilter::CloseOutput() {
	mOutputFile.close();
	return true;

}

//	//IFileSource Interface
//STDMETHODIMP OggMuxFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
//	//Return the filename and mediatype of the raw data
//
//	 
//	LPOLESTR x = SysAllocString(mFileName.c_str());
//	*outFileName = x;
//	
//	return S_OK;
//}
//STDMETHODIMP OggMuxFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
//	//Initialise the file here and setup all the streams
//	CAutoLock locLock(m_pLock);
//	mFileName = inFileName;
//	
//	return SetUpPins();
//}

//BaseFilter Interface
int OggMuxFilter::GetPinCount() {
	//TO DO::: Change this for multiple streams
	return mInputPins.size();
}
CBasePin* OggMuxFilter::GetPin(int inPinNo) {

	if ((inPinNo < mInputPins.size()) && (inPinNo >= 0)) {
		return mInputPins[inPinNo];
	} else {
		return NULL;
	}
	//if (inPinNo >= 0 && inPinNo < mStreamMapper->numStreams()) {
	//	return mStreamMapper->getOggStream(inPinNo)->getPin();
	//} else {
	//	return NULL;
	//}
}

//CAMThread Stuff
//DWORD OggMuxFilter::ThreadProc(void) {
//	while(true) {
//		DWORD locThreadCommand = GetRequest();
//		switch(locThreadCommand) {
//			case THREAD_EXIT:
//				Reply(S_OK);
//				return S_OK;
//
//			//case THREAD_PAUSE:
//			//	// we are paused already
//			//	Reply(S_OK);
//			//	break;
//
//			case THREAD_RUN:
//				Reply(S_OK);
//				DataProcessLoop();
//				break;
//		}
//	
//	
//	}
//	return S_OK;
//}

//Helper methods

//void OggMuxFilter::resetStream() {
//
//	mSourceFile.clear();
//	mSourceFile.close();
//	mOggBuffer.clearData();
//	mSourceFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::in|ios_base::binary);
//
//	mSourceFile.seekg(mStreamMapper->startOfData(), ios_base::beg);
//	for (unsigned long i = 0; i < mStreamMapper->numStreams(); i++) {
//		mStreamMapper->getOggStream(i)->setSendExcess(true);	
//	}
//}

//HRESULT OggMuxFilter::DataProcessLoop() {
//	DWORD locCommand = 0;
//	char* locBuff = new  char[4096];
//	bool locKeepGoing = true;;
//	while (!mSourceFile.eof() && locKeepGoing) {
//		if(CheckRequest(&locCommand) == TRUE) {
//			return S_OK;
//		}
//
//		mSourceFile.read(locBuff, 4096);
//		unsigned long locBytesRead = mSourceFile.gcount();
//		locKeepGoing = mOggBuffer.feed(locBuff, locBytesRead);
//	}
//	DeliverEOS();
//	delete locBuff;
//	//Memory leak
//	//FIXED
//	
//	
//}
//HRESULT OggMuxFilter::SetUpPins() {
//	mSourceFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::in|ios_base::binary);
//	//Error check
//	
//	//Register a callback
//	mOggBuffer.registerVirtualCallback(this);
//
//	char* locBuff = new char[RAW_BUFFER_SIZE];
//	
//	//Feed the data in until we have seen all BOS pages.
//	while(!mStreamMapper->isReady()) {
//		mSourceFile.read(locBuff, RAW_BUFFER_SIZE);
//		mOggBuffer.feed(locBuff, RAW_BUFFER_SIZE);
//
//	}
//	//Memory leak
//	//FIXED
//	delete locBuff;
//	return S_OK;
//}
//IOggCallback Interface

//bool OggMuxFilter::acceptOggPage(OggPage* inOggPage) {
//	return mStreamMapper->acceptOggPage(inOggPage);
//}

//IMEdiaStreaming
STDMETHODIMP OggMuxFilter::Run(REFERENCE_TIME tStart) {
	//const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP OggMuxFilter::Pause(void) {
	CAutoLock locLock(m_pLock);
	//if (m_State == State_Stopped) {
	//	if (ThreadExists() == FALSE) {
	//		Create();
	//	}
	//	CallWorker(THREAD_RUN);
	//}
	
	HRESULT locHR = CBaseFilter::Pause();
	
	return locHR;
	
}
STDMETHODIMP OggMuxFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	//CallWorker(THREAD_EXIT);
	//Close();
	//DeliverBeginFlush();
	//DeliverEndFlush();

	CloseOutput();
	return CBaseFilter::Stop();
}
