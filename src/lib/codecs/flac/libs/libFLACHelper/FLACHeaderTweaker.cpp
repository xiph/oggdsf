//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
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

#include "StdAfx.h"
#include ".\flacheadertweaker.h"

FLACHeaderTweaker::FLACHeaderTweaker(void)
	:	mSeenAllHeaders(false)

{
	debugLog.open("G:\\logs\\flactweaker.log", ios_base::out);

}

FLACHeaderTweaker::~FLACHeaderTweaker(void)
{
	debugLog.close();

	deleteOldHeaders();
	deleteNewHeaders();
}

FLACHeaderTweaker::eFLACAcceptHeaderResult FLACHeaderTweaker::acceptHeader(OggPacket* inHeader) {
	debugLog<<endl<<"Accepting header.."<<endl;
	debugLog<<inHeader->toPackDumpString()<<endl;
	const unsigned char MORE_HEADERS_MASK = 128;
	if (!mSeenAllHeaders) {
		debugLog<<"Still tweaking... adding to old list..."<<endl;
		

		mOldHeaderList.push_back(inHeader);
		if ((inHeader->packetData()[0] & MORE_HEADERS_MASK)  != 0) {
			debugLog<<"This is the last header..."<<endl;
			//Last header
			mSeenAllHeaders = true;
			if (createNewHeaderList()) {
				debugLog<<"Create new headers OK"<<endl;
				return LAST_HEADER_ACCEPTED;
			} else {
				debugLog<<"Create new headers FAILED"<<endl;
				return HEADER_ERROR;
			}
		} else {
			debugLog<<"Still need more ehaders..."<<endl;
			//Still more headers to come...
			return HEADER_ACCEPTED;
		}
	} else {
		debugLog<<"All headers already seen"<<endl;
		return ALL_HEADERS_ALREADY_SEEN;
	}

}

bool FLACHeaderTweaker::createNewHeaderList() {
	
	debugLog<<"Create new header list method"<<endl;
	
	debugLog<<"Filling first pack"<<endl;
	unsigned char* locFirstPackBuff = new unsigned char[51];
	locFirstPackBuff[0] = '\177';
	locFirstPackBuff[1] = 'F';
	locFirstPackBuff[2] = 'L';
	locFirstPackBuff[3] = 'A';
	locFirstPackBuff[4] = 'C';
	locFirstPackBuff[5] = 1;
	locFirstPackBuff[6] = 0;
	locFirstPackBuff[7] = 0; //Num header HIGH BYTE
	locFirstPackBuff[8] = mOldHeaderList.size() - 2; //Num headers LOW BYTE
	locFirstPackBuff[9] = 'f';
	locFirstPackBuff[10] = 'L';
	locFirstPackBuff[11] = 'a';
	locFirstPackBuff[12] = 'C';

	debugLog<<"Copying in packet data"<<endl;
	memcpy((void*)(locFirstPackBuff + 13), (const void*) mOldHeaderList[1]->packetData(), 38);

	mNewHeaderList.empty();
	mNewHeaderList.clear();
	debugLog<<"Putting first header into new list"<<endl;
	mNewHeaderList.push_back(new OggPacket(locFirstPackBuff, 51, false, false));
	locFirstPackBuff = NULL;

	bool locFoundComment = false;
	int locCommentNo = -1;

	//Start at 2, 0 is just fLaC, 1 is the stream info
	for (int i = 2; i < mOldHeaderList.size(); i++) {
		//Loop through to find the comment packet...
		debugLog<<"Scanning old header "<<i<<endl;
		if ( ((mOldHeaderList[i]->packetData()[0]) & 127) == 4) {
			//It's the comment packet.
			debugLog<<"Found a comment packet..."<<endl;
			locFoundComment = true;
			locCommentNo = i;
			mNewHeaderList.push_back(mOldHeaderList[i]->clone());
		}
	}

	if (locFoundComment != true) {
		debugLog<<"No comments present... FATALITY !"<<endl;
		//Maybe make one... for now bail out !
		throw 0;
	}

	for (int i = 2; i < mOldHeaderList.size(); i++) {
	
		if (i != locCommentNo) {
			debugLog<<"Adding another ehader..."<<endl;
			//If it's not the comment packet we already added, put it in the list.
			mNewHeaderList.push_back(mOldHeaderList[i]->clone());
		}
	}

	for (int i = 1; i < mNewHeaderList.size(); i++) {
		//Loop through the new headers and make sure the flags are set right.
		if (i != mNewHeaderList.size() -1) {
			//Clear the first bit
			debugLog<<"Clearing header bit "<<i<<endl;
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] & 127;
		} else {
			debugLog<<"Setting header bit "<<i<<endl;
			//Set the first bit on the last header
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] | 128;
		}
	}

	debugLog<<"Deleting old headers..."<<endl;
	deleteOldHeaders();

	return true;


}

void FLACHeaderTweaker::deleteOldHeaders() {
	int locSize = mOldHeaderList.size();
	for (int i = 0; i < locSize; i++) {
		delete mOldHeaderList[i];		
	}

	mOldHeaderList.empty();
}

void FLACHeaderTweaker::deleteNewHeaders() {
	int locSize = mNewHeaderList.size();
	for (int i = 0; i < locSize; i++) {
		delete mNewHeaderList[i];		
	}

	mNewHeaderList.empty();
}

unsigned long FLACHeaderTweaker::numNewHeaders() {
	return mNewHeaderList.size();
}
OggPacket* FLACHeaderTweaker::getHeader(unsigned long inHeaderNo) {
	if (inHeaderNo < mNewHeaderList.size() ) {
		return mNewHeaderList[inHeaderNo];
	} else {
		return NULL;
	}
}