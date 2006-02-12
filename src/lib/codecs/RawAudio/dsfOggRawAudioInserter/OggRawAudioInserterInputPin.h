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

#pragma once


#include "AbstractTransformInputPin.h"
#include "OggRawAudioInserterInputPin.h"

#include "OggRawAudioInserterFilter.h"

#include <libilliCore/iBE_Math.h>
#include <libilliCore/iLE_Math.h>



//#include <fstream>
//using namespace std;
class OggRawAudioInserterInputPin
	:	public AbstractTransformInputPin
{
public:
	OggRawAudioInserterInputPin(AbstractTransformFilter* inFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~OggRawAudioInserterInputPin(void);

	
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

protected:

	static const unsigned long WORKING_BUFFER_SIZE = 1 <<20; //1meg buffer
	unsigned long identifyFormat();
	bool makeMainHeader(unsigned char** outBuff, unsigned long* outHeaderSize);
	bool makeCommentHeader(unsigned char** outBuff, unsigned long* outHeaderSize);
	HRESULT sendPacket(unsigned char* inPacketData, unsigned long inNumBytes, bool inIsHeader);


	HRESULT mHR;
//PURE VIRTUALS
	virtual HRESULT TransformData(unsigned char* inBuf, long inNumBytes);
	virtual bool ConstructCodec();
	virtual void DestroyCodec();



	__int64 mUptoFrame;

	enum eRawAudioFormat {
		FMT_S8,
		FMT_U8,
		FMT_S16_LE,
		FMT_S16_BE,
		FMT_S24_LE,
		FMT_S24_BE,
		FMT_S32_LE,
		FMT_S32_BE,

		FMT_ULAW		=	0x10,
		FMT_ALAW,

		FMT_FLT32_LE	=	0x20,
		FMT_FLT32_BE,
		FMT_FLT64_LE,
		FMT_FLT64_BE,

		
	};

	unsigned long mNumBufferedBytes;
	unsigned long mFrameByteWidth;
	unsigned char* mWorkingBuffer;

	unsigned long mFramesPerPacket;
	unsigned long mBytesPerPacket;

	bool mSentHeaders;

	
};
