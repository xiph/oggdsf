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
	,	mBegun(false)
	,	mUpto(0)
	,	mJustStopped(true)
	,	mTotalNumSamples(0)
	
	//,	mDecoder(NULL)
{
	m_pLock = new CCritSec;
	mCodecLock = new CCritSec;
	debugLog.open("G:\\logs\\NativeFLAC.log", ios_base::out);
	mFLACSourcePin = new NativeFLACSourcePin(this, m_pLock);
}

NativeFLACSourceFilter::~NativeFLACSourceFilter(void)
{
	debugLog.close();
	delete mFLACSourcePin;
	mFLACSourcePin = NULL;
	delete mCodecLock;
}

//BaseFilter Interface
int NativeFLACSourceFilter::GetPinCount() {
	return 1;
}
CBasePin* NativeFLACSourceFilter::GetPin(int inPinNo) {

	if (inPinNo == 0) {
		return mFLACSourcePin;
	} else {
		return NULL;
	}
}

//IAMFilterMiscFlags Interface
ULONG NativeFLACSourceFilter::GetMiscFlags(void) {
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}

	//IFileSource Interface
STDMETHODIMP NativeFLACSourceFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType) {
	//Return the filename and mediatype of the raw data

	 
	LPOLESTR x = SysAllocString(mFileName.c_str());
	*outFileName = x;
	
	return S_OK;
}

//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP NativeFLACSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	mFileName = inFileName;

	mInputFile.open(StringHelper::toNarrowStr(mFileName).c_str(), ios_base::in | ios_base::binary);

	mInputFile.seekg(0, ios_base::end);
	mFileSize = mInputFile.tellg();
	mInputFile.seekg(0, ios_base::beg);
	debugLog<<"File size is = "<<mFileSize<<endl;

	unsigned char locBuff[64];
	mInputFile.read((char*)&locBuff, 64);
	const unsigned char FLAC_CHANNEL_MASK = 14;  //00001110
	const unsigned char FLAC_BPS_START_MASK = 1; //00000001
	const unsigned char FLAC_BPS_END_MASK = 240;  //11110000

	//Fix the format block data... use header version and other version.
	//mFLACFormatBlock->FLACVersion = FLACMath::charArrToULong(mCodecHeaders->getPacket(1)->packetData() + 28);
	mNumChannels = (((locBuff[20]) & FLAC_CHANNEL_MASK) >> 1) + 1;
	mSampleRate = (iBE_Math::charArrToULong(&locBuff[18])) >> 12;
	
	mBitsPerSample =	(((locBuff[20] & FLAC_BPS_START_MASK) << 4)	|
											((locBuff[21] & FLAC_BPS_END_MASK) >> 4)) + 1;	


	mTotalNumSamples = (((__int64)(locBuff[21] % 16)) << 32) + ((__int64)(iBE_Math::charArrToULong(&locBuff[22])));
	debugLog<<mNumChannels<<" channels with "<<mSampleRate<<" Hz @ "<<mBitsPerSample<<" bits per sample"<<endl;
	debugLog<<"Total num samples = "<<mTotalNumSamples<<endl;


	//TODO::: NEed to handle the case where the number of samples is zero by making it non-seekable.
	mInputFile.seekg(0, ios_base::beg);

	debugLog<<"Pre init"<<endl;
	init();
	debugLog<<"Post init"<<endl;
	bool locResult = process_until_end_of_metadata();
	debugLog<<"Post meta data call..."<<endl;
	if (locResult) {
		debugLog<<"Process meta data ok"<<endl;
	}

	//Strip the extension...
	//size_t locDotPos = mFileName.find_last_of('.');
	//if (locDotPos != ios_base::npos) {
	//	mHDRFileName = mFileName.substr(0, locDotPos);
	//	mHDRFileName += ".hdr";
	//} else {
	//	return S_FALSE;
	//}

	//mInputFile.open(StringHelper::toNarrowStr(mFileName), ios_base::in | ios_base::binary);

	//if (!mInputFile.is_open()) {
	//	return S_FALSE;
	//}


	
	return S_OK;
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
STDMETHODIMP NativeFLACSourceFilter::Run(REFERENCE_TIME tStart) {
	debugLog<<"RUN @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	const REFERENCE_TIME A_LONG_TIME = UNITS * 1000;
	CAutoLock locLock(m_pLock);
	//debugLog<<"Run  :  time = "<<tStart<<endl;
	//DeliverNewSegment(tStart, tStart + A_LONG_TIME, 1.0);
	return CBaseFilter::Run(tStart);
	

}
STDMETHODIMP NativeFLACSourceFilter::Pause(void) {
	CAutoLock locLock(m_pLock);
	//debugLog << "** Pause called **"<<endl;
	debugLog<<"PAUSE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
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
STDMETHODIMP NativeFLACSourceFilter::Stop(void) {
	CAutoLock locLock(m_pLock);
	//debugLog<<"** Stop Called ** "<<endl;
	CallWorker(THREAD_EXIT);
	Close();
	debugLog<<"Stop ##################################################################"<<endl;
	debugLog<<"Pre seek to 0"<<endl;
	mJustStopped = true;
	mUpto = 0;
	debugLog<<"Post seek to 0"<<endl;
	mFLACSourcePin->DeliverBeginFlush();
	mFLACSourcePin->DeliverEndFlush();
	return CBaseFilter::Stop();
}

HRESULT NativeFLACSourceFilter::DataProcessLoop() {

	debugLog<<"Starting loop ***********************************************"<<endl;
	DWORD locCommand = 0;
	bool res = false;
	while (true) {
		if(CheckRequest(&locCommand) == TRUE) {
			debugLog<<"DataProcessLoop : Thread Command issued... leaving loop."<<endl;
			
			return S_OK;
		}
		if (mJustStopped) {
			mJustStopped = false;
			debugLog<<"!!!!!!!!!!!!!!!!!!!!!!! SEEK ABSOLUTE 0"<<endl;
			bool res2 = false;
			{
				CAutoLock locLock(mCodecLock);
				res2 = seek_absolute(0);
			}
			if (res2) {
				debugLog<<"Seek absolute success"<<endl;
			}
		}
		{
			CAutoLock locLock(mCodecLock);
			debugLog<<"Process it"<<endl;
			res = process_single();
		}
		if (res) {
			debugLog<<"Process OK"<<endl;
		} else {
			debugLog<<"Process FAILED"<<endl;
		}
	}

	mFLACSourcePin->DeliverEndOfStream();
	return S_OK;
}

//CAMThread Stuff
DWORD NativeFLACSourceFilter::ThreadProc(void) {
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


::FLAC__SeekableStreamDecoderReadStatus NativeFLACSourceFilter::read_callback(FLAC__byte outBuffer[], unsigned int* outNumBytes) {
	const unsigned long BUFF_SIZE = 8192;
	mInputFile.read((char*)outBuffer, BUFF_SIZE);
	*outNumBytes = mInputFile.gcount();
	debugLog<<"Read num bytes = "<<*outNumBytes<<endl;
	return FLAC__SEEKABLE_STREAM_DECODER_READ_STATUS_OK;
}
::FLAC__SeekableStreamDecoderSeekStatus NativeFLACSourceFilter::seek_callback(FLAC__uint64 inSeekPos) {
	debugLog<<"Seeking to "<<inSeekPos<<endl;
	mInputFile.seekg(inSeekPos);
	return FLAC__SEEKABLE_STREAM_DECODER_SEEK_STATUS_OK;
}
::FLAC__SeekableStreamDecoderTellStatus NativeFLACSourceFilter::tell_callback(FLAC__uint64* outTellPos) {
	*outTellPos = mInputFile.tellg();
	debugLog<<"Tell = "<<*outTellPos<<endl;
	return FLAC__SEEKABLE_STREAM_DECODER_TELL_STATUS_OK;
}
::FLAC__SeekableStreamDecoderLengthStatus NativeFLACSourceFilter::length_callback(FLAC__uint64* outLength) {
	*outLength = mFileSize;
	debugLog<<"Requested length = "<<mFileSize<<endl;
	return FLAC__SEEKABLE_STREAM_DECODER_LENGTH_STATUS_OK;
}
::FLAC__StreamDecoderWriteStatus NativeFLACSourceFilter::write_callback(const FLAC__Frame* inFrame,const FLAC__int32 *const inBuffer[]) {
	//Do the magic !
	if (! mBegun) {
	
		mBegun = true;
		const int SIZE_16_BITS = 2;
		mNumChannels = inFrame->header.channels;
		mFrameSize = mNumChannels * SIZE_16_BITS;
		mSampleRate = inFrame->header.sample_rate;
		
	}
	debugLog<<"Write callback"<<endl;
	unsigned long locNumFrames = inFrame->header.blocksize;
	unsigned long locActualSize = locNumFrames * mFrameSize;
	unsigned long locTotalFrameCount = locNumFrames * mNumChannels;

	//BUG::: There's a bug here. Implicitly assumes 2 channels.
	unsigned char* locBuff = new unsigned char[locActualSize];


	signed short* locShortBuffer = (signed short*)locBuff;		//Don't delete this.
	
	signed short tempInt = 0;
	int tempLong = 0;
	float tempFloat = 0;
	
	//FIX:::Move the clipping to the abstract function
	//Make sure our sample buffer is big enough

	//Modified for FLAC int32 not float

		
	//Must interleave and convert sample size.
	for(unsigned long i = 0; i < locNumFrames; i++) {
		for (unsigned long j = 0; j < mNumChannels; j++) {
			
			
				//No clipping required for ints
				//FIX:::Take out the unnescessary variable.
			tempLong = inBuffer[j][i];
				//Convert 32 bit to 16 bit

			//FIX::: Why on earth are you dividing by 2 ? It does not make sense !
			tempInt = (signed short)(tempLong/2);
		
			*locShortBuffer = tempInt;
			locShortBuffer++;
		}
	}


	debugLog<<"Size = "<<locActualSize<<"Frames from "<<mUpto<<" for "<<locNumFrames<<endl;
	
	mFLACSourcePin->deliverData(locBuff, locActualSize, (mUpto*UNITS) / mSampleRate, ((mUpto+locNumFrames)*UNITS) / mSampleRate);
	mUpto += locNumFrames;
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
void NativeFLACSourceFilter::metadata_callback(const FLAC__StreamMetadata* inMetaData) {
	debugLog<<"Meta callback..."<<endl;
}
void NativeFLACSourceFilter::error_callback(FLAC__StreamDecoderErrorStatus inStatus) {
	debugLog<<"Error callback..."<<endl;
}

bool NativeFLACSourceFilter::eof_callback(void) {
	debugLog<<"EOF Req"<<endl;
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
		//debugLog<<"IsFormatSupported	: TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsFormatSupported	: FALSE !!!"<<endl;
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
	debugLog<<"Request seek to "<<*pCurrent<<endl;
	
	unsigned __int64 locSampleToSeek = (*pCurrent) * mSampleRate/ UNITS;
	debugLog<<"W**** Which is sample no = "<<locSampleToSeek<<endl;
	mFLACSourcePin->DeliverBeginFlush();
	mFLACSourcePin->DeliverEndFlush();
	bool locRes = false;
	{
		CAutoLock locLock(mCodecLock);
		mUpto = 0;
		locRes = seek_absolute(locSampleToSeek);
	}
	if (locRes) {
		debugLog<<"Seek suceeded"<<endl;
	} else {
		debugLog<<"Seek failed"<<endl;
	}
	
	return S_OK;
}
STDMETHODIMP NativeFLACSourceFilter::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop){
	debugLog<<"Calling get positions _ NOTIMPL"<<endl;
	return E_NOTIMPL;
}
STDMETHODIMP NativeFLACSourceFilter::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest){
	*pEarliest = 0;
	*pLatest = (mTotalNumSamples * UNITS) / mSampleRate;
	debugLog<<"Get available"<<endl;
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
		//debugLog<<"IsFormatSupported	: TRUE"<<endl;
		return S_OK;
	} else {
		//debugLog<<"IsFormatSupported	: FALSE !!!"<<endl;
		return S_FALSE;
	}
}