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

class LIBOOOGG_API OggPacket
{
public:
	OggPacket(void);
	OggPacket(unsigned char* inPackData, unsigned long inPacketSize, bool inIsComplete);
	virtual ~OggPacket(void);

	static const unsigned long HEX_DUMP_LINE_LENGTH = 16;

	static const unsigned char A_BASE = 65;			//A in ASCII
	static const unsigned char ZERO_BASE = 48;
					
	virtual OggPacket* clone();
	unsigned long packetSize();
	unsigned char* packetData();
	bool isComplete();

	void setIsComplete (bool inIsComplete );
	void setPacketSize (unsigned long inPacketSize );
	void setPacketData (unsigned char* inPacketData );

	virtual void merge(OggPacket* inMorePacket);
	string toPackDumpString();
	
protected:
	unsigned long mPacketSize;
	unsigned char* mPacketData;
	bool mIsComplete;

	unsigned char OggPacket::digitToHex(unsigned char inChar);
	string OggPacket::charToHexString(unsigned char inChar);

	string OggPacket::dumpNCharsToString(unsigned char* inStartPoint, unsigned long inNumChars) ;
	string OggPacket::padField(string inString, unsigned long inPadWidth, unsigned char inPadChar);
	
};