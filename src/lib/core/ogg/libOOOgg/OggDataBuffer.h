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
#include <sstream>
#include "OggPage.h"
#include "SerialNoRego.h"
#include "IOggCallback.h"

//Only needed for debugging
#include <fstream>
//
using namespace std;





class LIBOOOGG_API OggDataBuffer 
	
	
{
public:
	enum eState {
		EOS,
		AWAITING_BASE_HEADER = 32,
		AWAITING_SEG_TABLE,
		AWAITING_DATA,
		LOST_PAGE_SYNC = 128
	};

	enum eFeedResult {
		FEED_OK,
		FEED_NULL_POINTER = 64,
		FEED_BUFFER_WRITE_ERROR
	};

	enum eProcessResult {
		PROCESS_OK,
		PROCESS_UNKNOWN_INTERNAL_ERROR = 256,
		PROCESS_STREAM_READ_ERROR,
		PROCESS_DISPATCH_FAILED,
		PROCESS_FAILED_TO_SET_HEADER,
		PROCESS_LOST_SYNC

	};
	OggDataBuffer(void);
	//Debug only
	OggDataBuffer::OggDataBuffer(bool x);
	//
	~OggDataBuffer(void);

	bool registerPageCallback(OggCallbackRego* inPageCB);
	bool registerSerialNo(SerialNoRego* inSerialRego);
	bool registerVirtualCallback(IOggCallback* inCBInterface);
	
	void clearData();
	
	OggDataBuffer::eFeedResult feed(const char* inData, unsigned long inNumBytes);
	
	//FIX ::: Add later
	//void unRegisterSerialNo(unsigned long inSerialNo);

	unsigned long numBytesAvail();

	eState state();

	//Debug only
	void debugWrite(string inString);
	//

protected:
	stringstream mStream;
	eState mState;
	
	eProcessResult processBuffer();
	virtual bool dispatch(OggPage* inOggPage);

	unsigned long mNumBytesNeeded;

	OggPage* pendingPage;

	vector<OggCallbackRego*> mAlwaysCallList;
	vector<SerialNoRego*> mSerialNoCallList;
	vector<IOggCallback*> mVirtualCallbackList;

	//DEBUG
	fstream debugLog;
	//

private:
	eProcessResult processBaseHeader();
	eProcessResult processSegTable();
	eProcessResult processDataSegment();
};
