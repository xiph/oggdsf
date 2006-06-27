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
#include "FLACencodeinputpin.h"

FLACEncodeInputPin::FLACEncodeInputPin(AbstractTransformFilter* inParentFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("FLACEncodeInputPin"), L"PCM In", inAcceptableMediaTypes)
	,	mTweakedHeaders(false)
	,	mBegun(false)
	,	mWaveFormat(NULL)
	,	mUptoFrame(0)
	
	
{
	//debugLog.open("G:\\logs\\FLACenc.log", ios_base::out);
}

FLACEncodeInputPin::~FLACEncodeInputPin(void)
{
	//debugLog.close();
	DestroyCodec();
}


//PURE VIRTUALS
HRESULT FLACEncodeInputPin::TransformData(unsigned char* inBuf, long inNumBytes) {

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
	delete[] locFLACBuff;

	if (locRetVal == true) {
		return 0;
	} else {
		return -1;
	}

}
bool FLACEncodeInputPin::ConstructCodec() 
{

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
void FLACEncodeInputPin::DestroyCodec()
{

	//Should there be some cleanup function ??
}




::FLAC__StreamEncoderWriteStatus FLACEncodeInputPin::write_callback(const FLAC__byte inBuffer[], unsigned inNumBytes, unsigned inNumSamples, unsigned inCurrentFrame) {

	//This is called back with encoded data after raw data is fed in by stream_encoder_process or
	// stream_encoder_process_interleaved.


	//debugLog<<"Write CAllback.."<<endl;
	LONGLONG locFrameStart = 0;
	LONGLONG locFrameEnd = 0;


	if (!mTweakedHeaders) {
		//debugLog<<"Still tweaking headers..."<<endl;
		//Still handling headers...
		unsigned char* locBuf = new unsigned char[inNumBytes];
		memcpy((void*)locBuf, (const void*) inBuffer, inNumBytes);
		//debugLog<<"Sending header to tweaker..."<<endl;
		FLACHeaderTweaker::eFLACAcceptHeaderResult locResult = mHeaderTweaker.acceptHeader(new OggPacket(locBuf, inNumBytes, false, false));
		//debugLog<<"Tweaker returned... "<<(int)locResult<<endl;
		if (locResult == FLACHeaderTweaker::LAST_HEADER_ACCEPTED) {
			//debugLog<<"Last Header accepted..."<<endl;
			//Send all the headers
			mTweakedHeaders = true;

			for (unsigned long i = 0; i < mHeaderTweaker.numNewHeaders(); i++) {
				//Loop through firing out all the headers.
				//debugLog<<"Sending new header "<<i<<endl;

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

					
					HRESULT locHR = ((FLACEncodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
					if (locHR != S_OK) {
						//debugLog<<"Sample rejected"<<endl;
					} else {
						//debugLog<<"Sample Delivered"<<endl;
					}
				}

				


			}

			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else if (locResult == FLACHeaderTweaker::HEADER_ACCEPTED) {
			//Another header added.
			//debugLog<<"Header accepted"<<endl;
			return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
		} else {
			//debugLog<<"Header failed..."<<endl;
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

	//**** WARNING 4018::: Leave this alone.
	if (locSample->GetSize() >= inNumBytes) {

		memcpy((void*)locBuffer, (const void*)inBuffer, inNumBytes);
		
		//Set the sample parameters.
		SetSampleParams(locSample, inNumBytes, &locFrameStart, &locFrameEnd);

		{
			CAutoLock locLock(m_pLock);

			
			HRESULT locHR = ((FLACEncodeOutputPin*)(mOutputPin))->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
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
void FLACEncodeInputPin::metadata_callback(const ::FLAC__StreamMetadata *metadata) 
{

	//Ignore it.
}

STDMETHODIMP FLACEncodeInputPin::EndOfStream(void) {
	//Catch the end of stream so we can send a finish signal.
	finish();			//Tell flac we are done so it can flush
	return AbstractTransformInputPin::EndOfStream();		//Call the base class.
}

HRESULT FLACEncodeInputPin::SetMediaType(const CMediaType* inMediaType) 
{

	if (	(inMediaType->subtype == MEDIASUBTYPE_PCM) &&
			(inMediaType->formattype == FORMAT_WaveFormatEx)) {

		mWaveFormat = (WAVEFORMATEX*)inMediaType->pbFormat;
		
	} else {
		//Failed... should never be here !
		throw 0;
	}
	//This is here and not the constructor because we need audio params from the
	// input pin to construct properly.	
	
	ConstructCodec();

	return CBaseInputPin::SetMediaType(inMediaType);

}