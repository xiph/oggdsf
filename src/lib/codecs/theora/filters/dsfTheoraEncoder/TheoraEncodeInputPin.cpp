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
	mYUV.y = NULL;
	mYUV.u = NULL;
	mYUV.v = NULL;
	
}

TheoraEncodeInputPin::~TheoraEncodeInputPin(void)
{
	DestroyCodec();
	delete mYUV.y;
	delete mYUV.u;
	delete mYUV.v;

}


HRESULT TheoraEncodeInputPin::deliverData(LONGLONG inStart, LONGLONG inEnd, unsigned char* inBuf, unsigned long inNumBytes) {
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
	
	unsigned char* locUptoPtr = inBuf;  //View only... don't delete locUptoPtr
	
	for (long line = 0; line < mYUV.y_height; line++) {
		memcpy((void*)mYUV.y, (const void*)locUptoPtr, mYUV.y_width);
		locUptoPtr += mYUV.y_width;
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

	for (long line = 0; line < mYUV.uv_height; line++) {
		memcpy((void*)mYUV.v, (const void*)locUptoPtr, mYUV.uv_width);
		locUptoPtr += mYUV.uv_width;

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

	for (long line = 0; line < mYUV.uv_height; line++) {
		memcpy((void*)mYUV.u, (const void*)locUptoPtr, mYUV.uv_width);
		locUptoPtr += mYUV.uv_width;
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
	locFrameEnd		= mUptoFrame 
					= locPacket->endTime();

	return deliverData(locFrameStart, locFrameEnd, locPacket->packetData(), locPacket->packetSize());



	
	//float* locFloatBuf = new float[inNumBytes/2];
	//short locTempShort = 0;
	//float locTempFloat = 0;

	////__int64 locGranPos = 0;
	////locGranPos = fish_sound_get_frameno(mFishSound);
	////Removed the hack
	////fish_sound_command(mFishSound, 7, &locGranPos, sizeof(__int64));
	////

	////mUptoFrame = locGranPos;
	////__int64 locTemp = ((FishSoundVorbisInfo*)mFishSound->codec_data)->vd.pcm_returned;
	//for (int i = 0; i < inNumBytes; i += 2) {
	//	locTempShort = *((short*)(inBuf + i));
	//	locTempFloat = (float)locTempShort;
	//	locTempFloat /= 32767.0;
	//	locFloatBuf[i/2] = locTempFloat;;
	//}
	////FIX::: The 2 is the size of a sample ie 16 bits
	//long locErr = fish_sound_encode(mFishSound, (float**)locFloatBuf, inNumBytes/(mFishInfo.channels*2));
	//delete locFloatBuf;
	////FIX::: Do something here ?
	//if (locErr < 0) {
	//
	//} else {
	//
	//}

}
bool TheoraEncodeInputPin::ConstructCodec() {
//	typedef struct {
//  ogg_uint32_t  width;
//  ogg_uint32_t  height;
//  ogg_uint32_t  frame_width;
//  ogg_uint32_t  frame_height;
//  ogg_uint32_t  offset_x;
//  ogg_uint32_t  offset_y;
//  ogg_uint32_t  fps_numerator;
//  ogg_uint32_t  fps_denominator;
//  ogg_uint32_t  aspect_numerator;
//  ogg_uint32_t  aspect_denominator;
//  theora_colorspace colorspace;
//  int           target_bitrate;
//  int           quality;
//  int           quick_p;  /* quick encode/decode */
//
//  /* decode only */
//  unsigned char version_major;
//  unsigned char version_minor;
//  unsigned char version_subminor;
//
//  void *codec_setup;
//
//  /* encode only */
//  int           dropframes_p;
//  int           keyframe_auto_p;
//  ogg_uint32_t  keyframe_frequency;
//  ogg_uint32_t  keyframe_frequency_force;  /* also used for decode init to
//                                              get granpos shift correct */
//  ogg_uint32_t  keyframe_data_target_bitrate;
//  ogg_int32_t   keyframe_auto_threshold;
//  ogg_uint32_t  keyframe_mindistance;
//  ogg_int32_t   noise_sensitivity;
//  ogg_int32_t   sharpness;
//
//} theora_info;

//theora_info_init(&ti);
//  ti.width=video_x;
//  ti.height=video_y;
//  ti.frame_width=frame_x;
//  ti.frame_height=frame_y;
//  ti.offset_x=frame_x_offset;
//  ti.offset_y=frame_y_offset;
//  ti.fps_numerator=video_hzn;
//  ti.fps_denominator=video_hzd;
//  ti.aspect_numerator=video_an;
//  ti.aspect_denominator=video_ad;
//  ti.colorspace=OC_CS_UNSPECIFIED;
//  ti.target_bitrate=video_r;
//  ti.quality=video_q;
//
//  ti.dropframes_p=0;
//  ti.quick_p=1;
//  ti.keyframe_auto_p=1;
//  ti.keyframe_frequency=64;
//  ti.keyframe_frequency_force=64;
//  ti.keyframe_data_target_bitrate=video_r*1.5;
//  ti.keyframe_auto_threshold=80;
//  ti.keyframe_mindistance=8;
//  ti.noise_sensitivity=1; 

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

	mYUV.y				=	new char[(mHeight * mWidth)];
	mYUV.u				=	new char[(mHeight * mWidth)/4];
	mYUV.v				=	new char[(mHeight * mWidth)/4];

	mTheoraInfo.frame_width=mVideoFormat->bmiHeader.biWidth;
	mTheoraInfo.frame_height=mVideoFormat->bmiHeader.biHeight;
	mTheoraInfo.offset_x=0;
	mTheoraInfo.offset_y=0;
	

	//HACK:::Bit of a hack to convert dshow nanos to a fps num/denom.
	unsigned long locNum = (((double)10000000) / ((double)mVideoFormat->AvgTimePerFrame)) + (double)0.5;

	mTheoraInfo.fps_numerator = locNum;
	mTheoraInfo.fps_denominator = 1;
	//I don't think this is right !
	mTheoraInfo.aspect_numerator=mVideoFormat->bmiHeader.biWidth;//video_an;
	mTheoraInfo.aspect_denominator=mVideoFormat->bmiHeader.biHeight;//video_ad;
	//
	mTheoraInfo.colorspace=OC_CS_UNSPECIFIED;
	mTheoraInfo.target_bitrate=mVideoFormat->dwBitRate;
	//Hard code for now
	mTheoraInfo.quality=16; //video_q;

	mTheoraInfo.dropframes_p=0;
	mTheoraInfo.quick_p=1;
	mTheoraInfo.keyframe_auto_p=1;
	mTheoraInfo.keyframe_frequency=64;
	mTheoraInfo.keyframe_frequency_force=64;
	mTheoraInfo.keyframe_data_target_bitrate=mVideoFormat->dwBitRate*1.5;
	mTheoraInfo.keyframe_auto_threshold=80;
	mTheoraInfo.keyframe_mindistance=8;
	mTheoraInfo.noise_sensitivity=1; 








	//mFishInfo.channels = mWaveFormat->nChannels;
	//mFishInfo.format = FISH_SOUND_VORBIS;
	//mFishInfo.samplerate = mWaveFormat->nSamplesPerSec;


	//mFishInfo.format = FISH_SOUND_VORBIS;
	//mFishSound = fish_sound_new (FISH_SOUND_ENCODE, &mFishInfo);

	//int i = 1;
	////FIX::: Use new API for interleave setting
	//fish_sound_command(mFishSound, FISH_SOUND_SET_INTERLEAVE, &i, sizeof(int));

	//fish_sound_set_encoded_callback (mFishSound, VorbisEncodeInputPin::VorbisEncoded, this);
	//FIX::: Proper return value
	return true;

}
void TheoraEncodeInputPin::DestroyCodec() {
	//fish_sound_delete(mFishSound);
	//mFishSound = NULL;
}


//Encoded callback
//int TheoraEncodeInputPin::VorbisEncoded (FishSound* inFishSound, unsigned char* inPacketData, long inNumBytes, void* inThisPointer) 
//{
//
//	VorbisEncodeInputPin* locThis = reinterpret_cast<VorbisEncodeInputPin*> (inThisPointer);
//	VorbisEncodeFilter* locFilter = reinterpret_cast<VorbisEncodeFilter*>(locThis->m_pFilter);
//	
//
//	//Time stamps are granule pos not directshow times
//
//	LONGLONG locFrameStart = locThis->mUptoFrame;
//	LONGLONG locFrameEnd	= locThis->mUptoFrame
//							= fish_sound_get_frameno(locThis->mFishSound);
//	//Get a pointer to a new sample stamped with our time
//	IMediaSample* locSample;
//	HRESULT locHR = locThis->mOutputPin->GetDeliveryBuffer(&locSample, &locFrameStart, &locFrameEnd, NULL);
//
//	if (FAILED(locHR)) {
//		//We get here when the application goes into stop mode usually.
//		return locHR;
//	}	
//	
//	BYTE* locBuffer = NULL;
//
//	
//	//Make our pointers set to point to the samples buffer
//	locSample->GetPointer(&locBuffer);
//
//	
//
//	if (locSample->GetSize() >= inNumBytes) {
//
//		memcpy((void*)locBuffer, (const void*)inPacketData, inNumBytes);
//		
//		//Set the sample parameters.
//		locThis->SetSampleParams(locSample, inNumBytes, &locFrameStart, &locFrameEnd);
//
//		{
//			CAutoLock locLock(locThis->m_pLock);
//
//			//Add a reference so it isn't deleted en route.
//			//locSample->AddRef();
//			HRESULT locHR = locThis->mOutputPin->mDataQueue->Receive(locSample);						//->DownstreamFilter()->Receive(locSample);
//			if (locHR != S_OK) {
//				
//			} else {
//			}
//		}
//
//		return 0;
//	} else {
//		throw 0;
//	}
//}


HRESULT TheoraEncodeInputPin::SetMediaType(const CMediaType* inMediaType) {
	AbstractVideoEncodeInputPin::SetMediaType(inMediaType);

	ConstructCodec();


	return S_OK;
	
}