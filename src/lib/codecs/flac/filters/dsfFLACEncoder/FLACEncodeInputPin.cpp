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
#include "FLACencodeinputpin.h"

FLACEncodeInputPin::FLACEncodeInputPin(AbstractAudioEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractAudioEncodeOutputPin* inOutputPin)
	:	AbstractAudioEncodeInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("FLACEncodeInputPin"), L"PCM In")
	,	mTweakedHeaders(false)
	
	
{
	//debugLog.open("C:\\temp\\FLACenc.log", ios_base::out);
}

FLACEncodeInputPin::~FLACEncodeInputPin(void)
{
	//debugLog.close();
	DestroyCodec();
}


//PURE VIRTUALS
long FLACEncodeInputPin::encodeData(unsigned char* inBuf, long inNumBytes) {

	if (mBegun == false) {

		//First bit of data, set up the encoder.
		mBegun = true;
		init();
	}
	FLAC__int32* locFLACBuff = NULL;
	FLACEncodeFilter* locParentFilter = (FLACEncodeFilter*)mParentFilter;	//View only don't delete.
	unsigned long locFLACBuffSize = (inNumBytes * 8) / locParentFilter->mFLACFormatBlock.numBitsPerSample;
	unsigned long locNumSamplesPerChannel = locFLACBuffSize / locParentFilter->mFLACFormatBlock.numChannels;

	locFLACBuff = new FLAC__int32[locFLACBuffSize];

	//QUERY::: Are the flac buffers supposed to stretch the data to 32 bits ?
	//Assuming No for now, otherwise whats the point of set_sample_size.

	//POTENTIAL BUG::: This assumes 16 bit samples !!

	short locTempShort = 0;
	for (int i = 0; i < inNumBytes; i += 2) {
		locTempShort = *((short*)(inBuf + i));
		locFLACBuff[i/2] = locTempShort;
	}

	bool locRetVal = process_interleaved(locFLACBuff, locNumSamplesPerChannel);

	if (locRetVal == true) {
		return 0;
	} else {
		return -1;
	}
	////debugLog << "encodeData receives : "<<inNumBytes<<" bytes"<<endl;
	//
	//float* locFloatBuf = new float[inNumBytes/2];
	//short locTempShort = 0;
	//float locTempFloat = 0;

	////__int64 locGranPos = 0;
	////Removed hack for gran pos
	////fish_sound_command(mFishSound, 8, &locGranPos, sizeof(__int64));
	////
	////locGranPos = fish_sound_get_frameno(mFishSound);
	////mUptoFrame = locGranPos;
	////__int64 locTemp = ((FishSoundFLACInfo*)mFishSound->codec_data)->vd.pcm_returned;
	//for (int i = 0; i < inNumBytes; i += 2) {
	//	locTempShort = *((short*)(inBuf + i));
	//	locTempFloat = (float)locTempShort;
	//	locTempFloat /= 32767.0;
	//	locFloatBuf[i/2] = locTempFloat;;
	//}
	////debugLog<<"Calling encode"<<endl;
	////FIX::: The 2 is the size of a sample ie 16 bits
	//long locErr = fish_sound_encode(mFishSound, (float**)locFloatBuf, inNumBytes/(mFishInfo.channels*2));
	//delete locFloatBuf;
	////FIX::: Do something here ?
	//if (locErr < 0) {
	//	//debugLog<<"Fishsound reports error"<<endl;
	//} else {
	//
	//}
	//return locErr;
	
}
bool FLACEncodeInputPin::ConstructCodec() {
	//mFishInfo.channels = mWaveFormat->nChannels;
	//mFishInfo.format = FISH_SOUND_FLAC;
	//mFishInfo.samplerate = mWaveFormat->nSamplesPerSec;

	set_channels(mWaveFormat->nChannels);
	set_sample_rate(mWaveFormat->nSamplesPerSec);
	set_bits_per_sample(mWaveFormat->wBitsPerSample);

	FLACEncodeFilter* locParentFilter = (FLACEncodeFilter*)mParentFilter;	//View only don't delete.
	locParentFilter->mFLACFormatBlock.numBitsPerSample = mWaveFormat->wBitsPerSample;
	locParentFilter->mFLACFormatBlock.numChannels = mWaveFormat->nChannels;
	locParentFilter->mFLACFormatBlock.sampleRate = mWaveFormat->nSamplesPerSec;
	
	//This can't be here, it causes callbacks to fire, and the data can't be delivered
	// because the filter is not fully set up yet.
	//init();

	////FIX::: Proper return value
	return true;
}
void FLACEncodeInputPin::DestroyCodec() {
	//fish_sound_delete(mFishSound);
	//mFishSound = NULL;
}




::FLAC__StreamEncoderWriteStatus FLACEncodeInputPin::write_callback(const FLAC__byte inBuffer[], unsigned inNumBytes, unsigned inNumSamples, unsigned inCurrentFrame) {

	//This is called back with encoded data after raw data is fed in by stream_encoder_process or
	// stream_encoder_process_interleaved.

	//if (mHeadersSeen == 0) {
	//	//We haven't converted the headers yet
	//	//This should be a 4 byte fLaC
	//	ASSERT (inNumBytes == 4);
	//	mHeadersSeen ++;
	//	//Do nothing.
	//	return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
	//} else if (mHeadersSeen == 1) {
	//	//This should be the stream info header

	//	//Get a pointer to a new sample stamped with our time
	//	IMediaSample* locSample;
	//	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, NULL);

	//	if (FAILED(locHR)) {
	//		//We get here when the application goes into stop mode usually.
	//		//locThis->debugLog<<"Getting buffer failed"<<endl;
	//		return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
	//	}	
	//	
	//	BYTE* locBuffer = NULL;

	//	//Make our pointers set to point to the samples buffer
	//	locSample->GetPointer(&locBuffer);

	//	locBuffer[0] = '\177';
	//	locBuffer[1] = 'F';
	//	locBuffer[2] = 'L';
	//	locBuffer[3] = 'A';
	//	locBuffer[4] = 'C';
	//	locBuffer[5] = 1;
	//	locBuffer[6] = 0;

	LONGLONG locFrameStart = 0;
	LONGLONG locFrameEnd = 0;


	if (!mTweakedHeaders) {
		//Still handling headers...
		unsigned char* locBuf = new unsigned char[inNumBytes];
		memcpy((void*)locBuf, (const void*) inBuffer, inNumBytes);
		FLACHeaderTweaker::eFLACAcceptHeaderResult locResult = mHeaderTweaker.acceptHeader(new OggPacket(locBuf, inNumBytes, false, false));
		if (locResult == FLACHeaderTweaker::LAST_HEADER_ACCEPTED) {
			//Send all the headers
			mTweakedHeaders = true;

			for (int i = 0; i < mHeaderTweaker.numNewHeaders(); i++) {
				//Loop through firing out all the headers.


				//Get a pointer to a new sample stamped with our time
				IMediaSample* locSample;
				HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);

				if (FAILED(locHR)) {
					//We get here when the application goes into stop mode usually.
					//locThis->debugLog<<"Getting buffer failed"<<endl;
					return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
				}	
				
				BYTE* locBuffer = NULL;

				//Make our pointers set to point to the samples buffer
				locSample->GetPointer(&locBuffer);

				memcpy((void*)locBuffer, (const void*)mHeaderTweaker.getHeader(i)->packetData(), mHeaderTweaker.getHeader(i)->packetSize());
				
				//Set the sample parameters. (stamps will be 0)
				SetSampleParams(locSample, mHeaderTweaker.getHeader(i)->packetSize(), &locFrameStart, &locFrameEnd);

				{
					CAutoLock locLock(m_pLock);

					
					HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
					if (locHR != S_OK) {
						//locThis->debugLog<<"Sample rejected"<<endl;
					} else {
						//locThis->debugLog<<"Sample Delivered"<<endl;
					}
				}

				return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;


			}

			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else if (locResult == FLACHeaderTweaker::HEADER_ACCEPTED) {
			//Another header added.
			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else {
			return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
		}

	}

	locFrameStart = mUptoFrame;
	if (inNumSamples != 0) {
		mUptoFrame += inNumSamples;
	}
	locFrameEnd = mUptoFrame;


	//Get a pointer to a new sample stamped with our time
	IMediaSample* locSample;
	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, NULL);

	if (FAILED(locHR)) {
		//We get here when the application goes into stop mode usually.
		//locThis->debugLog<<"Getting buffer failed"<<endl;
		return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
	}	
	
	BYTE* locBuffer = NULL;

	//Make our pointers set to point to the samples buffer
	locSample->GetPointer(&locBuffer);

	if (locSample->GetSize() >= inNumBytes) {

		memcpy((void*)locBuffer, (const void*)inBuffer, inNumBytes);
		
		//Set the sample parameters.
		SetSampleParams(locSample, inNumBytes, &locFrameStart, &locFrameEnd);

		{
			CAutoLock locLock(m_pLock);

			
			HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
			if (locHR != S_OK) {
				//locThis->debugLog<<"Sample rejected"<<endl;
			} else {
				//locThis->debugLog<<"Sample Delivered"<<endl;
			}
		}

		return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
	} else {
		throw 0;
	}


}
void FLACEncodeInputPin::metadata_callback(const ::FLAC__StreamMetadata *metadata) {
	//This is called back at the *end* of encoding with the headers that need to be written at the *start* of the stream.
	//This is going to make it painful to get the data to the mux filter in multi stream ogg.
	//It's basically going to totally break the mux filter stream abstraction, 
	//by forcing yet more codec specific code into the mux.
	//Oh well... if you're going to break it... break it good !

	//It also makes it impractical for streaming as decode can't begin without metadata, and metadata
	// is not created until all encoding is finished. I guess that's not such a big deal
	// because flac is unlikely to be streamed live.
}

STDMETHODIMP FLACEncodeInputPin::EndOfStream(void) {
	//Catch the end of stream so we can send a finish signal.
	finish();			//Tell flac we are done so it can flush and send us the metadata.
	return AbstractAudioEncodeInputPin::EndOfStream();		//Call the base class.
}

HRESULT FLACEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	AbstractAudioEncodeInputPin::SetMediaType(inMediaType);

	ConstructCodec();

	return S_OK;
	
}