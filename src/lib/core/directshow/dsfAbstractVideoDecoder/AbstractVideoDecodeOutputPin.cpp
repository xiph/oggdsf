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
	debugLog.open("g:\\logs\\absvidlog.log", ios_base::out|ios_base::binary);
}
AbstractVideoDecodeOutputPin::~AbstractVideoDecodeOutputPin(void)
{
	debugLog.close();
	
	delete mDataQueue;
	mDataQueue = NULL;
}

STDMETHODIMP AbstractVideoDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
	debugLog<<"QueryInterface : {"<<riid.Data1<<"-"<<riid.Data2<<"-"<<riid.Data3<<"-"<<riid.Data4<<"}"<<endl;
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}
HRESULT AbstractVideoDecodeOutputPin::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
		debugLog<<endl;	debugLog<<"DecideBufferSize :"<<endl;	//FIX::: Abstract this out properly	

	debugLog<<"Allocator is "<<(unsigned long)inAllocator<<endl;
	//Our error variable
	HRESULT locHR = S_OK;

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	debugLog<<"DecideBufferSize : Requested :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<inPropertyRequest->cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<inPropertyRequest->cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<inPropertyRequest->cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<inPropertyRequest->cBuffers<<endl;


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

	debugLog<<"DecideBufferSize : Modified Request :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<locReqAlloc.cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<locReqAlloc.cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<locReqAlloc.cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<locReqAlloc.cBuffers<<endl;


	//Set the properties in the allocator
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	debugLog<<"DecideBufferSize : SetProperties returns "<<locHR<<endl;
	debugLog<<"DecideBufferSize : Actual Params :"<<endl;
	debugLog<<"DecideBufferSize : Align     : "<<locActualAlloc.cbAlign<<endl;
	debugLog<<"DecideBufferSize : BuffSize  : "<<locActualAlloc.cbBuffer<<endl;
	debugLog<<"DecideBufferSize : Prefix    : "<<locActualAlloc.cbPrefix<<endl;
	debugLog<<"DecideBufferSize : NumBuffs  : "<<locActualAlloc.cBuffers<<endl;

	//Check the response
	switch (locHR) {
		case E_POINTER:
			debugLog<<"DecideBufferSize : SetProperties - NULL POINTER"<<endl;
			return locHR;
			

		case VFW_E_ALREADY_COMMITTED:
			debugLog<<"DecideBufferSize : SetProperties - Already COMMITED"<<endl;
			return locHR;
			
		case VFW_E_BADALIGN:
			debugLog<<"DecideBufferSize : SetProperties - Bad ALIGN"<<endl;
			return locHR;
			
		case VFW_E_BUFFERS_OUTSTANDING:
			debugLog<<"DecideBufferSize : SetProperties - BUFFS OUTSTANDING"<<endl;
			return locHR;
			

		case S_OK:

			break;
		default:
			debugLog<<"DecideBufferSize : SetProperties - UNKNOWN ERROR"<<endl;
			break;

	}

	
	//TO DO::: Do we commit ?
	//RESOLVED ::: Yep !
	
	locHR = inAllocator->Commit();
	debugLog<<"DecideBufferSize : Commit Returned "<<locHR<<endl;


	switch (locHR) {
		case E_FAIL:
			debugLog<<"DecideBufferSize : Commit - FAILED "<<endl;
			return locHR;
		case E_POINTER:
			debugLog<<"DecideBufferSize : Commit - NULL POINTER "<<endl;
			return locHR;
		case E_INVALIDARG:
			debugLog<<"DecideBufferSize : Commit - INVALID ARG "<<endl;
			return locHR;
		case E_NOTIMPL:
			debugLog<<"DecideBufferSize : Commit - NOT IMPL"<<endl;
			return locHR;
		case S_OK:
			debugLog<<"DecideBufferSize : Commit - ** SUCCESS **"<<endl;
			break;
		default:
			debugLog<<"DecideBufferSize : Commit - UNKNOWN ERROR "<<endl;
			return locHR;
	}


	return S_OK;
}
HRESULT AbstractVideoDecodeOutputPin::CheckMediaType(const CMediaType *inMediaType) {
	debugLog<<"CheckMediaType : "<<endl;
	if ((inMediaType->majortype == MEDIATYPE_Video) && (inMediaType->subtype == MEDIASUBTYPE_YV12) && (inMediaType->formattype == FORMAT_VideoInfo)) {
		
		//FIX::: SHould this stuff be in a check routine ??? Shouldn't it be in set mediatype ?

		//debugLog << "CheckMediaType : Accepting..."<<endl;
	
		VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)inMediaType->Format();
		debugLog << "CheckMediaType : Height = " << locVideoHeader->bmiHeader.biHeight<<endl;
		debugLog << "CheckMediaType : Width  = " << locVideoHeader->bmiHeader.biWidth<<endl;
		debugLog << "CheckMediaType : Size = "<<locVideoHeader->bmiHeader.biSizeImage<<endl<<endl;
		debugLog << "CheckMediaType : mHeight was = " << mParentFilter->mInputPin->mHeight<<endl;
		debugLog << "CheckMediaType : mWidth  was = " << mParentFilter->mInputPin->mWidth<<endl<<endl;
		debugLog<<"CheckMediaType : Size was = "<<mParentFilter->mInputPin->mFrameSize<<endl<<endl;
		mParentFilter->mInputPin->mHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
		mParentFilter->mInputPin->mWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);
		mParentFilter->mInputPin->mFrameSize = (unsigned long)locVideoHeader->bmiHeader.biSizeImage;
		debugLog<<"CheckMediaType : SUCCESS"<<endl;
		
		return S_OK;
	} else {
		debugLog<<"CheckMediaType : FAILURE"<<endl;
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
		
		debugLog<<"Vid format size "<<locVideoFormat->bmiHeader.biSizeImage<<endl;
		outMediaType->SetSampleSize(locVideoFormat->bmiHeader.biSizeImage);
		debugLog<<"Returning from GetMediaType"<<endl;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}


//Overriden for a data queue
HRESULT AbstractVideoDecodeOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	//mPartialPacket = NULL;
	debugLog << "DeliverNewSegment: start"<<endl;
	CBasePin::NewSegment(tStart, tStop, dRate);
	mDataQueue->NewSegment(tStart, tStop, dRate);
	//debugLog << "DeliverNewSegment: end"<<endl;

	return S_OK;
}
HRESULT AbstractVideoDecodeOutputPin::DeliverEndOfStream(void)
{
	debugLog << "DeliverEndOfStream: executed" << endl;
	//mPartialPacket = NULL;
	mDataQueue->EOS();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::DeliverEndFlush(void)
{
	debugLog << "DeliverEndFlush" << endl;
	mDataQueue->EndFlush();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::DeliverBeginFlush(void)
{
	//mPartialPacket = NULL;
	debugLog << "DeliverBeginFlush"<<endl;
	mDataQueue->BeginFlush();
    return S_OK;
}

HRESULT AbstractVideoDecodeOutputPin::CompleteConnect (IPin *inReceivePin)
{
	debugLog<<"ComlpeteConnect"<<endl;
	HRESULT locHR = S_OK;

	//Here when another pin connects to us, we internally connect the seek delegate
	// from this output pin onto the input pin... and we release it on breakconnect.
	//
	IMediaSeeking* locSeeker = NULL;
	mParentFilter->mInputPin->NonDelegatingQueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	//
	//This may cause issue if pins are disconnected and reconnected
	//DELETE in DEStructor
	mDataQueue = new COutputQueue (inReceivePin, &locHR, FALSE, TRUE, 1, TRUE, 15);
	if (FAILED(locHR)) {
		debugLog<<"Creating Output Q failed."<<endl;
		locHR = locHR;
	}
	
	return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT AbstractVideoDecodeOutputPin::BreakConnect(void) {
	debugLog<<"Break Connect"<<endl;
	//CAutoLock locLock(mFilterLock);
	HRESULT locHR = CBaseOutputPin::BreakConnect();
	debugLog<<"Base BreakConnect returns "<<locHR<<endl;
	ReleaseDelegate();
	delete mDataQueue;
	mDataQueue = NULL;
	return locHR;
}

//Testing
HRESULT AbstractVideoDecodeOutputPin::InitAllocator(IMemAllocator **ppAlloc) {
	debugLog<<"InitAllocator Called"<<endl;
	HRESULT locHR = CBaseOutputPin::InitAllocator(ppAlloc);
	debugLog<<"Base InitAlloc returns "<<locHR<<endl;
	return locHR;
}

HRESULT AbstractVideoDecodeOutputPin::SetMediaType(const CMediaType *pmt) {
	debugLog<<"SETTING MEDIA TYPE"<<endl;
	HRESULT locHR = CBaseOutputPin::SetMediaType(pmt);
	debugLog<<"ase SetMedia Type returns "<<locHR<<endl;
	return locHR;
}