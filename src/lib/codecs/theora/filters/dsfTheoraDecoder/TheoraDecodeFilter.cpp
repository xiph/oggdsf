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

#include "TheoraDecodeFilter.h"



//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Theora Decode Filter",					// Name
	    &CLSID_TheoraDecodeFilter,				// CLSID
	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



TheoraDecodeFilter::TheoraDecodeFilter() 
	:	CTransformFilter( NAME("Theora Decode Filter"), NULL, CLSID_TheoraDecodeFilter)
	,	mHeight(0)
	,	mWidth(0)
	,	mFrameSize(0)
	,	mFrameCount(0)
	,	mYOffset(0)
	,	mXOffset(0)
	,	mFrameDuration(0)
	,	mBegun(false)
	,	mSeekTimeBase(0)
	,	mLastSeenStartGranPos(0)
	,	mTheoraFormatInfo(NULL)
{
	debugLog.open("G:\\logs\\newtheofilter.log", ios_base::out);

	mTheoraDecoder = new TheoraDecoder;
	mTheoraDecoder->initCodec();

}

TheoraDecodeFilter::~TheoraDecodeFilter() {
	delete mTheoraDecoder;
	mTheoraDecoder = NULL;
	debugLog.close();

}

CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 
void TheoraDecodeFilter::FillMediaType(CMediaType* outMediaType, unsigned long inSampleSize) {
	outMediaType->SetType(&MEDIATYPE_Video);
	outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
	outMediaType->SetFormatType(&FORMAT_VideoInfo);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(inSampleSize);		

}
bool TheoraDecodeFilter::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer) {
	TheoraDecodeFilter* locFilter = this;

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / locFilter->mTheoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->mTheoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = 12;   //12 bits per pixel
	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth
	inFormatBuffer->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
	inFormatBuffer->bmiHeader.biHeight = locFilter->mTheoraFormatInfo->frameHeight;   //Not sure
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	inFormatBuffer->bmiHeader.biSizeImage = ((locFilter->mTheoraFormatInfo->frameHeight * locFilter->mTheoraFormatInfo->frameWidth) * 3)/2;    //Size in bytes of image ??
	inFormatBuffer->bmiHeader.biWidth = locFilter->mTheoraFormatInfo->frameWidth;
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->mTheoraFormatInfo->frameHeight;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->mTheoraFormatInfo->frameWidth;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->mTheoraFormatInfo->frameHeight;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->mTheoraFormatInfo->frameWidth;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}

HRESULT TheoraDecodeFilter::CheckInputType(const CMediaType* inMediaType) {
	
	if	( (inMediaType->majortype == MEDIATYPE_Video) &&
			(inMediaType->subtype == MEDIASUBTYPE_Theora) && (inMediaType->formattype == FORMAT_Theora)
		)
	{
		return S_OK;
	} else {
		return S_FALSE;
	}
}
HRESULT TheoraDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) {
	if ((CheckInputType(inInputMediaType) == S_OK) &&
		((inOutputMediaType->majortype == MEDIATYPE_Video) && (inOutputMediaType->subtype == MEDIASUBTYPE_YV12) && (inOutputMediaType->formattype == FORMAT_VideoInfo)
		)) {

	
		return S_OK;
	} else {
		return S_FALSE;
	}
}
HRESULT TheoraDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) {
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
HRESULT TheoraDecodeFilter::GetMediaType(int inPosition, CMediaType* outOutputMediaType) {
	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if (inPosition == 0) {
		
		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		FillVideoInfoHeader(locVideoFormat);
		FillMediaType(outOutputMediaType, locVideoFormat->bmiHeader.biSizeImage);
		debugLog<<"Vid format size "<<locVideoFormat->bmiHeader.biSizeImage<<endl;
		//outMediaType->SetSampleSize(locVideoFormat->bmiHeader.biSizeImage);
		debugLog<<"Returning from GetMediaType"<<endl;
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}

void TheoraDecodeFilter::ResetFrameCount() {
	mFrameCount = 0;
	
}
HRESULT TheoraDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample) {

	//CAutoLock locLock(mStreamLock);
	debugLog<<endl<<"Transform "<<endl;
	
	HRESULT locHR;
	BYTE* locBuff = NULL;
	//Get a source poitner into the input buffer
	locHR = inInputSample->GetPointer(&locBuff);
	BYTE* locNewBuff = new unsigned char[inInputSample->GetActualDataLength()];
	memcpy((void*)locNewBuff, (const void*)locBuff, inInputSample->GetActualDataLength());


	if (FAILED(locHR)) {
		debugLog<<"Receive : Get pointer failed..."<<locHR<<endl;	
		return locHR;
	} else {
		debugLog<<"Receive : Get pointer succeeds..."<<endl;	
		//New start time hacks
		REFERENCE_TIME locStart = 0;
		REFERENCE_TIME locEnd = 0;
		inInputSample->GetTime(&locStart, &locEnd);
		//Error chacks needed here
		debugLog<<"Input Sample Time - "<<locStart<<" to "<<locEnd<<endl;
		
		//More work arounds for that stupid granule pos scheme in theora!
		REFERENCE_TIME locTimeBase = 0;
		REFERENCE_TIME locDummy = 0;
		inInputSample->GetMediaTime(&locTimeBase, &locDummy);
		mSeekTimeBase = locTimeBase;
		//

		debugLog<<"SeekTimeBase = "<<mSeekTimeBase<<endl;
		
		if ((mLastSeenStartGranPos != locStart) && (locStart != -1)) {
			debugLog<<"Resetting frame count"<<endl;
			ResetFrameCount();
			mLastSeenStartGranPos = locStart;
			debugLog<<"Setting base gran pos to "<<locStart<<endl;
		}
		
		//End of additions



		AM_MEDIA_TYPE* locMediaType = NULL;
		inInputSample->GetMediaType(&locMediaType);
		if (locMediaType == NULL) {
			debugLog<<"No dynamic change..."<<endl;
		} else {
			debugLog<<"Attempting dynamic change..."<<endl;
		}
				
		StampedOggPacket* locPacket = new StampedOggPacket(locNewBuff, inInputSample->GetActualDataLength(), false, false, locStart, locEnd, StampedOggPacket::OGG_END_ONLY);
		yuv_buffer* locYUV = mTheoraDecoder->decodeTheora(locPacket);
		if (locYUV != NULL) {
			if (TheoraDecoded(locYUV, outOutputSample) != 0) {
				debugLog<<"Decoded *** FALSE ***"<<endl;
				return S_FALSE;
			}
		} else {
			debugLog<<"!@&#^()!&@#!()*@#&)!(*@#&()!*@# NULL Decode"<<endl;
			return S_FALSE;
		}

		return S_OK;
		
	}
	
}

int TheoraDecodeFilter::TheoraDecoded (yuv_buffer* inYUVBuffer, IMediaSample* outSample) 
{
	debugLog<<"TheoraDecoded... #################### "<<endl;
	
		
	if (!mBegun) {
		debugLog<<"First time..."<<endl;
		mBegun = true;
		
		//How many UNITS does one frame take.
		mFrameDuration = (UNITS * mTheoraFormatInfo->frameRateDenominator) / (mTheoraFormatInfo->frameRateNumerator);
		mFrameSize = (mHeight * mWidth * 3) / 2;
		mFrameCount = 0;
		debugLog<<"Frame Durn = "<<mFrameDuration<<endl;
		debugLog<<"FrameSize = "<<mFrameSize<<endl;
		
		
	}

	debugLog<<"y_height x width = "<<inYUVBuffer->y_height<<" x "<<inYUVBuffer->y_width<<"  ("<<inYUVBuffer->y_stride<<endl;
	debugLog<<"uv_height x width = "<<inYUVBuffer->uv_height<<" x "<<inYUVBuffer->uv_width<<"  ("<<inYUVBuffer->y_stride<<endl;

	debugLog<<"Offsets x,y = "<<mXOffset<<", "<<mYOffset<<endl;
	////FIX::: Most of this will be obselete... the demux does it all.
	//

	////TO DO::: Fix this up... needs to move around order and some only needs to be done once, move it into the block aboce and use member data

	////Make the start timestamp
	////FIX:::Abstract this calculation
	DbgLog((LOG_TRACE,1,TEXT("Frame Count = %d"), mFrameCount));
	debugLog<<"Frame Count = "<<mFrameCount<<endl;
	//REFERENCE_TIME locFrameStart = CurrentStartTime() + (mFrameCount * mFrameDuration);

	//Timestamp hacks start here...
	unsigned long locMod = (unsigned long)pow(2, mTheoraFormatInfo->maxKeyframeInterval);
	
	DbgLog((LOG_TRACE,1,TEXT("locSeenGranPos = %d"), mLastSeenStartGranPos));
	DbgLog((LOG_TRACE,1,TEXT("locMod = %d"), locMod));
	
	debugLog<<"locMod = "<<locMod<<endl;
	unsigned long locInterFrameNo = (mLastSeenStartGranPos) % locMod;
	
	DbgLog((LOG_TRACE,1,TEXT("InterFrameNo = %d"), locInterFrameNo));
	debugLog<<"Interframe No = "<<locInterFrameNo<<endl;
	LONGLONG locAbsFramePos = ((mLastSeenStartGranPos >> mTheoraFormatInfo->maxKeyframeInterval)) + locInterFrameNo;
	
	debugLog<<"Abs frame No = "<<locAbsFramePos<<endl;
	DbgLog((LOG_TRACE,1,TEXT("AbsFrameNo = %d"), locAbsFramePos));
	DbgLog((LOG_TRACE,1,TEXT("mSeekTimeBase = %d"), mSeekTimeBase));
	
	debugLog<<"Seek time base = "<<mSeekTimeBase<<endl;
	REFERENCE_TIME locTimeBase = (locAbsFramePos * mFrameDuration) - mSeekTimeBase;

	debugLog<<"LocTimeBase = "<<locTimeBase<<endl;
	
	DbgLog((LOG_TRACE,1,TEXT("locTimeBase = %d"), locTimeBase));
	//
	//

	REFERENCE_TIME locFrameStart = locTimeBase + (mFrameCount * mFrameDuration);
	//Increment the frame counter
	mFrameCount++;
	
	//Make the end frame counter
	//REFERENCE_TIME locFrameEnd = CurrentStartTime() + (mFrameCount * mFrameDuration);
	REFERENCE_TIME locFrameEnd = locTimeBase + (mFrameCount * mFrameDuration);

	DbgLog((LOG_TRACE,1,TEXT("Frame Runs From %d"), locFrameStart));
	DbgLog((LOG_TRACE,1,TEXT("Frame Runs To %d"), locFrameEnd));

	
	debugLog<<"Sample times = "<<locFrameStart<<" to "<<locFrameEnd<<endl;
	
	//FILTER_STATE locFS;
	//GetState(0, &locFS);
	//debugLog<<"State Before = "<<locFS<<endl;
	//HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, locFlags);
	//GetState(0, &locFS);
	//debugLog<<"State After = "<<locFS<<endl;
	
	

	//Debuggin code
	AM_MEDIA_TYPE* locMediaType = NULL;
	outSample->GetMediaType(&locMediaType);
	if (locMediaType == NULL) {
		debugLog<<"No dynamic change..."<<endl;
	} else {
		debugLog<<"Attempting dynamic change..."<<endl;
		if (locMediaType->majortype == MEDIATYPE_Video) {
			debugLog<<"Still MEDIATYPE_Video"<<endl;
		}

		if (locMediaType->subtype == MEDIASUBTYPE_YV12) {
			debugLog<<"Still MEDIASUBTYPE_YV12"<<endl;
		}

		if (locMediaType->formattype == FORMAT_VideoInfo) {
			debugLog<<"Still FORMAT_VideoInfo"<<endl;
			VIDEOINFOHEADER* locVF = (VIDEOINFOHEADER*)locMediaType->pbFormat;
			debugLog<<"Size = "<<locVF->bmiHeader.biSizeImage<<endl;
			debugLog<<"Dim   = "<<locVF->bmiHeader.biWidth<<" x " <<locVF->bmiHeader.biHeight<<endl;
		}

		debugLog<<"Major  : "<<DSStringer::GUID2String(&locMediaType->majortype);
		debugLog<<"Minor  : "<<DSStringer::GUID2String(&locMediaType->subtype);
		debugLog<<"Format : "<<DSStringer::GUID2String(&locMediaType->formattype);
		debugLog<<"Form Sz: "<<locMediaType->cbFormat;


	}
	//

	////Create pointers for the samples buffer to be assigned to
	BYTE* locBuffer = NULL;
	
	//
	////Make our pointers set to point to the samples buffer
	outSample->GetPointer(&locBuffer);
	


	//Fill the buffer with yuv data...
	//	



	//Set up the pointers
	unsigned char* locDestUptoPtr = locBuffer;
	char* locSourceUptoPtr = inYUVBuffer->y;

	//Strides from theora are generally -'ve so absolutise them.
	unsigned long locYStride = inYUVBuffer->y_stride;
	unsigned long locUVStride = inYUVBuffer->uv_stride;

	//
	//Y DATA
	//

	//NEW WAY with offsets Y Data
	long locTopPad = inYUVBuffer->y_height - mHeight - mYOffset;
	ASSERT(locTopPad >= 0);
	if (locTopPad < 0) {
		locTopPad = 0;
	}

	//Skip the top padding
	locSourceUptoPtr += (locTopPad * locYStride);

	for (long line = 0; line < mHeight; line++) {
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + mXOffset), mWidth);
		locSourceUptoPtr += locYStride;
		locDestUptoPtr += mWidth;
	}

	locSourceUptoPtr += (mYOffset * locYStride);

	//Source advances by (y_height * y_stride)
	//Dest advances by (mHeight * mWidth)

	//
	//V DATA
	//

	//Half the padding for uv planes... is this correct ? 
	locTopPad = locTopPad /2;
	
	locSourceUptoPtr = inYUVBuffer->v;

	//Skip the top padding
	locSourceUptoPtr += (locTopPad * locYStride);

	for (long line = 0; line < mHeight / 2; line++) {
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mWidth / 2);
		locSourceUptoPtr += locUVStride;
		locDestUptoPtr += (mWidth / 2);
	}
	locSourceUptoPtr += ((mYOffset/2) * locUVStride);

	//Source advances by (locTopPad + mYOffset/2 + mHeight /2) * uv_stride
	//where locTopPad for uv = (inYUVBuffer->y_height - mHeight - mYOffset) / 2
	//						=	(inYUVBuffer->yheight/2 - mHeight/2 - mYOffset/2)
	// so source advances by (y_height/2) * uv_stride
	//Dest advances by (mHeight * mWidth) /4


	//
	//U DATA
	//

	locSourceUptoPtr = inYUVBuffer->u;

	//Skip the top padding
	locSourceUptoPtr += (locTopPad * locYStride);

	for (long line = 0; line < mHeight / 2; line++) {
		memcpy((void*)(locDestUptoPtr), (const void*)(locSourceUptoPtr + (mXOffset / 2)), mWidth / 2);
		locSourceUptoPtr += locUVStride;
		locDestUptoPtr += (mWidth / 2);
	}
	locSourceUptoPtr += ((mYOffset/2) * locUVStride);



	//Set the sample parameters.
	BOOL locIsKeyFrame = (locInterFrameNo == 0);
	locIsKeyFrame = TRUE;
	if (locIsKeyFrame == TRUE) {
		debugLog<<"KEY FRAME ++++++"<<endl;
	};
	SetSampleParams(outSample, mFrameSize, &locFrameStart, &locFrameEnd, locIsKeyFrame);

	
	
	return 0;


}


HRESULT TheoraDecodeFilter::SetMediaType(PIN_DIRECTION inDirection, const CMediaType* inMediaType) {

	if (inDirection == PINDIR_INPUT) {
		if (inMediaType->subtype == MEDIASUBTYPE_Theora) {
			debugLog<<"Setting format block"<<endl;
			setTheoraFormat((sTheoraFormatBlock*)inMediaType->pbFormat);
			
			//Set some other stuff here too...
			mXOffset = ((sTheoraFormatBlock*)inMediaType->pbFormat)->xOffset;
			mYOffset = ((sTheoraFormatBlock*)inMediaType->pbFormat)->yOffset;

		} else {
			//Failed... should never be here !
			throw 0;
		}
		return CTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);//CVideoTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);
	} else {
		debugLog<<"Setting Output Stuff"<<endl;
		//Output pin SetMediaType
		VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)inMediaType->Format();
		mHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
		mWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);
		mFrameSize = (unsigned long)locVideoHeader->bmiHeader.biSizeImage;

		debugLog<<"Size = "<<mWidth<<" x "<<mHeight<<" ("<<mFrameSize<<")"<<endl;
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
BOOL TheoraDecodeFilter::ShouldSkipFrame(IMediaSample* inSample) {
	//m_bSkipping = FALSE;
	debugLog<<"Don't skip"<<endl;
	return FALSE;
}

sTheoraFormatBlock* TheoraDecodeFilter::getTheoraFormatBlock() 
{
	return mTheoraFormatInfo;
}
void TheoraDecodeFilter::setTheoraFormat(sTheoraFormatBlock* inFormatBlock) 
{
	delete mTheoraFormatInfo;
	mTheoraFormatInfo = new sTheoraFormatBlock;
	*mTheoraFormatInfo = *inFormatBlock;
}
//---------------------------------------
//OLD IMPLOEMENTATION....
//---------------------------------------
//#include "theoradecodefilter.h"
//
//
//
////COM Factory Template
//CFactoryTemplate g_Templates[] = 
//{
//    { 
//		L"Theora Decode Filter",					// Name
//	    &CLSID_TheoraDecodeFilter,				// CLSID
//	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
//        NULL,									// Initialization function
//        NULL									// Set-up information (for filters)
//    }
//
//};
//
//// Generic way of determining the number of items in the template
//int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 
//
//
//
////Theora Decode Filter Implementation
////==================================
//TheoraDecodeFilter::TheoraDecodeFilter()
//	:	AbstractVideoDecodeFilter(NAME("Ogg Video Decoder"), CLSID_TheoraDecodeFilter, THEORA)
//	,	mTheoraFormatInfo(NULL)
//{
//
//	bool locWasConstructed = ConstructPins();
//}
//
//bool TheoraDecodeFilter::ConstructPins() 
//{
//	
//	//Output pin must be done first because it's passed to the input pin.
//	mOutputPin = new TheoraDecodeOutputPin(this, m_pLock);
//
//	CMediaType* locAcceptMediaType = new CMediaType(&MEDIATYPE_Video);
//	locAcceptMediaType->subtype = MEDIASUBTYPE_Theora;
//	locAcceptMediaType->formattype = FORMAT_Theora;
//	mInputPin = new TheoraDecodeInputPin(this, m_pLock, mOutputPin, locAcceptMediaType);
//	return true;
//}
//
//TheoraDecodeFilter::~TheoraDecodeFilter(void)
//{
//	//DestroyPins();
//	delete mTheoraFormatInfo;
//}
//
//CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
//{
//	//This routine is the COM implementation to create a new Filter
//	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();
//    if (pNewObject == NULL) {
//        *pHr = E_OUTOFMEMORY;
//    }
//	return pNewObject;
//} 
//
//STDMETHODIMP TheoraDecodeFilter::GetState(DWORD dw, FILTER_STATE *pState)
//{
//    CheckPointer(pState, E_POINTER);
//    *pState = m_State;
//	if (m_State == State_Paused) {
//        return VFW_S_CANT_CUE;
//	} else {
//        return S_OK;
//	}
//}
//
////QUERY::: Do we need these ? Aren't we all friedns here ??
////RESULT::: Keep them, set function must be kept... get could go... but keep for consistency
//sTheoraFormatBlock* TheoraDecodeFilter::getTheoraFormatBlock() 
//{
//	return mTheoraFormatInfo;
//}
//void TheoraDecodeFilter::setTheoraFormat(sTheoraFormatBlock* inFormatBlock) 
//{
//	delete mTheoraFormatInfo;
//	mTheoraFormatInfo = new sTheoraFormatBlock;
//	*mTheoraFormatInfo = *inFormatBlock;
//}