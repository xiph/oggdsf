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

#include "AbstractTransformInputPin.h"
#include "SpeexEncodeInputPin.h"
#include "SpeexEncodeFilter.h"
#include "SpeexEncoder.h"
#include "SpeexEncodeSettings.h"

class SpeexEncodeInputPin
	:	public AbstractTransformInputPin
{
public:
    friend class SpeexEncodeFilter;

	SpeexEncodeInputPin(        AbstractTransformFilter* inFilter
                            ,   CCritSec* inFilterLock
                            ,   AbstractTransformOutputPin* inOutputPin
                            ,   vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~SpeexEncodeInputPin(void);

	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
    virtual STDMETHODIMP EndOfStream();
    virtual HRESULT CompleteConnect (IPin *inReceivePin);

protected:
	virtual HRESULT TransformData(unsigned char* inBuf, long inNumBytes);
	virtual bool ConstructCodec();
	virtual void DestroyCodec();

    void deletePacketsAndEmptyVector(vector<StampedOggPacket*>& inPackets);
    HRESULT sendPackets(const vector<StampedOggPacket*>& inPackets);
    unsigned long bufferBytesToSampleCount(long inByteCount);


	WAVEFORMATEX* mWaveFormat;
	__int64 mUptoFrame;
    bool mBegun;

    SpeexEncoder mSpeexEncoder;
    SpeexEncodeSettings mEncoderSettings;


};
