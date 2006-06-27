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

#include "VorbisDecodeInputPin.h"


VorbisDecodeInputPin::VorbisDecodeInputPin	(		AbstractTransformFilter* inFilter
												,	CCritSec* inFilterLock
												,	AbstractTransformOutputPin* inOutputPin
												,	vector<CMediaType*> inAcceptableMediaTypes
											)

	:	AbstractTransformInputPin			(		inFilter
												,	inFilterLock
												,	inOutputPin
												,	NAME("VorbisDecodeInputPin")
												,	L"Vorbis In", inAcceptableMediaTypes
											)
	,	mBegun(false)

	,	mNumChannels(0)
	,	mFrameSize(0)
	,	mSampleRate(0)
	,	mUptoFrame(0)
	,	mSetupState(VSS_SEEN_NOTHING)
	,	mDecodedBuffer(NULL)
	,	mDecodedByteCount(0)
	,	mRateNumerator(RATE_DENOMINATOR)
	,	mOggOutputPinInterface(NULL)
	,	mSentStreamOffset(false)
		
{
	debugLog.open(L"\\Memory Card\\vorbinpin.txt", ios_base::out);
	debugLog<<"Pin constructor"<<endl;
	ConstructCodec();
	debugLog<<"Pin constructor - post construct codec"<<endl;

	mDecodedBuffer = new unsigned char[DECODED_BUFFER_SIZE];
}

VorbisDecodeInputPin::~VorbisDecodeInputPin(void)
{
	debugLog.close();
	DestroyCodec();
	delete[] mDecodedBuffer;

}
//Is this needed ??
STDMETHODIMP VorbisDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IOggDecoder) {
		*ppv = (IOggDecoder*)this;
		//((IUnknown*)*ppv)->AddRef();
		return NOERROR;

	}

	return AbstractTransformInputPin::NonDelegatingQueryInterface(riid, ppv); 
}
bool VorbisDecodeInputPin::ConstructCodec() 
{
	return true;
	//Vorbis decoder should be good to go
}
void VorbisDecodeInputPin::DestroyCodec() 
{

}


STDMETHODIMP VorbisDecodeInputPin::NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate) 
{
	CAutoLock locLock(mStreamLock);
	//debugLog<<"New segment "<<inStartTime<<" - "<<inStopTime<<endl;
	mUptoFrame = 0;
	mRateNumerator = RATE_DENOMINATOR * inRate;
	if (mRateNumerator > RATE_DENOMINATOR) {
		mRateNumerator = RATE_DENOMINATOR;
	}
	return AbstractTransformInputPin::NewSegment(inStartTime, inStopTime, inRate);
	
}

STDMETHODIMP VorbisDecodeInputPin::EndFlush()
{
	CAutoLock locLock(m_pLock);
	
	HRESULT locHR = AbstractTransformInputPin::EndFlush();
	mDecodedByteCount = 0;
	return locHR;
}



STDMETHODIMP VorbisDecodeInputPin::Receive(IMediaSample* inSample) 
{
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

				locStart = convertGranuleToTime(locEnd) - (((mDecodedByteCount / mFrameSize) * UNITS) / mSampleRate);

				REFERENCE_TIME locGlobalOffset = 0;
				//Handle stream offsetting
				if (!mSentStreamOffset && (mOggOutputPinInterface != NULL)) {
					mOggOutputPinInterface->notifyStreamBaseTime(locStart);
					mSentStreamOffset = true;
					
				}

				if (mOggOutputPinInterface != NULL) {
					locGlobalOffset = mOggOutputPinInterface->getGlobalBaseTime();
				}

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

					debugLog<<"Sample Size = "<<locSample->GetSize()<<endl;
					locBytesToCopy = ((mDecodedByteCount - locBytesCopied) <= locSample->GetSize()) ? (mDecodedByteCount - locBytesCopied) : locSample->GetSize();
					debugLog<<"Filled size = "<<locBytesToCopy<<endl;
					debugLog<<"Actual Buffer count = "<<mOutputPin->actualBufferCount()<<endl;
					//locBytesCopied += locBytesToCopy;

					locSampleDuration = (((locBytesToCopy/mFrameSize) * UNITS) / mSampleRate);
					locEnd = locStart + locSampleDuration;

					//Adjust the time stamps for rate and seeking
					REFERENCE_TIME locAdjustedStart = (locStart * RATE_DENOMINATOR) / mRateNumerator;
					REFERENCE_TIME locAdjustedEnd = (locEnd * RATE_DENOMINATOR) / mRateNumerator;
					locAdjustedStart -= (m_tStart + locGlobalOffset);
					locAdjustedEnd -= (m_tStart + locGlobalOffset);

					__int64 locSeekStripOffset = 0;
					if (locAdjustedEnd < 0) {
						locSample->Release();
					} else {
						if (locAdjustedStart < 0) {
							locSeekStripOffset = (-locAdjustedStart) * mSampleRate;
							locSeekStripOffset *= mFrameSize;
							locSeekStripOffset /= UNITS;
							locSeekStripOffset += (mFrameSize - (locSeekStripOffset % mFrameSize));
							__int64 locStrippedDuration = (((locSeekStripOffset/mFrameSize) * UNITS) / mSampleRate);
							locAdjustedStart += locStrippedDuration;
						}
							

					

						memcpy((void*)locBuffer, (const void*)&mDecodedBuffer[locBytesCopied + locSeekStripOffset], locBytesToCopy - locSeekStripOffset);

						locSample->SetTime(&locAdjustedStart, &locAdjustedEnd);
						locSample->SetMediaTime(&locStart, &locEnd);
						locSample->SetSyncPoint(TRUE);
						locSample->SetActualDataLength(locBytesToCopy - locSeekStripOffset);
						locHR = ((VorbisDecodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);
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

HRESULT VorbisDecodeInputPin::TransformData(BYTE* inBuf, long inNumBytes) 
{
	//TODO::: Return types !!!

	VorbisDecoder::eVorbisResult locResult;
	unsigned long locNumSamples = 0;
	locResult = mVorbisDecoder.decodePacket(	inBuf
											,	inNumBytes
											,	(short*)(mDecodedBuffer + mDecodedByteCount)
											,	DECODED_BUFFER_SIZE - mDecodedByteCount
											,	&locNumSamples);

	if (locResult == VorbisDecoder::VORBIS_DATA_OK) {
		mDecodedByteCount += locNumSamples * mFrameSize;
		return S_OK;
	}

	//For now, just silently ignore busted packets.
	return S_OK;

}


HRESULT VorbisDecodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//FIX:::Error checking

	if (CheckMediaType(inMediaType) == S_OK) {
		((VorbisDecodeFilter*)mParentFilter)->setVorbisFormat(inMediaType->pbFormat);
		debugLog<<"Set media type"<<endl;
	} else {
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}
HRESULT VorbisDecodeInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (AbstractTransformInputPin::CheckMediaType(inMediaType) == S_OK) {
		if (inMediaType->cbFormat == VORBIS_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "\001vorbis", 7) == 0) {
				//TODO::: Possibly verify version
				debugLog<<"Check media type ok"<<endl;
				return S_OK;
			}
		}
	}
	debugLog<<"Check media type failed"<<endl;
	return S_FALSE;
	
}

HRESULT VorbisDecodeInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = VORBIS_BUFFER_SIZE;
	outRequestedProps->cBuffers = VORBIS_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}

LOOG_INT64 VorbisDecodeInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
	if (mBegun) {	
		return (inGranule * UNITS) / mSampleRate;
	} else {
		return -1;
	}
}

LOOG_INT64 VorbisDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}
IOggDecoder::eAcceptHeaderResult VorbisDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	unsigned long locDummy;
	switch (mSetupState) {
		case VSS_SEEN_NOTHING:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\001vorbis", 7) == 0) {
				//TODO::: Possibly verify version
				if (mVorbisDecoder.decodePacket(		inCodecHeaderPacket->packetData()
													,	inCodecHeaderPacket->packetSize()
													,	NULL
													,	0
													,	&locDummy) == VorbisDecoder::VORBIS_HEADER_OK) {
					mSetupState = VSS_SEEN_BOS;
					debugLog<<"Saw first header"<<endl;
					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				}
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
			
		case VSS_SEEN_BOS:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\003vorbis", 7) == 0) {
				if (mVorbisDecoder.decodePacket(		inCodecHeaderPacket->packetData()
													,	inCodecHeaderPacket->packetSize()
													,	NULL
													,	0
													,	&locDummy) == VorbisDecoder::VORBIS_COMMENT_OK) {

					mSetupState = VSS_SEEN_COMMENT;
					debugLog<<"Saw second header"<<endl;
					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				}
				
				
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
			
		case VSS_SEEN_COMMENT:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\005vorbis", 7) == 0) {
				if (mVorbisDecoder.decodePacket(		inCodecHeaderPacket->packetData()
													,	inCodecHeaderPacket->packetSize()
													,	NULL
													,	0
													,	&locDummy) == VorbisDecoder::VORBIS_CODEBOOK_OK) {

		
					//Is mBegun useful ?
					mBegun = true;
			
					mNumChannels = mVorbisDecoder.numChannels();
					mFrameSize = mNumChannels * SIZE_16_BITS;
					mSampleRate = mVorbisDecoder.sampleRate(); 

		
					mSetupState = VSS_ALL_HEADERS_SEEN;
					debugLog<<"Saw third header"<<endl;
					return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
				}
				
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
		case VSS_ALL_HEADERS_SEEN:
		case VSS_ERROR:
		default:
			return IOggDecoder::AHR_UNEXPECTED;
	}
}
string VorbisDecodeInputPin::getCodecShortName()
{
	return "vorbis";
}
string VorbisDecodeInputPin::getCodecIdentString()
{
	//TODO::: Get full ident string
	return "vorbis";
}

HRESULT VorbisDecodeInputPin::CompleteConnect(IPin *inReceivePin)
{
	IOggOutputPin* locOggOutput = NULL;
	mSentStreamOffset = false;
	HRESULT locHR = inReceivePin->QueryInterface(IID_IOggOutputPin, (void**)&locOggOutput);
	if (locHR == S_OK) {
		mOggOutputPinInterface = locOggOutput;
		
	} else {
		mOggOutputPinInterface = NULL;
	}
	debugLog<<"Complete Connect"<<endl;
	return AbstractTransformInputPin::CompleteConnect(inReceivePin);
	
}