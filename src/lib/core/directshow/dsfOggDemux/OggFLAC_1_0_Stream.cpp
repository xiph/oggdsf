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

#include "stdafx.h"
#include "OggFLAC_1_0_Stream.h"
//#include "FLACMath.h"

OggFLAC_1_0_Stream::OggFLAC_1_0_Stream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mFLACFormatBlock(NULL)
	,	mIsFixedNumHeaders(false)
	//,	mNumHeaderPackets(0)
{
	InitCodec(inBOSPage->getStampedPacket(0));
}

OggFLAC_1_0_Stream::~OggFLAC_1_0_Stream(void)
{
	delete mFLACFormatBlock;
}

bool OggFLAC_1_0_Stream::InitCodec(StampedOggPacket* inOggPacket) {
	//Can probably abstract this out of here too !
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::FLAC;
	mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	//What to do with commment fields ??
	//We set this to 1... and we override the header processor
	//When we see the last header packet ie starts with 1xxxxxxx then we decrement it.
	
	//NEW::: Since the ogg flac 1.0 mapping there's now another option... if the new first header
	// specifies the number of headers, we can count directly, otherwise we stll use the old
	// method for maximum compatability.
	mNumHeadersNeeded = iBE_Math::charArrToUShort(inOggPacket->packetData() + 7);
	//debugLog<<"Num FLAC Headers needed = "<<mNumHeadersNeeded<<endl;
	if (mNumHeadersNeeded == 0) {
		//Variable number
		// Use the old method of setting this to 1, and then decrementing it when we see the last one.
		mNumHeadersNeeded = 1;
		mIsFixedNumHeaders = false;
	} else {
		mIsFixedNumHeaders = true;
	}
	return true;
}

wstring OggFLAC_1_0_Stream::getPinName() {
	wstring locName = L"FLAC Out";
	return locName;
}

bool OggFLAC_1_0_Stream::createFormatBlock() {
	const unsigned char FLAC_CHANNEL_MASK = 14;  //00001110
	const unsigned char FLAC_BPS_START_MASK = 1; //00000001
	const unsigned char FLAC_BPS_END_MASK = 240;  //11110000
	mFLACFormatBlock = new sFLACFormatBlock;
	//Fix the format block data... use header version and other version.
	//mFLACFormatBlock->FLACVersion = FLACMath::charArrToULong(mCodecHeaders->getPacket(1)->packetData() + 28);
	mFLACFormatBlock->numChannels = (((mCodecHeaders->getPacket(0)->packetData()[29]) & FLAC_CHANNEL_MASK) >> 1) + 1;
	mFLACFormatBlock->samplesPerSec = (iBE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 27)) >> 12;
	
	mFLACFormatBlock->numBitsPerSample =	(((mCodecHeaders->getPacket(0)->packetData()[29] & FLAC_BPS_START_MASK) << 4)	|
											((mCodecHeaders->getPacket(0)->packetData()[30] & FLAC_BPS_END_MASK) >> 4)) + 1;	
	return true;
}
BYTE* OggFLAC_1_0_Stream::getFormatBlock() {

	return (BYTE*)mFLACFormatBlock;


}
unsigned long OggFLAC_1_0_Stream::getFormatBlockSize() {
	//Do something
	return sizeof(sFLACFormatBlock);
}
GUID OggFLAC_1_0_Stream::getFormatGUID() {
	return FORMAT_FLAC;
}
GUID OggFLAC_1_0_Stream::getSubtypeGUID() {
	return MEDIASUBTYPE_FLAC;
}
GUID OggFLAC_1_0_Stream::getMajorTypeGUID() {
	return MEDIATYPE_Audio;
}

unsigned long OggFLAC_1_0_Stream::getNumBuffers() {
	return OGG_FLAC_1_0_NUM_BUFFERS;
}
unsigned long OggFLAC_1_0_Stream::getBufferSize() {
	return OGG_FLAC_1_0_BUFFER_SIZE;
}

//Need to override from oggstream because we have variable number of headers
bool OggFLAC_1_0_Stream::processHeaderPacket(StampedOggPacket* inPacket) {
	//FIX::: Return values
	const unsigned char MORE_HEADERS_MASK = 128;   //10000000
	//We don't delete the packet... the codecheader list will delete when it's done.
	//StampedOggPacket* locPacket = processPacket(inPacket);
	if (inPacket != NULL) {
		//We got a comlpete packet
		mCodecHeaders->addPacket(inPacket);
		if (mIsFixedNumHeaders) {
			mNumHeadersNeeded--;
		} else {
			if ((inPacket->packetData()[0] & MORE_HEADERS_MASK) != 0) {
				mNumHeadersNeeded--;
				//mNumHeaderPackets++;
			}
		}
	}
	return true;
}
void OggFLAC_1_0_Stream::setLastEndGranPos(__int64 inPos) {
	mLastEndGranulePos = (inPos * (__int64)mFLACFormatBlock->samplesPerSec)/ UNITS;
}
bool OggFLAC_1_0_Stream::deliverCodecHeaders() {
	//debugLog<<"Delivering Codec Headers... "<<mCodecHeaders->numPackets()<<endl;
	StampedOggPacket* locPacket = NULL;
	for (unsigned long i = 0; i < mCodecHeaders->numPackets(); i++) {
		if (i==0) {
			//Need to reconstruct what the meta-data blocks look like in naked flac,
			// so strip away most of the new header.
			//
			//New part of ogg header is 9 bytes
			//old flac ident is 4 bytes
			//StreamInfoHeader is 38 bytes
			//
			//So we discard the first 9 bytes, and keep the next 42 bytes.
			unsigned char* locPackBuf = new unsigned char[42];			//Given away to the StampedPacket
			
			//locPacket = (StampedOggPacket*)mCodecHeaders->getPacket(0)->clone();
			memcpy((void*)locPackBuf, (const void*)(mCodecHeaders->getPacket(0)->packetData() + 9), 42);
			locPacket = new StampedOggPacket(locPackBuf, 42, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);
		} else {
			locPacket->merge(mCodecHeaders->getPacket(i));
		}
	}
	if (mCodecHeaders->numPackets() > 0) {
		dispatchPacket(locPacket);
	}
	return true;
}

LONGLONG OggFLAC_1_0_Stream::getCurrentPos() {
	return (mLastEndGranulePos * UNITS) / mFLACFormatBlock->samplesPerSec;
}

//unsigned long OggFLAC_1_0_Stream::numCodecHeaders() {
//	return mNumHeaderPackets;  //is this even needed ?
//}