#include "StdAfx.h"
#include ".\testpatternoggpacketchecker.h"

TestPatternOggPacketChecker::TestPatternOggPacketChecker(void)
{
}

TestPatternOggPacketChecker::~TestPatternOggPacketChecker(void)
{
}

bool TestPatternOggPacketChecker::acceptStampedOggPacket(StampedOggPacket* inPacket) 
{
	unsigned long locPacketSize = inPacket->packetSize();

	const unsigned char* locPackBuff = inPacket->packetData();		//Don't delete
	
	bool retVal = false;

	if (locPacketSize == 0) {
		retVal = true;
	} else if (locPacketSize < 8) {
		for (int i = 0; i < 8; i++) {
			if (locPackBuff[i] != i) {
				retVal = false;
			}
		}
		return true;
	} else {
		for (int i = 0; i < 4; i++) {
			if (locPackBuff[i] != '[') {
				retVal = false;
			}
		}

		for (int i = 4; i < locPacketSize - 4; i++) {
			if (locPackBuff[i] != ('a' + (unsigned char)(i % 26))) {
				retVal = false;
			}
		}

		for (int i = 1; i <=4; i++) {
			if (locPackBuff[locPacketSize - i] != ']') {
				retVal = false;
			}
		}
		retVal = true;
	}

	cout<<"Check Packet Granule "<<inPacket->endTime()<<"   Size = "<<inPacket->packetSize()<<"   ";
	if (retVal) {
		cout<<"OK"<<endl;
	} else {
		cout<<"FAILED"<<endl;
	}
	delete inPacket;
	return retVal;
}