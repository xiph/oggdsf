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

//STL Includes
#include <vector>
using namespace std;

//Class Includes
#include <libOOOgg/StampedOggPacket.h>
#include <libOOOgg/OggPageHeader.h>
#include <libOOOgg/IOggPackSource.h>

//OggPage represents an Ogg Encapsulation Format page
class LIBOOOGG_API OggPage
	: public IOggPackSource
{
public:
	//Constants
	static const unsigned long HEX_DUMP_LINE_LENGTH = 16;
	
	//Constructors
	OggPage(void);
	virtual ~OggPage(void);

	//Cloning
	OggPage* clone();
	
	//Size functions
	unsigned long pageSize();
	unsigned long headerSize();
	unsigned long dataSize();

	//IOggPackSource Implementation
	virtual OggPacket* getPacket(unsigned long inPacketNo);
	virtual unsigned long numPackets();

	//Packet access
	StampedOggPacket* getStampedPacket(unsigned long inPacketNo);
	bool addPacket(StampedOggPacket* inPacket);
	
	//Header access
	OggPageHeader* header();
	
	//Serialise
	unsigned char* createRawPageData();

protected:
	//Member data of packets and header.
	vector<StampedOggPacket*> mPacketList;
	OggPageHeader* mHeader;

private:
	OggPage& operator=(const OggPage& other);  /* Don't assign me */
	OggPage(const OggPage& other); /* Don't copy me */
};
