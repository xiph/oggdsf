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

#include <string>
using namespace std;
#include "StringHelper.h"

class LIBOOOGG_API OggPacket
{
public:
	//Constructors
	OggPacket(void);
	OggPacket(unsigned char* inPackData, unsigned long inPacketSize, bool inIsTruncated, bool inIsContinuation);
	virtual ~OggPacket(void);
	virtual OggPacket* clone();

	static const unsigned long HEX_DUMP_LINE_LENGTH = 16;
					
	//Packet accessors
	unsigned long packetSize() const;
	unsigned char* packetData();
	//bool isComplete() const;
	bool isTruncated() const;
	bool isContinuation() const;

	//Packet Mutators
	//void setIsComplete (bool inIsComplete );

	void setIsTruncated(bool inIsTruncated);
	void setIsContinuation(bool inIsContinuation);
	void setPacketSize (unsigned long inPacketSize );
	void setPacketData (unsigned char* inPacketData );

	//Merge function
	virtual void merge(const OggPacket* inMorePacket);

	//TODO::: Should this be here ?
	string toPackDumpString();
	
protected:
	//Packet member data
	unsigned long mPacketSize;
	unsigned char* mPacketData;
	//bool mIsComplete;
	bool mIsTruncated;
	bool mIsContinuation;

	//TODO::Should these be here ?
	string OggPacket::dumpNCharsToString(unsigned char* inStartPoint, unsigned long inNumChars) ;
	string OggPacket::padField(string inString, unsigned long inPadWidth, unsigned char inPadChar);

private:
	OggPacket& operator=(const OggPacket& other);  /* Don't assign me */
	OggPacket(const OggPacket& other); /* Don't copy me */
	
};
