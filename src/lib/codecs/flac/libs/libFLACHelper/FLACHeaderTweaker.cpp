#include "StdAfx.h"
#include ".\flacheadertweaker.h"

FLACHeaderTweaker::FLACHeaderTweaker(void)
	:	mSeenAllHeaders(false)

{
}

FLACHeaderTweaker::~FLACHeaderTweaker(void)
{
	deleteOldHeaders();
	deleteNewHeaders();
}

FLACHeaderTweaker::eFLACAcceptHeaderResult FLACHeaderTweaker::acceptHeader(OggPacket* inHeader) {
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

bool FLACHeaderTweaker::createNewHeaderList() {
	
	
	
	unsigned char* locFirstPackBuff = new unsigned char[51];
	locFirstPackBuff[0] = '\177';
	locFirstPackBuff[1] = 'F';
	locFirstPackBuff[2] = 'L';
	locFirstPackBuff[3] = 'A';
	locFirstPackBuff[4] = 'C';
	locFirstPackBuff[5] = 1;
	locFirstPackBuff[6] = 0;
	locFirstPackBuff[7] = 0; //Num header HIGH BYTE
	locFirstPackBuff[8] = mOldHeaderList.size() - 1; //Num headers LOW BYTE
	locFirstPackBuff[9] = 'f';
	locFirstPackBuff[10] = 'L';
	locFirstPackBuff[11] = 'a';
	locFirstPackBuff[12] = 'C';

	memcpy((void*)(locFirstPackBuff + 13), (const void*) mOldHeaderList[1]->packetData(), 38);

	mNewHeaderList.empty();
	mNewHeaderList.clear();
	mNewHeaderList.push_back(new OggPacket(locFirstPackBuff, 51, false, false));
	locFirstPackBuff = NULL;

	bool locFoundComment = false;
	int locCommentNo = -1;

	//Start at 2, 0 is just fLaC, 1 is the stream info
	for (int i = 2; i < mOldHeaderList.size(); i++) {
		//Loop through to find the comment packet...
		if ( (( (mOldHeaderList[i]->packetData()[0]) << 1) >> 1) == 4) {
			//It's the comment packet.
			locFoundComment = true;
			locCommentNo = i;
			mNewHeaderList.push_back(mOldHeaderList[i]->clone());
		}
	}

	if (locFoundComment != true) {
		//Maybe make one... for now bail out !
		throw 0;
	}

	for (int i = 2; i < mOldHeaderList.size(); i++) {
		if (i != locFoundComment) {
			//If it's not the comment packet we already added, put it in the list.
			mNewHeaderList.push_back(mOldHeaderList[i]->clone());
		}
	}

	for (int i = 1; i < mNewHeaderList.size(); i++) {
		//Loop through the new headers and make sure the flags are set right.
		if (i != mNewHeaderList.size() -1) {
			//Clear the first bit
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] & 127;
		} else {
			//Set the first bit on the last header
			mNewHeaderList[i]->packetData()[0] = mNewHeaderList[i]->packetData()[0] | 128;
		}
	}

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