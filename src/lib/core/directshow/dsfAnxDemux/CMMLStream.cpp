//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
#include "cmmlstream.h"

CMMLStream::CMMLStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter, bool inAllowSeek)
	:	OggStream(inBOSPage, inOwningFilter, inAllowSeek)
	,	mCMMLFormatBlock(NULL)
{
	InitCodec(inBOSPage->getStampedPacket(0));
	//debugLog.open("G:\\logs\\cmmlstream.log", ios_base::out);
}

CMMLStream::~CMMLStream(void)
{
	delete mCMMLFormatBlock;
}

bool CMMLStream::AddPin() {
	createFormatBlock();
	CMediaType* locMediaType = createMediaType(	getMajorTypeGUID(),
												getSubtypeGUID(), 
												getFormatGUID(), 
												getFormatBlockSize(), 
												getFormatBlock());

	//LEAK CHECK::: Where does this get deleted ?
	OggDemuxSourcePin* locSourcePin = new CMMLSourcePin(	NAME("Ogg Source Pin"), 
																mOwningFilter, 
																mOwningFilter->theLock(), 
																mCodecHeaders, 
																locMediaType, 
																getPinName(),
																getNumBuffers(),
																getBufferSize());
	mStreamReady = true;
	mSourcePin = locSourcePin;
	
	return true;
	
}
bool CMMLStream::InitCodec(StampedOggPacket* inOggPacket) {
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::CMML;
	mAnxDataPacket = inOggPacket->clone();
	//mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	mNumHeadersNeeded = 1;		//TODO::: This should be 1 ???
	return true;
}

BYTE* CMMLStream::getFormatBlock() {

	return (BYTE*)mCMMLFormatBlock;

}
bool CMMLStream::createFormatBlock() {
	//Check where this gets deleted
	mCMMLFormatBlock = new sCMMLFormatBlock;
	mCMMLFormatBlock->granuleNumerator = iLE_Math::charArrToULong(mAnxDataPacket->packetData() + 8) + ((iLE_Math::charArrToULong(mAnxDataPacket->packetData() + 12)) << 32);
	mCMMLFormatBlock->granuleDenominator = iLE_Math::charArrToULong(mAnxDataPacket->packetData() + 16) + ((iLE_Math::charArrToULong(mAnxDataPacket->packetData() + 20)) << 32);

	return true;
}

void CMMLStream::setLastEndGranPos(__int64 inPos) {
	//debugLog<<"CMML Stream Reset : inPos = "<<inPos<<"   --  last end gran = ";
	//debugLog<<"gran rate = "<<mCMMLFormatBlock->granuleNumerator<<" / "<<mCMMLFormatBlock->granuleDenominator<<endl;
	mLastEndGranulePos = ((inPos * mCMMLFormatBlock->granuleNumerator) / mCMMLFormatBlock->granuleDenominator)/ UNITS;
	//debugLog<<mLastEndGranulePos<<endl;
	//osDebug<<"Vorbis sets End Gran : "<<mLastEndGranulePos<<endl;
}
unsigned long CMMLStream::getFormatBlockSize() {
	return sizeof(sCMMLFormatBlock);
}
GUID CMMLStream::getFormatGUID() {
	return FORMAT_CMML;
}
GUID CMMLStream::getSubtypeGUID() {
	return MEDIASUBTYPE_CMML;
}
wstring CMMLStream::getPinName() {
	wstring retName = L"CMML Out";
	return retName;
}
GUID CMMLStream::getMajorTypeGUID() {
	return MEDIATYPE_Text;
}

LONGLONG CMMLStream::getCurrentPos() {
	return (mLastEndGranulePos * UNITS * mCMMLFormatBlock->granuleDenominator) / mCMMLFormatBlock->granuleNumerator;
}

unsigned long CMMLStream::getNumBuffers() {
	return CMML_NUM_BUFFERS;
}
unsigned long CMMLStream::getBufferSize() {
	return CMML_BUFFER_SIZE;
}