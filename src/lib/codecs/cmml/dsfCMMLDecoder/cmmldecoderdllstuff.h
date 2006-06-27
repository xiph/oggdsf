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


//// {53696C76-6961-40b2-B136-436F6E726164}
//DEFINE_GUID(FORMAT_CMML, 
//0x53696c76, 0x6961, 0x40b2, 0xb1, 0x36, 0x43, 0x6f, 0x6e, 0x72, 0x61, 0x64);
//
//
//// {5A656E74-6172-6F26-B79C-D6416E647282}
//DEFINE_GUID(MEDIASUBTYPE_CMML, 
//0x5a656e74, 0x6172, 0x6f26, 0xb7, 0x9c, 0xd6, 0x41, 0x6e, 0x64, 0x72, 0x82);
//


// {966D76B4-7D2F-4f01-B840-94E425D2214F}
//DEFINE_GUID(MEDIASUBTYPE_SubtitleVMR9, 
//0x966d76b4, 0x7d2f, 0x4f01, 0xb8, 0x40, 0x94, 0xe4, 0x25, 0xd2, 0x21, 0x4f);
static const GUID MEDIASUBTYPE_SubtitleVMR9 =
{ 0x966d76b4, 0x7d2f, 0x4f01, { 0xb8, 0x40, 0x94, 0xe4, 0x25, 0xd2, 0x21, 0x4f} };

// {BF1121D1-8739-45e1-BCD8-90B828F643AB}
static const GUID CLSID_CMMLDecodeFilter =
{ 0xbf1121d1, 0x8739, 0x45e1, {0xbc, 0xd8, 0x90, 0xb8, 0x28, 0xf6, 0x43, 0xab} };


// {60891713-C24F-4767-B6C9-6CA05B3338FC}
//DEFINE_GUID(MEDIATYPE_OggPacketStream, 
//0x60891713, 0xc24f, 0x4767, 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc);
static const GUID MEDIATYPE_OggPacketStream =
{ 0x60891713, 0xc24f, 0x4767, { 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc } };

// {95388704-162C-42a9-8149-C3577C12AAF9}
//DEFINE_GUID(FORMAT_OggIdentHeader, 
//0x95388704, 0x162c, 0x42a9, 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9);
static const GUID FORMAT_OggIdentHeader =
{ 0x95388704, 0x162c, 0x42a9, { 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9 } };

// {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
//DEFINE_GUID(IID_IOggDecoder, 
//0x43f0f818, 0x10b0, 0x4c86, 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62);
static const GUID IID_IOggDecoder =
{ 0x43f0f818, 0x10b0, 0x4c86, { 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62 } };


// {83D7F506-53ED-4f15-B6D8-7D8E9E72A918}
//DEFINE_GUID(IID_IOggOutputPin, 
//0x83d7f506, 0x53ed, 0x4f15, 0xb6, 0xd8, 0x7d, 0x8e, 0x9e, 0x72, 0xa9, 0x18);
static const GUID IID_IOggOutputPin =
{ 0x83d7f506, 0x53ed, 0x4f15, { 0xb6, 0xd8, 0x7d, 0x8e, 0x9e, 0x72, 0xa9, 0x18 } };



//// {60891713-C24F-4767-B6C9-6CA05B3338FC}
//DEFINE_GUID(MEDIATYPE_OggPacketStream, 
//0x60891713, 0xc24f, 0x4767, 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc);
//
//// {95388704-162C-42a9-8149-C3577C12AAF9}
//DEFINE_GUID(FORMAT_OggIdentHeader, 
//0x95388704, 0x162c, 0x42a9, 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9);
//
//// {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
//DEFINE_GUID(IID_IOggDecoder, 
//0x43f0f818, 0x10b0, 0x4c86, 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62);

//This structure defines the type of input we accept on the input pin... Stream/Annodex

const REGPINTYPES CMMLDecodeOutputTypes = {
    &MEDIATYPE_Text,
	&MEDIASUBTYPE_SubtitleVMR9
};

const REGPINTYPES CMMLDecodeInputTypes = {
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
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


#ifndef WINCE
const REGFILTER2 CMMLDecodeFilterReg = {
		1,
		MERIT_PREFERRED + 2,
		2,
        CMMLDecodePinReg
		
};
#else
const AMOVIESETUP_FILTER CMMLDecodeFilterReg = {
		&CLSID_CMMLDecodeFilter,
		L"CMML Decode Filter",
		MERIT_PREFERRED + 2,
		2,
        CMMLDecodePinReg
		
};
#endif

struct sCMMLFormatBlock {
	__int64 granuleNumerator;
	__int64 granuleDenominator;
	unsigned short granuleSplitBits;
};
