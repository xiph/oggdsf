//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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

#include "TheoraDecodeFilter.h"



//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Theora Decode Filter",					// Name
	    &CLSID_TheoraDecodeFilter,				// CLSID
	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
        NULL,									// Initialization function
#ifdef WINCE
		&TheoraDecodeFilterReg
#else
        NULL									// Set-up information (for filters)
#endif
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



TheoraDecodeFilter::TheoraDecodeFilter() 
	:	CTransformFilter( NAME("Theora Decode Filter"), NULL, CLSID_TheoraDecodeFilter)
	,	mPictureWidth(0)
	,	mPictureHeight(0)
	,	mBMIFrameSize(0)
	,	mFrameCount(0)
	,	mYOffset(0)
	,	mXOffset(0)
	,	mFrameDuration(0)
	,	mBegun(false)
	,	mSeekTimeBase(0)
	,	mLastSeenStartGranPos(0)

	,	mSegStart(0)
	,	mSegEnd(0)
	,	mPlaybackRate(0.0)
	,	mTheoraFormatInfo(NULL)
	,	mScratchBuffer(NULL)
{
#ifdef OGGCODECS_LOGGING
	debugLog.open("G:\\logs\\newtheofilter.log", ios_base::out);
#endif

#ifdef WINCE
	debugLog.clear();
	debugLog.open(L"\\Storage Card\\theo.txt", ios_base::out);
	debugLog<<"Loaded theora filter"<<endl;
	//debugLog.close();

#endif

	mCurrentOutputSubType = MEDIASUBTYPE_None;
	sOutputVideoParams locVideoParams;

	//YV12 media type
	CMediaType* locAcceptMediaType = NULL;
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_YV12;
	locAcceptMediaType->formattype = FORMAT_VideoInfo;
	mOutputMediaTypes.push_back(locAcceptMediaType);

	locVideoParams.bitsPerPixel = 12;
	locVideoParams.fourCC = MAKEFOURCC('Y','V','1','2');
	mOutputVideoParams.push_back(locVideoParams);


	//YUY2 Media Type
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_YUY2;
	locAcceptMediaType->formattype = FORMAT_VideoInfo;
	mOutputMediaTypes.push_back(locAcceptMediaType);

	locVideoParams.bitsPerPixel = 16;
	locVideoParams.fourCC = MAKEFOURCC('Y','U','Y','2');
	mOutputVideoParams.push_back(locVideoParams);

	//RGB565 Media Type
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_RGB565;
	locAcceptMediaType->formattype = FORMAT_VideoInfo;
	mOutputMediaTypes.push_back(locAcceptMediaType);

	locVideoParams.bitsPerPixel = 16;
	locVideoParams.fourCC = 0;
	mOutputVideoParams.push_back(locVideoParams);

	//RGB24 Media Type
	locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	locAcceptMediaType->subtype = MEDIASUBTYPE_RGB24;
	locAcceptMediaType->formattype = FORMAT_VideoInfo;
	mOutputMediaTypes.push_back(locAcceptMediaType);

	locVideoParams.bitsPerPixel = 32;
	locVideoParams.fourCC = 0;
	mOutputVideoParams.push_back(locVideoParams);




	mTheoraDecoder = new TheoraDecoder;
	mTheoraDecoder->initCodec();

	mScratchBuffer = new BYTE[1024*1024*2];

}

TheoraDecodeFilter::~TheoraDecodeFilter() 
{
	delete[] mScratchBuffer;
	for (size_t i = 0; i < mOutputMediaTypes.size(); i++) {
		delete mOutputMediaTypes[i];
	}

	delete mTheoraDecoder;
	mTheoraDecoder = NULL;

	delete mTheoraFormatInfo;
	mTheoraFormatInfo = NULL;
	debugLog.close();

}


#ifdef WINCE
LPAMOVIESETUP_FILTER TheoraDecodeFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&TheoraDecodeFilterReg;	
}

HRESULT TheoraDecodeFilter::Register()
{
	return CBaseFilter::Register();
}
#endif

CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 
void TheoraDecodeFilter::FillMediaType(int inPosition, CMediaType* outMediaType, unsigned long inSampleSize) 
{
	outMediaType->SetType(&(mOutputMediaTypes[inPosition]->majortype));
	outMediaType->SetSubtype(&(mOutputMediaTypes[inPosition]->subtype));
	outMediaType->SetFormatType(&(mOutputMediaTypes[inPosition]->formattype));
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(inSampleSize);		

}
bool TheoraDecodeFilter::FillVideoInfoHeader(int inPosition, VIDEOINFOHEADER* inFormatBuffer) 
{
	//MTS::: Needs changes for alternate media types. FOURCC and bitCOunt
	TheoraDecodeFilter* locFilter = this;

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / locFilter->mTheoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->mTheoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = mOutputVideoParams[inPosition].bitsPerPixel;  

	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth

	inFormatBuffer->bmiHeader.biCompression = mOutputVideoParams[inPosition].fourCC;
	inFormatBuffer->bmiHeader.biHeight = locFilter->mTheoraFormatInfo->pictureHeight;   //Not sure
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	inFormatBuffer->bmiHeader.biSizeImage = ((locFilter->mTheoraFormatInfo->pictureHeight * locFilter->mTheoraFormatInfo->pictureWidth) * inFormatBuffer->bmiHeader.biBitCount)/8;    //Size in bytes of image ??
	inFormatBuffer->bmiHeader.biWidth = locFilter->mTheoraFormatInfo->pictureWidth;
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->mTheoraFormatInfo->pictureHeight;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->mTheoraFormatInfo->pictureWidth;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->mTheoraFormatInfo->pictureHeight;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->mTheoraFormatInfo->pictureWidth;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}

HRESULT TheoraDecodeFilter::CheckInputType(const CMediaType* inMediaType) 
{
	if	( (inMediaType->majortype == MEDIATYPE_OggPacketStream) &&
			(inMediaType->subtype == MEDIASUBTYPE_None) && (inMediaType->formattype == FORMAT_OggIdentHeader)
		)
	{
		if (inMediaType->cbFormat == THEORA_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "\200theora", 7) == 0) {
				//TODO::: Possibly verify version
				debugLog<<"Input type ok"<<endl;
				return S_OK;
			}
		}
	}
	return S_FALSE;
}

HRESULT TheoraDecodeFilter::CheckOutputType(const CMediaType* inMediaType)
{
	for (size_t i = 0; i < mOutputMediaTypes.size(); i++) {
		if	(		(inMediaType->majortype == mOutputMediaTypes[i]->majortype) 
				&&	(inMediaType->subtype == mOutputMediaTypes[i]->subtype) 
				&&	(inMediaType->formattype == mOutputMediaTypes[i]->formattype)
			)
		{
			debugLog<<"Output type ok"<<endl;
			return S_OK;
		} 
	}
	debugLog<<"Output type no good"<<endl;

	if (inMediaType->majortype == MEDIATYPE_Video) {
		debugLog<<"Querying for video - FAIL"<<endl;
		debugLog<<"Sub type = "<<inMediaType->subtype.Data1<<"-"<<inMediaType->subtype.Data2<<"-"<<inMediaType->subtype.Data3<<"-"<<endl;
		debugLog<<"format type = "<<inMediaType->formattype.Data1<<"-"<<inMediaType->formattype.Data2<<"-"<<inMediaType->formattype.Data3<<"-"<<endl;
	} else {
		debugLog<<"Querying for non-video type"<<endl;
	}

	//If it matched none... return false.
	return S_FALSE;
}
HRESULT TheoraDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) {
	//MTS::: Needs multiple media types
	if ((CheckInputType(inInputMediaType) == S_OK) && (CheckOutputType(inOutputMediaType) == S_OK)) {
		VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)inOutputMediaType->Format();

		mBMIHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
		mBMIWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);


		mBMIFrameSize = (mBMIHeight * mBMIWidth * locVideoHeader->bmiHeader.biBitCount) / 8;
		debugLog<<"Check transform OK"<<endl;
		return S_OK;
	} else {
		debugLog<<"Check transform FAILED"<<endl;
		return S_FALSE;
	}
}
HRESULT TheoraDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) 
{

	HRESULT locHR = S_OK;

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	//MTS::: Maybe this needs to be reconsidered for other output types... ie rgb32 will be much bigger

	const unsigned long MIN_BUFFER_SIZE = 16*16;			//What should this be ????
	const unsigned long DEFAULT_BUFFER_SIZE = 1024*1024 * 2;
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
			//debugLog<<"DecideBufferSize : SetProperties - NULL POINTER"<<endl;
			return locHR;
		case VFW_E_ALREADY_COMMITTED:
			//debugLog<<"DecideBufferSize : SetProperties - Already COMMITED"<<endl;
			return locHR;
		case VFW_E_BADALIGN:
			//debugLog<<"DecideBufferSize : SetProperties - Bad ALIGN"<<endl;
			return locHR;
		case VFW_E_BUFFERS_OUTSTANDING:
			//debugLog<<"DecideBufferSize : SetProperties - BUFFS OUTSTANDING"<<endl;
			return locHR;
		case S_OK:
			break;
		default:
			//debugLog<<"DecideBufferSize : SetProperties - UNKNOWN ERROR"<<endl;
			break;
	}

	locHR = inAllocator->Commit();
	//debugLog<<"DecideBufferSize : Commit Returned "<<locHR<<endl;

	switch (locHR) {
		case E_FAIL:
			//debugLog<<"DecideBufferSize : Commit - FAILED "<<endl;
			return locHR;
		case E_POINTER:
			//debugLog<<"DecideBufferSize : Commit - NULL POINTER "<<endl;
			return locHR;
		case E_INVALIDARG:
			//debugLog<<"DecideBufferSize : Commit - INVALID ARG "<<endl;
			return locHR;
		case E_NOTIMPL:
			//debugLog<<"DecideBufferSize : Commit - NOT IMPL"<<endl;
			return locHR;
		case S_OK:
			//debugLog<<"DecideBufferSize : Commit - ** SUCCESS **"<<endl;
			break;
		default:
			//debugLog<<"DecideBufferSize : Commit - UNKNOWN ERROR "<<endl;
			return locHR;
	}

	debugLog<<"Buffer allocated"<<endl;

	return S_OK;
}
HRESULT TheoraDecodeFilter::GetMediaType(int inPosition, CMediaType* outOutputMediaType) 
{
	if (inPosition < 0) {
		return E_INVALIDARG;
	} else if (inPosition < mOutputMediaTypes.size()) {
		
		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		FillVideoInfoHeader(inPosition, locVideoFormat);
		FillMediaType(inPosition, outOutputMediaType, locVideoFormat->bmiHeader.biSizeImage);

		debugLog<<"Get Media Type"<<endl;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}

void TheoraDecodeFilter::ResetFrameCount() 
{
	//XTODO::: Maybe not needed
	mFrameCount = 0;
	
}

HRESULT TheoraDecodeFilter::NewSegment(REFERENCE_TIME inStart, REFERENCE_TIME inEnd, double inRate) 
{
	debugLog<<"Resetting frame count"<<endl;
	ResetFrameCount();
	mSegStart = inStart;
	mSegEnd = inEnd;
	mPlaybackRate = inRate;
	return CTransformFilter::NewSegment(inStart, inEnd, inRate);

}

HRESULT TheoraDecodeFilter::Receive(IMediaSample* inInputSample)
{
	BYTE* locBuff = NULL;
	//Get a source poitner into the input buffer
	HRESULT locHR = inInputSample->GetPointer(&locBuff);

	if (locHR != S_OK) {
		//debugLog<<"Receive : Get pointer failed..."<<locHR<<endl;	
		return S_FALSE;
	} else {

		if ((inInputSample->GetActualDataLength() > 0) && ((locBuff[0] & 128) != 0)) {
			//inInputSample->Release();

			//This is a header, so ignore it
			return S_OK;
		}
		//Make a copy of the packet buffer
		BYTE* locNewBuff = new unsigned char[inInputSample->GetActualDataLength()];		//This gets put into a packet.
		memcpy((void*)locNewBuff, (const void*)locBuff, inInputSample->GetActualDataLength());


		REFERENCE_TIME locStart = 0;
		REFERENCE_TIME locEnd = 0;
		inInputSample->GetTime(&locStart, &locEnd);

		debugLog<<"Theora::Receive - Sample: Size = "<<inInputSample->GetActualDataLength()<<" Time: "<<locStart<<" - "<<locEnd<<endl;

		//This packet is given to the decoder or buffered for later
		StampedOggPacket* locPacket = new StampedOggPacket(locNewBuff, inInputSample->GetActualDataLength(), false, false, locStart, locEnd, StampedOggPacket::OGG_END_ONLY);

		//Buffer all packets, even if we are about to send them anyway
		mBufferedPackets.push_back(locPacket);

		if (locEnd < 0) {

			//The packet was ok, but we just aren't going to deliver it yet
			return S_OK;
		} else {
			//Now we have one with a stamp, we can send all the previous ones.
			TheoraDecodeInputPin* locInputPin = (TheoraDecodeInputPin*)m_pInput;
			REFERENCE_TIME locGlobalEnd = locInputPin->convertGranuleToTime(locEnd);
			unsigned long locNumBufferedFrames = mBufferedPackets.size();
			REFERENCE_TIME locGlobalStart = locGlobalEnd - (locNumBufferedFrames * mFrameDuration);

			locStart = locGlobalStart;


			//Offsetting
			REFERENCE_TIME locGlobalOffset = 0;
			//Handle stream offsetting
			if (!locInputPin->getSentStreamOffset() && (locInputPin->getOutputPinInterface() != NULL)) {
				locInputPin->getOutputPinInterface()->notifyStreamBaseTime(locStart);
				locInputPin->setSentStreamOffset(true);
				
			}

			if (locInputPin->getOutputPinInterface() != NULL) {
				locGlobalOffset = locInputPin->getOutputPinInterface()->getGlobalBaseTime();
			}
			
			debugLog<<"Theora::Receive - "<<locNumBufferedFrames<<" frames buffered"<<endl;
			for (unsigned long i = 0; i < locNumBufferedFrames; i++) {
				debugLog<<"Theora::Receive - Processing buffered frame "<<i<<endl;
				bool locIsKeyFrame = mTheoraDecoder->isKeyFrame(mBufferedPackets[i]);
				debugLog<<"Pre theora decode"<<endl;
				yuv_buffer* locYUV = mTheoraDecoder->decodeTheora(mBufferedPackets[i]);		//This accept the packet and deletes it
				debugLog<<"Post theora decode"<<endl;
				locEnd = locStart + mFrameDuration;
				REFERENCE_TIME locAdjustedStart = locStart - mSegStart - locGlobalOffset;
				REFERENCE_TIME locAdjustedEnd = locEnd - mSegStart - locGlobalOffset;

				if (locAdjustedStart < 0) {
					locAdjustedStart = 0;
				}

				if (locAdjustedEnd >= 0) { 
					if (locYUV != NULL) {
						IMediaSample* locOutSample = NULL;
						debugLog<<"Theora::Receive - Pre output sample initialisation"<<endl;
						locHR = InitializeOutputSample(inInputSample, &locOutSample);
						if (locHR != S_OK) {
							//XTODO::: We need to trash our buffered packets
							debugLog<<"Theora::Receive - Output sample initialisation failed"<<endl;
							
							deleteBufferedPacketsAfter(i);
							
							return S_FALSE;
						}
						debugLog<<"Theora::Receive - Output sample initialisation suceeded"<<endl;

						//REFERENCE_TIME locAdjustedStart = (locStart * RATE_DENOMINATOR) / mRateNumerator;
						//REFERENCE_TIME locAdjustedEnd = (locEnd * RATE_DENOMINATOR) / mRateNumerator;


						//Fill the sample info
						if (TheoraDecoded(locYUV, locOutSample, locIsKeyFrame, locAdjustedStart, locAdjustedEnd) != S_OK) {
							
							//XTODO::: We need to trash our buffered packets
							locOutSample->Release();
							deleteBufferedPacketsAfter(i);
							return S_FALSE;
						} else {
							//Deliver the sample
							debugLog<<"Theora::Receive - Delivering: "<<locAdjustedStart<<" to "<<locAdjustedEnd<<(locIsKeyFrame ? "KEYFRAME": " ")<<endl;
							
							locHR = m_pOutput->Deliver(locOutSample);
							ULONG locTempRefCount = locOutSample->Release();
							debugLog<<"Theora::Receive - After deliver refcount = "<<locTempRefCount<<endl;
							debugLog<<"Theora::Receive - Post delivery"<<endl;
							if (locHR != S_OK) {
								//XTODO::: We need to trash our buffered packets
								debugLog<<"Theora::Receive - Delivery failed"<<endl;
								debugLog<<"Theora::Receive - locHR = "<<locHR<<endl;
								//locOutSample->Release();
								deleteBufferedPacketsAfter(i);
								return S_FALSE;
							}
							debugLog<<"Theora::Receive - Delivery Suceeded"<<endl;

						}
					} else {
						//XTODO::: We need to trash our buffered packets
						debugLog<<"locYUV == NULL"<<endl;
						deleteBufferedPacketsAfter(i);
						return S_FALSE;
					}
				}
				locStart = locEnd;
			}

			mBufferedPackets.clear();

			debugLog<<"Leaving receive method with S_OK"<<endl;
			return S_OK;
		}
	}
}

void TheoraDecodeFilter::deleteBufferedPacketsAfter(unsigned long inPacketIndex)
{
	for (size_t i = inPacketIndex + 1; i < mBufferedPackets.size(); i++) {
		delete mBufferedPackets[i];
	}

	mBufferedPackets.clear();
}
HRESULT TheoraDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample) 
{
	//debugLog<<"Theora::Transform NOT IMPLEMENTED"<<endl;
	return E_NOTIMPL;
}

HRESULT TheoraDecodeFilter::DecodeToRGB565(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd)
{
	//TODO::: This ineeds to be implemented correctly, currently just outputs a single colour
	BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

	const unsigned short RED_SHIFT = 11;
	const unsigned short GREEN_SHIFT = 6;
	const unsigned short BLUE_SHIFT = 0;


	unsigned short* locShortBuffer = (unsigned short*)locBuffer;
	for (unsigned int i = 0; i < mBMIWidth*mBMIHeight; i++) {
		locShortBuffer[i] = (31 << RED_SHIFT);
	}

	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locEnd = inEnd;

	BOOL locIsKeyFrame = FALSE;
	if (inIsKeyFrame) {
		locIsKeyFrame = TRUE;
	};
	SetSampleParams(outSample, mBMIFrameSize, &locStart, &locEnd, locIsKeyFrame);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToRGB24(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd)
{
	//TODO::: This ineeds to be implemented correctly, currently just outputs a single colour
	BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

	const unsigned short RED_SHIFT = 16;
	const unsigned short GREEN_SHIFT = 8;
	const unsigned short BLUE_SHIFT = 0;


	unsigned long* locLongBuffer = (unsigned long*)locBuffer;
	for (unsigned int i = 0; i < mBMIWidth*mBMIHeight; i++) {
		locLongBuffer[i] = (255 << RED_SHIFT);
	}

	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locEnd = inEnd;

	BOOL locIsKeyFrame = FALSE;
	if (inIsKeyFrame) {
		locIsKeyFrame = TRUE;
	};
	SetSampleParams(outSample, mBMIFrameSize, &locStart, &locEnd, locIsKeyFrame);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToYUY2(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{
	BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

	//Get the stride values and offsets
	long locYStride = inYUVBuffer->y_stride;
	long locUVStride = inYUVBuffer->uv_stride;
	long locDestPad = (mBMIWidth - mPictureWidth) * 2;
	long locSourceYPad = (locYStride - mPictureWidth);
	long locSourceUVPad = (locUVStride - (mPictureWidth/2));//locSourceYPad>>1;
	
	//Setup the source pointers into the planar data
	unsigned char* locSourceY = (unsigned char*)inYUVBuffer->y;
	unsigned char* locSourceU = (unsigned char*)inYUVBuffer->u;
	unsigned char* locSourceV = (unsigned char*)inYUVBuffer->v;

	//Skip over the Y Offset at the top of the picture and the X offset into the line
	locSourceY += (mYOffset * locYStride) + mXOffset;
	locSourceU += ((mYOffset/2) * locUVStride) + mXOffset/2;
	locSourceV += ((mYOffset/2) * locUVStride) + mXOffset/2;

	unsigned char* locSecondLineBuffer;

	for (int line = 0; line < mPictureHeight; line+= 2) {
		locSecondLineBuffer = mScratchBuffer;
		for (int col = 0; col < mPictureWidth; col+=2) {
			//Line 1
			*(locBuffer) = *(locSourceY);
			*(locBuffer+1) = *(locSourceU);
			*(locBuffer+2) = *(locSourceY+1);
			*(locBuffer+3) = *(locSourceV);

			//Line 2
			*(locSecondLineBuffer) = *(locSourceY+locYStride);
			*(locSecondLineBuffer+1) = *(locSourceU);
			*(locSecondLineBuffer+2) = *(locSourceY+locYStride+1);
			*(locSecondLineBuffer+3) = *(locSourceV);

			//Advance the pointers for this chunk of columns
			locBuffer += 4;
			locSecondLineBuffer += 4;
			locSourceY += 2;
			locSourceU++;
			locSourceV++;
		}

		//Advance the pointers for the line and copy the second line into the buffer
		locBuffer += locDestPad;
		memcpy((void*)locBuffer, (const void*)mScratchBuffer, mPictureWidth*2);
		locBuffer += mBMIWidth*2;
		locSourceY += locSourceYPad + locYStride;
		locSourceU += locSourceUVPad; //+ locUVStride;
		locSourceV += locSourceUVPad; //+ locUVStride;
	}

	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locEnd = inEnd;

	BOOL locIsKeyFrame = FALSE;
	if (inIsKeyFrame) {
		locIsKeyFrame = TRUE;
	};
	SetSampleParams(outSample, mBMIFrameSize, &locStart, &locEnd, locIsKeyFrame);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToYV12(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{
	BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

	//Set up the pointers
	unsigned char* locDestUptoPtr = locBuffer;
	char* locSourceUptoPtr = inYUVBuffer->y;

	//Strides from theora are generally -'ve
	long locYStride = inYUVBuffer->y_stride;
	long locUVStride = inYUVBuffer->uv_stride;



	debugLog<<"Y Stride = "<<locYStride<<endl;
	debugLog<<"UV Stride = "<<locUVStride<<endl;

	debugLog<<"PictureHeight = "<<mPictureHeight<<endl;
	debugLog<<"PictureWidth = "<<mPictureWidth<<endl;

	debugLog<<"BMIHeight = "<<mBMIHeight<<endl;
	debugLog<<"BMIWidth = "<<mBMIWidth<<endl;

	//
	//Y DATA
	//

	//Offsets Y Data
	long locTopPad = inYUVBuffer->y_height - mPictureHeight - mYOffset;
	debugLog<<"--------- TOP PAD = "<<locTopPad<<endl;


	//ASSERT(locTopPad >= 0);
	if (locTopPad < 0) {
		locTopPad = 0;
	} else {
		
	}

	//Skip the offset padding
	locSourceUptoPtr += (mYOffset * locYStride);

	for (unsigned long line = 0; line < mPictureHeight; line++) {
		//Ignore the x offset, and copy mPictureWidth bytes onto the destination
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + mXOffset), mPictureWidth);

		//Advance the source pointer by the stride
		locSourceUptoPtr += locYStride;

		//Advance the destination pointer by the BMI Width
		locDestUptoPtr += mBMIWidth;
	}

	//Skip the other padding
	locSourceUptoPtr += (locTopPad * locYStride);

	//Advance the destination to pad to the size the video renderer wants
	locDestUptoPtr += ((mBMIHeight - mPictureHeight) * mBMIWidth);

	//debugLog<<"Dest Distance(y) = "<<(unsigned long)(locDestUptoPtr - locBuffer)<<endl;

	//Source advances by (y_height * y_stride)
	//Dest advances by (mHeight * mWidth)

	//
	//V DATA
	//

	//TODO::: May be issue here with odd numbers

	//Half the padding for uv planes... is this correct ? 
	locTopPad = locTopPad /2;
	
	locSourceUptoPtr = inYUVBuffer->v;

	//Skip the top padding
	locSourceUptoPtr += ((mYOffset/2) * locYStride);

	for (unsigned long line = 0; line < mPictureHeight / 2; line++) {
		//Ignore the x offset and copy mPictureWidth/2 bytes to the destination
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mPictureWidth / 2);
		locSourceUptoPtr += locUVStride;
		locDestUptoPtr += (mBMIWidth / 2);
	}
	locSourceUptoPtr += (locTopPad * locUVStride);
	locDestUptoPtr += (((mBMIHeight/2) - (mPictureHeight/2)) * (mBMIWidth/2));

	//Source advances by (locTopPad + mYOffset/2 + mHeight /2) * uv_stride
	//where locTopPad for uv = (inYUVBuffer->y_height - mHeight - mYOffset) / 2
	//						=	(inYUVBuffer->yheight/2 - mHeight/2 - mYOffset/2)
	// so source advances by (y_height/2) * uv_stride
	//Dest advances by (mHeight * mWidth) /4


	//debugLog<<"Dest Distance(V) = "<<(unsigned long)(locDestUptoPtr - locBuffer)<<endl;
	//
	//U DATA
	//

	locSourceUptoPtr = inYUVBuffer->u;

	//Skip the top padding
	locSourceUptoPtr += ((mYOffset/2) * locYStride);

	for (unsigned long line = 0; line < mPictureHeight / 2; line++) {
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mPictureWidth / 2);
		locSourceUptoPtr += locUVStride;
		locDestUptoPtr += (mBMIWidth / 2);
	}

	//Redundant
	locSourceUptoPtr += (locTopPad * locUVStride);
	locDestUptoPtr += (((mBMIHeight/2) - (mPictureHeight/2)) * (mBMIWidth/2));

	//debugLog<<"Dest Distance(U) = "<<(unsigned long)(locDestUptoPtr - locBuffer)<<endl;
	//debugLog<<"Frame Size = "<<mFrameSize<<endl;

	//Set the sample parameters.
	//BOOL locIsKeyFrame = (locInterFrameNo == 0);
	BOOL locIsKeyFrame = FALSE;
	if (inIsKeyFrame) {
		locIsKeyFrame = TRUE;
	};

	REFERENCE_TIME locStart = inStart;
	REFERENCE_TIME locEnd = inEnd;

	SetSampleParams(outSample, mBMIFrameSize, &locStart, &locEnd, locIsKeyFrame);
	
	return S_OK;
}
HRESULT TheoraDecodeFilter::TheoraDecoded (yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{

	if (mCurrentOutputSubType == MEDIASUBTYPE_YV12) {
		debugLog<<"Decoding to YV12"<<endl;
		return DecodeToYV12(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} else if (mCurrentOutputSubType == MEDIASUBTYPE_YUY2) {
		debugLog<<"Decoding to YUY2"<<endl;
		return DecodeToYUY2(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} else if (mCurrentOutputSubType == MEDIASUBTYPE_RGB565) {
		debugLog<<"Decoding to RGB565"<<endl;
		return DecodeToRGB565(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} else if (mCurrentOutputSubType == MEDIASUBTYPE_RGB24) {
		debugLog<<"Decoding to RGB24"<<endl;
		return DecodeToRGB24(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} else {
		debugLog<<"Decoding to unknown type - failure"<<endl;
		return E_FAIL;
	}

}


HRESULT TheoraDecodeFilter::SetMediaType(PIN_DIRECTION inDirection, const CMediaType* inMediaType) 
{
	if (inDirection == PINDIR_INPUT) {
		if (CheckInputType(inMediaType) == S_OK) {
			//debugLog<<"Setting format block"<<endl;
			setTheoraFormat(inMediaType->pbFormat);
			
			//Set some other stuff here too...
			mXOffset = mTheoraFormatInfo->xOffset;
			mYOffset = mTheoraFormatInfo->yOffset;

			mPictureWidth = mTheoraFormatInfo->pictureWidth;
			mPictureHeight = mTheoraFormatInfo->pictureHeight;

			//How many UNITS does one frame take.
			mFrameDuration = (UNITS * mTheoraFormatInfo->frameRateDenominator) / (mTheoraFormatInfo->frameRateNumerator);

			mFrameCount = 0;
		} else {
			//Failed... should never be here !
			throw 0;
		}
		debugLog<<"SETTING input type"<<endl;
		return CTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);//CVideoTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);
	} else {

		mCurrentOutputSubType = inMediaType->subtype;
		debugLog<<"SETTING output type"<<endl;
		return CTransformFilter::SetMediaType(PINDIR_OUTPUT, inMediaType);//CVideoTransformFilter::SetMediaType(PINDIR_OUTPUT, inMediaType);
	}
}


bool TheoraDecodeFilter::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime, BOOL inIsSync) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(inIsSync);
	return true;
}
//BOOL TheoraDecodeFilter::ShouldSkipFrame(IMediaSample* inSample) {
//	//m_bSkipping = FALSE;
//	debugLog<<"Don't skip"<<endl;
//	return FALSE;
//}

sTheoraFormatBlock* TheoraDecodeFilter::getTheoraFormatBlock() 
{
	return mTheoraFormatInfo;
}
void TheoraDecodeFilter::setTheoraFormat(BYTE* inFormatBlock) 
{

	delete mTheoraFormatInfo;
	mTheoraFormatInfo = new sTheoraFormatBlock;			//Deelted in destructor.

	//0		-	55			theora ident						0	-	6
	//56	-	63			ver major							7	-	7
	//64	-	71			ver minor							8	-	8
	//72	-	79			ver subversion						9	=	9
	//80	-	95			width/16							10	-	11
	//96	-	111			height/16							12	-	13
	//112	-	135			framewidth							14	-	16
	//136	-	159			frameheight							17	-	19
	//160	-	167			xoffset								20	-	20
	//168	-	175			yoffset								21	-	21
	//176	-	207			framerateNum						22	-	25
	//208	-	239			frameratedenom						26	-	29
	//240	-	263			aspectNum							30	-	32
	//264	-	287			aspectdenom							33	-	35
	//288	-	295			colourspace							36	-	36
	//296	-	319			targetbitrate						37	-	39
	//320	-	325			targetqual							40	-	40.75
	//326	-	330			keyframintlog						40.75-  41.375

	unsigned char* locIdentHeader = inFormatBlock;
	mTheoraFormatInfo->theoraVersion = (iBE_Math::charArrToULong(locIdentHeader + 7)) >>8;
	mTheoraFormatInfo->outerFrameWidth = (iBE_Math::charArrToUShort(locIdentHeader + 10)) * 16;
	mTheoraFormatInfo->outerFrameHeight = (iBE_Math::charArrToUShort(locIdentHeader + 12)) * 16;
	mTheoraFormatInfo->pictureWidth = (iBE_Math::charArrToULong(locIdentHeader + 14)) >>8;
	mTheoraFormatInfo->pictureHeight = (iBE_Math::charArrToULong(locIdentHeader + 17)) >>8;
	mTheoraFormatInfo->xOffset = locIdentHeader[20];
	mTheoraFormatInfo->yOffset = locIdentHeader[21];
	mTheoraFormatInfo->frameRateNumerator = iBE_Math::charArrToULong(locIdentHeader + 22);
	mTheoraFormatInfo->frameRateDenominator = iBE_Math::charArrToULong(locIdentHeader + 26);
	mTheoraFormatInfo->aspectNumerator = (iBE_Math::charArrToULong(locIdentHeader + 30)) >>8;
	mTheoraFormatInfo->aspectDenominator = (iBE_Math::charArrToULong(locIdentHeader + 33)) >>8;
	mTheoraFormatInfo->colourSpace = locIdentHeader[36];
	mTheoraFormatInfo->targetBitrate = (iBE_Math::charArrToULong(locIdentHeader + 37)) >>8;
	mTheoraFormatInfo->targetQuality = (locIdentHeader[40]) >> 2;

	mTheoraFormatInfo->maxKeyframeInterval= (((locIdentHeader[40]) % 4) << 3) + (locIdentHeader[41] >> 5);
}

CBasePin* TheoraDecodeFilter::GetPin(int inPinNo)
{
    HRESULT locHR = S_OK;

    // Create an input pin if necessary

    if (m_pInput == NULL) {

        m_pInput = new TheoraDecodeInputPin(this, &locHR);		//Deleted in base destructor

        
        if (m_pInput == NULL) {
            return NULL;
        }
        m_pOutput = new TheoraDecodeOutputPin(this, &locHR);	//Deleted in base destructor
			

        if (m_pOutput == NULL) {
            delete m_pInput;
            m_pInput = NULL;
        }
    }

    // Return the pin

    if (inPinNo == 0) {
        return m_pInput;
    } else if (inPinNo == 1) {
        return m_pOutput;
    } else {
        return NULL;
    }
}