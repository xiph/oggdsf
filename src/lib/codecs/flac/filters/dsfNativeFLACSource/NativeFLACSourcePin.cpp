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
#include ".\NativeFLACSourcePin.h"

NativeFLACSourcePin::NativeFLACSourcePin(NativeFLACSourceFilter* inParentFilter, CCritSec* inFilterLock)
	:	CBaseOutputPin(NAME("Native FLAC Source Pin"), inParentFilter, inFilterLock, &mFilterHR, L"PCM Out")
	,	mParentFilter(inParentFilter)
	,	mDataQueue(NULL)

{
	//Subvert COM and do this directly... this way, the source filter won't expose the interface to the
	// graph but we can still delegate to it.
	IMediaSeeking* locSeeker = NULL;
	locSeeker = (IMediaSeeking*)mParentFilter;
	SetDelegate(locSeeker);
}

NativeFLACSourcePin::~NativeFLACSourcePin(void)
{
	SetDelegate(NULL);		//Avoid infinite destructor loop.
	delete mDataQueue;
	mDataQueue = NULL;
}

STDMETHODIMP NativeFLACSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}
	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT NativeFLACSourcePin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	mDataQueue->NewSegment(tStart, tStop, dRate);
	return S_OK;
}
HRESULT NativeFLACSourcePin::DeliverEndOfStream(void)
{
	mDataQueue->EOS();
    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverEndFlush(void)
{
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverBeginFlush(void)
{
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT NativeFLACSourcePin::CompleteConnect (IPin *inReceivePin)
{
	mFilterHR = S_OK;
	//Deleted in destructor
	mDataQueue = new COutputQueue (inReceivePin, &mFilterHR, FALSE, TRUE,1,TRUE, NUM_BUFFERS);
	if (FAILED(mFilterHR)) {
		//TODO::: Probably should handle this !
        //CHECK::: See if it ever silently reports failure but actually does work before bailing here.
        
		mFilterHR = mFilterHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT NativeFLACSourcePin::BreakConnect(void) {
	delete mDataQueue;
	mDataQueue = NULL;
	return CBaseOutputPin::BreakConnect();
}



HRESULT NativeFLACSourcePin::SetMediaType(const CMediaType* inMediaType)
{

    if (((WAVEFORMATEX*)inMediaType->pbFormat)->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        //mUsingExtendedWave = true;
    } else if (((WAVEFORMATEX*)inMediaType->pbFormat)->wFormatTag == WAVE_FORMAT_PCM) {
        //mUsingExtendedWave = false;
    }

    return CBaseOutputPin::SetMediaType(inMediaType);

}
HRESULT NativeFLACSourcePin::GetMediaType(int inPosition, CMediaType* outMediaType) 
{
    //WFE::: Also offer extensible format
	if (inPosition == 0) {
		outMediaType->SetType(&MEDIATYPE_Audio);
		outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
		outMediaType->SetFormatType(&FORMAT_WaveFormatEx);
		outMediaType->SetTemporalCompression(FALSE);
		outMediaType->SetSampleSize(0);

		WAVEFORMATEXTENSIBLE* locFormatEx = (WAVEFORMATEXTENSIBLE*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEXTENSIBLE));
        
		locFormatEx->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

		locFormatEx->Format.nChannels = (WORD)mParentFilter->mNumChannels;
		locFormatEx->Format.nSamplesPerSec =  mParentFilter->mSampleRate;

        
        locFormatEx->Samples.wValidBitsPerSample = (WORD)mParentFilter->mSignificantBitsPerSample;

        switch (locFormatEx->Format.nChannels)
        {
        case 1:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT;
            break;
        case 2:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT |
                                         SPEAKER_FRONT_RIGHT;
            break;
        case 3:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT |
                                         SPEAKER_FRONT_RIGHT |
                                         SPEAKER_FRONT_CENTER;
            break;
        case 4:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT |
                                         SPEAKER_FRONT_RIGHT |
                                         SPEAKER_BACK_LEFT |
                                         SPEAKER_BACK_RIGHT;
            break;
        case 5:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT |
                                         SPEAKER_FRONT_RIGHT |
                                         SPEAKER_FRONT_CENTER |
                                         SPEAKER_BACK_LEFT |
                                         SPEAKER_BACK_RIGHT;
            break;

        case 6:
            locFormatEx->dwChannelMask = SPEAKER_FRONT_LEFT |
                                         SPEAKER_FRONT_RIGHT |
                                         SPEAKER_FRONT_CENTER |
                                         SPEAKER_LOW_FREQUENCY |
                                         SPEAKER_BACK_LEFT |
                                         SPEAKER_BACK_RIGHT;
            break;

        default:
            locFormatEx->dwChannelMask = 0;
            break;


        }

        ////TODO::: Round up to multiple of 8 or something
        //if (mParentFilter->mBitsPerSample <= 16) {
        //    //Round up to multiple of 8, ie 8 or 16
        //    locFormatEx->Format.wBitsPerSample = (WORD)(mParentFilter->mBitsPerSample + 7) & 0xfff8;
        //} else if (mParentFilter->mBitsPerSample <= 32) {
        //    //Just use 32
        //    locFormatEx->Format.wBitsPerSample = 32;
        //}
		locFormatEx->Format.wBitsPerSample = (WORD)mParentFilter->mBitsPerSample;
		locFormatEx->Format.nBlockAlign = (WORD)((mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3));
		locFormatEx->Format.nAvgBytesPerSec = ((mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3)) * mParentFilter->mSampleRate;
		locFormatEx->Format.cbSize = 22; //sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        locFormatEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

        return S_OK;

        //mUsingExtendedWav = true;
    } else if (inPosition == 1) {
		outMediaType->SetType(&MEDIATYPE_Audio);
		outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
		outMediaType->SetFormatType(&FORMAT_WaveFormatEx);
		outMediaType->SetTemporalCompression(FALSE);
		outMediaType->SetSampleSize(0);

		WAVEFORMATEX* locFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
		locFormat->wFormatTag = WAVE_FORMAT_PCM;

		locFormat->nChannels = (WORD)mParentFilter->mNumChannels;
		locFormat->nSamplesPerSec =  mParentFilter->mSampleRate;
		locFormat->wBitsPerSample = (WORD)mParentFilter->mBitsPerSample;
		locFormat->nBlockAlign = (WORD)((mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3));
		locFormat->nAvgBytesPerSec = ((mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3)) * mParentFilter->mSampleRate;
		locFormat->cbSize = 0;
        //mUsingExtendedWav = false;
	
		return S_OK;

    } else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT NativeFLACSourcePin::CheckMediaType(const CMediaType* inMediaType) 
{
    //WFE::: Do check for extensible type
	if (               (inMediaType->majortype == MEDIATYPE_Audio) 
                    &&  (inMediaType->subtype == MEDIASUBTYPE_PCM) 
                    && (inMediaType->formattype == FORMAT_WaveFormatEx)) {
		return S_OK;
	} else {
		return E_FAIL;
	}
}
HRESULT NativeFLACSourcePin::DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest) 
{
	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	locReqAlloc.cbAlign = 1;
	locReqAlloc.cbBuffer = BUFFER_SIZE;
	locReqAlloc.cbPrefix = 0;
	locReqAlloc.cBuffers = NUM_BUFFERS;

	locHR = inoutAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		return locHR;
	}
	
	locHR = inoutAllocator->Commit();

	return locHR;
}

//This method is responsible for deleting the incoming buffer.
HRESULT NativeFLACSourcePin::deliverData(unsigned char* inBuff, unsigned long inBuffSize, __int64 inStart, __int64 inEnd) 
{
	//Locks !!
	
	IMediaSample* locSample = NULL;
	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locStop = inEnd;
	
	HRESULT	locHR = GetDeliveryBuffer(&locSample, &locStart, &locStop, NULL);
	
	//Error checks
	if (locHR != S_OK) {
		delete[] inBuff;
		return locHR;
	}

	locSample->SetTime(&locStart, &locStop);
	
	locSample->SetSyncPoint(TRUE);

	// Create a pointer for the samples buffer
	BYTE* locBuffer = NULL;
	locSample->GetPointer(&locBuffer);

	//*** WARNING 4018 ::: leave this.
	if (locSample->GetSize() >= inBuffSize) {
		memcpy((void*)locBuffer, (const void*)inBuff, inBuffSize);
		locSample->SetActualDataLength(inBuffSize);

		locHR = mDataQueue->Receive(locSample);

		if (locHR != S_OK) {
			delete[] inBuff;
			return locHR;
			
		} else {
			delete[] inBuff;
			return S_OK;
		}
	} else {
		delete[] inBuff;
		throw 0;
	}
}
