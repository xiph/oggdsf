//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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

#include "CMMLDecoderIIDs.h"
// {966D76B4-7D2F-4f01-B840-94E425D2214F}
DEFINE_GUID(MEDIASUBTYPE_SubtitleVMR9, 
0x966d76b4, 0x7d2f, 0x4f01, 0xb8, 0x40, 0x94, 0xe4, 0x25, 0xd2, 0x21, 0x4f);

// {53696C76-6961-40b2-B136-436F6E726164}
DEFINE_GUID(FORMAT_CMML, 
0x53696c76, 0x6961, 0x40b2, 0xb1, 0x36, 0x43, 0x6f, 0x6e, 0x72, 0x61, 0x64);


// {5A656E74-6172-6F26-B79C-D6416E647282}
DEFINE_GUID(MEDIASUBTYPE_CMML, 
0x5a656e74, 0x6172, 0x6f26, 0xb7, 0x9c, 0xd6, 0x41, 0x6e, 0x64, 0x72, 0x82);

// {BF1121D1-8739-45e1-BCD8-90B828F643AB}
DEFINE_GUID(CLSID_CMMLDecodeFilter, 
0xbf1121d1, 0x8739, 0x45e1, 0xbc, 0xd8, 0x90, 0xb8, 0x28, 0xf6, 0x43, 0xab);

//This structure defines the type of input we accept on the input pin... Stream/Annodex

const REGPINTYPES CMMLDecodeOutputTypes = {
    &MEDIATYPE_Text,
	&MEDIASUBTYPE_SubtitleVMR9
};

const REGPINTYPES CMMLDecodeInputTypes = {
	&MEDIATYPE_Text,
	&MEDIASUBTYPE_CMML
};
const REGFILTERPINS CMMLDecodePinReg[] = {
	{
    L"CMML Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&CMMLDecodeInputTypes				//Pointer to media type (Audio/Vorbis or Audio/Speex)
	} ,

	{
	L"Subtitle VMR9 Output",			//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//Only support one media type
	&CMMLDecodeOutputTypes				//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 CMMLDecodeFilterReg = {
		1,
		MERIT_PREFERRED + 2,
		2,
        CMMLDecodePinReg
		
};
	   

struct sCMMLFormatBlock {
	__int64 granuleNumerator;
	__int64 granuleDenominator;
};
