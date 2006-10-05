//===========================================================================
//Copyright (C) 2004-2006 Zentaro Kavanagh
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
#include <libOOOgg/dllstuff.h>
#include <libOOOgg/StampedOggPacket.h>
#include <vector>
#include <fstream>
using namespace std;
class FLACHeaderTweaker
{
public:
	FLACHeaderTweaker(void);
	~FLACHeaderTweaker(void);

	enum eFLACAcceptHeaderResult {
		HEADER_ACCEPTED = 0,
		LAST_HEADER_ACCEPTED = 1,
		HEADER_ERROR = 100,
		ALL_HEADERS_ALREADY_SEEN = 101
	};

	 eFLACAcceptHeaderResult acceptHeader(StampedOggPacket* inHeader);

	 unsigned long numNewHeaders();
	 StampedOggPacket* getHeader(unsigned long inHeaderNo);
protected:
	 bool createNewHeaderList();
	 void deleteOldHeaders();
	 void deleteNewHeaders();


	vector<StampedOggPacket*> mOldHeaderList;
	vector<StampedOggPacket*> mNewHeaderList;


	//fstream debugLog;
	bool mSeenAllHeaders;
};
