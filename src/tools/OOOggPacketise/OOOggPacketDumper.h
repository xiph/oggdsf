#pragma once
#include "libOOOgg.h"
#include "IStampedOggPacketSink.h"
class OOOggPacketDumper
	:	public IStampedOggPacketSink
{
public:
	OOOggPacketDumper(void);
	~OOOggPacketDumper(void);

	//IStampedOggPacketSink Implementation
	bool acceptStampedOggPacket(StampedOggPacket* inPacket);
};
