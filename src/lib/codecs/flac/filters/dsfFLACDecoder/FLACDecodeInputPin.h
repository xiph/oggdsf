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
#include "FLACdecoderdllstuff.h"
#include "FLAC++/decoder.h"
#include "OggPacket.h"
#include <queue>

//debug only
#include <fstream>
//

using namespace std;
using namespace FLAC::Decoder;

class FLACDecodeInputPin
	:	public AbstractAudioDecodeInputPin
	,	public Stream
{
public:

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	FLACDecodeInputPin(AbstractAudioDecodeFilter* inFilter, CCritSec* inFilterLock, AbstractAudioDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType);
	virtual ~FLACDecodeInputPin(void);


	HRESULT SetMediaType(const CMediaType* inMediaType);

	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], unsigned *bytes);
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	//VIRTUAL FUNCTIONS - AbstractAudioDecodeInputPin
	//FIX:::These should be protected.
	virtual bool ConstructCodec();
	virtual void DestroyCodec();
	//virtual STDMETHODIMP EndFlush();
	virtual STDMETHODIMP BeginFlush();
	virtual STDMETHODIMP EndOfStream(void);

	long decodeData(unsigned char* inBuf, long inNumBytes);

protected:
	bool mGotMetaData;
	
	//debug only
	//fstream debugLog;
	//

	CCritSec* mCodecLock;
	queue<OggPacket*> mPendingPackets;
	unsigned long mNumPacksBuffered;
};
