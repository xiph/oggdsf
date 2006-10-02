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
#include "flacdecodeinputpin.h"

FLACDecodeInputPin::FLACDecodeInputPin(     AbstractTransformFilter* inParentFilter
                                        ,   CCritSec* inFilterLock
                                        ,   AbstractTransformOutputPin* inOutputPin
                                        ,   vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(      inParentFilter
                                    ,   inFilterLock
                                    ,   inOutputPin
                                    ,   NAME("FLACDecodeInputPin")
                                    ,   L"FLAC In"
                                    ,   inAcceptableMediaTypes)
	,	mGotMetaData(false)
	,	mCodecLock(NULL)
	,	mFLACType(FT_UNKNOWN)
	,	mMetadataPacket(NULL)
	,	mSetupState(VSS_SEEN_NOTHING)
	,	mDecodedByteCount(0)
	,	mDecodedBuffer(NULL)
	,	mRateNumerator(RATE_DENOMINATOR)

{
	debugLog.open("F:\\logs\\flacdecodefilter.log", ios_base::out);
	mCodecLock = new CCritSec;			//Deleted in destructor.
	ConstructCodec();

	mDecodedBuffer = new unsigned char[DECODED_BUFFER_SIZE];
}

FLACDecodeInputPin::~FLACDecodeInputPin(void)
{
	debugLog.close();
	delete mCodecLock;
	delete mMetadataPacket;
	delete mDecodedBuffer;
	
}

STDMETHODIMP FLACDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}

bool FLACDecodeInputPin::ConstructCodec() 
{
	mFLACDecoder.initCodec();

	return true;
}

void FLACDecodeInputPin::DestroyCodec() 
{

}

STDMETHODIMP FLACDecodeInputPin::NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate) 
{
	CAutoLock locLock(mStreamLock);
    debugLog<<"New segment: "<<inStartTime<<" - "<<inStopTime<<endl;
	return AbstractTransformInputPin::NewSegment(inStartTime, inStopTime, inRate);
	
}

STDMETHODIMP FLACDecodeInputPin::Receive(IMediaSample* inSample) 
{
	CAutoLock locLock(mStreamLock);

	HRESULT locHR = CheckStreaming();

	if (locHR == S_OK) {
		BYTE* locBuff = NULL;
		locHR = inSample->GetPointer(&locBuff);

		if ((inSample->GetActualDataLength() > 1) && ((locBuff[0] != 0xff) || (locBuff[1] != 0xf8))) {
			//inInputSample->Release();
            debugLog<<"Receive: Ignoring Header"<<endl;
			//This is a header, so ignore it
			return S_OK;
		}

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
                debugLog<<"Receive: Transform returned "<<locResult<<endl;
				return S_FALSE;
			}
			if (locEnd > 0) {
				//Can dump it all downstream now	
				IMediaSample* locSample;
				unsigned long locBytesCopied = 0;
				unsigned long locBytesToCopy = 0;

				locStart = convertGranuleToTime(locEnd) - (((mDecodedByteCount / mFLACDecoder.frameSize()) * UNITS) / mFLACDecoder.sampleRate());
				do {
					HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);
					if (locHR != S_OK) {
                        debugLog<<"Receive: Get delivery buffer returned "<<locResult<<endl;
						return locHR;
					}

					BYTE* locBuffer = NULL;
					locHR = locSample->GetPointer(&locBuffer);
				
					if (locHR != S_OK) {
                        debugLog<<"Receive: Get pointer returned "<<locResult<<endl;
						return locHR;
					}

					locBytesToCopy = ((mDecodedByteCount - locBytesCopied) <= locSample->GetSize()) ? (mDecodedByteCount - locBytesCopied) : locSample->GetSize();
					//locBytesCopied += locBytesToCopy;

					locSampleDuration = (((locBytesToCopy/mFLACDecoder.frameSize()) * UNITS) / mFLACDecoder.sampleRate());
					locEnd = locStart + locSampleDuration;

					//Adjust the time stamps for rate and seeking
					REFERENCE_TIME locAdjustedStart = (locStart * RATE_DENOMINATOR) / mRateNumerator;
					REFERENCE_TIME locAdjustedEnd = (locEnd * RATE_DENOMINATOR) / mRateNumerator;
                    debugLog<<"Receive: Raw times = "<<locStart<<" - "<<locEnd<<endl;
					locAdjustedStart -= m_tStart;
					locAdjustedEnd -= m_tStart;

                    debugLog<<"Receive: Start adjusted times = "<<locAdjustedStart<<" - "<<locAdjustedEnd<<endl;

					__int64 locSeekStripOffset = 0;
					if (locAdjustedEnd < 0) {
                        debugLog<<"Receive: Pre-zero. Not delivering. m_tStart = "<<m_tStart<<endl;
						locSample->Release();
					} else {
						if (locAdjustedStart < 0) {
							locSeekStripOffset = (-locAdjustedStart) * mFLACDecoder.sampleRate();
							locSeekStripOffset *= mFLACDecoder.frameSize();
							locSeekStripOffset /= UNITS;
							locSeekStripOffset += (mFLACDecoder.frameSize() - (locSeekStripOffset % mFLACDecoder.frameSize()));
							__int64 locStrippedDuration = (((locSeekStripOffset/mFLACDecoder.frameSize()) * UNITS) / mFLACDecoder.sampleRate());
							locAdjustedStart += locStrippedDuration;
                            debugLog<<"Receive: strippedDuration = "<<locStrippedDuration<<endl;
						}
							

					

						memcpy((void*)locBuffer, (const void*)&mDecodedBuffer[locBytesCopied + locSeekStripOffset], locBytesToCopy - locSeekStripOffset);

                        debugLog<<"Receive: Seek adjusted times = "<<locAdjustedStart<<" - "<<locAdjustedEnd<<endl;
						locSample->SetTime(&locAdjustedStart, &locAdjustedEnd);
						locSample->SetMediaTime(&locStart, &locEnd);
						locSample->SetSyncPoint(TRUE);
						locSample->SetActualDataLength(locBytesToCopy - locSeekStripOffset);
						locHR = ((FLACDecodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);
						if (locHR != S_OK) {
                            debugLog<<"Receive: Deliver returned "<<locResult<<endl;
							return locHR;
						}
						locStart += locSampleDuration;

					}
					locBytesCopied += locBytesToCopy;
                    debugLog<<"Receive: bytes copied = "<<locBytesCopied<<endl;
                    debugLog<<"Receive: bytes to copy = "<<locBytesToCopy<<endl;

				
				} while(locBytesCopied < mDecodedByteCount);

                debugLog<<"Receive: out of loop"<<endl;
				mDecodedByteCount = 0;
				
			}
            debugLog<<"Receive: return ok"<<endl;
			return S_OK;

		}
	} else {
		//Not streaming - Bail out.
		return S_FALSE;
	}
}


HRESULT FLACDecodeInputPin::TransformData(BYTE* inBuf, long inNumBytes) 
{
	//TODO::: There is a thread blocking problem here. sometimes the this code
	//		inside the checkstream check can be called while the graph is flushing.
	//
	//		Probably just needs a lock here on the filter, and/or in the begin/end flush method
	//if (CheckStreaming() == S_OK) {
		unsigned char* locInBuff = new unsigned char[inNumBytes];
		memcpy((void*)locInBuff, (const void*)inBuf, inNumBytes);
		OggPacket* locInputPacket = new OggPacket(locInBuff, inNumBytes, false, false);

        StampedOggPacket* locStamped = NULL;
        {
            CAutoLock locCodecLock(mCodecLock);
		    locStamped = (StampedOggPacket*)mFLACDecoder.decodeFLAC(locInputPacket)->clone();
        }

		FLACDecodeFilter* locFilter = reinterpret_cast<FLACDecodeFilter*>(m_pFilter);

	

		unsigned long locActualSize = locStamped->packetSize();
		//unsigned long locTotalFrameCount = inFrames * locThis->mNumChannels;
		unsigned long locBufferRemaining = DECODED_BUFFER_SIZE - mDecodedByteCount;
		


		//Create a pointer into the buffer		
		//signed short* locShortBuffer = (signed short*)&mDecodedBuffer[mDecodedByteCount];
		
		
		//signed short tempInt = 0;
		//float tempFloat = 0;
		
		//FIX:::Move the clipping to the abstract function

		if (locBufferRemaining >= locActualSize) {
			
			memcpy((void*)&mDecodedBuffer[mDecodedByteCount], (const void*)locStamped->packetData(), locActualSize);

			mDecodedByteCount += locActualSize;
			delete locStamped;
			return S_OK;
		} else {
			throw 0;
		}
	//} else {
	//	DbgLog((LOG_TRACE,1,TEXT("Not streaming")));
	//	return -1;
	//}

}


STDMETHODIMP FLACDecodeInputPin::BeginFlush() 
{
	CAutoLock locLock(m_pLock);
	
	debugLog<<"BeginFlush :"<<endl;

	HRESULT locHR = AbstractTransformInputPin::BeginFlush();
	{	//PROTECT CODEC FROM IMPLODING
		CAutoLock locCodecLock(mCodecLock);
        debugLog<<"BeginFlush : Calling flush on the codec."<<endl;
		mFLACDecoder.flushCodec();
	}	//END CRITICAL SECTION
	return locHR;
	
}

STDMETHODIMP FLACDecodeInputPin::EndFlush()
{
	CAutoLock locLock(m_pLock);

	mDecodedByteCount = 0;	
	HRESULT locHR = AbstractTransformInputPin::EndFlush();
    debugLog<<"EndFlush : End of flush"<<endl;

	return locHR;
}

STDMETHODIMP FLACDecodeInputPin::EndOfStream(void) {
	CAutoLock locStreamLock(mStreamLock);

    debugLog<<"EndOfStream :"<<endl;
	{	//PROTECT CODEC FROM IMPLODING
		CAutoLock locCodecLock(mCodecLock);
        debugLog<<"EndOfStream : Calling flush on the codec."<<endl;
		mFLACDecoder.flushCodec();
	}	//END CRITICAL SECTION

	return AbstractTransformInputPin::EndOfStream();
}

HRESULT FLACDecodeInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = FLAC_BUFFER_SIZE;
	outRequestedProps->cBuffers = FLAC_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}
HRESULT FLACDecodeInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (AbstractTransformInputPin::CheckMediaType(inMediaType) == S_OK) {
		if (inMediaType->cbFormat == 4) {
			if (strncmp((char*)inMediaType->pbFormat, "fLaC", 4) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			}
		} else if (inMediaType->cbFormat > 4) {
			if (strncmp((char*)inMediaType->pbFormat, "\177FLAC", 5) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			}
		}
	}
	return S_FALSE;
	
}
HRESULT FLACDecodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.
	if (CheckMediaType(inMediaType) == S_OK) {
		//((FLACDecodeFilter*)mParentFilter)->setFLACFormatBlock(inMediaType->pbFormat);
		if (inMediaType->cbFormat == 4) {
			if (strncmp((char*)inMediaType->pbFormat, "fLaC", 4) == 0) {
				mFLACType = FT_CLASSIC;
				return S_OK;
			}
		} else if (inMediaType->cbFormat > 4) {
			if (strncmp((char*)inMediaType->pbFormat, "\177FLAC", 5) == 0) {
				mFLACType = FT_OGG_FLAC_1;
				return S_OK;
				
			}
		}
		return S_FALSE;
		
	} else {
		throw 0;
	}


	return CBaseInputPin::SetMediaType(inMediaType);
}

LOOG_INT64 FLACDecodeInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
		
	return (inGranule * UNITS) / ((FLACDecodeFilter*)mParentFilter)->getFLACFormatBlock()->samplesPerSec;
	
}

LOOG_INT64 FLACDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}
IOggDecoder::eAcceptHeaderResult FLACDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	const unsigned char MORE_HEADERS_MASK = 128;   //10000000
	switch (mFLACType) {
		case FT_CLASSIC:
			switch (mSetupState) {
				case VSS_SEEN_NOTHING:
					if (strncmp((char*)inCodecHeaderPacket->packetData(), "fLaC", 4) == 0) {
						mSetupState = VSS_SEEN_BOS;
						delete mMetadataPacket;
						mMetadataPacket = inCodecHeaderPacket->clone();
						return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
					}

					//TODO::: new flac format
					mSetupState = VSS_ERROR;
					return IOggDecoder::AHR_INVALID_HEADER;
				case VSS_SEEN_BOS:
					mMetadataPacket->merge(inCodecHeaderPacket);
					if ((inCodecHeaderPacket->packetData()[0] & MORE_HEADERS_MASK) != 0) {
						//Last packet
						mSetupState = VSS_ALL_HEADERS_SEEN; 
						((FLACDecodeFilter*)mParentFilter)->setFLACFormatBlock(mMetadataPacket->packetData());
						mFLACDecoder.acceptMetadata(mMetadataPacket);
						mMetadataPacket = NULL;
				
						//TODO::: Give it to the codec

						return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
					}

					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				default:
					return IOggDecoder::AHR_UNEXPECTED;
			}

		case FT_OGG_FLAC_1:
			switch(mSetupState) {
				case VSS_SEEN_NOTHING:
					if (strncmp((char*)inCodecHeaderPacket->packetData(),  "\177FLAC", 5) == 0) {
						mSetupState = VSS_SEEN_BOS;
						delete mMetadataPacket;
						unsigned char* locBuff = new unsigned char[inCodecHeaderPacket->packetSize() - 9];
						memcpy((void*)locBuff, (const void*)(inCodecHeaderPacket->packetData() + 9), inCodecHeaderPacket->packetSize() - 9);
						mMetadataPacket = new OggPacket(locBuff, inCodecHeaderPacket->packetSize() - 9,  false, false);
						return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
					}

					
					mSetupState = VSS_ERROR;
					return IOggDecoder::AHR_INVALID_HEADER;
				case VSS_SEEN_BOS:
					mMetadataPacket->merge(inCodecHeaderPacket);
					if ((inCodecHeaderPacket->packetData()[0] & MORE_HEADERS_MASK) != 0) {
						//Last packet
						mSetupState = VSS_ALL_HEADERS_SEEN; 
						((FLACDecodeFilter*)mParentFilter)->setFLACFormatBlock(mMetadataPacket->packetData());
						mFLACDecoder.acceptMetadata(mMetadataPacket);
						mMetadataPacket = NULL;
				
						//TODO::: Give it to the codec

						return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
					}

					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				default:
					return IOggDecoder::AHR_UNEXPECTED;


			};
		default:
			return IOggDecoder::AHR_INVALID_HEADER;


	}

}
string FLACDecodeInputPin::getCodecShortName()
{
	return "flac";
}
string FLACDecodeInputPin::getCodecIdentString()
{
	//TODO:::
	return "flac";
}

