#include "stdafx.h"
#include ".\testpatternoggpacketgenerator.h"

TestPatternOggPacketGenerator::TestPatternOggPacketGenerator(void)
{
}

TestPatternOggPacketGenerator::~TestPatternOggPacketGenerator(void)
{
}

StampedOggPacket* TestPatternOggPacketGenerator::generatePacket(unsigned long inPacketSize, __int64 inGranulePos) 
{
	cout<<"Generating packet of size "<<inPacketSize<<" with gran pos "<<inGranulePos<<endl;
	if (inPacketSize == 0) {
		
		return new StampedOggPacket(NULL, 0, false, false, 0, inGranulePos, StampedOggPacket::OGG_END_ONLY);
	}
	if (inPacketSize <= 8) {
		unsigned char* locBuff = new unsigned char[inPacketSize];

		//Numbers 01234567
		for (int i = 0; i < inPacketSize; i++) {
			locBuff[i] = ('0' + (unsigned  char)i);;
		}
		return new StampedOggPacket(locBuff, inPacketSize, false, false, 0, inGranulePos, StampedOggPacket::OGG_END_ONLY);

	} else {

		//        [[[[abcdefghij...abcde...]]]]
		unsigned char* locBuff = new unsigned char[inPacketSize];

		for (int i = 0; i < 4; i++) {
			locBuff[i] = '[';	
		}

		for (int i = 4; i < inPacketSize - 4; i++) {
			locBuff[i] = 'a' + (unsigned char)(i % 26); 
		}

		for (int i = 1; i <= 4; i++) {
			locBuff[inPacketSize - i] = ']';
		}

		return new StampedOggPacket(locBuff, inPacketSize, false, false, 0, inGranulePos, StampedOggPacket::OGG_END_ONLY);

	}

}