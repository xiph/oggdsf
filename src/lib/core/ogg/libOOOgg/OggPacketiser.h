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
#include "IOggCallback.h"
#include "IStampedOggPacketSink.h"
#include "OggPage.h"

//Debug only
#include <fstream>
using namespace std;
//

//TODO::: Loose mode controls
class LIBOOOGG_API OggPacketiser
	:	public IOggCallback
{
public:
	//Constants
	enum ePacketiserState {
		PKRSTATE_OK,
		PKRSTATE_AWAITING_CONTINUATION,
		PKRSTATE_INVALID_STREAM
	};

	//Constructors
	OggPacketiser(void);
	OggPacketiser(IStampedOggPacketSink* inPacketSink);
	virtual ~OggPacketiser(void);

	//IOggCallback implementation
	virtual bool acceptOggPage(OggPage* inOggPage);

	//Packet sink control
	void setPacketSink(IStampedOggPacketSink* inPacketSink);
	IStampedOggPacketSink* packetSink();

	//Ignore packets controls.
	void setNumIgnorePackets(unsigned long inNumIgnorePackets);
	unsigned long numIgnorePackets();

	//Packetiser state control
	bool reset();

protected:
    IStampedOggPacketSink* mPacketSink;
	StampedOggPacket* mPendingPacket;

	virtual bool dispatchStampedOggPacket(StampedOggPacket* inPacket);

	bool mLooseMode;
	unsigned long mNumIgnorePackets;
	LOOG_INT64 mPrevGranPos;
	LOOG_INT64 mCurrentGranPos;
	bool processPage(OggPage* inOggPage, bool inIncludeFirst, bool inIncludeLast);
	ePacketiserState mPacketiserState;

	//fstream debugLog;
private:
	OggPacketiser& operator=(const OggPacketiser& other);  /* Don't assign me */
	OggPacketiser(const OggPacketiser& other); /* Don't copy me */
};
