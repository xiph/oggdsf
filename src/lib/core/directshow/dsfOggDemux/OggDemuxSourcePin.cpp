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
#include "oggdemuxsourcepin.h"

OggDemuxSourcePin::OggDemuxSourcePin(	TCHAR* inObjectName, 
										OggDemuxSourceFilter* inParentFilter,
										CCritSec* inFilterLock,
										StreamHeaders* inHeaderSource, 
										CMediaType* inMediaType,
										wstring inPinName,
										bool inAllowSeek,
										unsigned long inNumBuffers,
										unsigned long inBufferSize)
	:	CBaseOutputPin(			NAME("Ogg Demux Output Pin")
							,	inParentFilter
							,	inFilterLock
							,	&mFilterHR
							,	inPinName.c_str() )
	,	mHeaders(inHeaderSource)
	,	mParentFilter(inParentFilter)
	,	mMediaType(inMediaType)
	,	mDataQueue(NULL)
	,	mFirstRun(true)
	,	mPartialPacket(NULL)
	,	mBufferSize(inBufferSize)
	,	mNumBuffers(inNumBuffers)
		
{
	//TODO::: Something about this is causing a COM reference leak.
#ifdef OGGCODECS_LOGGING
	debugLog.open("d:\\zen\\logs\\sourcefilterpin.log", ios_base::out);
#endif

	IMediaSeeking* locSeeker = NULL;
	//if (inAllowSeek) {
		//debugLog<<"Allowing seek"<<endl;
		//Subvert COM and do this directly... this way, the source filter won't expose the interface to the
		// graph but we can still delegate to it.
		
		//inParentFilter->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
		locSeeker = (IMediaSeeking*)mParentFilter;

		//UNdecided whether this should be adreff or not... if you do you have  a cyclic reference, if you don't see below.
		//locSeeker->AddRef();				//If you addref theres a weird destructor infinite loop. See the OggStreamMapper Destructor.

	
	//}
	SetDelegate(locSeeker);
}

OggDemuxSourcePin::~OggDemuxSourcePin(void)
{
	debugLog.close();
	
	delete mDataQueue;
	mDataQueue = NULL;
}

STDMETHODIMP OggDemuxSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		//debugLog<<"Pin queried for IMediaSeeking"<<endl;
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}


bool OggDemuxSourcePin::deliverOggPacket(StampedOggPacket* inPacket) 
{
	//We don't own the packet, so don't delete it.
	CAutoLock locStreamLock(mParentFilter->mStreamLock);


	IMediaSample* locSample = NULL;
	REFERENCE_TIME locStart = inPacket->startTime();
	REFERENCE_TIME locStop = inPacket->endTime();
	debugLog<<"Delivering packet : "<<locStart<< " - "<<locStop<<endl;


	HRESULT	locHR = GetDeliveryBuffer(&locSample, &locStart, &locStop, NULL);
	DbgLog((LOG_TRACE, 2, "* After get Buffer in Source Pin..."));
	//Error checks
	if (locHR != S_OK) {
		//Stopping, fluching or error

		debugLog<<"Failure... No buffer"<<endl;
		return false;
	}

	//More hacks so we can send a timebase after a seek, since granule pos in theora
	// is not convertible in both directions to time.
	
	//TIMESTAMP FIXING !
	locSample->SetTime(&locStart, &locStop);
	
	//TODO::: This style of timestamping should not be necessary anymore.
	//Yes this is way dodgy !
	locSample->SetMediaTime(&mParentFilter->mSeekTimeBase, &mParentFilter->mSeekTimeBase);
	locSample->SetSyncPoint(TRUE);
	

	// Create a pointer for the samples buffer
	BYTE* locBuffer = NULL;
	locSample->GetPointer(&locBuffer);

	if (locSample->GetSize() >= inPacket->packetSize()) {

		memcpy((void*)locBuffer, (const void*)inPacket->packetData(), inPacket->packetSize());
		locSample->SetActualDataLength(inPacket->packetSize());

		locHR = mDataQueue->Receive(locSample);

		if (locHR != S_OK) {
			debugLog << "Failure... Queue rejected sample..."<<endl;
			//Stopping ??

			return false;
		} else {

			return true;
		}
	} else {
		DbgLog((LOG_TRACE, 2, "* BUFFER TOO SMALL... FATALITY !!"));
		throw 0;
	}
}
HRESULT OggDemuxSourcePin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	debugLog<<"Delivering new segment"<<endl;
	NewSegment(tStart, tStop, dRate);

	mPartialPacket = NULL;
	mDataQueue->NewSegment(tStart, tStop, dRate);

	return S_OK;
}
HRESULT OggDemuxSourcePin::DeliverEndOfStream(void)
{
	mPartialPacket = NULL;
	mDataQueue->EOS();
    return S_OK;
}

HRESULT OggDemuxSourcePin::DeliverEndFlush(void)
{
	debugLog<<"Delivering End flush"<<endl;
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT OggDemuxSourcePin::DeliverBeginFlush(void)
{
	debugLog<<"Delivering begin flush"<<endl;
	mPartialPacket = NULL;
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT OggDemuxSourcePin::CompleteConnect (IPin *inReceivePin)
{
	mFilterHR = S_OK;
	//Set the delegate for seeking
	//((BasicSeekable*)(inReceivePin))->SetDelegate(this);
	//This may cause issue if pins are disconnected and reconnected
	//DELETE in DEStructor
	mDataQueue = new COutputQueue (inReceivePin, &mFilterHR, FALSE, TRUE,1,TRUE, NUM_BUFFERS);
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT OggDemuxSourcePin::BreakConnect(void) 
{
	delete mDataQueue;
	mDataQueue = NULL;
	return CBaseOutputPin::BreakConnect();
}

	//CSourceStream virtuals
HRESULT OggDemuxSourcePin::GetMediaType(int inPosition, CMediaType* outMediaType) 
{
	//Put it in from the info we got in the constructor.
	if (inPosition == 0) {
		*outMediaType = *mMediaType;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT OggDemuxSourcePin::CheckMediaType(const CMediaType* inMediaType) {
	if (inMediaType->majortype == mMediaType->majortype && inMediaType->subtype == mMediaType->subtype && inMediaType->formattype == mMediaType->formattype) {
		return S_OK;
	} else {
		return E_FAIL;
	}
}
HRESULT OggDemuxSourcePin::DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest) 
{
	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	locReqAlloc.cbAlign = 1;
	locReqAlloc.cbBuffer = mBufferSize; //BUFFER_SIZE;
	locReqAlloc.cbPrefix = 0;
	locReqAlloc.cBuffers = mNumBuffers; //NUM_BUFFERS;

	locHR = inoutAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		return locHR;
	}
	
	locHR = inoutAllocator->Commit();

	return locHR;

}


