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

//***************************** Old imp
//#ifdef DSFABSTRACOGGAUDIODECODER_EXPORTS
//#pragma message("----> Exporting from Abstract Library...")
//#define ABS_AUDIO_DEC_API __declspec(dllexport)
//#else
//#pragma message("<---- Importing from Abstract Library...")
//#define ABS_AUDIO_DEC_API __declspec(dllimport)
//#endif
//
//
//
//#include "AbstractAudioDecodeFilter.h"
//#include "AbstractAudioDecodeInputPin.h"
//#include "AbstractAudioDecodeOutputPin.h"
//***************************** Old imp ends


//**************************** New imp

#include "AbstractTransformFilter.h"
#include "AbstractTransformInputPin.h"
#include "AbstractTransformOutputPin.h"
//****************************** New imp ends

#include "VorbisDecodeInputPin.h"
#include "VorbisDecodeOutputPin.h"
#include "VorbisDecodeFilter.h"

#ifdef LIBOOOGG_EXPORTS
#define LIBOOOGG_API __declspec(dllexport)
#else
#define LIBOOOGG_API __declspec(dllimport)
#endif

// {05A1D945-A794-44ef-B41A-2F851A117155}
DEFINE_GUID(CLSID_VorbisDecodeFilter, 
0x5a1d945, 0xa794, 0x44ef, 0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55);

// {8A0566AC-42B3-4ad9-ACA3-93B906DDF98A}
DEFINE_GUID(MEDIASUBTYPE_Vorbis, 
0x8a0566ac, 0x42b3, 0x4ad9, 0xac, 0xa3, 0x93, 0xb9, 0x6, 0xdd, 0xf9, 0x8a);

// {44E04F43-58B3-4de1-9BAA-8901F852DAE4}
DEFINE_GUID(FORMAT_Vorbis, 
0x44e04f43, 0x58b3, 0x4de1, 0x9b, 0xaa, 0x89, 0x1, 0xf8, 0x52, 0xda, 0xe4);


const REGPINTYPES VorbisDecodeOutputTypes = {
    &MEDIATYPE_Audio,
	&MEDIASUBTYPE_PCM
};

const REGPINTYPES VorbisDecodeInputTypes = {
	&MEDIATYPE_Audio,
	&MEDIASUBTYPE_Vorbis
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