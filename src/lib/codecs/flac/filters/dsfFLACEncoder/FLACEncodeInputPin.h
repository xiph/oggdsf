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

#include "FLAC++/encoder.h"
#include "AbstractAudioEncodeInputPin.h"
#include "FLACEncodeInputPin.h"

#include "FLACEncodeFilter.h"
#include "FLACHeaderTweaker.h"

//extern "C" {
//#include <fishsound/fishsound.h>
////#include <../src/libfishsound/private.h>
//}

//#include <fstream>
//using namespace std;

using namespace FLAC::Encoder;

class FLACEncodeInputPin
	:	public AbstractAudioEncodeInputPin
	,	public Stream
{
public:
	FLACEncodeInputPin(AbstractAudioEncodeFilter* inFilter, CCritSec* inFilterLock, AbstractAudioEncodeOutputPin* inOutputPin);
	virtual ~FLACEncodeInputPin(void);

	//static int FLACEncodeInputPin::FLACEncoded (FishSound* inFishSound, unsigned char* inPacketData, long inNumBytes, void* inThisPointer) ;
	//PURE VIRTUALS from Flac Encoder
	virtual ::FLAC__StreamEncoderWriteStatus write_callback(const FLAC__byte buffer[], unsigned bytes, unsigned samples, unsigned current_frame);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	
	//PURE VIRTUALS from Abstract Encoder
	virtual long encodeData(unsigned char* inBuf, long inNumBytes);
	virtual bool ConstructCodec();
	virtual void DestroyCodec();
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	//OVerrides
	virtual STDMETHODIMP EndOfStream(void);
protected:
	HRESULT mHR;
	
	bool mTweakedHeaders;
	FLACHeaderTweaker mHeaderTweaker;
	//unsigned long mHeadersSeen;
	//bool mBegun;
	//SpeexDecodeOutputPin* mOutputPin;
	//__int64 mUptoFrame;

	//fstream debugLog;
/*	FishSound* mFishSound;
	FishSoundInfo mFishInfo;*/ 

	
};
