#pragma once
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/IStampedOggPacketSink.h>
class OOOggPacketDumper
	:	public IStampedOggPacketSink
{
public:
	OOOggPacketDumper(void);
	~OOOggPacketDumper(void);

	//IStampedOggPacketSink Implementation
	bool acceptStampedOggPacket(StampedOggPacket* inPacket);
};
