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


#include "AbstractTransformFilter.h"
#include "AbstractTransformInputPin.h"
#include "AbstractTransformOutputPin.h"

#include "VorbisDecodeInputPin.h"
#include "VorbisDecodeOutputPin.h"
#include "VorbisDecodeFilter.h"


#include "libilliCore/iLE_Math.h"
#include "libOOOgg/OggPacket.h"

#ifndef LOOG_INT64
# ifdef WIN32
#  define LOOG_INT64 signed __int64
# else  /* assume POSIX */
#  define LOOG_INT64 int64_t
# endif
#endif

#ifdef LIBOOOGG_EXPORTS
#define LIBOOOGG_API __declspec(dllexport)
#else
#define LIBOOOGG_API __declspec(dllimport)
#endif

// {05A1D945-A794-44ef-B41A-2F851A117155}
DEFINE_GUID(CLSID_VorbisDecodeFilter, 
0x5a1d945, 0xa794, 0x44ef, 0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55);

//// {8A0566AC-42B3-4ad9-ACA3-93B906DDF98A}
//DEFINE_GUID(MEDIASUBTYPE_Vorbis, 
//0x8a0566ac, 0x42b3, 0x4ad9, 0xac, 0xa3, 0x93, 0xb9, 0x6, 0xdd, 0xf9, 0x8a);
//
//// {44E04F43-58B3-4de1-9BAA-8901F852DAE4}
//DEFINE_GUID(FORMAT_Vorbis, 
//0x44e04f43, 0x58b3, 0x4de1, 0x9b, 0xaa, 0x89, 0x1, 0xf8, 0x52, 0xda, 0xe4);


// {60891713-C24F-4767-B6C9-6CA05B3338FC}
DEFINE_GUID(MEDIATYPE_OggPacketStream, 
0x60891713, 0xc24f, 0x4767, 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc);

// {95388704-162C-42a9-8149-C3577C12AAF9}
DEFINE_GUID(FORMAT_OggIdentHeader, 
0x95388704, 0x162c, 0x42a9, 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9);

// {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
DEFINE_GUID(IID_IOggDecoder, 
0x43f0f818, 0x10b0, 0x4c86, 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62);


// {83D7F506-53ED-4f15-B6D8-7D8E9E72A918}
DEFINE_GUID(IID_IOggOutputPin, 
0x83d7f506, 0x53ed, 0x4f15, 0xb6, 0xd8, 0x7d, 0x8e, 0x9e, 0x72, 0xa9, 0x18);

const REGPINTYPES VorbisDecodeOutputTypes = {
    &MEDIATYPE_Audio,
	&MEDIASUBTYPE_PCM
};

const REGPINTYPES VorbisDecodeInputTypes = {
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
};

const REGFILTERPINS VorbisDecodePinReg[] = {
	{
    L"Vorbis Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&VorbisDecodeInputTypes				//Pointer to media type (Audio/Vorbis or Audio/Speex)
	} ,

	{
	L"PCM Output",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//Only support one media type
	&VorbisDecodeOutputTypes					//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 VorbisDecodeFilterReg = {
		1,
		MERIT_NORMAL,
		2,
        VorbisDecodePinReg
		
};
	   

struct sVorbisFormatBlock {
	unsigned long vorbisVersion;
	unsigned long samplesPerSec;
	unsigned long minBitsPerSec;
	unsigned long avgBitsPerSec;
	unsigned long maxBitsPerSec;
	unsigned char numChannels;
};