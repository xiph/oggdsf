#include "StdAfx.h"
#include ".\autoanxseektable.h"

AutoAnxSeekTable::AutoAnxSeekTable(string inFileName)
	:	AutoOggSeekTable(inFileName)
	,	mAnxPackets(0)
	,	mSeenAnything(false)
	,	mAnnodexSerialNo(0)
	,	mReadyForOgg(false)
	,	mSkippedCMML(false)
{
}

AutoAnxSeekTable::~AutoAnxSeekTable(void)
{
}

//IOggCallback interface
bool AutoAnxSeekTable::acceptOggPage(OggPage* inOggPage) {
	if (mSeenAnything == false) {
		if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "Annodex", 7) == 0) {
			mAnnodexSerialNo = inOggPage->header()->StreamSerialNo();
			mSeenAnything = true;
			return true;
			//Need to grab other info here.
		} else {
			return false;
		}
	}

	if ((mAnnodexSerialNo == inOggPage->header()->StreamSerialNo()) && ((inOggPage->header()->HeaderFlags() & 4) != 0)) {
		//This is the EOS o the annodex section... everything that follows is ogg like
		mReadyForOgg = true;
		return true;
	}

	//if (mAnnodexSerialNo == inOggPage->header()->StreamSerialNo()) {
	//	//Ignore this stuff.
	//	return true;
	//} else if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "AnxData", 7) == 0) {
	//	//Seen all the annodex stuff.
	//	mReadyForOgg = true;
	//	return true;
	//}

	if (mReadyForOgg) {
		if (mSkippedCMML == false) {
			mSkippedCMML = true;
			return true;
		} else {
			return AutoOggSeekTable::acceptOggPage(inOggPage);
		}
	}
	
	return true;

}
