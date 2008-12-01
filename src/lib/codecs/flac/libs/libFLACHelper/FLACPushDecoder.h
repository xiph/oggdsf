//===========================================================================
//Copyright (C) 2004-2006 Zentaro Kavanagh
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
#include <libOOOgg/dllstuff.h>
#include <libOOOgg/StampedOggPacket.h>
#include <libOOOgg/OggPacket.h>
#include "FLAC++/decoder.h"

//Flac Stream Format Documentation http://flac.sourceforge.net/format.html#stream

using namespace FLAC::Decoder;
class FLACPushDecoder
	:	protected Stream
{
public:
	FLACPushDecoder(void);
	virtual ~FLACPushDecoder(void);

	StampedOggPacket* decodeFLAC(OggPacket* inPacket);
	bool acceptMetadata(OggPacket* inPacket);

	void initCodec();
	void flushCodec();

	unsigned long numberOfChannels()		{	return mNumChannels;	}
	unsigned long frameSize()				{	return mFrameSize;		}
	unsigned long sampleRate()				{	return mSampleRate;		}
private:
	static const int SIZE_16_BITS = 2;

	unsigned long mNumChannels;
	unsigned long mFrameSize;
	unsigned long mSampleRate;

	//Virtuals frmo FLAC decoder
	virtual ::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], size_t *bytes);
	virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);

	OggPacket* mInPacket;
	StampedOggPacket* mOutPacket;
	
	bool mGotMetaData;
	bool mBegun;
	
};
