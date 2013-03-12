//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//          (C) 2013 Cristian Adam
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
#include "Precompiled.h"
#include "oggmuxfilter.h"


CFactoryTemplate g_Templates[] = 
{
    { 
		L"Xiph.Org Ogg Muxer",			// Name
	    &CLSID_OggMuxFilter,            // CLSID
	    OggMuxFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,							// Initialization function
        NULL							// Set-up information (for filters)
    }
//	,
//   { 
//		L"Ogg Muxer Properties",		// Name
//	    &CLSID_PropsOggMux,             // CLSID
//	     PropsOggMux::CreateInstance,	// Method to create an instance of MyComponent
//       NULL,							// Initialization function
//       NULL							// Set-up information (for filters)
//   }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


CUnknown* WINAPI OggMuxFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	OggMuxFilter *pNewObject = new OggMuxFilter();
    if (pNewObject == NULL) 
    {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

void OggMuxFilter::NotifyComplete() 
{
	HRESULT locHR = NotifyEvent(EC_COMPLETE, S_OK, NULL);
    UNREFERENCED_PARAMETER(locHR);
}

STDMETHODIMP OggMuxFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSinkFilter) 
    {
        return GetInterface((IFileSinkFilter*)this, ppv);
	} 
    else if (riid == IID_IAMFilterMiscFlags) 
    {
		LOG(logDEBUG)<<"Queried for IAMMiscFlags"<<endl;
		return GetInterface((IAMFilterMiscFlags*)this, ppv);
	} 
    else if (riid == IID_IMediaSeeking) 
    {
		LOG(logDEBUG)<<"Queried for IMediaSeeking"<<endl;
		return GetInterface((IMediaSeeking*)this, ppv);
	} 
    else if (riid == IID_IOggMuxProgress) 
    {
		LOG(logDEBUG)<<"Queried for IOggMuxProgress"<<endl;
		return GetInterface((IOggMuxProgress*)this, ppv);
	} 
    else if (riid == IID_IOggMuxSettings) 
    {
		return GetInterface((IOggMuxSettings*)this, ppv);
	}
	//else if (riid == IID_ISpecifyPropertyPages) 
    //{
	//	return GetInterface((ISpecifyPropertyPages*)this, ppv);
	//}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}

LONGLONG __stdcall OggMuxFilter::getProgressTime() 
{
	if (mInterleaver != NULL) 
    {
		return mInterleaver->progressTime();
	} 
    else 
    {
		return -1;
	}
}

LONGLONG __stdcall OggMuxFilter::getBytesWritten() 
{
	if (mInterleaver != NULL) 
    {
		return mInterleaver->bytesWritten();
	} 
    else 
    {
		return -1;
	}
}

ULONG OggMuxFilter::GetMiscFlags() 
{
	LOG(logDEBUG)<<"GetMiscflags"<<endl;
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}


OggMuxFilter::OggMuxFilter()
	:	CBaseFilter(NAME("Xiph.Org Ogg Muxer"), NULL, m_pLock, CLSID_OggMuxFilter)
	,	mInterleaver(NULL)
{
	mInterleaver = new OggPageInterleaver(this, this);
	//LEAK CHECK:::Both get deleted in constructor.

	m_pLock = new CCritSec;
	mStreamLock = new CCritSec;
	mInputPins.push_back(new OggMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream()));

	//To avoid a circular reference... we do this without the addref.
	// This is safe because we control the lifetime of this pin, and it won't be deleted until we are.
	IMediaSeeking* locSeeker = (IMediaSeeking*)mInputPins[0];
	SetDelegate(locSeeker);	
}

OggMuxFilter::OggMuxFilter(REFCLSID inFilterGUID)
	:	CBaseFilter(NAME("OggMuxFilter"), NULL, m_pLock, inFilterGUID)
	,	mInterleaver(NULL)
{	
	m_pLock = new CCritSec;
	mStreamLock = new CCritSec;	
}

OggMuxFilter::~OggMuxFilter()
{
	//This is not a leak !! We just don't want it to be released... we never addreffed it.. see constructor.
	SetDelegate(NULL);
	
	delete mInterleaver;
	for (size_t i = 0; i < mInputPins.size(); i++) 
    {
		delete mInputPins[i];
	}

	delete m_pLock;
	delete mStreamLock;
}

HRESULT OggMuxFilter::addAnotherPin() 
{
	mInputPins.push_back(new OggMuxInputPin(this, m_pLock, &mHR, mInterleaver->newStream()));
	return S_OK;
}

//IFileSinkFilter Implementation
HRESULT OggMuxFilter::SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	SetupOutput();
	return S_OK;
}

HRESULT OggMuxFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
	//Return the filename and mediatype of the raw data

    CheckPointer(outFileName, E_POINTER);
    *outFileName = NULL;

    if (!mFileName.empty()) 
    {
    	unsigned int size  = sizeof(WCHAR) * (mFileName.size() + 1);

        *outFileName = (LPOLESTR) CoTaskMemAlloc(size);
        if (*outFileName != NULL) 
        {
              CopyMemory(*outFileName, mFileName.c_str(), size);
        }
    }
	
	return S_OK;
}

bool OggMuxFilter::acceptOggPage(OggPage* inOggPage) 
{			
    //Deletes Page correctly.
	//LOG(logDEBUG)<<"Page accepted... writing..."<<endl;
	unsigned char* locPageData = inOggPage->createRawPageData();
	mOutputFile.write((char*)locPageData, inOggPage->pageSize());

	delete inOggPage;
	delete[] locPageData;
	return true;
}

bool OggMuxFilter::SetupOutput() 
{
	mOutputFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::out | ios_base::binary);
	return mOutputFile.is_open();
}

bool OggMuxFilter::CloseOutput() 
{
	mOutputFile.close();
	return true;
}

//BaseFilter Interface
int OggMuxFilter::GetPinCount() 
{
	//TO DO::: Change this for multiple streams
	return (int)mInputPins.size();
}

CBasePin* OggMuxFilter::GetPin(int inPinNo) 
{
	if ((inPinNo >= 0) && ((size_t)inPinNo < mInputPins.size()) ) 
    {
		return mInputPins[inPinNo];
	} 
    
    return NULL;
}


//IMEdiaStreaming
HRESULT __stdcall OggMuxFilter::Run(REFERENCE_TIME tStart) 
{
	CAutoLock locLock(m_pLock);
	return CBaseFilter::Run(tStart);
}

HRESULT __stdcall OggMuxFilter::Pause(void) 
{
	CAutoLock locLock(m_pLock);
	
	HRESULT locHR = CBaseFilter::Pause();	
	return locHR;
}

HRESULT __stdcall OggMuxFilter::Stop() 
{
	CAutoLock locLock(m_pLock);

	CloseOutput();
	return CBaseFilter::Stop();
}

STDMETHODIMP OggMuxFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop) 
{
	HRESULT locHR = BasicSeekPassThrough::GetPositions(pCurrent, pStop);
	LOG(logDEBUG)<<"GetPos Before : "<<*pCurrent<<" - "<<*pStop<<endl;
	*pCurrent = mInterleaver->progressTime();
	LOG(logDEBUG)<<"GetPos After : "<<*pCurrent<<" - "<<*pStop<<endl;
	return locHR;
}

STDMETHODIMP OggMuxFilter::GetCurrentPosition(LONGLONG *pCurrent) 
{
	*pCurrent = mInterleaver->progressTime();
	LOG(logDEBUG)<<"GetCurrentPos : "<<*pCurrent<<endl;
	return S_OK;
}

bool __stdcall OggMuxFilter::setMaxPacketsPerPage(unsigned long inMaxPacketsPerPage) 
{
	for (std::vector<OggMuxInputPin*>::iterator locPinIterator = mInputPins.begin();
		 locPinIterator != mInputPins.end();
		 locPinIterator++) 
    {
		OggMuxInputPin* locPin = *locPinIterator;
		locPin->SetPaginatorMaximumPacketsPerPage(inMaxPacketsPerPage);
	}

	return true;
}

unsigned long __stdcall OggMuxFilter::maxPacketsPerPage() 
{
	unsigned long locCurrentMaximumPacketsPerPage = 0;

	for (std::vector<OggMuxInputPin*>::iterator locPinIterator = mInputPins.begin();
		 locPinIterator != mInputPins.end();
		 locPinIterator++) 
    {		
		OggMuxInputPin* locPin = *locPinIterator;

		unsigned long locMaximumPacketsPerPageForThisPin =
			locPin->PaginatorMaximumPacketsPerPage();

		if (locMaximumPacketsPerPageForThisPin > locCurrentMaximumPacketsPerPage) 
        {
			locCurrentMaximumPacketsPerPage = locMaximumPacketsPerPageForThisPin;
		}
	}

	return locCurrentMaximumPacketsPerPage;
}
