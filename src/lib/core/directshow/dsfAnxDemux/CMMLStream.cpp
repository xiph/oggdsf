#include "StdAfx.h"
#include "cmmlstream.h"

CMMLStream::CMMLStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter)
	:	OggStream(inBOSPage, inOwningFilter)
	,	mCMMLFormatBlock(NULL)
{
	InitCodec(inBOSPage->getStampedPacket(0));
}

CMMLStream::~CMMLStream(void)
{
	delete mCMMLFormatBlock;
}


bool CMMLStream::InitCodec(StampedOggPacket* inOggPacket) {
	mCodecHeaders = new StreamHeaders;
	mCodecHeaders->mCodecType = StreamHeaders::CMML;
	mAnxDataPacket = inOggPacket->clone();
	//mCodecHeaders->addPacket((StampedOggPacket*)inOggPacket->clone());
	mNumHeadersNeeded = 0;
	return true;
}

BYTE* CMMLStream::getFormatBlock() {

	return (BYTE*)mCMMLFormatBlock;

}
bool CMMLStream::createFormatBlock() {
	//Check where this gets deleted
	mCMMLFormatBlock = new sCMMLFormatBlock;
	mCMMLFormatBlock->granuleNumerator = OggMath::charArrToULong(mAnxDataPacket->packetData() + 8) + ((OggMath::charArrToULong(mAnxDataPacket->packetData() + 12)) << 32);
	mCMMLFormatBlock->granuleDenominator = OggMath::charArrToULong(mAnxDataPacket->packetData() + 16) + ((OggMath::charArrToULong(mAnxDataPacket->packetData() + 20)) << 32);

	return true;
}

void CMMLStream::setLastEndGranPos(__int64 inPos) {
	
	mLastEndGranulePos = ((inPos * mCMMLFormatBlock->granuleNumerator) / mCMMLFormatBlock->granuleDenominator)/ UNITS;
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