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

#include "oggdllstuff.h"
#include "ds_guids.h"
#include <libOOOgg/OggPacket.h>
#include <libOOOgg/OggPage.h>
#include "OggStream.h"
class OggStream;
class OggDemuxSourceFilter;

class VorbisStream
	:	public OggStream
{
public:
	VorbisStream(OggPage* inOggPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek);
	virtual ~VorbisStream(void);

	virtual bool InitCodec(StampedOggPacket* inOggPacket);
	void setLastEndGranPos(__int64 inPos);

	virtual BYTE* getFormatBlock() ;
	virtual unsigned long getFormatBlockSize();
	virtual GUID getFormatGUID();
	virtual GUID getSubtypeGUID();
	virtual wstring getPinName();
	virtual bool createFormatBlock();
	virtual GUID getMajorTypeGUID();
	virtual LONGLONG getCurrentPos();

	virtual unsigned long getNumBuffers();
	virtual unsigned long getBufferSize();


protected:
	static const unsigned long VORBIS_NUM_BUFFERS = 75;
	static const unsigned long VORBIS_BUFFER_SIZE = 65536;		//Should be plenty... wouldn't hurt to find out !!

	sVorbisFormatBlock* mVorbisFormatBlock;
};
