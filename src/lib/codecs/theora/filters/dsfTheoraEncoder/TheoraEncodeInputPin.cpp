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
#include "theoraencodeinputpin.h"

TheoraEncodeInputPin::TheoraEncodeInputPin(AbstractVideoEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoEncodeOutputPin* inOutputPin)
	:	AbstractVideoEncodeInputPin(inParentFilter, inFilterLock, inOutputPin, NAME("TheoraEncodeInputPin"), L"YV12 In")
	,	mBegun(false)
	

{
	debugLog.open("g:\\logs\\theoencfilt.log", ios_base::out);
	mYUV.y = NULL;
	mYUV.u = NULL;
	mYUV.v = NULL;
	
}

TheoraEncodeInputPin::~TheoraEncodeInputPin(void)
{
	debugLog.close();
	DestroyCodec();
	delete mYUV.y;
	delete mYUV.u;
	delete mYUV.v;

}


HRESULT TheoraEncodeInputPin::deliverData(LONGLONG inStart, LONGLONG inEnd, unsigned char* inBuf, unsigned long inNumBytes) {
	debugLog <<" deliverData : "<<inStart<<" - "<<inEnd<<"  :: size = "<<inNumBytes<<endl;
	//Get a pointer to a new sample stamped with our time
	IMediaSample* locSample;
	HRESULT locHR = mOutputPin->GetDeliveryBuffer(&locSample, &inStart, &inEnd, NULL);

	if (locHR != S_OK) {
		//We get here when the application goes into stop mode usually.
		return locHR;
	}	
	
	BYTE* locBuffer = NULL;

	
	//Make our pointers set to point to the samples buffer
	locSample->GetPointer(&locBuffer);

	if (locSample->GetSize() >= inNumBytes) {

		memcpy((void*)locBuffer, (const void*)inBuf, inNumBytes);
		
		//Set the sample parameters.
		SetSampleParams(locSample, inNumBytes, &inStart, &inEnd);

		{
			CAutoLock locLock(m_pLock);

			HRESULT locHR = mOutputPin->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
			if (locHR != S_OK) {
				return locHR;	
			} else {
			}
		}
		debugLog<<"deliverData : SUCCESS"<<endl;
		return S_OK;
	} else {
		throw 0;
	}

}
//PURE VIRTUALS
long TheoraEncodeInputPin::encodeData(unsigned char* inBuf, long inNumBytes) {

	//Time stamps are granule pos not directshow times

	LONGLONG locFrameStart = mUptoFrame;
	LONGLONG locFrameEnd = 0;
	HRESULT locHR = S_OK;
	if (!mBegun) {
		debugLog<<"encodeData : First time"<<endl;
		mBegun = true;
		
		StampedOggPacket** locHeaders;
		locHeaders = mTheoraEncoder.initCodec(mTheoraInfo);

		for (int i = 0; i < 3; i++) {
			locHR = deliverData(0,0,locHeaders[i]->packetData(), locHeaders[i]->packetSize());
			if (locHR != S_OK) {
				return locHR;
			}
		}
	}

	//yuv_buffer* locYUV = new yuv_buffer;
	
	

	////Fill the buffer with yuv data...
	////	

	////Y Data.
	//for ( long line = 0; line < inYUVBuffer->y_height; line++) {
	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->y + (inYUVBuffer->y_stride * (line))), inYUVBuffer->y_width);
	//	locBuffer += inYUVBuffer->y_width;
	
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	
	char* locDestUptoPtr = mYUV.y;
	for (long line = 0; line < mYUV.y_height; line++) {
		memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mYUV.y_width);
		locSourceUptoPtr += mYUV.y_width;
		locDestUptoPtr += mYUV.y_width;
	}

	//	if (mWidth > inYUVBuffer->y_width) {
	//		memset((void*)locBuffer, 0, mWidth - inYUVBuffer->y_width);
	//	}
	//	locBuffer += mWidth - inYUVBuffer->y_width;
	//}

	////Pad height...
	//for ( long line = 0; line < mHeight - inYUVBuffer->y_height; line++) {
	//	memset((void*)locBuffer, 0, mWidth);
	//	locBuffer += mWidth;
	//}

	////V Data
	//for ( long line = 0; line < inYUVBuffer->uv_height; line++) {
	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->v + (inYUVBuffer->uv_stride * (line))), inYUVBuffer->uv_width);
	//	locBuffer += inYUVBuffer->uv_width;

	locDestUptoPtr = mYUV.v;
	for (long line = 0; line < mYUV.uv_height; line++) {
		memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mYUV.uv_width);
		locSourceUptoPtr += mYUV.uv_width;
		locDestUptoPtr += mYUV.uv_width;

	}
	//	if (mWidth/2 > inYUVBuffer->uv_width) {
	//		memset((void*)locBuffer, 0, (mWidth/2) - inYUVBuffer->uv_width);
	//	}
	//	locBuffer += (mWidth/2) - inYUVBuffer->uv_width;
	//}

	////Pad height...
	//for ( long line = 0; line < (mHeight/2) - inYUVBuffer->uv_height; line++) {
	//	memset((void*)locBuffer, 0, mWidth/2);
	//	locBuffer += mWidth/2;
	//}

	////U Data
	//for (long line = 0; line < inYUVBuffer->uv_height; line++) {
	//	memcpy((void*)locBuffer, (const void*)(inYUVBuffer->u + (inYUVBuffer->uv_stride * (line))), inYUVBuffer->uv_width);
	//	locBuffer += inYUVBuffer->uv_width;

	locDestUptoPtr = mYUV.u;
	for (long line = 0; line < mYUV.uv_height; line++) {
		memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mYUV.uv_width);
		locSourceUptoPtr += mYUV.uv_width;
		locDestUptoPtr += mYUV.uv_width;
	}
	//	if (mWidth/2 > inYUVBuffer->uv_width) {
	//		memset((void*)locBuffer, 0, (mWidth/2) - inYUVBuffer->uv_width);
	//	}
	//	locBuffer += (mWidth/2) - inYUVBuffer->uv_width;
	//}

	////Pad height...
	//for ( long line = 0; line < (mHeight/2) - inYUVBuffer->uv_height; line++) {
	//	memset((void*)locBuffer, 0, mWidth/2);
	//	locBuffer += mWidth/2;
	//}




	StampedOggPacket* locPacket = mTheoraEncoder.encodeTheora(&mYUV);
	if (locPacket == NULL) {
		return S_FALSE;
	}
	locFrameEnd		= mUptoFrame 
					= locPacket->endTime();

	return deliverData(locFrameStart, locFrameEnd, locPacket->packetData(), locPacket->packetSize());

}
bool TheoraEncodeInputPin::ConstructCodec() {


	theora_info_init(&mTheoraInfo);
	
	mTheoraInfo.width	=	mWidth
						=	mYUV.y_width
						=	mYUV.y_stride
						=	mVideoFormat->bmiHeader.biWidth;

	mYUV.uv_width		=	mYUV.uv_stride
						=	mWidth/2;

	
	mTheoraInfo.height	=	mHeight
						=	mYUV.y_height
						=	mVideoFormat->bmiHeader.biHeight;

	mYUV.uv_height		=	mHeight/2;

	//Round up to multiple of 16
	mTheoraInfo.width = (((mTheoraInfo.width + 15)>>4)<<4);
	mTheoraInfo.height = (((mTheoraInfo.height + 15)>>4)<<4);


	mYUV.y				=	new char[(mHeight * mWidth)];
	mYUV.u				=	new char[(mHeight * mWidth)/4];
	mYUV.v				=	new char[(mHeight * mWidth)/4];

	debugLog<<"Width =y_w = y_s = "<<mWidth<<" ::: "<<"Height=y_h= "<<mHeight<<endl;
	debugLog<<"uv_w=uv_s= "<<mYUV.uv_stride<<" ::: " <<"uv_height = "<<mYUV.uv_height<<endl;
	mTheoraInfo.frame_width=mVideoFormat->bmiHeader.biWidth;
	mTheoraInfo.frame_height=mVideoFormat->bmiHeader.biHeight;
	mTheoraInfo.offset_x=0;
	mTheoraInfo.offset_y=0;
	

	//HACK:::Bit of a hack to convert dshow nanos to a fps num/denom.
	unsigned long locNum = (((double)10000000) / ((double)mVideoFormat->AvgTimePerFrame)) + (double)0.5;

	debugLog<<"FPS = "<<locNum<<endl;
	mTheoraInfo.fps_numerator = locNum;
	mTheoraInfo.fps_denominator = 1;
	//I don't think this is right !
	mTheoraInfo.aspect_numerator=0;//mVideoFormat->bmiHeader.biWidth;//video_an;
	mTheoraInfo.aspect_denominator=0;//mVideoFormat->bmiHeader.biHeight;//video_ad;
	
	mTheoraInfo.colorspace=OC_CS_UNSPECIFIED;
	mTheoraInfo.target_bitrate=400000; //mVideoFormat->dwBitRate;

	//Hard code for now
	mTheoraInfo.quality=30; //video_q;

	mTheoraInfo.dropframes_p=0;
	mTheoraInfo.quick_p=1;
	mTheoraInfo.keyframe_auto_p=1;
	mTheoraInfo.keyframe_frequency=64;   //If you change this... change the logged value below
	mTheoraInfo.keyframe_frequency_force=64;  //ditto
	mTheoraInfo.keyframe_data_target_bitrate=mTheoraInfo.target_bitrate*1.5;
	mTheoraInfo.keyframe_auto_threshold=80;
	mTheoraInfo.keyframe_mindistance=8;
	mTheoraInfo.noise_sensitivity=1; 

	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateNumerator = locNum;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.frameRateDenominator = 1;
	((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock.maxKeyframeInterval = 6;   //log2(keyframe_freq) from above
	return true;

}
void TheoraEncodeInputPin::DestroyCodec() {
	//fish_sound_delete(mFishSound);
	//mFishSound = NULL;
}



HRESULT TheoraEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	AbstractVideoEncodeInputPin::SetMediaType(inMediaType);

	ConstructCodec();


	return S_OK;
	
}

theora_info* TheoraEncodeInputPin::theoraInfo() {
	return &mTheoraInfo;
}