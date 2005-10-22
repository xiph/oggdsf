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
#include "ffdshowvideostream.h"



FFDShowVideoStream::FFDShowVideoStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mFFDShowVideoFormatBlock(NULL)
	,	mLastTimeStamp(0)
	,	mLastKnownTimeBase(0)
	,	mGranuleOffset(0)
{
	InitCodec(inBOSPage->getStampedPacket(0));
	//debugLog.open("g:\\logs\\ffd_dump.out", ios_base::out);
}

FFDShowVideoStream::~FFDShowVideoStream(void)
{
	//debugLog.close();
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
	mNumHeadersNeeded = 1;
	//mFirstRun = false;
	return true;
}


bool FFDShowVideoStream::deliverCodecHeaders() {
	StampedOggPacket* locPacket = NULL;

	//TODO::: Why 2 ? - there is a comment header
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

	
	__int64 locTimePerBlock = iLE_Math::CharArrToInt64(mHeaderPack->packetData() + 17);

	
	__int64 locSamplesPerBlock = iLE_Math::CharArrToInt64(mHeaderPack->packetData() + 25);

	//debugLog<<"t/block = "<<locTimePerBlock<<"        Sam/block = "<<locSamplesPerBlock<<endl;

	mFFDShowVideoFormatBlock->AvgTimePerFrame = locTimePerBlock / locSamplesPerBlock;

	//debugLog<<"Time per frame = "<<mFFDShowVideoFormatBlock->AvgTimePerFrame<<endl;

	__int64 locFPSec = (UNITS / locTimePerBlock) * locSamplesPerBlock;

	//debugLog<<"Rate = "<<locFPSec<<" fps"<<endl;
	unsigned short locBPSample = ((unsigned char)(mHeaderPack->packetData()[41])) + (((unsigned short)(mHeaderPack->packetData()[42])) * 256);

	
	
	mFFDShowVideoFormatBlock->bmiHeader.biBitCount = locBPSample;
	mFFDShowVideoFormatBlock->bmiHeader.biClrImportant = 0;   //All colours important
	mFFDShowVideoFormatBlock->bmiHeader.biClrUsed = 0;        //Use max colour depth
	mFFDShowVideoFormatBlock->bmiHeader.biCompression = mFourCCCode;

	unsigned long locHeight = iLE_Math::charArrToULong(mHeaderPack->packetData() + 49);
	unsigned long locWidth =  iLE_Math::charArrToULong(mHeaderPack->packetData() + 45);

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
	unsigned long locLenBytes = ((((unsigned long)inPacket->packetData()[0]) >> 4) | (inPacket->packetData()[0] & 2)) >> 1;
	unsigned char* locBuff = new unsigned char[inPacket->packetSize() - 1];
	memcpy((void*)locBuff, (const void*) (inPacket->packetData() + locLenBytes), inPacket->packetSize() - locLenBytes);
	

	//This is to help ffdshow handle timestamps the way it likes them.
	//Everytime, the start time changes, we record it and reset the granule counter (frame count)
	//This lets ogm stuff be seekable cleanly.
	if ((mLastKnownTimeBase != inPacket->startTime()) && (inPacket->startTime() != -1)) {
		mLastKnownTimeBase = inPacket->startTime();
		mLastTimeStamp = mLastKnownTimeBase * mFFDShowVideoFormatBlock->AvgTimePerFrame;
		//debugLog<<"Last Time base set  to  "<<mLastKnownTimeBase<<endl;
		//debugLog<<"Last time stamp set to "<<mLastTimeStamp<<endl;

		//Granule Offset may not be needed any more.
		mGranuleOffset = 0;
	}

	//debugLog<<"Packet stamps = "<<inPacket->startTime() << " - "<<inPacket->endTime()<<endl;

	//debugLog<<"m_tStart = "<<mSourcePin->CurrentStartTime()<<endl;
	LONGLONG locStart = mLastTimeStamp - mSourcePin->CurrentStartTime();
	LONGLONG locEnd = locStart + mFFDShowVideoFormatBlock->AvgTimePerFrame;
	mGranuleOffset++;

	//debugLog<<"Time Stamps = "<<locStart<<" - "<<locEnd<<endl;
	//debugLog<<"Granule offset " << mGranuleOffset<<endl;
	
	mLastTimeStamp = (locEnd >= mLastTimeStamp)		?	locEnd
													:	mLastTimeStamp;

	//debugLog << "Packet :    Start   =   "<<locStart<<"     -   End   =   "<<locEnd<<endl;
																					//We should only be delivering full packets here.
	StampedOggPacket* locPack = new StampedOggPacket(locBuff, inPacket->packetSize() - locLenBytes, false, false, locStart, locEnd, StampedOggPacket::DIRECTSHOW);
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

unsigned long FFDShowVideoStream::getNumBuffers() {
	return FFDSHOW_VIDEO_NUM_BUFFERS;
}
unsigned long FFDShowVideoStream::getBufferSize() {
	return FFDSHOW_VIDEO_BUFFER_SIZE;
}
