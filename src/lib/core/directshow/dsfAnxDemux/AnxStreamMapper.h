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

#include "OggStreamMapper.h"
#include "CMMLStream.h"
#include <libilliCore/iLE_Math.h>

#include <fstream>
using namespace std;
class AnxStreamMapper
	:	public OggStreamMapper
{
public:
	//Constructors
	AnxStreamMapper(void);
	AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter);
	virtual ~AnxStreamMapper(void);

	//Constants and Enumerations
	enum eAnxDemuxState {
		SEEN_NOTHING,
		SEEN_ANNODEX_BOS,
		SEEN_AN_ANXDATA,
		OGG_STATE,
		INVALID_STATE = 1000
	};

	enum eAnxVersions {
		ANX_TREAT_AS_OGG = 0,
		ANX_VERSION_2_0 = 2 << 16,
		ANX_VERSION_3_0 = 3 << 16
	};

	//Public Methods.
	virtual bool acceptOggPage(OggPage* inOggPage);

	virtual bool isReady();
	virtual bool toStartOfData();

protected:
	//Helper Methods
	bool isAnnodexEOS(OggPage* inOggPage);
	bool isAnnodexBOS(OggPage* inOggPage);
	bool isAnxDataPage(OggPage* inOggPage, bool inAnxDataAreBOS);
	unsigned long getAnxVersion(OggPage* inOggPage);
	bool handleAnxVersion_2_0(OggPage* inOggPage);
	bool handleAnxVersion_3_0(OggPage* inOggPage);

	bool isFisheadBOS(OggPage* inOggPage);

	//Member Data
	vector<unsigned long> mSeenStreams;
	bool mSeenAnnodexBOS;
	bool mReadyForCodecs;
	bool mSeenCMML;
	unsigned long mAnnodexSerial;		//TODO::: May not need this anymore
	OggPacket* mAnnodexHeader;
	vector<OggPacket*> mAnxDataHeaders;
	unsigned long mAnxVersion;
	eAnxDemuxState mDemuxState;

	fstream debugLog;
	
};
