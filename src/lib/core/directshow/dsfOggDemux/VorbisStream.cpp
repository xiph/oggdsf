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
#include "vorbisstream.h"

VorbisStream::VorbisStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mVorbisFormatBlock(NULL)
{
	InitCodec(inBOSPage->getStampedPacket(0));
	//debugLog.open("G:\\logs\\vorbisstream.log", ios_base::out);
}

VorbisStream::~VorbisStream(void)
{
	delete mVorbisFormatBlock;
}


bool VorbisStream::InitCodec(StampedOggPacket* inOggPacket) {
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::VORBIS;
	mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	mNumHeadersNeeded = 2;
	return true;
}

BYTE* VorbisStream::getFormatBlock() {

	return (BYTE*)mVorbisFormatBlock;

}
bool VorbisStream::createFormatBlock() {
	//Check where this gets deleted
	mVorbisFormatBlock = new sVorbisFormatBlock;
	mVorbisFormatBlock->vorbisVersion = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 7);
	mVorbisFormatBlock->numChannels = mCodecHeaders->getPacket(0)->packetData()[11];
	mVorbisFormatBlock->samplesPerSec = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 12);
	mVorbisFormatBlock->maxBitsPerSec = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 16);
	mVorbisFormatBlock->avgBitsPerSec = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 20);
	mVorbisFormatBlock->minBitsPerSec = iLE_Math::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 24);

	return true;
}

void VorbisStream::setLastEndGranPos(__int64 inPos) {
	//debugLog<<"Vorbis inPos = "<<inPos<<endl;
	mLastEndGranulePos = (inPos * (__int64)mVorbisFormatBlock->samplesPerSec)/ UNITS;
	//debugLog<<"Vorbis sets End Gran : "<<mLastEndGranulePos<<endl;
}
unsigned long VorbisStream::getFormatBlockSize() {
	return sizeof(sVorbisFormatBlock);
}
GUID VorbisStream::getFormatGUID() {
	return FORMAT_Vorbis;
}
GUID VorbisStream::getSubtypeGUID() {
	return MEDIASUBTYPE_Vorbis;
}
wstring VorbisStream::getPinName() {
	wstring retName = L"Vorbis Out";
	return retName;
}
GUID VorbisStream::getMajorTypeGUID() {
	return MEDIATYPE_Audio;
}

LONGLONG VorbisStream::getCurrentPos() {
	return (mLastEndGranulePos * UNITS) / mVorbisFormatBlock->samplesPerSec;
}

unsigned long VorbisStream::getNumBuffers() {
	return VORBIS_NUM_BUFFERS;
}
unsigned long VorbisStream::getBufferSize() {
	return VORBIS_BUFFER_SIZE;
}