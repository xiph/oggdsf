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
#include "theoradecodeinputpin.h"


TheoraDecodeInputPin::TheoraDecodeInputPin(CTransformFilter* inParentFilter, HRESULT* outHR) 
	:	CTransformInputPin(NAME("Theora Input Pin"), inParentFilter, outHR, L"Theora In")
{
	//debugLog.open("G:\\logs\\theoinput.log", ios_base::out);
}
TheoraDecodeInputPin::~TheoraDecodeInputPin() {
	//debugLog.close();
}

//STDMETHODIMP TheoraDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv) {
//	debugLog<<"Querying interface"<<endl;
//	if (riid == IID_IMediaSeeking) {
//		debugLog<<"Got Seeker"<<endl;
//		*ppv = (IMediaSeeking*)this;
//		((IUnknown*)*ppv)->AddRef();
//		
//		return NOERROR;
//	}
//
//	return CTransformInputPin::NonDelegatingQueryInterface(riid, ppv); 
//}

//HRESULT TheoraDecodeInputPin::BreakConnect() {
//	CAutoLock locLock(m_pLock);
//	debugLog<<"Break conenct"<<endl;
//	//Need a lock ??
//	ReleaseDelegate();
//	return CTransformInputPin::BreakConnect();
//}
//HRESULT TheoraDecodeInputPin::CompleteConnect (IPin *inReceivePin) {
//	CAutoLock locLock(m_pLock);
//	debugLog<<"Complete conenct"<<endl;
//	IMediaSeeking* locSeeker = NULL;
//	inReceivePin->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
//	if (locSeeker == NULL) {
//		debugLog<<"Seeker is null"<<endl;
//	}
//	SetDelegate(locSeeker);
//	return CTransformInputPin::CompleteConnect(inReceivePin);
//}

//----------------------
//OLD IMPLEMENTATION
//----------------------
//TheoraDecodeInputPin::TheoraDecodeInputPin(AbstractVideoDecodeFilter* inFilter, CCritSec* inFilterLock, AbstractVideoDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType)
//	:	AbstractVideoDecodeInputPin(inFilter, inFilterLock, inOutputPin, NAME("TheoraDecodeInputPin"), L"Theora In", inAcceptMediaType)
//	,	mXOffset(0)
//	,	mYOffset(0)
//
//{
//	debugLog.open("G:\\logs\\theoInputPin.log", ios_base::out);
//	ConstructCodec();
//}
//
//STDMETHODIMP TheoraDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
//{
//	if (riid == IID_IMediaSeeking) {
//		*ppv = (IMediaSeeking*)this;
//		((IUnknown*)*ppv)->AddRef();
//		return NOERROR;
//	}
//
//	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
//}
//bool TheoraDecodeInputPin::ConstructCodec() {
//
//
//	mTheoraDecoder = new TheoraDecoder;
//	mTheoraDecoder->initCodec();
//	return true;
//}
//void TheoraDecodeInputPin::DestroyCodec() {
//	delete mTheoraDecoder;
//}
//TheoraDecodeInputPin::~TheoraDecodeInputPin(void)
//{
//	debugLog.close();
//	DestroyCodec();
//	
//}
//
//
//
//int TheoraDecodeInputPin::TheoraDecoded (yuv_buffer* inYUVBuffer) 
//{
//	DbgLog((LOG_TRACE,1,TEXT("Decoded... Sending...")));
//	
//	debugLog<<"TheoraDecoded... "<<endl;
//	
//	TheoraDecodeFilter* locFilter = reinterpret_cast<TheoraDecodeFilter*>(m_pFilter);
//	//
//	if (!mBegun) {
//	
//	
//	
//		mBegun = true;
//		
//		//How many UNITS does one frame take.
//		mFrameDuration = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / (locFilter->mTheoraFormatInfo->frameRateNumerator);
//		mFrameSize = (mHeight * mWidth * 3) / 2;
//		mFrameCount = 0;
//	}
//
//
//
//	////FIX::: Most of this will be obselete... the demux does it all.
//	//
//
//	////TO DO::: Fix this up... needs to move around order and some only needs to be done once, move it into the block aboce and use member data
//
//	////Make the start timestamp
//	////FIX:::Abstract this calculation
//	DbgLog((LOG_TRACE,1,TEXT("Frame Count = %d"), mFrameCount));
//	//REFERENCE_TIME locFrameStart = CurrentStartTime() + (mFrameCount * mFrameDuration);
//
//	//Timestamp hacks start here...
//	unsigned long locMod = (unsigned long)pow(2, locFilter->mTheoraFormatInfo->maxKeyframeInterval);
//	DbgLog((LOG_TRACE,1,TEXT("locSeenGranPos = %d"), mLastSeenStartGranPos));
//	DbgLog((LOG_TRACE,1,TEXT("locMod = %d"), locMod));
//	unsigned long locInterFrameNo = (mLastSeenStartGranPos) % locMod;
//	DbgLog((LOG_TRACE,1,TEXT("InterFrameNo = %d"), locInterFrameNo));
//	LONGLONG locAbsFramePos = ((mLastSeenStartGranPos >> locFilter->mTheoraFormatInfo->maxKeyframeInterval)) + locInterFrameNo;
//	DbgLog((LOG_TRACE,1,TEXT("AbsFrameNo = %d"), locAbsFramePos));
//	DbgLog((LOG_TRACE,1,TEXT("mSeekTimeBase = %d"), mSeekTimeBase));
//	REFERENCE_TIME locTimeBase = (locAbsFramePos * mFrameDuration) - mSeekTimeBase;
//	DbgLog((LOG_TRACE,1,TEXT("locTimeBase = %d"), locTimeBase));
//	//
//	//
//
//	REFERENCE_TIME locFrameStart = locTimeBase + (mFrameCount * mFrameDuration);
//	//Increment the frame counter
//	mFrameCount++;
//	
//	//Make the end frame counter
//	//REFERENCE_TIME locFrameEnd = CurrentStartTime() + (mFrameCount * mFrameDuration);
//	REFERENCE_TIME locFrameEnd = locTimeBase + (mFrameCount * mFrameDuration);
//
//	DbgLog((LOG_TRACE,1,TEXT("Frame Runs From %d"), locFrameStart));
//	DbgLog((LOG_TRACE,1,TEXT("Frame Runs To %d"), locFrameEnd));
//
//	IMediaSample* locSample = NULL;
//	debugLog<<"Sample times = "<<locFrameStart<<" to "<<locFrameEnd<<endl;
//	DWORD locFlags = 0;//AM_GBF_PREVFRAMESKIPPED | AM_GBF_NOTASYNCPOINT;
//
//	FILTER_STATE locFS;
//	mParentFilter->GetState(0, &locFS);
//	debugLog<<"State Before = "<<locFS<<endl;
//	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, locFlags);
//	mParentFilter->GetState(0, &locFS);
//	debugLog<<"State After = "<<locFS<<endl;
//	if (locHR != S_OK) {
//		debugLog<<"Get DeliveryBuffer FAILED with "<<locHR<<endl;
//		debugLog<<"locSample is "<<(unsigned long)locSample<<endl;
//		//We get here when the application goes into stop mode usually.
//
//		switch (locHR) {
//			case VFW_E_SIZENOTSET:
//				debugLog<<"SIZE NOT SET"<<endl;
//				break;
//			case VFW_E_NOT_COMMITTED:
//				debugLog<<"NOT COMMITTED"<<endl;
//				break;
//			case VFW_E_TIMEOUT:
//				debugLog<<"TIMEOUT"<<endl;
//				break;
//			case VFW_E_STATE_CHANGED:
//				debugLog<<"STATE CHANGED"<<endl;
//				return S_OK;
//			default:
//				debugLog<<"SOMETHING ELSE !!!"<<endl;
//				break;
//		}
//		return locHR;
//	}	
//	
//	debugLog<<"GetDeliveryBuffer &** SUCCEEDED **"<<endl;
//
//	//Debuggin code
//	AM_MEDIA_TYPE* locMediaType = NULL;
//	locSample->GetMediaType(&locMediaType);
//	if (locMediaType == NULL) {
//		debugLog<<"No dynamic change..."<<endl;
//	} else {
//		debugLog<<"Attempting dynamic change..."<<endl;
//		if (locMediaType->majortype == MEDIATYPE_Video) {
//			debugLog<<"Still MEDIATYPE_Video"<<endl;
//		}
//
//		if (locMediaType->subtype == MEDIASUBTYPE_YV12) {
//			debugLog<<"Still MEDIASUBTYPE_YV12"<<endl;
//		}
//
//		if (locMediaType->formattype == FORMAT_VideoInfo) {
//			debugLog<<"Still FORMAT_VideoInfo"<<endl;
//			VIDEOINFOHEADER* locVF = (VIDEOINFOHEADER*)locMediaType->pbFormat;
//			debugLog<<"Size = "<<locVF->bmiHeader.biSizeImage<<endl;
//			debugLog<<"Dim   = "<<locVF->bmiHeader.biWidth<<" x " <<locVF->bmiHeader.biHeight<<endl;
//		}
//
//		debugLog<<"Major  : "<<DSStringer::GUID2String(&locMediaType->majortype);
//		debugLog<<"Minor  : "<<DSStringer::GUID2String(&locMediaType->subtype);
//		debugLog<<"Format : "<<DSStringer::GUID2String(&locMediaType->formattype);
//		debugLog<<"Form Sz: "<<locMediaType->cbFormat;
//
//
//	}
//	//
//
//	////Create pointers for the samples buffer to be assigned to
//	BYTE* locBuffer = NULL;
//	
//	//
//	////Make our pointers set to point to the samples buffer
//	locSample->GetPointer(&locBuffer);
//	
//
//
//	//Fill the buffer with yuv data...
//	//	
//
//
//
//	//Set up the pointers
//	unsigned char* locDestUptoPtr = locBuffer;
//	char* locSourceUptoPtr = inYUVBuffer->y;
//
//	//
//	//Y DATA
//	//
//
//	//NEW WAY with offsets Y Data
//	long locTopPad = inYUVBuffer->y_height - mHeight - mYOffset;
//	ASSERT(locTopPad >= 0);
//	if (locTopPad < 0) {
//		locTopPad = 0;
//	}
//
//	//Skip the top padding
//	locSourceUptoPtr += (locTopPad * inYUVBuffer->y_stride);
//
//	for (long line = 0; line < mHeight; line++) {
//		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + mXOffset), mWidth);
//		locSourceUptoPtr += inYUVBuffer->y_stride;
//		locDestUptoPtr += mWidth;
//	}
//
//	locSourceUptoPtr += (mYOffset * inYUVBuffer->y_stride);
//
//	//Source advances by (y_height * y_stride)
//	//Dest advances by (mHeight * mWidth)
//
//	//
//	//V DATA
//	//
//
//	//Half the padding for uv planes... is this correct ? 
//	locTopPad = locTopPad /2;
//	
//	locSourceUptoPtr = inYUVBuffer->v;
//
//	//Skip the top padding
//	locSourceUptoPtr += (locTopPad * inYUVBuffer->y_stride);
//
//	for (long line = 0; line < mHeight / 2; line++) {
//		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mWidth / 2);
//		locSourceUptoPtr += inYUVBuffer->uv_stride;
//		locDestUptoPtr += (mWidth / 2);
//	}
//	locSourceUptoPtr += ((mYOffset/2) * inYUVBuffer->uv_stride);
//
//	//Source advances by (locTopPad + mYOffset/2 + mHeight /2) * uv_stride
//	//where locTopPad for uv = (inYUVBuffer->y_height - mHeight - mYOffset) / 2
//	//						=	(inYUVBuffer->yheight/2 - mHeight/2 - mYOffset/2)
//	// so source advances by (y_height/2) * uv_stride
//	//Dest advances by (mHeight * mWidth) /4
//
//
//	//
//	//U DATA
//	//
//
//	locSourceUptoPtr = inYUVBuffer->u;
//
//	//Skip the top padding
//	locSourceUptoPtr += (locTopPad * inYUVBuffer->y_stride);
//
//	for (long line = 0; line < mHeight / 2; line++) {
//		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mWidth / 2);
//		locSourceUptoPtr += inYUVBuffer->uv_stride;
//		locDestUptoPtr += (mWidth / 2);
//	}
//	locSourceUptoPtr += ((mYOffset/2) * inYUVBuffer->uv_stride);
//
//
//	////Y Data.
//	//for ( long line = 0; line < inYUVBuffer->y_height; line++) {
//	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->y + (inYUVBuffer->y_stride * (line))), inYUVBuffer->y_width);
//	//	locBuffer += inYUVBuffer->y_width;
//
//	//	if (mWidth > inYUVBuffer->y_width) {
//	//		memset((void*)locBuffer, 0, mWidth - inYUVBuffer->y_width);
//	//	}
//	//	locBuffer += mWidth - inYUVBuffer->y_width;
//	//}
//
//	////Pad height...
//	//for ( long line = 0; line < mHeight - inYUVBuffer->y_height; line++) {
//	//	memset((void*)locBuffer, 0, mWidth);
//	//	locBuffer += mWidth;
//	//}
//
//	////V Data
//	//for ( long line = 0; line < inYUVBuffer->uv_height; line++) {
//	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->v + (inYUVBuffer->uv_stride * (line))), inYUVBuffer->uv_width);
//	//	locBuffer += inYUVBuffer->uv_width;
//
//	//	if (mWidth/2 > inYUVBuffer->uv_width) {
//	//		memset((void*)locBuffer, 0, (mWidth/2) - inYUVBuffer->uv_width);
//	//	}
//	//	locBuffer += (mWidth/2) - inYUVBuffer->uv_width;
//	//}
//
//	////Pad height...
//	//for ( long line = 0; line < (mHeight/2) - inYUVBuffer->uv_height; line++) {
//	//	memset((void*)locBuffer, 0, mWidth/2);
//	//	locBuffer += mWidth/2;
//	//}
//
//	////U Data
//	//for (long line = 0; line < inYUVBuffer->uv_height; line++) {
//	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->u + (inYUVBuffer->uv_stride * (line))), inYUVBuffer->uv_width);
//	//	locBuffer += inYUVBuffer->uv_width;
//
//	//	if (mWidth/2 > inYUVBuffer->uv_width) {
//	//		memset((void*)locBuffer, 0, (mWidth/2) - inYUVBuffer->uv_width);
//	//	}
//	//	locBuffer += (mWidth/2) - inYUVBuffer->uv_width;
//	//}
//
//	////Pad height...
//	//for ( long line = 0; line < (mHeight/2) - inYUVBuffer->uv_height; line++) {
//	//	memset((void*)locBuffer, 0, mWidth/2);
//	//	locBuffer += mWidth/2;
//	//}
//
//
//
//
//
//	//Set the sample parameters.
//	SetSampleParams(locSample, mFrameSize, &locFrameStart, &locFrameEnd);
//
//	{
//
//		//Add a reerence to the sample so it isn't deleted in the queue.
//		//locSample->AddRef();
//		HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
//		if (locHR != S_OK) {
//			debugLog<<"Data Q rejects sample... with "<<locHR<<endl;
//			return -1;
//
//		}
//	}
//
//	
//	return 0;
//
//
//}
//
//
//
//long TheoraDecodeInputPin::decodeData(BYTE* inBuf, long inNumBytes, LONGLONG inStart, LONGLONG inEnd) 
//{
//	DbgLog((LOG_TRACE,1,TEXT("decodeData")));
//																	//Not truncated or continued... it's a full packet
//	StampedOggPacket* locPacket = new StampedOggPacket(inBuf, inNumBytes, false, false, inStart, inEnd, StampedOggPacket::OGG_END_ONLY);
//	yuv_buffer* locYUV = mTheoraDecoder->decodeTheora(locPacket);
//	if (locYUV != NULL) {
//		if (TheoraDecoded(locYUV) != 0) {
//			return -1;
//		}
//	}
//
//	return 0;
//}
//
//
//
//HRESULT TheoraDecodeInputPin::SetMediaType(const CMediaType* inMediaType) {
//
//	if (inMediaType->subtype == MEDIASUBTYPE_Theora) {
//		((TheoraDecodeFilter*)mParentFilter)->setTheoraFormat((sTheoraFormatBlock*)inMediaType->pbFormat);
//		mParentFilter->mVideoFormat = AbstractVideoDecodeFilter::THEORA;
//		//Set some other stuff here too...
//		mXOffset = ((sTheoraFormatBlock*)inMediaType->pbFormat)->xOffset;
//		mYOffset = ((sTheoraFormatBlock*)inMediaType->pbFormat)->yOffset;
//
//	} else {
//		//Failed... should never be here !
//		throw 0;
//	}
//	return CBaseInputPin::SetMediaType(inMediaType);
//}
//
