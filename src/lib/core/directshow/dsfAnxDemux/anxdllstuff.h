#pragma once

#include "oggdllstuff.h"


// {6F767551-E3E1-461f-A8E5-C8ED36342ED1}
DEFINE_GUID(CLSID_AnxDemuxSourceFilter, 
0x6f767551, 0xe3e1, 0x461f, 0xa8, 0xe5, 0xc8, 0xed, 0x36, 0x34, 0x2e, 0xd1);

const REGFILTER2 AnxDemuxSourceFilterReg = {
		1,
		MERIT_NORMAL,
		0,
        NULL
		
};