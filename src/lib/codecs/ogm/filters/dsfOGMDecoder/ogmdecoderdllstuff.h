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
#include "libOOOgg/OggPacket.h"

#include <streams.h>
#include <pullpin.h>
#include <initguid.h>
//#include <streams.h>
//#include <pullpin.h>
//#include <initguid.h>

//#ifdef DSFABSTRACOGGAUDIODECODER_EXPORTS
//#pragma message("----> Exporting from Abstract Library...")
//#define ABS_AUDIO_DEC_API __declspec(dllexport)
//#else
//#pragma message("<---- Importing from Abstract Library...")
//#define ABS_AUDIO_DEC_API __declspec(dllimport)
//#endif

#ifndef OGMDECODER_DLL
	#define LIBOOOGG_API
#else
	#ifdef LIBOOOGG_EXPORTS
		#define LIBOOOGG_API __declspec(dllexport)
	#else
		#define LIBOOOGG_API __declspec(dllimport)
	#endif
#endif

// {368DD7F2-5644-4a88-AC4C-D334B82660B5}
DEFINE_GUID(CLSID_OGMDecodeFilter, 
0x368dd7f2, 0x5644, 0x4a88, 0xac, 0x4c, 0xd3, 0x34, 0xb8, 0x26, 0x60, 0xb5);



// {60891713-C24F-4767-B6C9-6CA05B3338FC}
DEFINE_GUID(MEDIATYPE_OggPacketStream, 
0x60891713, 0xc24f, 0x4767, 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc);

// {95388704-162C-42a9-8149-C3577C12AAF9}
DEFINE_GUID(FORMAT_OggIdentHeader, 
0x95388704, 0x162c, 0x42a9, 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9);

// {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
DEFINE_GUID(IID_IOggDecoder, 
0x43f0f818, 0x10b0, 0x4c86, 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62);
//
//const REGPINTYPES OGMDecodeOutputTypes = {
//    &MEDIATYPE_None,
//	&MEDIASUBTYPE_None
//};

const REGPINTYPES OGMDecodeInputTypes = {
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
};
const REGFILTERPINS OGMDecodePinReg[] = {
	{
    L"OGM Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&OGMDecodeInputTypes				//Pointer to media type (Audio/Vorbis or Audio/Speex)
	} ,

	{
	L"Video Output",					//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	0,									//Only support one media type
	NULL								//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 OGMDecodeFilterReg = {
		1,
		MERIT_NORMAL,
		2,
        OGMDecodePinReg
		
};
	   

//struct sSpeexFormatBlock {
//	unsigned long speexVersion;
//	unsigned long samplesPerSec;
//	unsigned long minBitsPerSec;
//	unsigned long avgBitsPerSec;
//	unsigned long maxBitsPerSec;
//	unsigned long numChannels;
//
//};