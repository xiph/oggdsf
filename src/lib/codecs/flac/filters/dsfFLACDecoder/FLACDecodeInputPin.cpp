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
	mFLACDecoder.initCodec();

	return true;
}
void FLACDecodeInputPin::DestroyCodec() 
{
	//delete mFLACDecoder;
	//mFLACDecoder = NULL;
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

		OggPacket* locPacket = new OggPacket(locBuff, inNumBytes, false, false);	//We give this away.

		if (mGotMetaData) {
			StampedOggPacket* locStamped = NULL;
			{
				CAutoLock locCodecLock(mCodecLock);
				//for(unsigned long i = 0; i < mPendingPackets.size(); i++) {
				 locStamped = (StampedOggPacket*)mFLACDecoder.decodeFLAC(locPacket)->clone();
			}

			if (locStamped != NULL) {
				//Do the directshow crap here....

				IMediaSample* locSample;

				HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, NULL, NULL, NULL);
				
				if (FAILED(locHR)) {
					//debugLog<<"Write_Callback : Get deliverybuffer failed. returning abort code."<<endl;
					//		//We get here when the application goes into stop mode usually.
						return -1;
				}	


				BYTE* locBuffer = NULL;


				//	//Make our pointers set to point to the samples buffer
				locSample->GetPointer(&locBuffer);




				if (locSample->GetSize() >= locStamped->packetSize()) {
					//	//ADDING TIMEBASE INFO.
					REFERENCE_TIME locTimeBase = ((mLastSeenStartGranPos * UNITS) / mFLACDecoder.mSampleRate) - mSeekTimeBase;
					REFERENCE_TIME locFrameStart = locTimeBase + (((__int64)(mUptoFrame * UNITS)) / mFLACDecoder.mSampleRate);
					
					//Increment the frame counter
					mUptoFrame += locStamped->endTime();
					
					//	//Make the end frame counter

					REFERENCE_TIME locFrameEnd = locTimeBase + (((__int64)(mUptoFrame * UNITS)) / mFLACDecoder.mSampleRate);

					memcpy((void*)locBuffer, (const void*)locStamped->packetData(), locStamped->packetSize());
					SetSampleParams(locSample, locStamped->packetSize(), &locFrameStart, &locFrameEnd);
					HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);
					if (locHR != S_OK) {
	
					} else {
						//debugLog<<"Write_Callback : Delivery of sample succeeded"<<endl;
					}
				} else {
					throw 0;		//SAMPLE SIZE IS TOO SMALL TO FIT DATA
				}


				delete locStamped;
				return 0;
			} else {
				return -1;
			}

		} else {
			{
				CAutoLock locCodecLock(mCodecLock);
				mGotMetaData = mFLACDecoder.acceptMetadata(locPacket);
			}
			if (mGotMetaData) {
				return 0;
			} else {
				return -1;
			}

			
			
		}
		//debugLog<<"decodeData : Successful return."<<endl;

		//Should be impossible to get here.
		return 0;
	} else {
		//debugLog<<"decodeData : Filter flushing... bad things !!!"<<endl;
		return -1;
	}

	
}


STDMETHODIMP FLACDecodeInputPin::BeginFlush() {
	CAutoLock locLock(mFilterLock);
	
	//debugLog<<"BeginFlush : Calling flush on the codec."<<endl;

	HRESULT locHR = AbstractAudioDecodeInputPin::BeginFlush();
	{	//PROTECT CODEC FROM IMPLODING
		CAutoLock locCodecLock(mCodecLock);
		mFLACDecoder.flushCodec();
	}	//END CRITICAL SECTION
	return locHR;
	
}

STDMETHODIMP FLACDecodeInputPin::EndOfStream(void) {
	CAutoLock locStreamLock(mStreamLock);
	{	//PROTECT CODEC FROM IMPLODING
		CAutoLock locCodecLock(mCodecLock);
		mFLACDecoder.flushCodec();
	}	//END CRITICAL SECTION

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

