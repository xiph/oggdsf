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
#include "abstractVideodecodeoutputpin.h"

AbstractVideoDecodeOutputPin::AbstractVideoDecodeOutputPin(AbstractVideoDecodeFilter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBaseOutputPin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName),
		mParentFilter(inParentFilter)
	,	mDataQueue(NULL)
{
	//debugLog.open("g:\\logs\\absvidlog.log", ios_base::out|ios_base::binary);
}
AbstractVideoDecodeOutputPin::~AbstractVideoDecodeOutputPin(void)
{
	//debugLog.close();
	
	delete mDataQueue;
}

STDMETHODIMP AbstractVideoDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}
HRESULT AbstractVideoDecodeOutputPin::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
		//FIX::: Abstract this out properly	

	//Our error variable
	HRESULT locHR = S_OK;

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	const unsigned long MIN_BUFFER_SIZE = 16*16;			//What should this be ????
	const unsigned long DEFAULT_BUFFER_SIZE = 1024*1024;
	const unsigned long MIN_NUM_BUFFERS = 1;
	const unsigned long DEFAULT_NUM_BUFFERS = 1;

	
	//Validate and change what we have been requested to do.
	//Allignment of data
	if (inPropertyRequest->cbAlign <= 0) {
		locReqAlloc.cbAlign = 1;
	} else {
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}

	//Size of each buffer
	if (inPropertyRequest->cbBuffer < MIN_BUFFER_SIZE) {
		locReqAlloc.cbBuffer = DEFAULT_BUFFER_SIZE;
	} else {
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}

	//How many prefeixed bytes
	if (inPropertyRequest->cbPrefix < 0) {
			locReqAlloc.cbPrefix = 0;
	} else {
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}

	//Number of buffers in the allcoator
	if (inPropertyRequest->cBuffers < MIN_NUM_BUFFERS) {
		locReqAlloc.cBuffers = DEFAULT_NUM_BUFFERS;
	} else {

		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	//Set the properties in the allocator
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	//Check the response
	switch (locHR) {
		case E_POINTER:
			return locHR;
			

		case VFW_E_ALREADY_COMMITTED:
			return locHR;
			
		case VFW_E_BADALIGN:
			return locHR;
			
		case VFW_E_BUFFERS_OUTSTANDING:
			return locHR;
			

		case S_OK:

			break;
	}

	
	//TO DO::: Do we commit ?
	//RESOLVED ::: Yep !
	
	locHR = inAllocator->Commit();

	switch (locHR) {
		case E_FAIL:
			return locHR;
		case E_POINTER:
			return locHR;
		case E_INVALIDARG:
			return locHR;
		case E_NOTIMPL:
			return locHR;
		case S_OK:
			break;
		default:
			return locHR;
	}


	return S_OK;
}
HRESULT AbstractVideoDecodeOutputPin::CheckMediaType(const CMediaType *inMediaType) {
	if ((inMediaType->majortype == MEDIATYPE_Video) && (inMediaType->subtype == MEDIASUBTYPE_YV12) && (inMediaType->formattype == FORMAT_VideoInfo)) {
		
		//FIX::: SHould this stuff be in a check routine ??? Shouldn't it be in set mediatype ?

		//debugLog << "CheckMediaType : Accepting..."<<endl;
	
		VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)inMediaType->Format();
		//debugLog << "CheckMediaType : Height = " << locVideoHeader->bmiHeader.biHeight<<endl;
		//debugLog << "CheckMediaType : Width  = " << locVideoHeader->bmiHeader.biWidth<<endl<<endl;
		
		//debugLog << "CheckMediaType : mHeight was = " << mParentFilter->mInputPin->mHeight<<endl;
		//debugLog << "CheckMediaType : mWidth  was = " << mParentFilter->mInputPin->mWidth<<endl<<endl;
		mParentFilter->mInputPin->mHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
		mParentFilter->mInputPin->mWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);



		return S_OK;
	} else {
		return S_FALSE;
	}
	
}

void AbstractVideoDecodeOutputPin::FillMediaType(CMediaType* outMediaType) {
	outMediaType->SetType(&MEDIATYPE_Video);
	outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
	outMediaType->SetFormatType(&FORMAT_VideoInfo);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(0);

}
HRESULT AbstractVideoDecodeOutputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if (inPosition == 0) {
		FillMediaType(outMediaType);
		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		FillVideoInfoHeader(locVideoFormat);
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}


//Overriden for a data queue
HRESULT AbstractVideoDecodeOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	//mPartialPacket = NULL;
	//debugLog << "DeliverNewSegment: start"<<endl;
	CBasePin::NewSegment(tStart, tStop, dRate);
	mDataQueue->NewSegment(tStart, tStop, dRate);
	//debugLog << "DeliverNewSegment: end"<<endl;

	return S_OK;
}
HRESULT AbstractVideoDecodeOutputPin::DeliverEndOfStream(void)
{
	//debugLog << "DeliverEndOfStream: executed" << endl;
	//mPartialPacket = NULL;
	mDataQueue->EOS();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::DeliverEndFlush(void)
{
	//debugLog << "DeliverEndFlush" << endl;
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::DeliverBeginFlush(void)
{
	//mPartialPacket = NULL;
	//debugLog << "DeliverBeginFlush"<<endl;
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::CompleteConnect (IPin *inReceivePin)
{
	HRESULT locHR = S_OK;
	//This may cause issue if pins are disconnected and reconnected
	//DELETE in DEStructor
	mDataQueue = new COutputQueue (inReceivePin, &locHR, FALSE, TRUE, 1, TRUE, 15);
	if (FAILED(locHR)) {
		locHR = locHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}