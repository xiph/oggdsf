#include "StdAfx.h"
#include ".\testpatternoggpagechecker.h"

TestPatternOggPageChecker::TestPatternOggPageChecker(void)
{
}

TestPatternOggPageChecker::~TestPatternOggPageChecker(void)
{
}

bool TestPatternOggPageChecker::acceptOggPage(OggPage* inOggPage) 
{
	bool locIsOK = true;
	for (int i = 0; i < inOggPage->numPackets(); i++) {
		locIsOK = locIsOK && (mPacketChecker.acceptStampedOggPacket( (StampedOggPacket*)(inOggPage->getStampedPacket(i)->clone()) ) );
	}

	cout<<"Check Page "<<inOggPage->header()->PageSequenceNo()<<" @ "<<inOggPage->header()->GranulePos()<<"  Size = "<<inOggPage->pageSize()<<"   Num Packs = "<<inOggPage->numPackets()<<"     ";
	if (locIsOK) {
		cout<<"OK"<<endl;
	} else {
		cout<<"FAILED"<<endl;
	}
	cout<<endl;
	delete inOggPage;
	return (locIsOK);
}