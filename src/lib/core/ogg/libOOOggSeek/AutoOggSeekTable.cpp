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
#include "autooggseektable.h"

AutoOggSeekTable::AutoOggSeekTable(string inFileName)
	:	mFilePos(0)
	,	mLastSeekTime(0)
	,	mPacketCount(0)
	,	mSampleRate(0)
	,	mFileDuration(0)
	,	mNumHeaders(0)
	,	mSerialNoToTrack(LINT_MAX)
	,	isTheora(false)
	,	mGranulePosShift(0)
	,	mLastIsSeekable(false)

{
	
	mFileName = inFileName;
	mOggDemux.registerVirtualCallback(this);
}

AutoOggSeekTable::~AutoOggSeekTable(void)
{
}

bool AutoOggSeekTable::acceptOggPage(OggPage* inOggPage) {
	mPacketCount += inOggPage->numPackets();

	//TODO ::: Some of this could be shared from other places.
	if (mPacketCount == 1) {
		if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "\001vorbis", 7) == 0) {
			mSampleRate = OggMath::charArrToULong(inOggPage->getPacket(0)->packetData() + 12);
			mNumHeaders = 3;
			mSerialNoToTrack = inOggPage->header()->StreamSerialNo();
		} else if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "Speex   ", 8) == 0) {
			mSampleRate = OggMath::charArrToULong(inOggPage->getPacket(0)->packetData() + 36);
			mNumHeaders = 2;
			mSerialNoToTrack = inOggPage->header()->StreamSerialNo();
		} else if ((strncmp((char*)inOggPage->getPacket(0)->packetData(), "\200theora", 7)) == 0){
			//FIX ::: Dunno what this is... do something better than this later !!
			//mEnabled = false;
			//mPacketCount == 0;
			isTheora = true;
			mSerialNoToTrack = inOggPage->header()->StreamSerialNo();
			mGranulePosShift = (((inOggPage->getPacket(0)->packetData()[40]) % 4) << 3) + ((inOggPage->getPacket(0)->packetData()[41]) >> 5);
			mSampleRate = FLACMath::charArrToULong(inOggPage->getPacket(0)->packetData() + 22) / FLACMath::charArrToULong(inOggPage->getPacket(0)->packetData() + 26);
			mNumHeaders = 3;
			//Need denominators
			//mTheoraFormatBlock->frameRateDenominator = FLACMath::charArrToULong(locIdentHeader + 26);
		} else {
			mEnabled = false;
			mSampleRate = 1;
			
		}
	}


	if (mSerialNoToTrack == inOggPage->header()->StreamSerialNo()) {
		//if ((mPacketCount > 3) && (mLastIsSeekable == true)) {
		if ((mPacketCount > mNumHeaders) && ((inOggPage->header()->HeaderFlags() & 1) != 1)) {
			addSeekPoint(mLastSeekTime, mFilePos);
			
		}

		mLastIsSeekable = true;
		
		if (isTheora) {
			unsigned long locMod = (unsigned long)pow(2, mGranulePosShift);
			unsigned long locInterFrameNo = ((inOggPage->header()->GranulePos()->value()) % locMod);
			
			//if (locInterFrameNo == 0) {
			//	mLastIsSeekable = true;
			//} else {
			//	mLastIsSeekable = false;
			//}
			mLastSeekTime = ((((inOggPage->header()->GranulePos()->value()) >> mGranulePosShift) + locInterFrameNo) * DS_UNITS) / mSampleRate;
		} else {
			mLastSeekTime = ((inOggPage->header()->GranulePos()->value()) * DS_UNITS) / mSampleRate;
			//stDebug<<"Last Seek Time : "<<mLastSeekTime;
		}
		if (((inOggPage->header()->HeaderFlags() & 1) == 1)) {
			//stDebug <<"    NOT SEEKABLE";
			mLastIsSeekable = false;
		}
		//stDebug<<endl;
		mFileDuration = mLastSeekTime;
	}
	mFilePos += inOggPage->pageSize();
	//stDebug<<"File Pos : "<<mFilePos<<endl;
	return true;
}

__int64 AutoOggSeekTable::fileDuration() {
	return mFileDuration;
}
bool AutoOggSeekTable::buildTable() {
	//HACK::: To ensure we don't try and build a table on the network file.
	if (mFileName.find("http") != 0) {
		mFile.open(mFileName.c_str(), ios_base::in | ios_base::binary);
		const unsigned long BUFF_SIZE = 4096;
		unsigned char* locBuff = new unsigned char[BUFF_SIZE];
		while (!mFile.eof()) {
			mFile.read((char*)locBuff, BUFF_SIZE);
			mOggDemux.feed((const char*)locBuff, mFile.gcount());
		}
		mFile.close();
		
	} else {
		mEnabled = false;
		mSampleRate = 1;
	}

	return true;
}