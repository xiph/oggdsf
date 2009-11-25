//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "SpeexDecodeFilter.h"
#include "SpeexDecodeOutputPin.h"
#include "SpeexDecodeInputPin.h"


SpeexDecodeInputPin::SpeexDecodeInputPin(AbstractTransformFilter* inFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(inFilter, inFilterLock, inOutputPin, NAME("SpeexDecodeInputPin"), L"Speex In", inAcceptableMediaTypes)

	,	mNumChannels(0)
	,	mSampleFrameSize(0)
	,	mSampleRate(0)
	,	mSpeexFrameSize(0)
	,	mUptoFrame(0)

	,	mDecodedByteCount(0)
	,	mDecodedBuffer(NULL)

	,	mRateNumerator(RATE_DENOMINATOR)

	,	mSetupState(VSS_SEEN_NOTHING)

	,	mBegun(false)
{
	ConstructCodec();

	mDecodedBuffer = new unsigned char[DECODED_BUFFER_SIZE];
}

bool SpeexDecodeInputPin::ConstructCodec() 
{
	//TODO::: Allow force options
	
	//Don't need to do much... mSpeexDecoder is good to go
	return true;

}
void SpeexDecodeInputPin::DestroyCodec() 
{

}
SpeexDecodeInputPin::~SpeexDecodeInputPin(void)
{
	DestroyCodec();

	delete[] mDecodedBuffer;
}

STDMETHODIMP SpeexDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) 
    {
        return GetInterface((IMediaSeeking*)this, ppv);
	} 
    else if (riid == IID_IOggDecoder) 
    {
        return GetInterface((IOggDecoder*)this, ppv);
	}

	return AbstractTransformInputPin::NonDelegatingQueryInterface(riid, ppv); 
}
STDMETHODIMP SpeexDecodeInputPin::NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate) 
{
	CAutoLock locLock(mStreamLock);
	//debugLog<<"New segment "<<inStartTime<<" - "<<inStopTime<<endl;
	mUptoFrame = 0;

	//Denominator and numerator are a 16 bit fraction
	mRateNumerator = RATE_DENOMINATOR * inRate;
	if (mRateNumerator > RATE_DENOMINATOR) 
    {
		mRateNumerator = RATE_DENOMINATOR;
	}
	return AbstractTransformInputPin::NewSegment(inStartTime, inStopTime, inRate);	
}

STDMETHODIMP SpeexDecodeInputPin::EndFlush()
{
	CAutoLock locLock(m_pLock);
	
	HRESULT locHR = AbstractTransformInputPin::EndFlush();
	mDecodedByteCount = 0;
	return locHR;
}
HRESULT SpeexDecodeInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = SPEEX_BUFFER_SIZE;
	outRequestedProps->cBuffers = SPEEX_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}


STDMETHODIMP SpeexDecodeInputPin::Receive(IMediaSample* inSample) 
{
	//TODO::: All the internal buffer handling should be abstracted - it's duped in vorbis speex and flac
	//TODO::: Document the buffer end point cutting better
	CAutoLock locLock(mStreamLock);

	HRESULT locHR = CheckStreaming();

	if (locHR == S_OK) {


		BYTE* locBuff = NULL;
		locHR = inSample->GetPointer(&locBuff);

		if (locHR != S_OK) {
			//TODO::: Do a debug dump or something here with specific error info.
			return locHR;
		} else {
			REFERENCE_TIME locStart = -1;
			REFERENCE_TIME locEnd = -1;
			__int64 locSampleDuration = 0;
			inSample->GetTime(&locStart, &locEnd);

			HRESULT locResult = TransformData(locBuff, inSample->GetActualDataLength());
			if (locResult != S_OK) {
				return S_FALSE;
			}
			if (locEnd > 0) {
				//Can dump it all downstream now	
				IMediaSample* locSample;
				unsigned long locBytesCopied = 0;
				unsigned long locBytesToCopy = 0;

				locStart = convertGranuleToTime(locEnd) - (((mDecodedByteCount / mSampleFrameSize) * UNITS) / mSampleRate);
				do {
					HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);
					if (locHR != S_OK) {
						return locHR;
					}

					BYTE* locBuffer = NULL;
					locHR = locSample->GetPointer(&locBuffer);
				
					if (locHR != S_OK) {
						return locHR;
					}

					locBytesToCopy = ((mDecodedByteCount - locBytesCopied) <= locSample->GetSize()) ? (mDecodedByteCount - locBytesCopied) : locSample->GetSize();
					//locBytesCopied += locBytesToCopy;

					locSampleDuration = (((locBytesToCopy/mSampleFrameSize) * UNITS) / mSampleRate);
					locEnd = locStart + locSampleDuration;

					//Adjust the time stamps for rate and seeking
					REFERENCE_TIME locAdjustedStart = (locStart * RATE_DENOMINATOR) / mRateNumerator;
					REFERENCE_TIME locAdjustedEnd = (locEnd * RATE_DENOMINATOR) / mRateNumerator;
					locAdjustedStart -= m_tStart;
					locAdjustedEnd -= m_tStart;

					__int64 locSeekStripOffset = 0;
					if (locAdjustedEnd < 0) {
						locSample->Release();
					} else {
						if (locAdjustedStart < 0) {
							locSeekStripOffset = (-locAdjustedStart) * mSampleRate;
							locSeekStripOffset *= mSampleFrameSize;
							locSeekStripOffset /= UNITS;
							locSeekStripOffset += (mSampleFrameSize - (locSeekStripOffset % mSampleFrameSize));
							__int64 locStrippedDuration = (((locSeekStripOffset/mSampleFrameSize) * UNITS) / mSampleRate);
							locAdjustedStart += locStrippedDuration;
						}
							

					

						memcpy((void*)locBuffer, (const void*)&mDecodedBuffer[locBytesCopied + locSeekStripOffset], locBytesToCopy - locSeekStripOffset);

						locSample->SetTime(&locAdjustedStart, &locAdjustedEnd);
						locSample->SetMediaTime(&locStart, &locEnd);
						locSample->SetSyncPoint(TRUE);
						locSample->SetActualDataLength(locBytesToCopy - locSeekStripOffset);
						locHR = ((SpeexDecodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);
						if (locHR != S_OK) {
							return locHR;
						}
						locStart += locSampleDuration;

					}
					locBytesCopied += locBytesToCopy;

				
				} while(locBytesCopied < mDecodedByteCount);

				mDecodedByteCount = 0;
				
			}
			return S_OK;

		}
	} else {
		//Not streaming - Bail out.
		return S_FALSE;
	}
}


HRESULT SpeexDecodeInputPin::TransformData(BYTE* inBuf, long inNumBytes) 
{

	//TODO::: Verify size of remaining buffer
	SpeexDecoder::eSpeexResult locResult;
	locResult = mSpeexDecoder.decodePacket(		inBuf
											,	inNumBytes
											,	(short*)(mDecodedBuffer + mDecodedByteCount)
											,	DECODED_BUFFER_SIZE - mDecodedByteCount);

	if (locResult == SpeexDecoder::SPEEX_DATA_OK) {
		mDecodedByteCount += mSpeexFrameSize;
		return S_OK;
	}

	//For now, just silently ignore busted packets.
	return S_OK;
}



HRESULT SpeexDecodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	if (CheckMediaType(inMediaType) == S_OK) 
    {
		((SpeexDecodeFilter*)mParentFilter)->setSpeexFormat(inMediaType->pbFormat);
	} 
    else 
    {
		throw 0;
	}
	
    return CBaseInputPin::SetMediaType(inMediaType);
}

HRESULT SpeexDecodeInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (AbstractTransformInputPin::CheckMediaType(inMediaType) == S_OK) {
		if (inMediaType->cbFormat == SPEEX_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "Speex   ", 8) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			}
		}
	}
	return S_FALSE;
	
}

LOOG_INT64 SpeexDecodeInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
	if (mBegun) {	
		return (inGranule * UNITS) / mSampleRate;
	} else {
		return -1;
	}
}

LOOG_INT64 SpeexDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}

IOggDecoder::eAcceptHeaderResult SpeexDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	switch (mSetupState) {
		case VSS_SEEN_NOTHING:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "Speex   ", 8) == 0) {
				//TODO::: Possibly verify version

				if (mSpeexDecoder.decodePacket(	inCodecHeaderPacket->packetData(),	inCodecHeaderPacket->packetSize(),	NULL, 0) == SpeexDecoder::SPEEX_HEADER_OK) {
					mSetupState = VSS_SEEN_BOS;
					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				}
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
			
		case VSS_SEEN_BOS:
			//The comment packet can't be easily identified in speex.
			//Just ignore the second packet we see, and hope for the best

			if (mSpeexDecoder.decodePacket(	inCodecHeaderPacket->packetData(),	inCodecHeaderPacket->packetSize(),	NULL, 0) == SpeexDecoder::SPEEX_COMMENT_OK) {
				mSetupState = VSS_ALL_HEADERS_SEEN;

				mBegun = true;
		
				mNumChannels = mSpeexDecoder.numChannels();//mFishInfo.channels;
				mSampleFrameSize = mNumChannels * SIZE_16_BITS;
				mSampleRate = mSpeexDecoder.sampleRate(); //mFishInfo.samplerate;
				mSpeexFrameSize = mSampleFrameSize * mSpeexDecoder.frameSize();

				return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
			}
				
			return IOggDecoder::AHR_INVALID_HEADER;
	
		case VSS_ALL_HEADERS_SEEN:
		case VSS_ERROR:
		default:
			return IOggDecoder::AHR_UNEXPECTED;
	}
}
string SpeexDecodeInputPin::getCodecShortName()
{
	return "speex";
}

string SpeexDecodeInputPin::getCodecIdentString()
{
	//TODO::: Get the full ident string from the decoder
	return "speex";
}

