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
#include ".\flacmetadatasplitter.h"

FLACMetadataSplitter::FLACMetadataSplitter(void)
	:	mMetadataBlock(NULL)
{
}

FLACMetadataSplitter::~FLACMetadataSplitter(void)
{
}


bool FLACMetadataSplitter::loadMetadata(OggPacket* inMetadata) {
	delete mMetadataBlock;
	mMetadataBlock = inMetadata;
	return splitMetadata();
}
unsigned long FLACMetadataSplitter::numHeaders() {
	return mHeaderTweaker.numNewHeaders();
}
StampedOggPacket* FLACMetadataSplitter::getHeader(unsigned long inIndex) {
	if (inIndex < mHeaderTweaker.numNewHeaders()) {
		return reinterpret_cast<StampedOggPacket*>(mHeaderTweaker.getHeader(inIndex));
	} else {
		return NULL;
	}
}

void FLACMetadataSplitter::emptyList() {


}
bool FLACMetadataSplitter::splitMetadata() {
	//emptyList();
	//OggPacket* locPacket = NULL;
	unsigned char* locBuff = NULL;
	if (mMetadataBlock == NULL) {
		return false;
	} else {
		if (verifyCodecID()) {
			addCodecIdent();
			addStreamInfo();
			addOtherHeaders();
		} else {
			return false;
		}

	}

}

bool FLACMetadataSplitter::addOtherHeaders() {
	unsigned long locUpto = 42;
	unsigned long locMetaSize = mMetadataBlock->packetSize();
	unsigned char* locSourceBuff = mMetadataBlock->packetData();	//Don't delete !
	unsigned char* locNewBuff = NULL;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locPacket = NULL;

	while ( locUpto < locMetaSize) {
		for (int i = 0; i < 3; i++) {
			locPacketSize <<=8;
			locPacketSize += locSourceBuff[locUpto+i];
		}
		
		locUpto += 4;

		locNewBuff = new unsigned char[locPacketSize];
		memcpy((void*)locNewBuff, (const void*)locSourceBuff, locPacketSize);

		locPacket = new StampedOggPacket(locNewBuff, locPacketSize, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);

		mHeaderTweaker.acceptHeader(locPacket);
		locPacket = NULL;

		locUpto += locPacketSize;


	}

	return true;
}
bool FLACMetadataSplitter::addStreamInfo() {
	StampedOggPacket* locPacket = NULL;
	unsigned char* locBuff = new unsigned char[38];

	memcpy((void*)locBuff, (const void*)(mMetadataBlock->packetData()+4), 38);
	locPacket = new StampedOggPacket(locBuff, 38, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);
	mHeaderTweaker.acceptHeader(locPacket);
	return true;
}
bool FLACMetadataSplitter::addCodecIdent() {
	StampedOggPacket* locPacket = NULL;
	unsigned char* locBuff = new unsigned char[4];
	locBuff[0] = 'f';
	locBuff[1] = 'L';
	locBuff[2] = 'a';
	locBuff[3] = 'C';
	locPacket = new StampedOggPacket(locBuff, 4, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);
	mHeaderTweaker.acceptHeader(locPacket);
	return true;
}

bool FLACMetadataSplitter::verifyCodecID() {
	if ((strncmp((char*)mMetadataBlock->packetData(), "fLaC\000\000\000\042", 8)) == 0) {
		return true;
	} else {
		return false;
	}

}