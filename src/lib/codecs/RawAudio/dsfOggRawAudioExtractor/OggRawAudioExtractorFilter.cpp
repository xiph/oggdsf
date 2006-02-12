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
#include "OggRawAudioExtractorFilter.h"



//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Ogg Raw Audio Extractor Filter",					// Name
	    &CLSID_OggRawAudioExtractorFilter,				// CLSID
	    OggRawAudioExtractorFilter::CreateInstance,		// Method to create an instance of Speex Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

OggRawAudioExtractorFilter::OggRawAudioExtractorFilter()
	:	CTransformFilter(NAME("Ogg Raw Audio Extractor"), NULL, CLSID_OggRawAudioExtractorFilter)
	,	mInputPin(NULL)
	,	mOutputPin(NULL)

	,	mWorkingBuff(NULL)
	,	mBytesBuffered(0)

	,	mSegStart(0)
	,	mSegEnd(0)
	,	mSegRate(0)
	
{
	mWorkingBuff = new unsigned char[1<<20];	//1 meg
	
}



OggRawAudioExtractorFilter::~OggRawAudioExtractorFilter(void)
{
	delete[] mWorkingBuff;
}

CUnknown* WINAPI OggRawAudioExtractorFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	OggRawAudioExtractorFilter *pNewObject = new OggRawAudioExtractorFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

HRESULT OggRawAudioExtractorFilter::CheckInputType(const CMediaType* inMediaType)
{
	return mInputPin->CheckMediaType(inMediaType);
}
HRESULT OggRawAudioExtractorFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType)
{

	return S_OK;
}
HRESULT OggRawAudioExtractorFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest)
{
	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;
	
	if (inPropertyRequest->cbAlign <= 0) {
		locReqAlloc.cbAlign = 1;
	} else {
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}


	if (inPropertyRequest->cbBuffer == 0) {
		locReqAlloc.cbBuffer = 65536*16;
	} else {
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}
	

	if (inPropertyRequest->cbPrefix < 0) {
			locReqAlloc.cbPrefix = 0;
	} else {
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}
	
	if (inPropertyRequest->cBuffers == 0) {
		locReqAlloc.cBuffers = 5;
	} else {
		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		//TODO::: Handle a fail state here.
		return locHR;
	} else {
		//TODO::: Need to save this pointer to decommit in destructor ???
		locHR = inAllocator->Commit();

	
		return locHR;
	}
	
}

HRESULT OggRawAudioExtractorFilter::NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate)
{
	mSegStart = inStartTime;
	mSegEnd = inStopTime;
	mSegRate = inRate;
	return CTransformFilter::NewSegment(inStartTime, inStopTime, inRate);
}
HRESULT OggRawAudioExtractorFilter::GetMediaType(int inPosition, CMediaType* outMediaType)
{
	
	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if ((inPosition == 0) && (mInputPin != NULL) && (mInputPin->IsConnected())) {
		
		WAVEFORMATEX* locWaveFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
		//*locVideoFormat = *mInputPin->getVideoFormatBlock();

		sOggRawAudioFormatBlock locOggFormat =  mInputPin->getFormatBlock();
		locWaveFormat->wFormatTag = WAVE_FORMAT_PCM;
		locWaveFormat->nChannels = locOggFormat.numChannels;
		locWaveFormat->nSamplesPerSec = locOggFormat.samplesPerSec;
		locWaveFormat->wBitsPerSample = locOggFormat.bitsPerSample;
		locWaveFormat->nBlockAlign = (locWaveFormat->nChannels) * (locWaveFormat->wBitsPerSample >> 3);
		locWaveFormat->nAvgBytesPerSec = ((locWaveFormat->nChannels) * (locWaveFormat->wBitsPerSample >> 3)) * locWaveFormat->nSamplesPerSec;
		locWaveFormat->cbSize = 0;
		
		outMediaType->majortype = MEDIATYPE_Audio;
		outMediaType->subtype = MEDIASUBTYPE_PCM;
		outMediaType->formattype = FORMAT_WaveFormatEx;

		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}


}

HRESULT OggRawAudioExtractorFilter::Receive(IMediaSample* inSample)
{
	BYTE* locInBuff = NULL;
	HRESULT locHR = inSample->GetPointer(&locInBuff);

	if (locHR == S_OK) {
		REFERENCE_TIME locStart = -1;
		REFERENCE_TIME locEnd = -1;
		inSample->GetTime(&locStart, &locEnd);
		if (locEnd == 0) {
			return S_OK;
		}

		memcpy((void*)&mWorkingBuff[mBytesBuffered], (const void*)locInBuff, inSample->GetActualDataLength());
		mBytesBuffered += inSample->GetActualDataLength();



		if (locEnd > 0) {
			//Can dump it all downstream now	
			IMediaSample* locSample;
			unsigned long locSampleDuration = 0;
			unsigned long locBytesCopied = 0;
			unsigned long locBytesToCopy = 0;
			unsigned long locSampleRate = mInputPin->getFormatBlock().samplesPerSec;
			unsigned long locFrameSize = (mInputPin->getFormatBlock().bitsPerSample * mInputPin->getFormatBlock().numChannels) >> 3;

			locStart = mInputPin->convertGranuleToTime(locEnd) - (((mBytesBuffered / locFrameSize) * UNITS) / locSampleRate);
			do {
				//HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);
				HRESULT locHR = InitializeOutputSample(inSample, &locSample);
				if (locHR != S_OK) {
					return locHR;
				}

				BYTE* locBuffer = NULL;
				locHR = locSample->GetPointer(&locBuffer);
			
				if (locHR != S_OK) {
					return locHR;
				}

				locBytesToCopy = ((mBytesBuffered - locBytesCopied) <= locSample->GetSize()) ? (mBytesBuffered - locBytesCopied) : locSample->GetSize();
				//locBytesCopied += locBytesToCopy;

				locSampleDuration = (((locBytesToCopy/locFrameSize) * UNITS) / locSampleRate);
				locEnd = locStart + locSampleDuration;

				//Adjust the time stamps for rate and seeking
				REFERENCE_TIME locAdjustedStart = locStart; //(locStart * RATE_DENOMINATOR) / mRateNumerator;
				REFERENCE_TIME locAdjustedEnd = locEnd; //(locEnd * RATE_DENOMINATOR) / mRateNumerator;
				locAdjustedStart -= mSegStart;
				locAdjustedEnd -= mSegStart;

				__int64 locSeekStripOffset = 0;
				if (locAdjustedEnd < 0) {
					locSample->Release();
				} else {
					if (locAdjustedStart < 0) {
						locSeekStripOffset = (-locAdjustedStart) * locSampleRate;
						locSeekStripOffset *= locFrameSize;
						locSeekStripOffset /= UNITS;
						locSeekStripOffset += (locFrameSize - (locSeekStripOffset % locFrameSize));
						__int64 locStrippedDuration = (((locSeekStripOffset/locFrameSize) * UNITS) / locSampleRate);
						locAdjustedStart += locStrippedDuration;
					}
						

				

					memcpy((void*)locBuffer, (const void*)&mWorkingBuff[locBytesCopied + locSeekStripOffset], locBytesToCopy - locSeekStripOffset);

					locSample->SetTime(&locAdjustedStart, &locAdjustedEnd);
					locSample->SetMediaTime(&locStart, &locEnd);
					locSample->SetSyncPoint(TRUE);
					locSample->SetActualDataLength(locBytesToCopy - locSeekStripOffset);
					locHR = mOutputPin->Deliver(locSample); //((SpeexDecodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);
					locSample->Release();
					if (locHR != S_OK) {
						return locHR;
					}
					locStart += locSampleDuration;

				}
				locBytesCopied += locBytesToCopy;

			
			} while(locBytesCopied < mBytesBuffered);

			mBytesBuffered = 0;

		}

		return S_OK;

	}

	return S_FALSE;

}

HRESULT OggRawAudioExtractorFilter::Transform(IMediaSample* inInputSample, IMediaSample* inOutputSample)
{

	return E_NOTIMPL;
}

CBasePin* OggRawAudioExtractorFilter::GetPin(int inPinNo)
{

    HRESULT locHR = S_OK;

    // Create an input pin if necessary

    if (m_pInput == NULL) {

        m_pInput = new OggRawAudioExtractorInputPin(this, &locHR);		//Deleted in base destructor


		mInputPin = (OggRawAudioExtractorInputPin*)m_pInput;
        m_pOutput = new CTransformOutputPin(NAME("PCM Out"), this, &locHR, L"PCM Out");	//Deleted in base destructor
		mOutputPin = m_pOutput;
			

    }

    // Return the pin

    if (inPinNo == 0) {
        return m_pInput;
    } else if (inPinNo == 1) {
        return m_pOutput;
    } else {
        return NULL;
    }

}