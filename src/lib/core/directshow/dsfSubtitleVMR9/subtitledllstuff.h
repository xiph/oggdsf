//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
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
#include <initguid.h>
// {966D76B4-7D2F-4f01-B840-94E425D2214F}
DEFINE_GUID(MEDIASUBTYPE_SubtitleVMR9, 
0x966d76b4, 0x7d2f, 0x4f01, 0xb8, 0x40, 0x94, 0xe4, 0x25, 0xd2, 0x21, 0x4f);

// {5709E326-01D8-49ad-9D08-F8723112D82E}
DEFINE_GUID(CLSID_SubtitleVMR9Filter, 
0x5709e326, 0x1d8, 0x49ad, 0x9d, 0x8, 0xf8, 0x72, 0x31, 0x12, 0xd8, 0x2e);


const REGPINTYPES SubtitleVMR9OutputTypes = {
    &MEDIATYPE_Video,
	&MEDIASUBTYPE_YV12
};

const REGPINTYPES SubtitleVMR9InputTypes = {
	&MEDIATYPE_Text,
	&MEDIASUBTYPE_SubtitleVMR9
};

const REGFILTERPINS SubtitleVMR9PinReg[] = {
	{
    L"Subtitle Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&SubtitleVMR9InputTypes				//Pointer to media type (Audio/Vorbis or Audio/Speex)
	} ,

	{
	L"Subtitle Output",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//Only support one media type
	&SubtitleVMR9OutputTypes					//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 SubtitleVMR9FilterReg = {
		1,
		MERIT_PREFERRED + 2,
		2,
        SubtitleVMR9PinReg
		
};
	   

struct sVorbisFormatBlock {
	unsigned long vorbisVersion;
	unsigned long samplesPerSec;
	unsigned long minBitsPerSec;
	unsigned long avgBitsPerSec;
	unsigned long maxBitsPerSec;
	unsigned char numChannels;
};