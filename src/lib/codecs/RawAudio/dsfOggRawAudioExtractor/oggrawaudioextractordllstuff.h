#pragma once


#include <streams.h>
#include <pullpin.h>
#include <initguid.h>

struct sOggRawAudioFormatBlock {
	unsigned long samplesPerSec;
	unsigned long numHeaders;
	unsigned long numChannels;
	unsigned long bitsPerSample;
	unsigned long maxFramesPerPacket;


};





#ifdef LIBOOOGG_EXPORTS
#define LIBOOOGG_API __declspec(dllexport)
#else
#define LIBOOOGG_API __declspec(dllimport)
#endif

#include "libOOOgg/OggPacket.h"

#include "OggRawAudioExtractorInputPin.h"
#include "OggRawAudioExtractorOutputPin.h"

#include "OggRawAudioExtractorFilter.h"

// {B9D15720-7AB4-4f0e-A215-D3C0EA8C0709}
DEFINE_GUID(CLSID_OggRawAudioExtractorFilter, 
0xb9d15720, 0x7ab4, 0x4f0e, 0xa2, 0x15, 0xd3, 0xc0, 0xea, 0x8c, 0x7, 0x9);


// {37535B3C-F068-4f93-9763-E7208277D71F}
DEFINE_GUID(MEDIASUBTYPE_RawOggAudio, 
0x37535b3c, 0xf068, 0x4f93, 0x97, 0x63, 0xe7, 0x20, 0x82, 0x77, 0xd7, 0x1f);

// {232D3C8F-16BF-404b-99AE-296F3DBB77EE}
DEFINE_GUID(FORMAT_RawOggAudio, 
0x232d3c8f, 0x16bf, 0x404b, 0x99, 0xae, 0x29, 0x6f, 0x3d, 0xbb, 0x77, 0xee);


// {60891713-C24F-4767-B6C9-6CA05B3338FC}
DEFINE_GUID(MEDIATYPE_OggPacketStream, 
0x60891713, 0xc24f, 0x4767, 0xb6, 0xc9, 0x6c, 0xa0, 0x5b, 0x33, 0x38, 0xfc);

// {95388704-162C-42a9-8149-C3577C12AAF9}
DEFINE_GUID(FORMAT_OggIdentHeader, 
0x95388704, 0x162c, 0x42a9, 0x81, 0x49, 0xc3, 0x57, 0x7c, 0x12, 0xaa, 0xf9);

// {43F0F818-10B0-4c86-B9F1-F6B6E2D33462}
DEFINE_GUID(IID_IOggDecoder, 
0x43f0f818, 0x10b0, 0x4c86, 0xb9, 0xf1, 0xf6, 0xb6, 0xe2, 0xd3, 0x34, 0x62);


const REGPINTYPES OggRawAudioExtractorOutputTypes = {
    &MEDIATYPE_Audio,
	&MEDIASUBTYPE_PCM
};

const REGPINTYPES OggRawAudioExtractorInputTypes = {
	&MEDIATYPE_OggPacketStream,
	&MEDIASUBTYPE_None
};

const REGFILTERPINS OggRawAudioExtractorPinReg[] = {
	{
    L"Ogg Raw Audio Input",						//Name (obsoleted)
	FALSE,								//Renders from this pin ?? Not sure about this.
	FALSE,								//Not an output pin
	FALSE,								//Cannot have zero instances of this pin
	FALSE,								//Cannot have more than one instance of this pin
	NULL,								//Connects to filter (obsoleted)
	NULL,								//Connects to pin (obsoleted)
	1,									//upport two media type
	&OggRawAudioExtractorInputTypes				//Pointer to media type (Audio/Speex or Audio/Speex)
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
	&OggRawAudioExtractorOutputTypes					//Pointer to media type (Audio/PCM)

	}
};



const REGFILTER2 OggRawAudioExtractorFilterReg = {
		1,
		MERIT_NORMAL,
		2,
        OggRawAudioExtractorPinReg
		
};
	   


