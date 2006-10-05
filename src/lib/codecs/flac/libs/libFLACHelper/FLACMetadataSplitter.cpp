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
#include ".\flacmetadatasplitter.h"

FLACMetadataSplitter::FLACMetadataSplitter(void)
	:	mMetadataBlock(NULL)
{
}

FLACMetadataSplitter::~FLACMetadataSplitter(void)
{
	delete mMetadataBlock;
	//Delete stuff !!
}
StampedOggPacket* FLACMetadataSplitter::convertToStampedPacket(OggPacket* inPacket) 
{
	//Convert the old packet to the new one.
	//This function deletes the incoming packet... and transfers the buffer directly into
	// the stamped packet without a memcpy
	StampedOggPacket* locStamped = new StampedOggPacket(inPacket->packetData(), inPacket->packetSize(), false, false, 0,0,StampedOggPacket::OGG_END_ONLY);
	//Ensure when we delete the old packet, it doesn't delete it's buffer
	inPacket->setPacketData(NULL);
	delete inPacket;		
	return locStamped;
}

bool FLACMetadataSplitter::loadMetadata(OggPacket* inMetadata) 
{
	delete mMetadataBlock;
	mMetadataBlock = inMetadata;
	return splitMetadata();
}
unsigned long FLACMetadataSplitter::numHeaders() {
	return mHeaderTweaker.numNewHeaders();
}
StampedOggPacket* FLACMetadataSplitter::getHeader(unsigned long inIndex) 
{
	if (inIndex < mHeaderTweaker.numNewHeaders()) {
		return (FLACMetadataSplitter::convertToStampedPacket(mHeaderTweaker.getHeader(inIndex)));
	} else {
		return NULL;
	}
}

void FLACMetadataSplitter::emptyList() 
{


}
bool FLACMetadataSplitter::splitMetadata() 
{
	if (mMetadataBlock == NULL) {
		return false;
	} else {
		if (verifyCodecID()) {
			addCodecIdent();
			addStreamInfo();
			addOtherHeaders();

			//TODO::: Should really verify this !
			return true;
		} else {
			return false;
		}
	}
}

bool FLACMetadataSplitter::addOtherHeaders() 
{
	//debugLog<<"Add other headers..."<<endl;
	unsigned long locUpto = 42;
	unsigned long locMetaSize = mMetadataBlock->packetSize();
	unsigned char* locSourceBuff = mMetadataBlock->packetData();	//Don't delete !
	unsigned char* locNewBuff = NULL;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locPacket = NULL;

	while ( locUpto < locMetaSize) {
		for (int i = 1; i < 4; i++) {
			locPacketSize <<=8;
			locPacketSize += locSourceBuff[locUpto+i];
		}
		locPacketSize += 4;
		//debugLog<<"Packet size = "<<locPacketSize<<endl;
		//locUpto += 4;

		locNewBuff = new unsigned char[locPacketSize];
		memcpy((void*)locNewBuff, (const void*)(locSourceBuff + locUpto), locPacketSize);

        locPacket = new StampedOggPacket(locNewBuff, locPacketSize, false, false, 0, 0, StampedOggPacket::OGG_BOTH);
		mHeaderTweaker.acceptHeader(locPacket);
		locPacket = NULL;

		locUpto += (locPacketSize);
		

	}

	return true;
}
bool FLACMetadataSplitter::addStreamInfo() 
{
	StampedOggPacket* locPacket = NULL;
	unsigned char* locBuff = new unsigned char[38];
	
	memcpy((void*)locBuff, (const void*)(mMetadataBlock->packetData()+4), 38);
	locPacket = new StampedOggPacket(locBuff, 38, false, false, 0, 0, StampedOggPacket::OGG_BOTH);		//No need to delete
	mHeaderTweaker.acceptHeader(locPacket);
	return true;
}
bool FLACMetadataSplitter::addCodecIdent() 
{
	StampedOggPacket* locPacket = NULL;
	unsigned char* locBuff = new unsigned char[4];
	locBuff[0] = 'f';
	locBuff[1] = 'L';
	locBuff[2] = 'a';
	locBuff[3] = 'C';
	locPacket = new StampedOggPacket(locBuff, 4, false, false, 0, 0, StampedOggPacket::OGG_BOTH);		//No need to delete.
	mHeaderTweaker.acceptHeader(locPacket);
	return true;
}

bool FLACMetadataSplitter::verifyCodecID() 
{
	if ((strncmp((char*)mMetadataBlock->packetData(), "fLaC\000\000\000\042", 8)) == 0) {
		//debugLog<<"Codec verified"<<endl;
		return true;
	} else {
		//debugLog<<"Codec NOT VERIFIED"<<endl;
		return false;
	}

}