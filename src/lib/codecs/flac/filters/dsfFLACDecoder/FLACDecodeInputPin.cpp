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
#include "flacdecodeinputpin.h"

FLACDecodeInputPin::FLACDecodeInputPin(AbstractAudioDecodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractAudioDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType)
	:	AbstractAudioDecodeInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("FLACDecodeInputPin"), L"FLAC In", inAcceptMediaType)
	,	mGotMetaData(false)
	,	mCodecLock(NULL)
	
	//,	mNumPacksBuffered(0)
{
	//debugLog.open("G:\\logs\\flacfilter.log", ios_base::out);
	mCodecLock = new CCritSec;
	ConstructCodec();
}

FLACDecodeInputPin::~FLACDecodeInputPin(void)
{
	//debugLog.close();
	delete mCodecLock;
	
}

STDMETHODIMP FLACDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}
bool FLACDecodeInputPin::ConstructCodec() 
{
	//mFLACDecoder = new FLAC__StreamDecoder;
	init();

	return true;
}
void FLACDecodeInputPin::DestroyCodec() 
{
	//delete mFLACDecoder;
	//mFLACDecoder = NULL;
}




//FLAC Callbacks
::FLAC__StreamDecoderReadStatus FLACDecodeInputPin::read_callback(FLAC__byte outBuffer[], unsigned* outNumBytes) 
{
	//Put some data into the buffer.
	//ASSERT(mPendingPackets.size() == 1);
	unsigned long locNumPacks = mPendingPackets.size();

	//debugLog<<"Read_Callback : numpacks = "<<locNumPacks<<endl;
	//First packet
	if (locNumPacks != 1) {
		//throw 0;
		//debugLog<<"Read_Callback : Bailing out with abort code."<<endl;
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	}
	OggPacket* locPacket = mPendingPackets.front()->clone();
	
	delete mPendingPackets.front();
	mPendingPackets.pop();
	ASSERT(mPendingPackets.size() == 0);
	//mNumPacksBuffered++;

	//for (int i = 0; i < locNumPacks; i++) {
	//	//mNumPacksBuffered++;
	//	//Any more than one packet merge them
	//	locPacket->merge(mPendingPackets.front());
	//	delete mPendingPackets.front();
	//	mPendingPackets.pop();
	//}

    memcpy((void*)outBuffer, (const void*)locPacket->packetData(), locPacket->packetSize());

	*outNumBytes = locPacket->packetSize();
	delete locPacket;
	locPacket = NULL;
	
	//debugLog<<"Read_callback : Buffer filled returning sucess"<<endl;
		//What return value ??
		return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}
::FLAC__StreamDecoderWriteStatus FLACDecodeInputPin::write_callback(const ::FLAC__Frame* inFrame, const FLAC__int32* const inBuffer[]) 
{
	//inFrame->header.blocksize

	
		//Do we need to delete the pcm structure ???? 
	//More of this can go to the abstract class.

	//For convenience we do all these cast once and for all here.

	//locFilter	
	if (! mBegun) {
		
	
		
		mBegun = true;
		
		mNumChannels = inFrame->header.channels;
		mFrameSize = mNumChannels * SIZE_16_BITS;
		mSampleRate = inFrame->header.sample_rate;
		
	}

	//FIX::: Most of this will be obselete... the demux does it all.
	

	//TO DO::: Fix this up... needs to move around order and some only needs to be done once, move it into the block aboce and use member data
	unsigned long locNumFrames = inFrame->header.blocksize;
	unsigned long locActualSize = locNumFrames * mFrameSize;
	unsigned long locTotalFrameCount = locNumFrames * mNumChannels;
	
	//Make the start timestamp
	//FIX:::Abstract this calculation
	//REFERENCE_TIME locFrameStart = CurrentStartTime() + (((__int64)(mUptoFrame * UNITS)) / mSampleRate);


	//ADDING TIMEBASE INFO.
	REFERENCE_TIME locTimeBase = ((mLastSeenStartGranPos * UNITS) / mSampleRate) - mSeekTimeBase;


	REFERENCE_TIME locFrameStart = locTimeBase + (((__int64)(mUptoFrame * UNITS)) / mSampleRate);
	//Increment the frame counter
	mUptoFrame += locNumFrames;
	//Make the end frame counter
	//REFERENCE_TIME locFrameEnd = CurrentStartTime() + (((__int64)(mUptoFrame * UNITS)) / mSampleRate);
	REFERENCE_TIME locFrameEnd = locTimeBase + (((__int64)(mUptoFrame * UNITS)) / mSampleRate);

	//Get a pointer to a new sample stamped with our time
	IMediaSample* locSample;
	//debugLog<<"Write_callback : Calling Getdeliverybuffer................"<<endl;
	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, NULL);

	if (FAILED(locHR)) {
		//debugLog<<"Write_Callback : Get deliverybuffer failed. returning abort code."<<endl;
		//We get here when the application goes into stop mode usually.
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}	
	//debugLog<<"Write_CAllback : Get delivery buffer returned ok"<<endl;
	//debugLog<<"Write_Callback : Sample times "<<locFrameStart <<" to "<<locFrameEnd<<endl;
	//Set the timestamps
	//Now done in SetsampleParams
	//	locSample->SetTime(&locFrameStart, &locFrameEnd);

	//Create pointers for the samples buffer to be assigned to
	BYTE* locBuffer = NULL;
	signed short* locShortBuffer = NULL;
	
	//Make our pointers set to point to the samples buffer
	locSample->GetPointer(&locBuffer);
	locShortBuffer = (short *) locBuffer;
	
	signed short tempInt = 0;
	int tempLong = 0;
	float tempFloat = 0;
	
	//FIX:::Move the clipping to the abstract function
	//Make sure our sample buffer is big enough

	//Modified for FLAC int32 not float
	if (locSample->GetSize() >= locActualSize) {
		
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
				
		//Set the sample parameters.
		SetSampleParams(locSample, locActualSize, &locFrameStart, &locFrameEnd);

		{
			CAutoLock locLock(m_pLock);
			//debugLog<<"Write_Callback : Calling deliver............"<<endl;
			
			
			//BUGFIX::: I think this is one of the sources of the seeking bug... the base class has a queue
			//			 on the output pin, this call was bypassing it... however the flush calls were
			//			 flushing the queue, and thus not the actuall buffer.
			//HRESULT locHR = mOutputPin->Deliver(locSample);			

			HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);
			if (locHR != S_OK) {
				//debugLog<<"Write_Callback : Delivery of sample failed. - "<<locHR<<endl;
			} else {
				//debugLog<<"Write_Callback : Delivery of sample succeeded"<<endl;
			}
		}

		//locSample->Release();

		//debugLog<<"WriteCallback : Returning Sucess code."<<endl;
		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
	} else {
		//debugLog<<"Write_Callback : Buffer too small."<<endl;
		throw 0;
	}

}
void FLACDecodeInputPin::metadata_callback(const ::FLAC__StreamMetadata* inMetadata) 
{
	int i = 0;
}
void FLACDecodeInputPin::error_callback(::FLAC__StreamDecoderErrorStatus inStatus) 
{
	int i = 0;
}


long FLACDecodeInputPin::decodeData(BYTE* inBuf, long inNumBytes) 
{
	//TODO::: Locks ???

	//What happens when another packet arrives and the other one is still there ?
	//delete mPendingPacket;
	//debugLog<<"decodeData : "<<endl;
	if(!m_bFlushing) {
		unsigned char* locBuff = new unsigned char[inNumBytes];
		memcpy((void*)locBuff, (const void*)inBuf, inNumBytes);

		//HACK !!
		if (mPendingPackets.size() != 0) {
			
			unsigned long locSize = mPendingPackets.size();
			//debugLog<<"decodeData : ERROR packet buffer not empty - "<<locSize<<" packets deleting"<<endl;
			for (int i = 0; i < locSize; i++) {
				delete mPendingPackets.front();
				mPendingPackets.pop();
			}
		}

		ASSERT(mPendingPackets.size() == 0);
														//Packet not truncated or continued.. it's a full packet.
		mPendingPackets.push(new OggPacket(locBuff, inNumBytes, false, false));
		ASSERT(mPendingPackets.size() == 1);
		if (mGotMetaData) {
			int locRet = 0;
			//for(unsigned long i = 0; i < mPendingPackets.size(); i++) {
			ASSERT((locBuff[0] == 255) && (locBuff[1] == 248));
			if (mPendingPackets.size() == 1) {
				//debugLog<<"decodeData : Calling process_single with 1 packet."<<endl;
				locRet = process_single();
			} else {
				//debugLog<<"decodeData : Something bad happened !"<<endl;
				return -1;
				//Shouldn't be possible to get here !
			}
			//}
			//mNumPacksBuffered = 0;
		} else {
			
			int locRet = process_until_end_of_metadata();
			mGotMetaData = true;
		}
		//debugLog<<"decodeData : Successful return."<<endl;
		return 0;
	} else {
		//debugLog<<"decodeData : Filter flushing... bad things !!!"<<endl;
		return -1;
	}
	
}

//STDMETHODIMP FLACDecodeInputPin::EndFlush() {
//	flush();
//	unsigned long locSize = mPendingPackets.size();
//	for (unsigned long i = 0; i < locSize; i++) {
//		delete mPendingPackets.front();
//		mPendingPackets.pop();
//	}
//	return AbstractAudioDecodeInputPin::EndFlush();
//	
//}

STDMETHODIMP FLACDecodeInputPin::BeginFlush() {
	CAutoLock locLock(mFilterLock);
	CAutoLock locCodecLock(mCodecLock);
	//debugLog<<"BeginFlush : Calling flush on the codec."<<endl;

	HRESULT locHR = AbstractAudioDecodeInputPin::BeginFlush();
	flush();
	unsigned long locSize = mPendingPackets.size();
	//debugLog<<"BeginFlush : deleting "<<locSize<<" packets."<<endl;
	for (unsigned long i = 0; i < locSize; i++) {
		delete mPendingPackets.front();
		mPendingPackets.pop();
	}
	return locHR;
	
}

STDMETHODIMP FLACDecodeInputPin::EndOfStream(void) {
	flush();
	unsigned long locSize = mPendingPackets.size();
	for (unsigned long i = 0; i < locSize; i++) {
		delete mPendingPackets.front();
		mPendingPackets.pop();
	}
	return AbstractAudioDecodeInputPin::EndOfStream();
}

HRESULT FLACDecodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	//FIX:::Error checking
	//RESOLVED::: Bit better.

	if (inMediaType->subtype == MEDIASUBTYPE_FLAC) {
		
		//Keep the format block
		
		((FLACDecodeFilter*)mParentFilter)->setFLACFormatBlock((sFLACFormatBlock*)inMediaType->pbFormat);

	} else {
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);
}

