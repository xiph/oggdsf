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
#include "oggpacket.h"

class LIBOOOGG_API StampedOggPacket 
	:	public OggPacket
{
public:
	//Constructors
	StampedOggPacket(void);
	StampedOggPacket(unsigned char* inPackData, unsigned long inPacketSize, bool inIsTruncated, bool inIsContinuation, __int64 inStartTime, __int64 inEndTime, unsigned short inStampType);
	virtual ~StampedOggPacket(void);
	virtual OggPacket* clone();

	//Public Constants and Enums
	enum eStampType {
		NONE = 0,
		OGG_END_ONLY = 1,
		OGG_BOTH = 2,
		DIRECTSHOW = 3
	};
	//TODO::: should not be global.
	unsigned short mStampType;

	//Time stamp access
	__int64 startTime();
	__int64 endTime();

	void setStartTime(__int64 inStartTime);
	void setEndTime(__int64 inEndTime);
	void StampedOggPacket::setTimeStamp(__int64 inStartTime, __int64 inEndTime, StampedOggPacket::eStampType inStampType);

	//Merge function
	virtual void merge(StampedOggPacket* inMorePacket);

protected:
	__int64 mStartTime;
	__int64 mEndTime;
};
