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



#include <streams.h>
#include <pullpin.h>
#include <initguid.h>

#ifdef DSFDIRACENCODEFILTER_EXPORTS
#define DSFDIRACENCODEFILTER_API __declspec(dllexport)
#else
#define DSFDIRACENCODEFILTER_API __declspec(dllimport)
#endif



 
struct sDiracFormatBlock {
	int chromaFormat;
	int width;
	int height;
	int frameRate;
	bool isInterlaced;
	bool isTopFieldFirst;

};

// {855236D7-01D6-432e-9563-4D0A322B2FFB}
DEFINE_GUID(CLSID_DiracEncodeFilter, 
0x855236d7, 0x1d6, 0x432e, 0x95, 0x63, 0x4d, 0xa, 0x32, 0x2b, 0x2f, 0xfb);

// {15B9B7CA-A7C0-448c-A68D-4D45A4137E38}
DEFINE_GUID(MEDIASUBTYPE_Dirac, 
0x15b9b7ca, 0xa7c0, 0x448c, 0xa6, 0x8d, 0x4d, 0x45, 0xa4, 0x13, 0x7e, 0x38);

// {4D8CEB62-0F52-44d0-B1EB-4AD6B24EA62C}
DEFINE_GUID(FORMAT_Dirac, 
0x4d8ceb62, 0xf52, 0x44d0, 0xb1, 0xeb, 0x4a, 0xd6, 0xb2, 0x4e, 0xa6, 0x2c);


const REGPINTYPES DiracEncodeInputTypes = {
    &MEDIATYPE_Video,
	&MEDIASUBTYPE_YV12
};

const REGPINTYPES DiracEncodeOutputTypes = {
	&MEDIATYPE_Video,
	&MEDIASUBTYPE_Dirac
};

const REGFILTERPINS DiracEncodePinReg[] = {
	{
    L"YV12 Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&DiracEncodeInputTypes				//Pointer to media type (Video/VY12)
	} ,

	{
	L"Dirac Output",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	TRUE,								//Is an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//Only support one media type
	&DiracEncodeOutputTypes					//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 DiracEncodeFilterReg = {
		1,
		MERIT_NORMAL,
		2,
        DiracEncodePinReg
		
};





//
//#ifdef LIBOOOGG_EXPORTS
//#define LIBOOOGG_API __declspec(dllexport)
//#else
//#define LIBOOOGG_API __declspec(dllimport)
//#endif
//
//// {121EA765-6D3F-4519-9686-A0BA6E5281A2}
//DEFINE_GUID(CLSID_PropsTheoraEncoder, 
//0x121ea765, 0x6d3f, 0x4519, 0x96, 0x86, 0xa0, 0xba, 0x6e, 0x52, 0x81, 0xa2);
//
//// {4F063B3A-B397-4c22-AFF4-2F8DB96D292A}
//DEFINE_GUID(IID_ITheoraEncodeSettings, 
//0x4f063b3a, 0xb397, 0x4c22, 0xaf, 0xf4, 0x2f, 0x8d, 0xb9, 0x6d, 0x29, 0x2a);
//
//// {5C769985-C3E1-4f95-BEE7-1101C465F5FC}
//DEFINE_GUID(CLSID_TheoraEncodeFilter, 
//0x5c769985, 0xc3e1, 0x4f95, 0xbe, 0xe7, 0x11, 0x1, 0xc4, 0x65, 0xf5, 0xfc);
//
//// {D124B2B1-8968-4ae8-B288-FE16EA34B0CE}
//DEFINE_GUID(MEDIASUBTYPE_Theora, 
//0xd124b2b1, 0x8968, 0x4ae8, 0xb2, 0x88, 0xfe, 0x16, 0xea, 0x34, 0xb0, 0xce);
//
//// {A99F116C-DFFA-412c-95DE-725F99874826}
//DEFINE_GUID(FORMAT_Theora, 
//0xa99f116c, 0xdffa, 0x412c, 0x95, 0xde, 0x72, 0x5f, 0x99, 0x87, 0x48, 0x26);
//
//const REGPINTYPES TheoraEncodeInputTypes = {
//    &MEDIATYPE_Video,
//	&MEDIASUBTYPE_YV12
//};
//
//const REGPINTYPES TheoraEncodeOutputTypes = {
//	&MEDIATYPE_Video,
//	&MEDIASUBTYPE_Theora
//};
//
//const REGFILTERPINS TheoraEncodePinReg[] = {
//	{
//    L"YV12 Input",						//Name (obsoleted)
//	FALSE,								//Renders from this pin ?? Not sure about this.
//	FALSE,								//Not an output pin
//	FALSE,								//Cannot have zero instances of this pin
//	FALSE,								//Cannot have more than one instance of this pin
//	NULL,								//Connects to filter (obsoleted)
//	NULL,								//Connects to pin (obsoleted)
//	1,									//upport two media type
//	&TheoraEncodeInputTypes				//Pointer to media type (Video/VY12)
//	} ,
//
//	{
//	L"Theora Output",						//Name (obsoleted)
//	FALSE,								//Renders from this pin ?? Not sure about this.
//	TRUE,								//Is an output pin
//	FALSE,								//Cannot have zero instances of this pin
//	FALSE,								//Cannot have more than one instance of this pin
//	NULL,								//Connects to filter (obsoleted)
//	NULL,								//Connects to pin (obsoleted)
//	1,									//Only support one media type
//	&TheoraEncodeOutputTypes					//Pointer to media type (Audio/PCM)
//
//	}
//};
//
//
//
//const REGFILTER2 TheoraEncodeFilterReg = {
//		1,
//		MERIT_NORMAL,
//		2,
//        TheoraEncodePinReg
//		
//};
//	   
//struct sTheoraFormatBlock {
//	unsigned long theoraVersion;
//	unsigned long width;
//	unsigned long height;
//	unsigned long frameWidth;
//	unsigned long frameHeight;
//	unsigned long frameRateNumerator;
//	unsigned long frameRateDenominator;
//	unsigned long aspectNumerator;
//	unsigned long aspectDenominator;
//	unsigned long maxKeyframeInterval;
//	unsigned long targetBitrate;
//	unsigned char targetQuality;
//	unsigned char xOffset;
//	unsigned char yOffset;
//	unsigned char colourSpace;
//};
//
