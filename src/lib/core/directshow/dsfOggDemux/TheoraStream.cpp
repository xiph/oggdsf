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
#include "StdAfx.h"
#include "theorastream.h"

TheoraStream::TheoraStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mTheoraFormatBlock(NULL)
{
	InitCodec(inBOSPage->getStampedPacket(0));
	//debugLog.open("G:\\logs\\theorastream.log", ios_base::out);
}

TheoraStream::~TheoraStream(void)
{
	delete mTheoraFormatBlock;
}

bool TheoraStream::InitCodec(StampedOggPacket* inOggPacket) {
	//Can probably abstract this out of here too !
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::THEORA;
	mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	//What to do with commment fields ??
	mNumHeadersNeeded = 2;
	return true;
}

wstring TheoraStream::getPinName() {
	wstring locName = L"Theora Out";
	return locName;
}

bool TheoraStream::createFormatBlock() {
	mTheoraFormatBlock = new sTheoraFormatBlock;
	//Fix the format block data... use header version and other version.
	unsigned char* locIdentHeader = mCodecHeaders->getPacket(0)->packetData();
	//mTheoraFormatBlock->TheoraVersion = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 28);
	//mTheoraFormatBlock->numChannels = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 48);
	//mTheoraFormatBlock->samplesPerSec = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 36);

	mTheoraFormatBlock->theoraVersion = (iBE_Math::charArrToULong(locIdentHeader + 7)) >>8;
	mTheoraFormatBlock->width = (iBE_Math::charArrToUShort(locIdentHeader + 10)) * 16;
	mTheoraFormatBlock->height = (iBE_Math::charArrToUShort(locIdentHeader + 12)) * 16;
	mTheoraFormatBlock->frameWidth = (iBE_Math::charArrToULong(locIdentHeader + 14)) >>8;
	mTheoraFormatBlock->frameHeight = (iBE_Math::charArrToULong(locIdentHeader + 17)) >>8;
	mTheoraFormatBlock->xOffset = locIdentHeader[20];
	mTheoraFormatBlock->yOffset = locIdentHeader[21];
	mTheoraFormatBlock->frameRateNumerator = iBE_Math::charArrToULong(locIdentHeader + 22);
	mTheoraFormatBlock->frameRateDenominator = iBE_Math::charArrToULong(locIdentHeader + 26);
	mTheoraFormatBlock->aspectNumerator = (iBE_Math::charArrToULong(locIdentHeader + 30)) >>8;
	mTheoraFormatBlock->aspectDenominator = (iBE_Math::charArrToULong(locIdentHeader + 33)) >>8;
	mTheoraFormatBlock->colourSpace = locIdentHeader[36];
	mTheoraFormatBlock->targetBitrate = (iBE_Math::charArrToULong(locIdentHeader + 37)) >>8;
	mTheoraFormatBlock->targetQuality = (locIdentHeader[40]) >> 2;

	mTheoraFormatBlock->maxKeyframeInterval= (((locIdentHeader[40]) % 4) << 3) + (locIdentHeader[41] >> 5);

	return true;
}

void TheoraStream::setLastEndGranPos(__int64 inPos) {
	//debugLog<<"Theora inPos: "<<inPos<<endl;
	LONGLONG locFrameDuration = (UNITS * mTheoraFormatBlock->frameRateDenominator) / (mTheoraFormatBlock->frameRateNumerator);

	LONGLONG locAbsFramePos = inPos / locFrameDuration;
	//Timestamp hacks start here...
	unsigned long locMod = (unsigned long)pow(2, mTheoraFormatBlock->maxKeyframeInterval);
	mLastEndGranulePos = ((locAbsFramePos/locMod) << mTheoraFormatBlock->maxKeyframeInterval) + (locAbsFramePos % locMod);
	//unsigned long locInterFrameNo = (mLastSeenStartGranPos) % locMod);
	//LONGLONG locAbsFramePos = ((mLastSeenStartGranPos >> locFilter->mTheoraFormatInfo->maxKeyframeInterval) * locMod) + locInterFrameNo;
	//REFERENCE_TIME locTimeBase = ((locAbsFramePos * mFrameDuration) - locThis->mSeekTimeBase;
	//mLastEndGranulePos = (inPos * (__int64)mSpeexFormatBlock->samplesPerSec)/ UNITS;
	//debugLog<<"Theora sets End Gran : "<<mLastEndGranulePos<<endl;
}
BYTE* TheoraStream::getFormatBlock() {

	return (BYTE*)mTheoraFormatBlock;
}

GUID TheoraStream::getMajorTypeGUID() {
	return MEDIATYPE_Video;
}
unsigned long TheoraStream::getFormatBlockSize() {
	//Do something
	return sizeof(sTheoraFormatBlock);
}
GUID TheoraStream::getFormatGUID() {
	return FORMAT_Theora;
}
GUID TheoraStream::getSubtypeGUID() {
	return MEDIASUBTYPE_Theora;
}

LONGLONG TheoraStream::getCurrentPos() {
	return 0; //(mLastGranulePos * UNITS) / mVorbisFormatBlock->samplesPerSec;
}