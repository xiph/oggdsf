#include "StdAfx.h"
#include ".\oooggpacketdumper.h"

OOOggPacketDumper::OOOggPacketDumper(void)
{
}

OOOggPacketDumper::~OOOggPacketDumper(void)
{
}
bool OOOggPacketDumper::acceptStampedOggPacket(StampedOggPacket* inPacket) {

	cout << "------ Packet : Gran Pos =  "<<inPacket->endTime()<<", Size = "<< inPacket->packetSize() << " bytes -------";
	
	if (inPacket->isContinuation()) {
		cout<<"  ** CONT **";			///These should be here if you use the packetiser
	}
	if (inPacket->isTruncated()) {
		cout<<" ** TRUNC **";		////These sohuldn't be here if using the packetiser
	}
	cout<<endl;
	cout << inPacket->toPackDumpString();
	return true;
}