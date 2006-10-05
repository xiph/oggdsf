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

#pragma once

#include "FLAC++/encoder.h"
#include "AbstractTransformInputPin.h"
#include "FLACEncodeInputPin.h"

#include "FLACEncodeFilter.h"
#include "FLACHeaderTweaker.h"


//#include <fstream>
//using namespace std;

using namespace FLAC::Encoder;

class FLACEncodeInputPin
	:	public AbstractTransformInputPin
	,	public Stream
{
public:
	FLACEncodeInputPin(     AbstractTransformFilter* inParentFilter
                        ,   CCritSec* inFilterLock
                        ,   AbstractTransformOutputPin* inOutputPin
                        ,   vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~FLACEncodeInputPin(void);

	//PURE VIRTUALS from Flac Encoder
	virtual ::FLAC__StreamEncoderWriteStatus write_callback(        const FLAC__byte buffer[]
                                                                ,   unsigned bytes
                                                                ,   unsigned samples
                                                                ,   unsigned current_frame);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);

	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	//OVerrides
	virtual STDMETHODIMP EndOfStream(void);
protected:

	//PURE VIRTUALS from Abstract Encoder
	virtual HRESULT TransformData(unsigned char* inBuf, long inNumBytes);
	virtual bool ConstructCodec();
	virtual void DestroyCodec();

	HRESULT mHR;
	WAVEFORMATEX* mWaveFormat;
	bool mTweakedHeaders;
	FLACHeaderTweaker mHeaderTweaker;
	
	bool mBegun;
	
	__int64 mUptoFrame;

};
