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




#include "AbstractVideoEncodeFilter.h"
#include "AbstractVideoEncodeInputPin.h"
#include "AbstractVideoEncodeOutputPin.h"
#include "TheoraEncodeInputPin.h"
#include "TheoraEncodeOutputPin.h"
#include "TheoraEncodeFilter.h"

#ifdef LIBOOOGG_EXPORTS
#define LIBOOOGG_API __declspec(dllexport)
#else
#define LIBOOOGG_API __declspec(dllimport)
#endif

// {D124B2B1-8968-4ae8-B288-FE16EA34B0CE}
DEFINE_GUID(MEDIASUBTYPE_Theora, 
0xd124b2b1, 0x8968, 0x4ae8, 0xb2, 0x88, 0xfe, 0x16, 0xea, 0x34, 0xb0, 0xce);

// {A99F116C-DFFA-412c-95DE-725F99874826}
DEFINE_GUID(FORMAT_Theora, 
0xa99f116c, 0xdffa, 0x412c, 0x95, 0xde, 0x72, 0x5f, 0x99, 0x87, 0x48, 0x26);

const REGPINTYPES TheoraEncodeInputTypes = {
    &MEDIATYPE_Video,
	&MEDIASUBTYPE_YV12
};

const REGPINTYPES VorbisEncodeOutputTypes = {
	&MEDIATYPE_Video,
	&MEDIASUBTYPE_Theora
};

const REGFILTERPINS VorbisEncodePinReg[] = {
	{
    L"YV12 Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&TheoraEncodeInputTypes				//Pointer to media type (Audio/Vorbis or Audio/Speex)
	} ,

	{
	L"Theora Output",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//Only support one media type
	&TheoraEncodeOutputTypes					//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 TheoraEncodeFilterReg = {
		1,
		MERIT_NORMAL,
		2,
        TheoraEncodePinReg
		
};
	   
struct sTheoraFormatBlock {
	unsigned long theoraVersion;
	unsigned long width;
	unsigned long height;
	unsigned long frameWidth;
	unsigned long frameHeight;
	unsigned long frameRateNumerator;
	unsigned long frameRateDenominator;
	unsigned long aspectNumerator;
	unsigned long aspectDenominator;
	unsigned long maxKeyframeInterval;
	unsigned long targetBitrate;
	unsigned char targetQuality;
	unsigned char xOffset;
	unsigned char yOffset;
	unsigned char colourSpace;
};

