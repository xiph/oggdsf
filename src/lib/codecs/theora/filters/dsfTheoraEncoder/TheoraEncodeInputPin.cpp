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
	,	mXOffset(0)
	,	mYOffset(0)
	

{
	//debugLog.open("g:\\logs\\theoencfilt.log", ios_base::out);
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
	//debugLog <<" deliverData : "<<inStart<<" - "<<inEnd<<"  :: size = "<<inNumBytes<<endl;
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
		//debugLog<<"deliverData : SUCCESS"<<endl;
		return S_OK;
	} else {
		throw 0;
	}

}

long TheoraEncodeInputPin::encodeYV12ToYV12(unsigned char* inBuf, long inNumBytes) {
	//Source Buffer all in one buffer

	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//YYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYY
	//VVVVVVVVVVVVVVVVVVVVVVVV VVVVVVVVVVVVVVVVVVVVVVVV
	//UUUUUUUUUUUUUUUUUUUUUUUU UUUUUUUUUUUUUUUUUUUUUUUU


	//Destination buffer Y, U, V in seperate planes, lowercase are line padding, "." height padding
	//.............................. ..............................
	//.............................. ..............................
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//yyyyyyYYYYYYYYYYYYYYYYYYYYYYYY YYYYYYYYYYYYYYYYYYYYYYYYyyyyyy
	//.............................. ..............................
	//.............................. ..............................

	//.............................. ..............................
	//vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv vvvVVVVVVVVVVVVVVVVVVVVVVVVvvv
	//.............................. ..............................
	
	//.............................. ..............................
	//uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu uuuUUUUUUUUUUUUUUUUUUUUUUUUuuu
	//.............................. ..............................

	//NOTE	: mHeight, mWidth are the actual video sizes and are the sizes of the incoming buffer
	//		  The yuv width, height parameters are a /16 up rounded size of the output buffer to be sent to libtheora


	//Setup the source pointer
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	//
	
	//========
	//Y DATA |
	//=========================================================================================================

	//Setup the destination pointer
	char* locDestUptoPtr = mYUV.y;
	//

	//Pad top of Y plane buffer with mYOffset lines of width mYUV.y_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (mYOffset * mYUV.y_width);
	}
	//Source pointer does not advance
	//

	//Add mHeight lines of data of width mWidth plus padding of mXOffset at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;
		}
	} else {
		for (long line = 0; line < mHeight; line++) {
			//Pad the start of the line with mXOffset bytes
			memset((void*)locDestUptoPtr, NULL, mXOffset);
			locDestUptoPtr += mXOffset;

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth);
			locSourceUptoPtr += mWidth;
			locDestUptoPtr += mWidth;

			//Pad the end of the line with mXOffset bytes
			memset((void*)locDestUptoPtr, NULL, mXOffset);
			locDestUptoPtr += mXOffset;
		}

	}

	//Pad bottom of Y plane buffer with mYOffset lines of width mYUV.y_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, mYOffset * mYUV.y_width);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += (mYOffset * mYUV.y_width);
		//Source pointer does not advance
	}
	


	//========
	//V DATA |
	//=========================================================================================================

	//Set the destination poitner
	locDestUptoPtr = mYUV.v;
	//

	//Pad top of V plane buffer with mYOffset/2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	//

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			//Pad the end of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance
		}

	}

	//Pad bottom of V plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	




	//========
	//U DATA |
	//=========================================================================================================

	//Set the destination pointer
	locDestUptoPtr = mYUV.u;
	//

	//Pad top of U plane buffer with mYOffset/2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}
	//

	//Add mHeight/2 lines of data of length mWidth/2 plus padded by mXOffset/2 at each end
	if (mXOffset == 0) {
		//Slight optimisation to keep the inner loop tighter
		for (long line = 0; line < mHeight / 2; line++) {
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);
		}
	} else {
		for (long line = 0; line < mHeight / 2; line++) {
			//Pad the start of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance

			//Fill in the meaty bit
			memcpy((void*)locDestUptoPtr, (const void*)locSourceUptoPtr, mWidth / 2);
			locSourceUptoPtr += (mWidth / 2);
			locDestUptoPtr += (mWidth / 2);

			//Pad the end of the line
			memset((void*)locDestUptoPtr, NULL, mXOffset / 2);
			locDestUptoPtr += (mXOffset / 2);
			//Source pointer does not advance
		}

	}

	//Pad bottom of U plane buffer with mYOffset / 2 lines of width mYUV.uv_width
	if (mYOffset != 0) {
		memset((void*)locDestUptoPtr, NULL, (mYOffset * mYUV.uv_width) / 2);			//Is it needed to zero this out ? Or just leave junk ?
		locDestUptoPtr += ((mYOffset * mYUV.uv_width) / 2);
		//Source pointer does not advance
	}

	//======================================================================================================
	return 0;

}

long TheoraEncodeInputPin::encodeYUY2ToYV12(unsigned char* inBuf, long inNumBytes) {
	unsigned char* locSourceUptoPtr = inBuf;  //View only... don't delete locUptoPtr

	//YUY2 is Y0 U0 Y1 V0 Y2 U1 Y3 V1
	// it has twice as much sampling height as YV12 so downsample it.

	char* locYUpto = mYUV.y;
	char* locUUpto = mYUV.u;
	char* locVUpto = mYUV.v;

	

	//After downsampling... from each block of 8, we get 4 y samples and 1 each of u and v


	for (int i = 0; i < mHeight / 2; i++) {
		//TO DO: Draw memory layouts.

		//***Part of the average method... store the pointer to the last of the previous line
		//locLastUUpto = locUUpto;
		//locLastVUpto = locVUpto;
		//***

		for (int j = 0; j < mWidth / 2; j++) {
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locUUpto++) = *(locSourceUptoPtr++);
			*(locYUpto++) = *(locSourceUptoPtr++);
			*(locVUpto++) = *(locSourceUptoPtr++);
		}

		
		//***Drop line method
		for (int j = 0; j < mWidth / 2; j++) {
			//Ignore the second line
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
			*(locYUpto++) = *(locSourceUptoPtr++);
			locSourceUptoPtr++;
		}
		//***

		//*** PArt of the Alternate method to average...
		//for (int j = 0; j < mWidth / 2; j++) {
		//	//Ignore the second line
		//	*(locYUpto++) = *(locSourceUptoPtr++);
		//	*(locLastUUpto++) = ((short)(*locLastUUpto) + ((short)(*locUUpto))) / 2;
		//	
		//	*(locYUpto++) = *(locSourceUptoPtr++);
		//	*(locLastVUpto++) = ((short)(*locLastVUpto) + ((short)(*locVUpto))) / 2;
		//	
		//}
		//***



	}
	return 0;
}
//PURE VIRTUALS
long TheoraEncodeInputPin::encodeData(unsigned char* inBuf, long inNumBytes) {

	//TODO::: Break this function up a bit !!

	//Time stamps are granule pos not directshow times

	LONGLONG locFrameStart = mUptoFrame;
	LONGLONG locFrameEnd = 0;
	HRESULT locHR = S_OK;
	if (!mBegun) {
		//debugLog<<"encodeData : First time"<<endl;
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

	if (mPinInputType.subtype == MEDIASUBTYPE_YUY2) {
		encodeYUY2ToYV12(inBuf, inNumBytes);
	} else {
		//Should be more specifc.
		encodeYV12ToYV12(inBuf, inNumBytes);
	}
	

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
	
	//Round up to multiple of 16 for theora

	//---------------------------------------------------------------------------------------------------------------
	//mTheoraInfo values
	//==================
	//width, height					-	/16 up rounded values
	//frame_width, frame_height		-	raw video source values
	//offset_x						-	CENTRED - *half* the difference between width and frame_width
	//offset_y						-	CENTRED - *half* the difference between height and frame_heigth

	//mYUV values - for YV12 format
	//=============================
	//y_width, y_stride				-	Both equal and equal to the /16 up rounded wdith values
	//uv_width, uv_stride			-	Both equal and equal to *half* the /16 up rounded width values
	//y_height						-	Equal to the /16 up rounded height value
	//uv_height						-	Equal to *half* the /16 up rounded height value
	//y								-	Buffer of size y_width*y_height (/16 up rounded values)
	//u,v							-	Buffers each *quarter* the size of the y buffer (/16 up rounded values)

	//Member data
	//===========
	//mWidth						-	raw video source values... equal to frame_width
	//mHeight						-	raw video source values... equal to frame_height
	//mXOffset						-	x offset
	//mYOffset						-	y offset
	//---------------------------------------------------------------------------------------------------------------

	//Width data
	mTheoraInfo.width			=	mYUV.y_width
								=	mYUV.y_stride
								=	(((mVideoFormat->bmiHeader.biWidth + 15)>>4)<<4);

	mTheoraInfo.frame_width		=	mWidth
								=	mVideoFormat->bmiHeader.biWidth;

	mYUV.uv_width				=	mYUV.uv_stride
								=	mYUV.y_width/2;

	
	//

	//Height data
	mTheoraInfo.height			=	mYUV.y_height
								=	(((mVideoFormat->bmiHeader.biHeight + 15)>>4)<<4);

	mTheoraInfo.frame_height	=	mHeight
								=	mVideoFormat->bmiHeader.biHeight;

	mYUV.uv_height				=	mYUV.y_height/2;

	
	//

	//Set offset values... centred
	mTheoraInfo.offset_x		=	mXOffset
								=	(mTheoraInfo.width - mWidth) / 2;

	mTheoraInfo.offset_y		=	mYOffset
								=	(mTheoraInfo.height - mHeight) / 2;
	
	unsigned long locYBuffSize = mYUV.y_height * mYUV.y_width;
	mYUV.y				=	new char[locYBuffSize];
	mYUV.u				=	new char[locYBuffSize/4];
	mYUV.v				=	new char[locYBuffSize/4];

	//End YV12 specifics
	//

	//debugLog<<"Width =y_w = y_s = "<<mWidth<<" ::: "<<"Height=y_h= "<<mHeight<<endl;
	//debugLog<<"uv_w=uv_s= "<<mYUV.uv_stride<<" ::: " <<"uv_height = "<<mYUV.uv_height<<endl;
	//=mVideoFormat->bmiHeader.biWidth;
	//=mVideoFormat->bmiHeader.biHeight;
	//mTheoraInfo.offset_x=0;
	//mTheoraInfo.offset_y=0;
	

	//HACK:::Bit of a hack to convert dshow nanos to a fps num/denom.
	unsigned long locNum = (((double)10000000) / ((double)mVideoFormat->AvgTimePerFrame)) + (double)0.5;

	//debugLog<<"FPS = "<<locNum<<endl;
	mTheoraInfo.fps_numerator = locNum;
	mTheoraInfo.fps_denominator = 1;
	
	mTheoraInfo.aspect_numerator=0;
	mTheoraInfo.aspect_denominator=0;
	
	mTheoraInfo.colorspace=OC_CS_UNSPECIFIED;		//YV12
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