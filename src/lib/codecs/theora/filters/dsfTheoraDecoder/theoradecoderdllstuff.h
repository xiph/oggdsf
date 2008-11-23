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

#include <streams.h>
#include <pullpin.h>
#include <initguid.h>
#include <dvdmedia.h>

#ifdef LIBOOOGG_EXPORTS
#define LIBOOOGG_API __declspec(dllexport)
#else
#define LIBOOOGG_API __declspec(dllimport)
#endif

// TheoraDecodeFilter {05187161-5C36-4324-A734-22BF37509F2D}
static const GUID  CLSID_TheoraDecodeFilter =
{ 0x5187161, 0x5c36, 0x4324, { 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d } };

// OggPacketStream {60891713-C24F-4767-B6C9-6CA05B3338FC}
static const GUID MEDIATYPE_OggPacketStream =
{ 0x60891713, 0xc24f, 0x4767, { 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc } };

// OggIdentHeader {95388704-162C-42a9-8149-C3577C12AAF9}
static const GUID FORMAT_OggIdentHeader =
{ 0x95388704, 0x162c, 0x42a9, { 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9 } };

// IOggDecoder {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
static const GUID IID_IOggDecoder =
{ 0x43f0f818, 0x10b0, 0x4c86, { 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62 } };

// IOggOutputPin {83D7F506-53ED-4f15-B6D8-7D8E9E72A918}
static const GUID IID_IOggOutputPin =
{ 0x83d7f506, 0x53ed, 0x4f15, { 0xb6, 0xd8, 0x7d, 0x8e, 0x9e, 0x72, 0xa9, 0x18 } };

#ifndef WINCE

const REGPINTYPES TheoraDecodeOutputTypes[] = 
{
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_YV12
	},
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_YUY2
	}
};

const REGPINTYPES TheoraDecodeInputTypes = 
{
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
};

const REGFILTERPINS TheoraDecodePinReg[] = 
{
	{
    L"Theora Input",					//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&TheoraDecodeInputTypes				//Pointer to media type (Video/Theora)
	} ,

	{
	L"Theora Output",					//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	2,									//Only support one media type
	TheoraDecodeOutputTypes				//Pointer to media types (YV12)
	}
};

const REGFILTER2 TheoraDecodeFilterReg = 
{
		1,
		MERIT_NORMAL,
		2,
        TheoraDecodePinReg		
};
	   
#else

const AMOVIESETUP_MEDIATYPE TheoraDecodeOutputTypes[] = {
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_YV12
	},
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_YUY2
	//},
	//{
	//	&MEDIATYPE_Video,
	//	&MEDIASUBTYPE_RGB565
	//},
	//{
	//	&MEDIATYPE_Video,
	//	&MEDIASUBTYPE_RGB24
		
	}
};

const AMOVIESETUP_MEDIATYPE TheoraDecodeInputTypes = 
{
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
};

const AMOVIESETUP_PIN TheoraDecodePinReg[] = 
{
	{
    L"Theora Input",					//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	&GUID_NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&TheoraDecodeInputTypes				//Pointer to media type (Video/Theora)
	} ,

	{
	L"Theora Output",					//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	&GUID_NULL,							//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	2,									//Supports two types
	TheoraDecodeOutputTypes					//Pointer to media type (YV12)

	}
};

const AMOVIESETUP_FILTER TheoraDecodeFilterReg = 
{
		&CLSID_TheoraDecodeFilter,
		L"Theora Decode Fiter",
		MERIT_NORMAL,
		2,
        TheoraDecodePinReg		
};

#endif

struct sTheoraFormatBlock 
{
	unsigned long theoraVersion;
	unsigned long outerFrameWidth;
	unsigned long outerFrameHeight;
	unsigned long pictureWidth;
	unsigned long pictureHeight;
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
