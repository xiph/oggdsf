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

#include "stdafx.h"
#include "VorbisComments.h"

VorbisComments::VorbisComments(void)
{
}

VorbisComments::~VorbisComments(void)
{
}

string VorbisComments::vendorString() {
	return mVendorString;
}
bool VorbisComments::setVendorString(string inVendorString) {
	//FIX::: Validation needed
	mVendorString = inVendorString;
	return true;
}

unsigned long VorbisComments::numUserComments() {
	return (unsigned long)mCommentList.size();
}
SingleVorbisComment* VorbisComments::getUserComment(unsigned long inIndex) {
	//FIX::: Bounds checking
	return mCommentList[inIndex];
}
	
vector<SingleVorbisComment*> VorbisComments::getCommentsByKey(string inKey) {
	//FIX::: Probably faster not to iterate... but who cares for now.. there aren't many.
	vector<SingleVorbisComment*> retComments;
	SingleVorbisComment* locCurrComment = NULL;

	for (size_t i = 0; i < mCommentList.size(); i++) {
		locCurrComment = mCommentList[i];
		//FIX::: Need to upcase everything
		if (locCurrComment->key() == inKey) {
			retComments.push_back(locCurrComment);
		}
	}
	return retComments;
}

bool VorbisComments::addComment(SingleVorbisComment* inComment) {
	mCommentList.push_back(inComment);
	return true;
}
bool VorbisComments::addComment(string inKey, string inValue) {
	SingleVorbisComment* locComment = new SingleVorbisComment;
	locComment->setKey(inKey);
	locComment->setValue(inValue);
	mCommentList.push_back(locComment);
	return true;
}

bool VorbisComments::parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset) {
	//FIX::: Validate it is a comment packet
	unsigned long locPackSize = inPacket->packetSize();

	//Account for header ident stuff
	unsigned long locUpto = inStartOffset;
	unsigned long locVendorLength = 0;
	string locVendorString;
	char* tempBuff = NULL;
	unsigned char* locPackBuff = inPacket->packetData();
	unsigned long locNumComments = 0;
	vector<SingleVorbisComment*> locCommentList;

	if (locPackSize < locUpto + 4 - 1) {
		//FAILED - No vendor length
		return false;
	}

	locVendorLength = iLE_Math::charArrToULong(inPacket->packetData() + locUpto);
	locUpto+=4;

	if (locPackSize < locUpto + locVendorLength - 1) {
		//FAILED - Vendor string not present
		return false;
	}

	tempBuff = new char[locVendorLength + 1];

	if (tempBuff == NULL) {
		//FAILED - Vendor length too big, out of memory
		return false;
	}

	memcpy((void*)tempBuff, (const void*)(locPackBuff + locUpto), locVendorLength);
	tempBuff[locVendorLength] = '\0';

	locVendorString = tempBuff;
	delete[] tempBuff;
	tempBuff = NULL;

	locUpto += locVendorLength;

	if (locPackSize < locUpto + 4 - 1) {
		//FAILED - User comment list length not present
		return false;
	}

	locNumComments = iLE_Math::charArrToULong(locPackBuff + locUpto);
	locUpto += 4;

	unsigned long locUserCommentLength = 0;
	bool locFailed = false;
	string locUserComment;
	unsigned long i = 0;
	while (!locFailed && (i < locNumComments)) {
		if (locPackSize < locUpto + 4 -1) {
			//FAILED - User comment string length not present
			return false;
		}

		locUserCommentLength = iLE_Math::charArrToULong(locPackBuff + locUpto);
		locUpto += 4;


		if (locPackSize < locUpto + locUserCommentLength - 1) {
			//FAILED - User comment string not present
			return false;
		}

		tempBuff = new char[locUserCommentLength+1];

		memcpy((void*)tempBuff, (const void*)(locPackBuff + locUpto), locUserCommentLength);
		tempBuff[locUserCommentLength] = '\0';

        locUserComment = tempBuff;
		delete[] tempBuff;
		locUpto += locUserCommentLength;


		SingleVorbisComment* locComment = new SingleVorbisComment;
		if (locComment->parseComment(locUserComment)) {
			locCommentList.push_back(locComment);
		} else {
			//FAILED - Comment not parsable
			return false;
		}

		i++;

	}

	//Check the bit.
	if (locPackSize < locUpto) {
		//FAILED - No check bit
		return false;
	}

	//Everythings ok... put it into the class fields
	if ((locPackBuff[locUpto] & 1) == 1) {
		//OK
		mVendorString = locVendorString;
		
		mCommentList.empty();
		mCommentList.clear();
		for (size_t j = 0; j < locCommentList.size(); j++) {
			mCommentList.push_back(locCommentList[j]);	
		}
	} else {
		//FAILED - Check bit not set
		return false;
	}

	return true;



	

	
}

string VorbisComments::toString() {
	string retStr;

	retStr = "VENDOR : " + mVendorString + "\n";
	for (size_t i = 0; i < mCommentList.size(); i++) {
		retStr += "-- " + mCommentList[i]->toString() + "\n";
	}
	retStr += "\n";
	return retStr;

}
unsigned long VorbisComments::size() {
	unsigned long locPackSize = 0;

	locPackSize = (unsigned long)mVendorString.size() + 4;

	for (size_t i = 0; i < mCommentList.size(); i++) {
		locPackSize += mCommentList[i]->length() + 4;
	}

	//Check bit
	locPackSize++;
	
	return locPackSize;
}
OggPacket* VorbisComments::toOggPacket(unsigned char* inPrefixBuff, unsigned long inPrefixBuffSize) {

	unsigned long locPackSize = size();
	unsigned long locUpto = 0;
	unsigned char* locPackData = NULL;

	if (inPrefixBuff != NULL && inPrefixBuffSize != 0) {
		locPackSize += inPrefixBuffSize;
		locPackData = new unsigned char[locPackSize];
		memcpy((void*)locPackData, (const void*)inPrefixBuff, inPrefixBuffSize);
		locUpto += inPrefixBuffSize;
	} else {
		locPackData = new unsigned char[locPackSize];
	}
	
	iLE_Math::ULongToCharArr((unsigned long)mVendorString.length(), locPackData + locUpto);
	locUpto += 4;

	memcpy((void*)(locPackData + locUpto), (const void*)mVendorString.c_str(), mVendorString.length());
	locUpto += (unsigned long)mVendorString.length();

	iLE_Math::ULongToCharArr((unsigned long)mCommentList.size(), locPackData + locUpto);
	locUpto += 4;

	for (size_t i = 0; i < mCommentList.size(); i++) {
		iLE_Math::ULongToCharArr(mCommentList[i]->length(), locPackData + locUpto);
		locUpto += 4;

		memcpy((void*)(locPackData + locUpto), (const void*)mCommentList[i]->toString().c_str(), mCommentList[i]->length());
		locUpto += mCommentList[i]->length();
	}

	locPackData[locUpto] = 1;

	OggPacket* locPacket = NULL;
													//Full packet not truncated or continued.
	locPacket = new OggPacket(locPackData, locPackSize, false, false);

	return locPacket;

}
