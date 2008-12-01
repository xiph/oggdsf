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
#include ".\NativeFLACSourceFilter.h"

CFactoryTemplate g_Templates[] = 
{
    { 
		L"Native FLAC SourceFilter",						// Name
	    &CLSID_NativeFLACSourceFilter,            // CLSID
	    NativeFLACSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 


CUnknown* WINAPI NativeFLACSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	NativeFLACSourceFilter *pNewObject = new NativeFLACSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 

NativeFLACSourceFilter::NativeFLACSourceFilter(void)
	:	CBaseFilter(NAME("NativeFLACSourceFilter"), NULL, m_pLock, CLSID_NativeFLACSourceFilter)
	,	mNumChannels(0)
	,	mSampleRate(0)
	,	mBitsPerSample(0)
    ,   mSignificantBitsPerSample(0)
	,	mBegun(false)
	,	mUpto(0)
	,	mJustSeeked(true)
	,	mSeekRequest(0)
	,	mTotalNumSamples(0)
	,	mWasEOF(false)
{
	m_pLock = new CCritSec;
	mFLACSourcePin = new NativeFLACSourcePin(this, m_pLock);
}

NativeFLACSourceFilter::~NativeFLACSourceFilter(void)
{
	delete mFLACSourcePin;
	mFLACSourcePin = NULL;
}

//BaseFilter Interface
int NativeFLACSourceFilter::GetPinCount() 
{
	return 1;
}
CBasePin* NativeFLACSourceFilter::GetPin(int inPinNo) 
{
	if (inPinNo == 0) {
		return mFLACSourcePin;
	} else {
		return NULL;
	}
}

//IAMFilterMiscFlags Interface
ULONG NativeFLACSourceFilter::GetMiscFlags(void) 
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

	//IFileSource Interface
STDMETHODIMP NativeFLACSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) 
{
    CheckPointer(outFileName, E_POINTER);
    *outFileName = NULL;

    if (!mFileName.empty()) {
    	unsigned int size  = sizeof(WCHAR) * (mFileName.size() + 1);

        *outFileName = (LPOLESTR) CoTaskMemAlloc(size);
        if (*outFileName != NULL) {
              CopyMemory(*outFileName, mFileName.c_str(), size);
        }
    }
	
	return S_OK;
}


STDMETHODIMP NativeFLACSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	//Initialise the file here and setup the stream
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	mInputFile.open(mFileName.c_str(), ios_base::in | ios_base::binary);

	//CT> Added header check (for FLAC files with ID3 v1/2 tags in them)
	//    We'll look in the first 128kb of the file
	unsigned long locStart = 0;
	int locHeaderFound = 0;
	for(int j = 0; !locHeaderFound && j < 128; j++)	{
		unsigned char locTempBuf[1024]={0,};
		mInputFile.read((char*)&locTempBuf, sizeof(locTempBuf));
		unsigned char* locPtr = locTempBuf;
		for(int i = 0; i < 1023; i++) {
			if(locPtr[i]=='f' && locPtr[i+1]=='L' && locPtr[i+2]=='a' && locPtr[i+3]=='C')			{
				locHeaderFound = 1;
				locStart = i + (j * 1024);
				break;
			}
		}
	}
	if(!locHeaderFound) {
		return E_FAIL;
	}

	mInputFile.seekg(0, ios_base::end);
	mFileSize = mInputFile.tellg();
	mFileSize -= locStart;
	mInputFile.seekg(locStart, ios_base::beg);

	unsigned char locBuff[64];
	mInputFile.read((char*)&locBuff, 64);
	const unsigned char FLAC_CHANNEL_MASK = 14;  //00001110
	const unsigned char FLAC_BPS_START_MASK = 1; //00000001
	const unsigned char FLAC_BPS_END_MASK = 240;  //11110000

	mNumChannels = (((locBuff[20]) & FLAC_CHANNEL_MASK) >> 1) + 1;
	mSampleRate = (iBE_Math::charArrToULong(&locBuff[18])) >> 12;
	mSignificantBitsPerSample =	(((locBuff[20] & FLAC_BPS_START_MASK) << 4)	| ((locBuff[21] & FLAC_BPS_END_MASK) >> 4)) + 1;

    mBitsPerSample = (mSignificantBitsPerSample + 7)  & 0xfffffff8UL;

    if (mBitsPerSample == 24) {
        mBitsPerSample = 32;
    }

	mTotalNumSamples = (((__int64)(locBuff[21] % 16)) << 32) + ((__int64)(iBE_Math::charArrToULong(&locBuff[22])));

	//TODO::: NEed to handle the case where the number of samples is zero by making it non-seekable.
	mInputFile.seekg(locStart, ios_base::beg);

	init();
	bool locResult = process_until_end_of_metadata();

	return (locResult ? S_OK : E_FAIL);

}

STDMETHODIMP NativeFLACSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IFileSourceFilter) {
		*ppv = (IFileSourceFilter*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv); 
}


//IMEdiaStreaming
STDMETHODIMP NativeFLACSourceFilter::Run(REFERENCE_TIME tStart) 
{
	CAutoLock locLock(m_pLock);
	return CBaseFilter::Run(tStart);
}
STDMETHODIMP NativeFLACSourceFilter::Pause(void) 
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
STDMETHODIMP NativeFLACSourceFilter::Stop(void) 
{
	CAutoLock locLock(m_pLock);
	CallWorker(THREAD_EXIT);
	Close();
	mJustSeeked = true;
	mSeekRequest = 0;
	mUpto = 0;
	mFLACSourcePin->DeliverBeginFlush();
	mFLACSourcePin->DeliverEndFlush();
	return CBaseFilter::Stop();
}

HRESULT NativeFLACSourceFilter::DataProcessLoop() {
	DWORD locCommand = 0;
	bool res = false;
	while (true) {
		if(CheckRequest(&locCommand) == TRUE) {
			return S_OK;
		}
		{
			if (mJustSeeked) {
				mUpto = 0;
				mJustSeeked = false;
				bool res2 = false;
				res2 = seek_absolute(mSeekRequest);
				if (!res2)
				{
					//ERROR???
					flush();
					break;
				}
			}
			
			res = process_single();
			if (!res)
			{
				//ERROR???
				flush();
			}

			if (mWasEOF) {
				break;
			}
			
		}
	}

	mInputFile.clear();
	mInputFile.seekg(0);
	mWasEOF = false;
	mFLACSourcePin->DeliverEndOfStream();
	return S_OK;
}

//CAMThread Stuff
DWORD NativeFLACSourceFilter::ThreadProc(void) 
{
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
            //OTHER CASES?
		}
	}
	return S_OK;
}


::FLAC__StreamDecoderReadStatus NativeFLACSourceFilter::read_callback(FLAC__byte outBuffer[], size_t* outNumBytes) 
{
	const unsigned long BUFF_SIZE = 8192;
	mInputFile.read((char*)outBuffer, BUFF_SIZE);
	*outNumBytes = mInputFile.gcount();
	mWasEOF = mInputFile.eof();
	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}
::FLAC__StreamDecoderSeekStatus NativeFLACSourceFilter::seek_callback(FLAC__uint64 inSeekPos) 
{
	mInputFile.seekg(inSeekPos);
	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}
::FLAC__StreamDecoderTellStatus NativeFLACSourceFilter::tell_callback(FLAC__uint64* outTellPos) 
{
	*outTellPos = mInputFile.tellg();
	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}
::FLAC__StreamDecoderLengthStatus NativeFLACSourceFilter::length_callback(FLAC__uint64* outLength) 
{
	*outLength = mFileSize;
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}
::FLAC__StreamDecoderWriteStatus NativeFLACSourceFilter::write_callback(const FLAC__Frame* inFrame,const FLAC__int32 *const inBuffer[]) 
{
	

    
	if (! mBegun) {
		mBegun = true;
		
	
		mNumChannels = inFrame->header.channels;
        mFrameSize = mNumChannels * (mBitsPerSample >> 3);

		mSampleRate = inFrame->header.sample_rate;
	}

	unsigned long locNumFrames = inFrame->header.blocksize;
	unsigned long locBufferSize = locNumFrames * mFrameSize;
	unsigned long locTotalFrameCount = locNumFrames * mNumChannels;



    // TODO::: It's not clear whether in the 32 bit flac sample, the significant bits are always rightmost (
    //      ie justified into the least significant bits. They seem to be for nbits = 16. But it's unclear
    //      whether eg a 20 bit sample is 0000 0000 1111 1111 1111 1111 1111 0000 so it's still a valid
    //      24 bit sample, or whether it would require a 4 bit left shift to be a true 24 bit sample.

    //      For now, working on the basis that it is truly right shifted.


    //It could actually be a single buffer for the class.????
    unsigned char* locBuff = new unsigned char[locBufferSize];			//Gives to the deliverdata method
    unsigned long locLeftShift = mBitsPerSample - mSignificantBitsPerSample;

    if (mBitsPerSample == 8) {
        unsigned char* locByteBuffer = (unsigned char*)locBuff;

        if (locLeftShift == 0) {
	        for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {
                    *(locByteBuffer++) = (unsigned char)(inFrame + 128);
                }

            }
        } else {
	        for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {
                    *(locByteBuffer++) = (unsigned char)(inFrame + 128) << locLeftShift;
                }

            }

        }


    } else if (mBitsPerSample == 16) {
	    signed short* locShortBuffer = (signed short*)locBuff;		//Don't delete this.
        if (locLeftShift == 0) {
        
	        for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {

                    *(locShortBuffer++) = (signed short)inBuffer[j][i];
			        //tempLong = inBuffer[j][i];

			        ////FIX::: Why on earth are you dividing by 2 ? It does not make sense !
			        ////tempInt = (signed short)(tempLong/2);
			        //tempInt = (signed short)(tempLong);
        		
			        //*locShortBuffer = tempInt;
			        //locShortBuffer++;
		        }
	        }
        } else {
	        for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {

                    *(locShortBuffer++) = (signed short)inBuffer[j][i] << locLeftShift;
		        }
	        }
        }
    } else if (mBitsPerSample == 32) {
        signed long* locLongBuffer = (signed long*)locBuff;
        if (locLeftShift == 8) {
            //Special case for 24 bit, let the shift be hardcoded.
            for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {
                    *(locLongBuffer++) = inBuffer[j][i] << 8;
                }
            }
        } else if (locLeftShift == 0) {
            //Real 32 bit data
            for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {
                    *(locLongBuffer++) = inBuffer[j][i];
                }
            }

        } else {
            
            for(unsigned long i = 0; i < locNumFrames; i++) {
		        for (unsigned long j = 0; j < mNumChannels; j++) {
                    *(locLongBuffer++) = inBuffer[j][i] << locLeftShift;
                }
            }

        }
    }


	
	mFLACSourcePin->deliverData(locBuff, locBufferSize, (mUpto*UNITS) / mSampleRate, ((mUpto+locNumFrames)*UNITS) / mSampleRate);
	mUpto += locNumFrames;
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
void NativeFLACSourceFilter::metadata_callback(const FLAC__StreamMetadata* inMetaData) {

}
void NativeFLACSourceFilter::error_callback(FLAC__StreamDecoderErrorStatus inStatus) {

}

bool NativeFLACSourceFilter::eof_callback(void) {
	return mInputFile.eof();
}



STDMETHODIMP NativeFLACSourceFilter::GetCapabilities(DWORD* inCapabilities) {
	*inCapabilities = AM_SEEKING_CanSeekAbsolute |
						AM_SEEKING_CanSeekForwards |
						AM_SEEKING_CanSeekBackwards |
						AM_SEEKING_CanGetCurrentPos |
						AM_SEEKING_CanGetStopPos |
						AM_SEEKING_CanGetDuration;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::CheckCapabilities(DWORD *pCapabilities) {
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::IsFormatSupported(const GUID *pFormat) {
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		return S_OK;
	} else {
		return S_FALSE;
	}
}
STDMETHODIMP NativeFLACSourceFilter::QueryPreferredFormat(GUID *pFormat) {
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::SetTimeFormat(const GUID *pFormat) {
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::GetTimeFormat( GUID *pFormat) {
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetDuration(LONGLONG *pDuration) {
	*pDuration = (mTotalNumSamples * UNITS) / mSampleRate;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetStopPosition(LONGLONG *pStop) {
	*pStop = (mTotalNumSamples * UNITS) / mSampleRate;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetCurrentPosition(LONGLONG *pCurrent){
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat){
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags){
	unsigned __int64 locSampleToSeek = (*pCurrent) * mSampleRate/ UNITS;
	mFLACSourcePin->DeliverBeginFlush();
	mFLACSourcePin->DeliverEndFlush();

	mJustSeeked = true;
	mSeekRequest = locSampleToSeek;
	
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop){
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest){
	*pEarliest = 0;
	*pLatest = (mTotalNumSamples * UNITS) / mSampleRate;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::SetRate(double dRate){
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::GetRate(double *dRate){
	*dRate = 1.0;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetPreroll(LONGLONG *pllPreroll){
	*pllPreroll = 0;
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::IsUsingTimeFormat(const GUID *pFormat){
	if (*pFormat == TIME_FORMAT_MEDIA_TIME) {
		return S_OK;
	} else {
		return S_FALSE;
	}
}