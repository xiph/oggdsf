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
#include "SpeexDecodeInputPin.h"


SpeexDecodeInputPin::SpeexDecodeInputPin(AbstractAudioDecodeFilter* inFilter, CCritSec* inFilterLock, AbstractAudioDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType)
	:	AbstractAudioDecodeInputPin(inFilter, inFilterLock, inOutputPin, NAME("SpeexDecodeInputPin"), L"Speex In", inAcceptMediaType)
{
	ConstructCodec();
}

bool SpeexDecodeInputPin::ConstructCodec() {
	mFishSound = fish_sound_new (FISH_SOUND_DECODE, &mFishInfo);

	int i = 1;
	//FIX::: Use new API for interleave setting
	fish_sound_command(mFishSound, FISH_SOUND_SET_INTERLEAVE, &i, sizeof(int));

	fish_sound_set_decoded_callback (mFishSound, SpeexDecodeInputPin::SpeexDecoded, this);
	//FIX::: Proper return value
	return true;
}
void SpeexDecodeInputPin::DestroyCodec() {
	fish_sound_delete(mFishSound);
	mFishSound = NULL;
}
SpeexDecodeInputPin::~SpeexDecodeInputPin(void)
{
	DestroyCodec();
}

STDMETHODIMP SpeexDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}


int SpeexDecodeInputPin::SpeexDecoded (FishSound* inFishSound, float** inPCM, long inFrames, void* inThisPointer) 
{
	//Do we need to delete the pcm structure ???? 
	//More of this can go to the abstract class.

	//For convenience we do all these cast once and for all here.
	SpeexDecodeInputPin* locThis = reinterpret_cast<SpeexDecodeInputPin*> (inThisPointer);
	SpeexDecodeFilter* locFilter = reinterpret_cast<SpeexDecodeFilter*>(locThis->m_pFilter);
	

	if (! locThis->mBegun) {
	
		fish_sound_command (locThis->mFishSound, FISH_SOUND_GET_INFO, &(locThis->mFishInfo), sizeof (FishSoundInfo)); 
		locThis->mBegun = true;
		
		locThis->mNumChannels = locThis->mFishInfo.channels;
		locThis->mFrameSize = locThis->mNumChannels * SIZE_16_BITS;
		locThis->mSampleRate = locThis->mFishInfo.samplerate;

	}


	//************************ OLD WAY
	//REFERENCE_TIME locFrameStart = locThis->CurrentStartTime() + (((__int64)(locThis->mUptoFrame * HUNDRED_NANOS)) / locThis->mSampleRate);
	//REFERENCE_TIME locFrameStart = (((__int64)(locThis->mUptoFrame * UNITS)) / locThis->mSampleRate);
	////Increment the frame counter
	//locThis->mUptoFrame += inFrames;
	////Make the end frame counter
	////REFERENCE_TIME locFrameEnd = locThis->CurrentStartTime() + (((__int64)(locThis->mUptoFrame * HUNDRED_NANOS)) / locThis->mSampleRate);
	//REFERENCE_TIME locFrameEnd = (((__int64)(locThis->mUptoFrame * UNITS)) / locThis->mSampleRate);
	//***********************************************

		//Start time hacks
	REFERENCE_TIME locTimeBase = ((locThis->mLastSeenStartGranPos * UNITS) / locThis->mSampleRate) - locThis->mSeekTimeBase;

	//Temp - this will break seeking
	REFERENCE_TIME locFrameStart = locTimeBase + (((__int64)(locThis->mUptoFrame * UNITS)) / locThis->mSampleRate);
	//Increment the frame counter
	locThis->mUptoFrame += inFrames;
	//Make the end frame counter

	//REFERENCE_TIME locFrameEnd = locThis->CurrentStartTime() + (((__int64)(locThis->mUptoFrame * UNITS)) / locThis->mSampleRate);
	REFERENCE_TIME locFrameEnd = locTimeBase + (((__int64)(locThis->mUptoFrame * UNITS)) / locThis->mSampleRate);


	locThis->aadDebug<<"Last Seen  : " <<locThis->mLastSeenStartGranPos<<endl;
	locThis->aadDebug<<"Last Seen  : " << locThis->mLastSeenStartGranPos<<endl;
	locThis->aadDebug<<"Time Base  : " << locTimeBase << endl;
	locThis->aadDebug<<"FrameCount : " <<locThis->mUptoFrame<<endl;
	locThis->aadDebug<<"Seek TB    : " <<locThis->mSeekTimeBase<<endl;

	//TO DO::: Move this somewhere else
	unsigned long locActualSize = inFrames * locThis->mFrameSize;
	unsigned long locTotalFrameCount = inFrames * locThis->mNumChannels;
	
	//Make the start timestamp
	//FIX:::Abstract this calculation

	

	IMediaSample* locSample;
	HRESULT locHR = locThis->mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, NULL);

	if (locHR != S_OK) {
		return locHR;
	}	
	
	//Create pointers for the samples buffer to be assigned to
	BYTE* locBuffer = NULL;
	signed short* locShortBuffer = NULL;
	
	//Make our pointers set to point to the samples buffer
	locSample->GetPointer(&locBuffer);
	locShortBuffer = (short *) locBuffer;
	
	signed short tempInt = 0;
	float tempFloat = 0;
	
	//FIX:::Move the clipping to the abstract function

	if (locSample->GetSize() >= locActualSize) {
		//Do float to int conversion with clipping
		float SINT_MAX_AS_FLOAT = 32767.0f;
		for (unsigned long i = 0; i < locTotalFrameCount; i++) {
			//Clipping because vorbis puts out floats out of range -1 to 1
			if (((float*)inPCM)[i] <= -1.0f) {
				tempInt = SINT_MIN;	
			} else if (((float*)inPCM)[i] >= 1.0f) {
				tempInt = SINT_MAX;
			} else {
				//FIX:::Take out the unnescessary variable.
				tempFloat = ((( (float*) inPCM )[i]) * SINT_MAX_AS_FLOAT);
				//ASSERT((tempFloat <= 32767.0f) && (tempFloat >= -32786.0f));
				tempInt = (signed short)(tempFloat);
				//tempInt = (signed short) ((( (float*) inPCM )[i]) * SINT_MAX_AS_FLOAT);
			}
			
			*locShortBuffer = tempInt;
			locShortBuffer++;
		}
		
		//Set the sample parameters.
		locThis->SetSampleParams(locSample, locActualSize, &locFrameStart, &locFrameEnd);

		{
			CAutoLock locLock(locThis->m_pLock);
			HRESULT locHR = locThis->mOutputPin->mDataQueue->Receive(locSample);
			if (locHR != S_OK) {
				return locHR;				
			}
		}

		
		return 0;
	} else {
		throw 0;
	}

}



long SpeexDecodeInputPin::decodeData(BYTE* inBuf, long inNumBytes) 
{
	long locErr = fish_sound_decode(mFishSound, inBuf, inNumBytes);
	return locErr;
}



HRESULT SpeexDecodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.

	if (inMediaType->subtype == MEDIASUBTYPE_Speex) {
		((SpeexDecodeFilter*)mParentFilter)->setSpeexFormat((sSpeexFormatBlock*)inMediaType->pbFormat);
		mParentFilter->mAudioFormat = AbstractAudioDecodeFilter::SPEEX;
	} else {
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}

