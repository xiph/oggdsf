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
#include "stdafx.h"
#include "oggstreamfactory.h"



OggStreamFactory::OggStreamFactory(void)
{
}

OggStreamFactory::~OggStreamFactory(void)
{
}

//New codecs need to be added here and write a derived Stream Class.
OggStream* OggStreamFactory::CreateStream(OggPage* inOggPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek) {
	switch (OggStreamFactory::IdentifyCodec(inOggPage->getPacket(0))) {
		case StreamHeaders::VORBIS:
			return new VorbisStream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::SPEEX:
			return new SpeexStream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::FLAC:
			return new FLACStream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::OGG_FLAC_1_0:
			return new OggFLAC_1_0_Stream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::THEORA:
			return new TheoraStream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::FFDSHOW_VIDEO:
			return new FFDShowVideoStream(inOggPage, inOwningFilter, inAllowSeek);
		case StreamHeaders::NONE:
		default:
			return NULL;
	}
}

StreamHeaders::eCodecType OggStreamFactory::IdentifyCodec(OggPacket* inOggPacket) {
	if (strncmp((char*)inOggPacket->packetData(), "\001vorbis", 7) == 0) {
		return StreamHeaders::VORBIS;
	} else if (strncmp((char*)inOggPacket->packetData(), "Speex   ", 8) == 0) {
		return StreamHeaders::SPEEX;
	} else if ((strncmp((char*)inOggPacket->packetData(), "fLaC", 4)) == 0) {
		return StreamHeaders::FLAC;
	} else if ((strncmp((char*)inOggPacket->packetData(), "\177FLAC", 5)) == 0) {
		return StreamHeaders::OGG_FLAC_1_0;
	} else if ((strncmp((char*)inOggPacket->packetData(), "\200theora", 7)) == 0) {
		return StreamHeaders::THEORA;
	} else if ((strncmp((char*)inOggPacket->packetData(), "\001video\000\000\000", 9)) == 0) {
		return StreamHeaders::FFDSHOW_VIDEO;
	}
	
	return StreamHeaders::NONE;
	
}