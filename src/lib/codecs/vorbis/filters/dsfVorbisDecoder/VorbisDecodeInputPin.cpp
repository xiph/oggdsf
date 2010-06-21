//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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

#include "stdafx.h"
#include "VorbisDecodeInputPin.h"
#include "vorbisdecoderdllstuff.h"


VorbisDecodeInputPin::VorbisDecodeInputPin(AbstractTransformFilter* inFilter,	CCritSec* inFilterLock,	
                                           AbstractTransformOutputPin* inOutputPin, 
                                           vector<CMediaType*> inAcceptableMediaTypes) :	
AbstractTransformInputPin (inFilter, inFilterLock,	inOutputPin, NAME("VorbisDecodeInputPin"),	
                           L"Vorbis In", inAcceptableMediaTypes),	
mBegun(false),	
mNumChannels(0),	
mFrameSize(0),	
mSampleRate(0),
// mUptoFrame(0),
mSetupState(VSS_SEEN_NOTHING),	
mDecodedBuffer(NULL),	
mDecodedByteCount(0),	
mRateNumerator(RATE_DENOMINATOR),	
mOggOutputPinInterface(NULL),	
mSentStreamOffset(false)		
{
	LOG(logDEBUG) << "Pin constructor";
	ConstructCodec();
	LOG(logDEBUG) << "Pin constructor - post construct codec";

	mDecodedBuffer = new unsigned char[DECODED_BUFFER_SIZE];
}

VorbisDecodeInputPin::~VorbisDecodeInputPin(void)
{
	DestroyCodec();
	delete[] mDecodedBuffer;
}

//Is this needed ??
STDMETHODIMP VorbisDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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
	//LOG(logDEBUG) << "New segment " << inStartTime<< " - " << inStopTime;
	//mUptoFrame = 0;
	mRateNumerator = RATE_DENOMINATOR * inRate;
	if (mRateNumerator > RATE_DENOMINATOR) 
    {
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

    HRESULT hr = CheckStreaming();

    if (hr != S_OK) 
    {
        //Not streaming - Bail out.
        return S_FALSE;
    }

    BYTE* buffer = NULL;
    hr = inSample->GetPointer(&buffer);

    if (hr != S_OK) 
    {
        LOG(logERROR) << __FUNCTIONW__ << " inSample->GetPointer failed, error code: 0x" << std::hex << hr;
        return hr;
    } 

    REFERENCE_TIME tStart = -1;
    REFERENCE_TIME tStop = -1;
    __int64 sampleDuration = 0;
    inSample->GetTime(&tStart, &tStop);

    hr = TransformData(buffer, inSample->GetActualDataLength());
    if (hr != S_OK) 
    {
        return S_FALSE;
    }
    if (tStop > 0) 
    {
        //Can dump it all downstream now	
        IMediaSample* sample;
        unsigned long bytesCopied = 0;
        unsigned long bytesToCopy = 0;

        REFERENCE_TIME globalOffset = 0;
        //Handle stream offsetting
        if (!mSentStreamOffset && (mOggOutputPinInterface != NULL)) 
        {
            mOggOutputPinInterface->notifyStreamBaseTime(tStart);
            mSentStreamOffset = true;	
        }

        if (mOggOutputPinInterface != NULL) 
        {
            tStart = convertGranuleToTime(tStop) - (((mDecodedByteCount / mFrameSize) * UNITS) / mSampleRate);
            globalOffset = mOggOutputPinInterface->getGlobalBaseTime();
        }

        do 
        {
            HRESULT hr = mOutputPin->GetDeliveryBuffer(&sample, NULL, NULL, NULL);
            if (hr != S_OK) 
            {
                return hr;
            }

            BYTE* locBuffer = NULL;
            hr = sample->GetPointer(&locBuffer);

            if (hr != S_OK) 
            {
                return hr;
            }

            LOG(logDEBUG4) << __FUNCTIONW__ << " Sample Size: " << sample->GetSize();
            bytesToCopy = sample->GetSize();

            if (mDecodedByteCount - bytesCopied < sample->GetSize()) 
            {
                bytesToCopy = mDecodedByteCount - bytesCopied;
            }

            LOG(logDEBUG4) << __FUNCTIONW__ << " Bytes to copy: " << bytesToCopy;
            LOG(logDEBUG4) << __FUNCTIONW__ << " Actual Buffer count = " << mOutputPin->actualBufferCount();
            //bytesCopied += bytesToCopy;

            sampleDuration = (((bytesToCopy/mFrameSize) * UNITS) / mSampleRate);
            tStop = tStart + sampleDuration;

            //Adjust the time stamps for rate and seeking
            REFERENCE_TIME adjustedStart = (tStart * RATE_DENOMINATOR) / mRateNumerator;
            REFERENCE_TIME adjustedStop = (tStop * RATE_DENOMINATOR) / mRateNumerator;
            adjustedStart -= (m_tStart + globalOffset);
            adjustedStop -= (m_tStart + globalOffset);

            __int64 seekStripOffset = 0;
            if (adjustedStop < 0) 
            {
                sample->Release();
            } 
            else 
            {
                if (adjustedStart < 0) 
                {
                    seekStripOffset = (-adjustedStart) * mSampleRate;
                    seekStripOffset *= mFrameSize;
                    seekStripOffset /= UNITS;
                    seekStripOffset += (mFrameSize - (seekStripOffset % mFrameSize));
                    __int64 strippedDuration = (((seekStripOffset/mFrameSize) * UNITS) / mSampleRate);
                    adjustedStart += strippedDuration;
                }					

                LOG(logDEBUG4) << __FUNCTIONW__ << " Seek strip offset: " << seekStripOffset;

                if (bytesToCopy - seekStripOffset < 0)
                {
                    sample->Release();
                }
                else
                {
                    //memcpy((void*)locBuffer, (const void*)&mDecodedBuffer[bytesCopied + seekStripOffset], bytesToCopy - seekStripOffset);
                    reorderChannels(locBuffer, &mDecodedBuffer[bytesCopied + seekStripOffset], bytesToCopy - seekStripOffset);

                    sample->SetTime(&adjustedStart, &adjustedStop);
                    sample->SetMediaTime(&tStart, &tStop);
                    sample->SetSyncPoint(TRUE);
                    sample->SetActualDataLength(bytesToCopy - seekStripOffset);
                    hr = ((VorbisDecodeOutputPin*)(mOutputPin))->mDataQueue->Receive(sample);
                    if (hr != S_OK) 
                    {
                        return hr;
                    }
                    tStart += sampleDuration;
                }
            }
            bytesCopied += bytesToCopy;


        } while(bytesCopied < mDecodedByteCount);

        mDecodedByteCount = 0;

    }
    return S_OK;
}

void VorbisDecodeInputPin::reorderChannels(unsigned char* inDestBuffer, const unsigned char* inSourceBuffer, unsigned long inNumBytes)
{
    //memcpy((void*)locBuffer, (const void*)&mDecodedBuffer[bytesCopied + seekStripOffset], bytesToCopy - seekStripOffset);

    if (GetFilter()->USE_CORRECT_VORBIS_CHANNEL_MAPPING && 
        (mNumChannels == 6  || mNumChannels == 3 || mNumChannels == 5)) 
    {
        //We only have to reorder the channels if we are using the extended format, we have declared that we want to map correctly
        // and the number channels is 3 or 6. All other cases we just memcpy
    
        unsigned long locSampleCount = inNumBytes / (mNumChannels * sizeof(short));

        short* locDest = (short*)inDestBuffer;
        const short* locSource = (short*)inSourceBuffer;

        if (mNumChannels == 6)
        {
            for (unsigned long i = 0; i < locSampleCount; i++)
            {
                *locDest++ = *locSource;
                *locDest++ = locSource[2];
                *locDest++ = locSource[1];
                *locDest++ = locSource[5];
                *locDest++ = locSource[3];
                *locDest++ = locSource[4];

                locSource += 6;
             }

        } 
        else if (mNumChannels == 3) 
        {
            //3 channels
            for (unsigned long i = 0; i < locSampleCount; i++)
            {
                *locDest++ = *locSource;
                *locDest++ = locSource[2];
                *locDest++ = locSource[1];
                locSource += 3;
            }
        } 
        else 
        {
            //5 channels
            for (unsigned long i = 0; i < locSampleCount; i++)
            {
                *locDest++ = *locSource;
                *locDest++ = locSource[2];
                *locDest++ = locSource[1];
                *locDest++ = locSource[3];
                *locDest++ = locSource[4];
                locSource += 5;
            }
        }
        return;
    }
    
    memcpy((void*)inDestBuffer, (const void*)inSourceBuffer, inNumBytes);
}

HRESULT VorbisDecodeInputPin::TransformData(BYTE* inBuf, long inNumBytes) 
{
	//TODO::: Return types !!!

	VorbisDecoder::eVorbisResult locResult;
	unsigned long locNumSamples = 0;
	locResult = mVorbisDecoder.decodePacket(inBuf, inNumBytes,(short*)(mDecodedBuffer + mDecodedByteCount),	
                    DECODED_BUFFER_SIZE - mDecodedByteCount, &locNumSamples);

	if (locResult == VorbisDecoder::VORBIS_DATA_OK) 
    {
		mDecodedByteCount += locNumSamples * mFrameSize;
		return S_OK;
	}

	//For now, just silently ignore busted packets.
	return S_OK;
}

VorbisDecodeFilter* VorbisDecodeInputPin::GetFilter()
{
    return static_cast<VorbisDecodeFilter*>(mParentFilter);
}

HRESULT VorbisDecodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//FIX:::Error checking
	if (CheckMediaType(inMediaType) == S_OK) 
    {
        if (inMediaType->majortype == MEDIATYPE_OggPacketStream &&
            inMediaType->formattype == FORMAT_OggIdentHeader &&
            inMediaType->cbFormat == VORBIS_IDENT_HEADER_SIZE) 
        {
		    GetFilter()->setVorbisFormat(inMediaType->pbFormat);
        }
        else if (inMediaType->majortype == MEDIATYPE_Audio &&
                 inMediaType->subtype == MEDIASUBTYPE_Vorbis &&
                 inMediaType->formattype == FORMAT_Vorbis)
        {
            GetFilter()->setVorbisFormat(reinterpret_cast<VORBISFORMAT*>(inMediaType->pbFormat));
        }
	} 
    else 
    {
		throw 0;
	}
	
    return CBaseInputPin::SetMediaType(inMediaType);
}
HRESULT VorbisDecodeInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (AbstractTransformInputPin::CheckMediaType(inMediaType) == S_OK) 
    {
		if (inMediaType->majortype == MEDIATYPE_OggPacketStream &&
            inMediaType->formattype == FORMAT_OggIdentHeader &&
            inMediaType->cbFormat == VORBIS_IDENT_HEADER_SIZE) 
        {
			if (strncmp((char*)inMediaType->pbFormat, "\001vorbis", 7) != 0) 
            {
                LOG(logDEBUG) << "Check media type failed";
                return S_FALSE;
			}
		}
        LOG(logDEBUG) << "Check media type ok";
        return S_OK;
	}
    LOG(logDEBUG) << "Check media type failed";
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
	if (mBegun) 
    {	
		return (inGranule * UNITS) / mSampleRate;
	} 
    else 
    {
		return -1;
	}
}

LOOG_INT64 VorbisDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
    //return (inGranule <= 4096) ? 0 : (inGranule - 4096);
    return inGranule;
}
IOggDecoder::eAcceptHeaderResult VorbisDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	unsigned long locDummy;
	switch (mSetupState) 
    {
		case VSS_SEEN_NOTHING:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\001vorbis", 7) == 0) 
            {
				//TODO::: Possibly verify version
				if (mVorbisDecoder.decodePacket(		inCodecHeaderPacket->packetData()
													,	inCodecHeaderPacket->packetSize()
													,	NULL
													,	0
													,	&locDummy) == VorbisDecoder::VORBIS_HEADER_OK) {
					mSetupState = VSS_SEEN_BOS;
					LOG(logDEBUG) << "Saw first header";
					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				}
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
			
		case VSS_SEEN_BOS:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\003vorbis", 7) == 0) 
            {
				if (mVorbisDecoder.decodePacket(		inCodecHeaderPacket->packetData()
													,	inCodecHeaderPacket->packetSize()
													,	NULL
													,	0
													,	&locDummy) == VorbisDecoder::VORBIS_COMMENT_OK) {

					mSetupState = VSS_SEEN_COMMENT;
					LOG(logDEBUG) << "Saw second header";
					return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
				}	
			}
			return IOggDecoder::AHR_INVALID_HEADER;
			
			
		case VSS_SEEN_COMMENT:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\005vorbis", 7) == 0) 
            {
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
					LOG(logDEBUG) << "Saw third header";
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
	if (locHR == S_OK) 
    {
		mOggOutputPinInterface = locOggOutput;	
	} 
    else 
    {
		mOggOutputPinInterface = NULL;
	}
	LOG(logDEBUG) << "Complete Connect";

    if (GetFilter()->getVorbisFormatBlock())
    {
        mNumChannels = GetFilter()->getVorbisFormatBlock()->numChannels;
        mFrameSize = mNumChannels * SIZE_16_BITS;
        mSampleRate = GetFilter()->getVorbisFormatBlock()->samplesPerSec;
    }

    return AbstractTransformInputPin::CompleteConnect(inReceivePin);
}
