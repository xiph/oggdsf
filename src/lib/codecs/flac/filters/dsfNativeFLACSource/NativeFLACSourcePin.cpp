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
#include ".\NativeFLACSourcePin.h"

NativeFLACSourcePin::NativeFLACSourcePin(NativeFLACSourceFilter* inParentFilter, CCritSec* inFilterLock)
	:	CBaseOutputPin(NAME("Native FLAC Source Pin"), inParentFilter, inFilterLock, &mFilterHR, L"PCM Out")
	,	mParentFilter(inParentFilter)
	,	mDataQueue(NULL)

{
}

NativeFLACSourcePin::~NativeFLACSourcePin(void)
{
}

STDMETHODIMP NativeFLACSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	
	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT NativeFLACSourcePin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	
	mDataQueue->NewSegment(tStart, tStop, dRate);

	return S_OK;
}
HRESULT NativeFLACSourcePin::DeliverEndOfStream(void)
{
	
	mDataQueue->EOS();
    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverEndFlush(void)
{
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverBeginFlush(void)
{
	
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT NativeFLACSourcePin::CompleteConnect (IPin *inReceivePin)
{
	mFilterHR = S_OK;
	//Set the delegate for seeking
	//((BasicSeekable*)(inReceivePin))->SetDelegate(this);
	//This may cause issue if pins are disconnected and reconnected
	//DELETE in DEStructor
	mDataQueue = new COutputQueue (inReceivePin, &mFilterHR, FALSE, TRUE,1,TRUE, NUM_BUFFERS);
	if (FAILED(mFilterHR)) {
		mFilterHR = mFilterHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT NativeFLACSourcePin::BreakConnect(void) {
	delete mDataQueue;
	mDataQueue = NULL;
	return CBaseOutputPin::BreakConnect();
}

	//CSourceStream virtuals
HRESULT NativeFLACSourcePin::GetMediaType(int inPosition, CMediaType* outMediaType) {
	//Put it in from the info we got in the constructor.
	//NOTE::: May have missed some fields ????
	//NOTE::: May want to check for null pointers
	//outMediaType->SetFormat(mMediaType->Format(), mMediaType->FormatLength());
	if (inPosition == 0) {
		outMediaType->SetType(&MEDIATYPE_Audio);
		outMediaType->SetSubtype(&MEDIASUBTYPE_PCM);
		outMediaType->SetFormatType(&FORMAT_WaveFormatEx);
		outMediaType->SetTemporalCompression(FALSE);
		outMediaType->SetSampleSize(0);


		WAVEFORMATEX* locFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
		locFormat->wFormatTag = WAVE_FORMAT_PCM;

		locFormat->nChannels = mParentFilter->mNumChannels;
		locFormat->nSamplesPerSec =  mParentFilter->mSampleRate;
		locFormat->wBitsPerSample = mParentFilter->mBitsPerSample;
		locFormat->nBlockAlign = (mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3);
		locFormat->nAvgBytesPerSec = ((mParentFilter->mNumChannels) * (mParentFilter->mBitsPerSample >> 3)) * mParentFilter->mSampleRate;
		locFormat->cbSize = 0;
		//outMediaType->pbFormat = locFormat;
		
		
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}
HRESULT NativeFLACSourcePin::CheckMediaType(const CMediaType* inMediaType) {
	if ((inMediaType->majortype == MEDIATYPE_Audio) && (inMediaType->subtype == MEDIASUBTYPE_PCM) && (inMediaType->formattype == FORMAT_WaveFormatEx)) {
		return S_OK;
	} else {
		return E_FAIL;
	}
}
HRESULT NativeFLACSourcePin::DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest) {

	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;


	locReqAlloc.cbAlign = 1;
	locReqAlloc.cbBuffer = BUFFER_SIZE;
	locReqAlloc.cbPrefix = 0;
	locReqAlloc.cBuffers = NUM_BUFFERS;

	locHR = inoutAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		return locHR;
	}
	
	locHR = inoutAllocator->Commit();

	return locHR;

}

HRESULT NativeFLACSourcePin::deliverData(unsigned char* inBuff, unsigned long inBuffSize, __int64 inStart, __int64 inEnd) {
	//Locks !!
	
	IMediaSample* locSample = NULL;
	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locStop = inEnd;
	//debugLog<<"Start   : "<<locStart<<endl;
	//debugLog<<"End     : "<<locStop<<endl;
	DbgLog((LOG_TRACE, 2, "Getting Buffer in Source Pin..."));
	HRESULT	locHR = GetDeliveryBuffer(&locSample, &locStart, &locStop, NULL);
	DbgLog((LOG_TRACE, 2, "* After get Buffer in Source Pin..."));
	//Error checks
	if (locHR != S_OK) {
		//Stopping, fluching or error
		//debugLog<<"Failure... No buffer"<<endl;
		return locHR;
	}

	//More hacks so we can send a timebase after a seek, since granule pos in theora
	// is not convertible in both directions to time.
	
	//TIMESTAMP FIXING !
	locSample->SetTime(&locStart, &locStop);
	
	//Yes this is way dodgy !
	//locSample->SetMediaTime(&mParentFilter->mSeekTimeBase, &mParentFilter->mSeekTimeBase);
	locSample->SetSyncPoint(TRUE);
	

	// Create a pointer for the samples buffer
	BYTE* locBuffer = NULL;
	locSample->GetPointer(&locBuffer);

	//DbgLog((LOG_TRACE, 2, "* Packet size is %d"));
	if (locSample->GetSize() >= inBuffSize) {

		memcpy((void*)locBuffer, (const void*)inBuff, inBuffSize);
		locSample->SetActualDataLength(inBuffSize);

		locHR = mDataQueue->Receive(locSample);

		//REF_CHECK::: In theory should release here.
		//The sample has ref_count of 1 by virtue of it's creation... we should release that 1 ref count here.
		
		if (locHR != S_OK) {
			//debugLog << "Failure... Queue rejected sample..."<<endl;
			//Stopping ??
			return locHR;
			
		} else {
			return S_OK;
		}
	} else {
		DbgLog((LOG_TRACE, 2, "* BUFFER TOO SMALL... FATALITY !!"));
		throw 0;
	}
}
