#pragma once

// {4BB64C4A-1674-436b-A49D-D6B3B64DBD60}
DEFINE_GUID(CLSID_PropsAbout, 
0x4bb64c4a, 0x1674, 0x436b, 0xa4, 0x9d, 0xd6, 0xb3, 0xb6, 0x4d, 0xbd, 0x60);

//New section
// {31CA0186-1FF0-4181-AA38-3CA4040BD260}
DEFINE_GUID(CLSID_OggDemuxSourceFilter, 
0x31ca0186, 0x1ff0, 0x4181, 0xaa, 0x38, 0x3c, 0xa4, 0x4, 0xb, 0xd2, 0x60);

// {3913F0AB-E7ED-41c4-979B-1D1FDD983C07}
DEFINE_GUID(MEDIASUBTYPE_FLAC, 
0x3913f0ab, 0xe7ed, 0x41c4, 0x97, 0x9b, 0x1d, 0x1f, 0xdd, 0x98, 0x3c, 0x7);


// {8A0566AC-42B3-4ad9-ACA3-93B906DDF98A}
DEFINE_GUID(MEDIASUBTYPE_Vorbis, 
0x8a0566ac, 0x42b3, 0x4ad9, 0xac, 0xa3, 0x93, 0xb9, 0x6, 0xdd, 0xf9, 0x8a);

// {25A9729D-12F6-420e-BD53-1D631DC217DF}
DEFINE_GUID(MEDIASUBTYPE_Speex, 
0x25a9729d, 0x12f6, 0x420e, 0xbd, 0x53, 0x1d, 0x63, 0x1d, 0xc2, 0x17, 0xdf);



// {44E04F43-58B3-4de1-9BAA-8901F852DAE4}
DEFINE_GUID(FORMAT_Vorbis, 
0x44e04f43, 0x58b3, 0x4de1, 0x9b, 0xaa, 0x89, 0x1, 0xf8, 0x52, 0xda, 0xe4);

// {78701A27-EFB5-4157-9553-38A7854E3E81}
DEFINE_GUID(FORMAT_Speex, 
0x78701a27, 0xefb5, 0x4157, 0x95, 0x53, 0x38, 0xa7, 0x85, 0x4e, 0x3e, 0x81);

// {1CDC48AC-4C24-4b8b-982B-7007A29D83C4}
DEFINE_GUID(FORMAT_FLAC, 
0x1cdc48ac, 0x4c24, 0x4b8b, 0x98, 0x2b, 0x70, 0x7, 0xa2, 0x9d, 0x83, 0xc4);


// {05187161-5C36-4324-A734-22BF37509F2D}
DEFINE_GUID(CLSID_TheoraDecodeFilter, 
0x5187161, 0x5c36, 0x4324, 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d);

// {D124B2B1-8968-4ae8-B288-FE16EA34B0CE}
DEFINE_GUID(MEDIASUBTYPE_Theora, 
0xd124b2b1, 0x8968, 0x4ae8, 0xb2, 0x88, 0xfe, 0x16, 0xea, 0x34, 0xb0, 0xce);

// {A99F116C-DFFA-412c-95DE-725F99874826}
DEFINE_GUID(FORMAT_Theora, 
0xa99f116c, 0xdffa, 0x412c, 0x95, 0xde, 0x72, 0x5f, 0x99, 0x87, 0x48, 0x26);

//This structure defines the type of input we accept on the input pin... Stream/Annodex


//Structure defining the registration details of the filter
const REGFILTER2 OggDemuxSourceFilterReg = {
		1,
		MERIT_NORMAL,
		0,
        NULL
		
};


struct sVorbisFormatBlock {
	unsigned long vorbisVersion;
	unsigned long samplesPerSec;
	unsigned long minBitsPerSec;
	unsigned long avgBitsPerSec;
	unsigned long maxBitsPerSec;
	unsigned char numChannels;
};

struct sSpeexFormatBlock {
	unsigned long speexVersion;
	unsigned long samplesPerSec;
	unsigned long minBitsPerSec;
	unsigned long avgBitsPerSec;
	unsigned long maxBitsPerSec;
	unsigned long numChannels;

};

struct sFLACFormatBlock {
	unsigned short numChannels;
	unsigned long numBitsPerSample;
	unsigned long samplesPerSec;

};

struct sTheoraFormatBlock {
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