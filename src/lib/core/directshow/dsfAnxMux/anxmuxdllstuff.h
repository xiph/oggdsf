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

//#ifdef DSFANXMUX_EXPORTS
//#define DSFANXMUX_API __declspec(dllexport)
//#else
//#define DSFANXMUX_API __declspec(dllimport)
//#endif
#ifdef DSFOGGMUX_EXPORTS
#pragma message("----> Exporting from Ogg Mux...")
#define OGG_MUX_API __declspec(dllexport)
#else
#pragma message("<---- Importing from Ogg Mux...")
#define OGG_MUX_API __declspec(dllimport)
#endif


// {6C5746D3-D309-4988-8694-D435AB86460D}
DEFINE_GUID(CLSID_AnxMuxFilter, 
0x6c5746d3, 0xd309, 0x4988, 0x86, 0x94, 0xd4, 0x35, 0xab, 0x86, 0x46, 0xd);


//// {53696C76-6961-40b2-B136-436F6E726164}
//DEFINE_GUID(FORMAT_CMML, 
//0x53696c76, 0x6961, 0x40b2, 0xb1, 0x36, 0x43, 0x6f, 0x6e, 0x72, 0x61, 0x64);
//
//
//// {5A656E74-6172-6F26-B79C-D6416E647282}
//DEFINE_GUID(MEDIASUBTYPE_CMML, 
//0x5a656e74, 0x6172, 0x6f26, 0xb7, 0x9c, 0xd6, 0x41, 0x6e, 0x64, 0x72, 0x82);

// {BF1121D1-8739-45e1-BCD8-90B828F643AB}
DEFINE_GUID(CLSID_CMMLDecodeFilter, 
0xbf1121d1, 0x8739, 0x45e1, 0xbc, 0xd8, 0x90, 0xb8, 0x28, 0xf6, 0x43, 0xab);


//const REGPINTYPES AnxMuxInputTypes[] = {
//	{	
//		&MEDIATYPE_Audio,
//		&MEDIASUBTYPE_Speex
//	},
//	{
//		&MEDIATYPE_Audio,
//		&MEDIASUBTYPE_Vorbis
//	},
//	{
//		&MEDIATYPE_Audio,
//		&MEDIASUBTYPE_OggFLAC_1_0
//	},
//	{
//		&MEDIATYPE_Video,
//		&MEDIASUBTYPE_Theora
//	},
//	{
//		&MEDIATYPE_Audio,
//		&MEDIASUBTYPE_FLAC
//	}
//};
//const REGFILTERPINS AnxMuxPinReg = {
//	
//    L"Ogg Packet Input",				//Name (obsoleted)
//	TRUE,								//Renders from this pin ?? Not sure about this.
//	FALSE,								//Not an output pin
//	FALSE,								//Cannot have zero instances of this pin
//	FALSE,								//Cannot have more than one instance of this pin
//	NULL,								//Connects to filter (obsoleted)
//	NULL,								//Connects to pin (obsoleted)
//	5,									//upport two media type
//	AnxMuxInputTypes					//Pointer to media type (Audio/Vorbis or Audio/Speex)
//};

const REGFILTER2 AnxMuxFilterReg = {
		1,
		MERIT_DO_NOT_USE,
		1,
        NULL
		
};

//struct sCMMLFormatBlock {
//	__int64 granuleNumerator;
//	__int64 granuleDenominator;
//};