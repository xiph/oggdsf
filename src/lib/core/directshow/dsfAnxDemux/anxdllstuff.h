#pragma once

#include "oggdllstuff.h"


// {6F767551-E3E1-461f-A8E5-C8ED36342ED1}
DEFINE_GUID(CLSID_AnxDemuxSourceFilter, 
0x6f767551, 0xe3e1, 0x461f, 0xa8, 0xe5, 0xc8, 0xed, 0x36, 0x34, 0x2e, 0xd1);

// {53696C76-6961-40b2-B136-436F6E726164}
DEFINE_GUID(FORMAT_CMML, 
0x53696c76, 0x6961, 0x40b2, 0xb1, 0x36, 0x43, 0x6f, 0x6e, 0x72, 0x61, 0x64);


// {5A656E74-6172-6F26-B79C-D6416E647282}
DEFINE_GUID(MEDIASUBTYPE_CMML, 
0x5a656e74, 0x6172, 0x6f26, 0xb7, 0x9c, 0xd6, 0x41, 0x6e, 0x64, 0x72, 0x82);

const REGFILTER2 AnxDemuxSourceFilterReg = {
		1,
		MERIT_NORMAL,
		0,
        NULL
		
};

struct sCMMLFormatBlock {
	__int64 granuleNumerator;
	__int64 granuleDenominator;
};