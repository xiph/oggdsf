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

#include "stdafx.h"
#include ".\flacheadertweaker.h"

FLACHeaderTweaker::FLACHeaderTweaker(void)
	:	mSeenAllHeaders(false)

{
	//debugLog.open("G:\\logs\\flactweaker.log", ios_base::out);

}

FLACHeaderTweaker::~FLACHeaderTweaker(void)
{
	
	//debugLog<<"Pre delete old..."<<endl;
	deleteOldHeaders();
	//debugLog<<"Pre delete new..."<<endl;
	deleteNewHeaders();
	//debugLog<<"Post delete..."<<endl;
	//debugLog.close();
}

FLACHeaderTweaker::eFLACAcceptHeaderResult FLACHeaderTweaker::acceptHeader(StampedOggPacket* inHeader) 
{
	const unsigned char MORE_HEADERS_MASK = 128;
	if (!mSeenAllHeaders) {
		mOldHeaderList.push_back(inHeader);
		if ((inHeader->packetData()[0] & MORE_HEADERS_MASK)  != 0) {
			//Last header
			mSeenAllHeaders = true;
			if (createNewHeaderList()) {
				return LAST_HEADER_ACCEPTED;
			} else {
				return HEADER_ERROR;
			}
		} else {
			//Still more headers to come...
			return HEADER_ACCEPTED;
		}
	} else {
		return ALL_HEADERS_ALREADY_SEEN;
	}
}

bool FLACHeaderTweaker::createNewHeaderList() 
{
	
	unsigned char* locFirstPacketBuffur = new unsigned char[51];
	locFirstPacketBuffur[0] = '\177';
	locFirstPacketBuffur[1] = 'F';
	locFirstPacketBuffur[2] = 'L';
	locFirstPacketBuffur[3] = 'A';
	locFirstPacketBuffur[4] = 'C';
	locFirstPacketBuffur[5] = 1;
	locFirstPacketBuffur[6] = 0;
	locFirstPacketBuffur[7] = 0; //Num header HIGH BYTE

	//*** VERIFY ::: Is this even safe ie -2 ... are we sure this can't go negative ????
	locFirstPacketBuffur[8] = mOldHeaderList.size() - 2; //Num headers LOW BYTE
	locFirstPacketBuffur[9] = 'f';
	locFirstPacketBuffur[10] = 'L';
	locFirstPacketBuffur[11] = 'a';
	locFirstPacketBuffur[12] = 'C';

	memcpy((void*)(locFirstPacketBuffur + 13), (const void*) mOldHeaderList[1]->packetData(), 38);

	mNewHeaderList.empty();
	mNewHeaderList.clear();
    mNewHeaderList.push_back(new StampedOggPacket(locFirstPacketBuffur, 51, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY));
	locFirstPacketBuffur = NULL;

	bool locFoundComment = false;
	int locCommentNo = -1;

	//Start at 2, 0 is just fLaC, 1 is the stream info
	for (size_t i = 2; i < mOldHeaderList.size(); i++) {
		//Loop through to find the comment packet...
		if ( ((mOldHeaderList[i]->packetData()[0]) & 127) == 4) {
			//It's the comment packet.
			locFoundComment = true;
			locCommentNo = (int)i;
			mNewHeaderList.push_back(mOldHeaderList[i]->cloneStamped());
		}
	}

	if (locFoundComment != true) {
		//debugLog<<"No comments present... FATALITY !"<<endl;
		//Maybe make one... for now bail out !
		throw 0;
	}

	for (size_t i = 2; i < mOldHeaderList.size(); i++) {
	
		//**** WARNING ::: Leave this unless you check it !
		if (i != locCommentNo) {
			//If it's not the comment packet we already added, put it in the list.
			mNewHeaderList.push_back(mOldHeaderList[i]->cloneStamped());
		}
	}

	for (size_t i = 1; i < mNewHeaderList.size(); i++) {
		//Loop through the new headers and make sure the flags are set right.
		if (i != mNewHeaderList.size() -1) {
			//Clear the first bit
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] & 127;
		} else {
			//debugLog<<"Setting header bit "<<i<<endl;
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] | 128;
		}
	}

	deleteOldHeaders();

	return true;

}

void FLACHeaderTweaker::deleteOldHeaders() 
{
	size_t locSize = mOldHeaderList.size();
	for (size_t i = 0; i < locSize; i++) {
		delete mOldHeaderList[i];		
	}
	mOldHeaderList.clear();
}

void FLACHeaderTweaker::deleteNewHeaders() 
{
	size_t locSize = mNewHeaderList.size();
	for (size_t i = 0; i < locSize; i++) {
		delete mNewHeaderList[i];		
	}

	mNewHeaderList.clear();
}

unsigned long FLACHeaderTweaker::numNewHeaders() 
{
	return (unsigned long)mNewHeaderList.size();
}

StampedOggPacket* FLACHeaderTweaker::getHeader(unsigned long inHeaderNo) 
{
	if (inHeaderNo < mNewHeaderList.size() ) {
		return mNewHeaderList[inHeaderNo]->cloneStamped();
	} else {
		return NULL;
	}
}