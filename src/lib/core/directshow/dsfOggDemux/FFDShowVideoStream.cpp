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
#include "ffdshowvideostream.h"



FFDShowVideoStream::FFDShowVideoStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter)
	:	OggStream(inBOSPage, inOwningFilter)
	,	mFFDShowVideoFormatBlock(NULL)
	,	mLastTimeStamp(0)
	//,	mLastGranulePos(0)
	,	mGranuleOffset(0)
{
	InitCodec(inBOSPage->getStampedPacket(0));
	debugLog.open("g:\\downloads\\ffd_dump.out", ios_base::out|ios_base::binary);
}

FFDShowVideoStream::~FFDShowVideoStream(void)
{
	debugLog.close();
	delete mFFDShowVideoFormatBlock;
}

bool FFDShowVideoStream::InitCodec(StampedOggPacket* inOggPacket) {
	//Can probably abstract this out of here too !
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::FFDSHOW_VIDEO;
	mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	mHeaderPack = (StampedOggPacket*)inOggPacket->clone();

	unsigned char* locPackData = inOggPacket->packetData();
	mFourCCCode = MAKEFOURCC(locPackData[9], locPackData[10], locPackData[11], locPackData[12]);
	mMediaSubTypeGUID = (GUID)(FOURCCMap(mFourCCCode));

	unsigned char* locFourCCString = new unsigned char[5];
	for (int i = 0; i < 4; i++) {
		locFourCCString[i] = locPackData[9+i];
	}
	locFourCCString[4] = 0;

	string locPinName = (char*)locFourCCString;
	locPinName = "FOURCC( " + locPinName + " ) out";
	mPinName = StringHelper::toWStr(locPinName);

	//What to do with commment fields ??
	mNumHeadersNeeded = 3;
	//mFirstRun = false;
	return true;
}


bool FFDShowVideoStream::deliverCodecHeaders() {
	StampedOggPacket* locPacket = NULL;
	for (unsigned long i = 2; i < mCodecHeaders->numPackets(); i++) {
		locPacket = mCodecHeaders->getPacket(i);

		dispatchPacket(locPacket);
	}
	return true;

}
wstring FFDShowVideoStream::getPinName() {
	
	return mPinName;
}

bool FFDShowVideoStream::createFormatBlock() {
	mFFDShowVideoFormatBlock = new VIDEOINFOHEADER;
	////Fix the format block data... use header version and other version.
	//unsigned char* locIdentHeader = mCodecHeaders->getPacket(0)->packetData();
	////mTheoraFormatBlock->TheoraVersion = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 28);
	////mTheoraFormatBlock->numChannels = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 48);
	////mTheoraFormatBlock->samplesPerSec = OggMath::charArrToULong(mCodecHeaders->getPacket(0)->packetData() + 36);

	//mTheoraFormatBlock->theoraVersion = (FLACMath::charArrToULong(locIdentHeader + 7)) >>8;
	//mTheoraFormatBlock->width = (FLACMath::charArrToUShort(locIdentHeader + 10)) * 16;
	//mTheoraFormatBlock->height = (FLACMath::charArrToUShort(locIdentHeader + 12)) * 16;
	//mTheoraFormatBlock->frameWidth = (FLACMath::charArrToULong(locIdentHeader + 14)) >>8;
	//mTheoraFormatBlock->frameHeight = (FLACMath::charArrToULong(locIdentHeader + 17)) >>8;
	//mTheoraFormatBlock->xOffset = locIdentHeader[20];
	//mTheoraFormatBlock->yOffset = locIdentHeader[21];
	//mTheoraFormatBlock->frameRateNumerator = FLACMath::charArrToULong(locIdentHeader + 22);
	//mTheoraFormatBlock->frameRateDenominator = FLACMath::charArrToULong(locIdentHeader + 26);
	//mTheoraFormatBlock->aspectNumerator = (FLACMath::charArrToULong(locIdentHeader + 30)) >>8;
	//mTheoraFormatBlock->aspectDenominator = (FLACMath::charArrToULong(locIdentHeader + 33)) >>8;
	//mTheoraFormatBlock->colourSpace = locIdentHeader[36];
	//mTheoraFormatBlock->targetBitrate = (FLACMath::charArrToULong(locIdentHeader + 37)) >>8;
	//mTheoraFormatBlock->targetQuality = (locIdentHeader[40]) >> 2;
	////FIX:: When you can be bothered spanning bits over bytes.
	//mTheoraFormatBlock->maxKeyframeInterval= 0;



	//FORMAT OF HEADER IN OGG BOS
	//-----------------------------------------
	//0		-		8		=		STRING		Stream Type (char[8] is NULL)
	//9		-		12		=		STRING		Sub Type (FOURCC)
	//13	-		16		=		INT32		Size of structure ???
	//17	-		24		=		INT64		Time per "unit" in DSHOW UNITS
	//25	-		32		=		INT64		Samples per "unit"
	//33	-		36		=		INT32		Deefault length ????? in media type ?? WTF ?
	//37	-		40		=		INT32		Buffer Size
	//41	-		42		=		INT16		Bits per sample
	//43	-		44		=		-----		HOLE IN DATA
	//45	-		48		=		INT32		Video Width
	//49	-		52		=		INT32		Video Height


 

	//------------------------------------------

	OggInt64 locInt64;
	locInt64.setData(mHeaderPack->packetData() + 17);
	__int64 locTimePerBlock = locInt64.value();

	locInt64.setData(mHeaderPack->packetData() + 25);
	__int64 locSamplesPerBlock = locInt64.value();


	mFFDShowVideoFormatBlock->AvgTimePerFrame = locTimePerBlock / locSamplesPerBlock;

	__int64 locFPSec = (UNITS / locTimePerBlock) * locSamplesPerBlock;
	unsigned short locBPSample = ((unsigned char)(mHeaderPack->packetData()[41])) + (((unsigned short)(mHeaderPack->packetData()[42])) * 256);

	
	
	mFFDShowVideoFormatBlock->bmiHeader.biBitCount = locBPSample;
	mFFDShowVideoFormatBlock->bmiHeader.biClrImportant = 0;   //All colours important
	mFFDShowVideoFormatBlock->bmiHeader.biClrUsed = 0;        //Use max colour depth
	mFFDShowVideoFormatBlock->bmiHeader.biCompression = mFourCCCode;

	unsigned long locHeight = OggMath::charArrToULong(mHeaderPack->packetData() + 49);
	unsigned long locWidth =  OggMath::charArrToULong(mHeaderPack->packetData() + 45);

	mFFDShowVideoFormatBlock->dwBitRate = 0;

	mFFDShowVideoFormatBlock->bmiHeader.biHeight = locHeight;
	mFFDShowVideoFormatBlock->bmiHeader.biPlanes = 1;    //Must be 1
	mFFDShowVideoFormatBlock->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	mFFDShowVideoFormatBlock->bmiHeader.biSizeImage = (locHeight * locWidth * locBPSample) / 8;    //Size in bytes of image ??
	mFFDShowVideoFormatBlock->bmiHeader.biWidth = locWidth;
	mFFDShowVideoFormatBlock->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	mFFDShowVideoFormatBlock->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	mFFDShowVideoFormatBlock->rcSource.top = 0;
	mFFDShowVideoFormatBlock->rcSource.bottom = locHeight;
	mFFDShowVideoFormatBlock->rcSource.left = 0;
	mFFDShowVideoFormatBlock->rcSource.right = locWidth;

	mFFDShowVideoFormatBlock->rcTarget.top = 0;
	mFFDShowVideoFormatBlock->rcTarget.bottom = locHeight;
	mFFDShowVideoFormatBlock->rcTarget.left = 0;
	mFFDShowVideoFormatBlock->rcTarget.right = locWidth;

	mFFDShowVideoFormatBlock->dwBitErrorRate=0;
	return true;
}


bool FFDShowVideoStream::dispatchPacket(StampedOggPacket* inPacket) {
	//This is to get aroudn the extra mystery byte that ogg encapsulated divx gets at the start :|
	unsigned char* locBuff = new unsigned char[inPacket->packetSize() - 1];
	memcpy((void*)locBuff, (const void*) (inPacket->packetData() + 1), inPacket->packetSize() - 1);
	
	
	/*if (inPacket->endTime() == mLastGranulePos) {
		mGranuleOffset++;
	} else {
		mLastGranulePos = inPacket->endTime();
		mGranuleOffset = 0;
	}*/


	LONGLONG locStart = mLastTimeStamp;
	LONGLONG locEnd = (mGranuleOffset) * mFFDShowVideoFormatBlock->AvgTimePerFrame;
	mGranuleOffset++;
	
	mLastTimeStamp = (locEnd >= mLastTimeStamp)		?	locEnd
													:	mLastTimeStamp;

	debugLog << "Packet :    Start   =   "<<locStart<<"     -   End   =   "<<locEnd<<endl;
	StampedOggPacket* locPack = new StampedOggPacket(locBuff, inPacket->packetSize() - 1, true, locStart, locEnd, StampedOggPacket::DIRECTSHOW);
	return OggStream::dispatchPacket(locPack);
}
BYTE* FFDShowVideoStream::getFormatBlock() {

	return (BYTE*)mFFDShowVideoFormatBlock;
}

GUID FFDShowVideoStream::getMajorTypeGUID() {
	return MEDIATYPE_Video;
}
unsigned long FFDShowVideoStream::getFormatBlockSize() {
	//Do something
	return sizeof(VIDEOINFOHEADER);
}
GUID FFDShowVideoStream::getFormatGUID() {
	return FORMAT_VideoInfo;
}
GUID FFDShowVideoStream::getSubtypeGUID() {
	return mMediaSubTypeGUID;
}

LONGLONG FFDShowVideoStream::getCurrentPos() {
	return 0;
}