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
#include "speexstream.h"

SpeexStream::SpeexStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mSpeexFormatBlock(NULL)
{
	InitCodec(inBOSPage->getStampedPacket(0));
}

SpeexStream::~SpeexStream(void)
{
	delete mSpeexFormatBlock;
}

bool SpeexStream::InitCodec(StampedOggPacket* inOggPacket) {
	//Can probably abstract this out of here too !
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::SPEEX;
	mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	//What to do with commment fields ??
	mNumHeadersNeeded = 1;
	return true;
}

wstring SpeexStream::getPinName() {
	wstring locName = L"Speex Out";
	return locName;
}

bool SpeexStream::createFormatBlock() {
	mSpeexFormatBlock = new sSpeexFormatBlock;
	//Fix the format block data... use header version and other version.
	mSpeexFormatBlock->speexVersion = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 28);
	mSpeexFormatBlock->numChannels = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 48);
	mSpeexFormatBlock->samplesPerSec = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 36);
	return true;
}
BYTE* SpeexStream::getFormatBlock() {

	return (BYTE*)mSpeexFormatBlock;


}

void SpeexStream::setLastEndGranPos(__int64 inPos) {
	
	mLastEndGranulePos = (inPos * (__int64)mSpeexFormatBlock->samplesPerSec)/ UNITS;
	//osDebug<<"Speex sets End Gran : "<<mLastEndGranulePos<<endl;
}
unsigned long SpeexStream::getFormatBlockSize() {
	//Do something
	return sizeof(sSpeexFormatBlock);
}
GUID SpeexStream::getFormatGUID() {
	return FORMAT_Speex;
}
GUID SpeexStream::getSubtypeGUID() {
	return MEDIASUBTYPE_Speex;
}

GUID SpeexStream::getMajorTypeGUID() {
	return MEDIATYPE_Audio;
}

LONGLONG SpeexStream::getCurrentPos() {
	return (mLastEndGranulePos * UNITS) / mSpeexFormatBlock->samplesPerSec;
}

unsigned long SpeexStream::getNumBuffers() {
	return SPEEX_NUM_BUFFERS;
}
unsigned long SpeexStream::getBufferSize() {
	return SPEEX_BUFFER_SIZE;
}