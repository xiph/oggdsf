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

#include <libOOOgg/IOggPackSource.h>
#include <libOOOgg/StampedOggPacket.h>
//This holds the preliminary headers for a logical stream... then the output pins of the demux can give them to the codec at their leisure.
//The demux will never present these headers again... seeking to the start of the file will seek to the start
//of the data. ie the first page after the headers.

//More than likely though, you'll have to parse them yourself in the output pin(demuxer) if you want to be able to connect up the codec filter to
//something like the directshow audio renderer... it won't connect unless it knows the sample rate etc... and it
//can't be streamed this data without connecting the filter.

//If the codec needs them again... you have to sort it out from the output pin. The codec should remember !

class OGG_DEMUX_API StreamHeaders
	:	public IOggPackSource
{
public:
	StreamHeaders(void);
	virtual ~StreamHeaders(void);

	enum eCodecType {
		NONE = 0,
		VORBIS = 1,
		SPEEX = 2,
		FLAC = 3,
		THEORA = 4,
		OGG_FLAC_1_0 = 5,
		CMML = 20,
		FFDSHOW_VIDEO = 100
	};
	//IOggPacketSource
	virtual StampedOggPacket* getPacket(unsigned long inPacketNo);
	virtual unsigned long numPackets();
	eCodecType mCodecType;

	//Other
	bool addPacket(StampedOggPacket* inPacket);
protected:
	vector<StampedOggPacket*> mPacketList;
};
