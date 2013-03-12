//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//          (C) 2013 Cristian Adam
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
#include <streams.h>
#include <pullpin.h>
#include <initguid.h>

#include "common/OggTypes.h"
#include "common/VorbisTypes.h"
#include "common/TheoraTypes.h"
#include "common/FlacTypes.h"
#include "common/SpeexTypes.h"

// {BBCD12AC-0000-0010-8000-00aa00389b71}
DEFINE_GUID(MEDIASUBTYPE_Schroedinger, 
0xBBCD12AC, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// {BBCD12AC-c356-11ce-bf01-00aa0055595a}
DEFINE_GUID(FORMAT_Schroedinger,
0xBBCD12AC, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a);

// {37535B3C-F068-4f93-9763-E7208277D71F}
DEFINE_GUID(MEDIASUBTYPE_RawOggAudio, 
0x37535b3c, 0xf068, 0x4f93, 0x97, 0x63, 0xe7, 0x20, 0x82, 0x77, 0xd7, 0x1f);

// {232D3C8F-16BF-404b-99AE-296F3DBB77EE}
DEFINE_GUID(FORMAT_RawOggAudio, 
0x232d3c8f, 0x16bf, 0x404b, 0x99, 0xae, 0x29, 0x6f, 0x3d, 0xbb, 0x77, 0xee);
const REGPINTYPES OggMuxInputTypes[] = {
	{	
		&MEDIATYPE_Audio,
		&MEDIASUBTYPE_Speex
	},
	{
		&MEDIATYPE_Audio,
		&MEDIASUBTYPE_Vorbis
	},
	{
		&MEDIATYPE_Audio,
		&MEDIASUBTYPE_OggFLAC_1_0
	},
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_Theora
	},
	{
		&MEDIATYPE_Audio,
		&MEDIASUBTYPE_FLAC
	},
	{
		&MEDIATYPE_Audio,
		&MEDIASUBTYPE_RawOggAudio
	}
};
const REGFILTERPINS OggMuxPinReg = {
	
    L"Ogg Packet Input",				//Name (obsoleted)
	TRUE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	6,									//Support two media type
	OggMuxInputTypes					//Pointer to media type (Audio/Vorbis or Audio/Speex)
};

const REGFILTER2 OggMuxFilterReg = 
{
	1,
	MERIT_DO_NOT_USE,
	1,
    &OggMuxPinReg		
};

struct OGGRAWAUDIOFORMAT 
{
	unsigned long samplesPerSec;
	unsigned long numHeaders;
	unsigned long numChannels;
	unsigned long bitsPerSample;
	unsigned long maxFramesPerPacket;
};
