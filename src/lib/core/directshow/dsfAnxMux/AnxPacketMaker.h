#pragma once

#include "StreamHeaders.h"

class AnxPacketMaker
{
public:
	AnxPacketMaker(void);
	~AnxPacketMaker(void);

	static const ANX_2_0_ANNODEX_BOS_SIZE = 48;

	static OggPage* makeAnnodexBOS	(				unsigned long inSerialNo
												,	unsigned short inVersionMajor
												,	unsigned short inVersionMinor
												,	unsigned __int64 inTimebaseNum
												,	unsigned __int64 inTimebaseDenom
												,	const unsigned char* inUTC
											);

	static StampedOggPacket* makeAnxData	(		unsigned short inVersionMajor
												,	unsigned short inVersionMinor
												,	unsigned __int64 inGranuleRateNum
												,	unsigned __int64 inGranuleDenom
												,	unsigned long inNumSecHeaders
												,	vector<string> inMessageHeaders
											);

	static StampedOggPacket* makeAnxData_2_0 (OggMuxStream* inMuxStream, OggPaginator* inPaginator);
	static StampedOggPacket* makeAnxData (OggMuxStream* inMuxStream, OggPaginator* inPaginator);

	static StreamHeaders::eCodecType AnxPacketMaker::IdentifyCodec(OggPacket* inOggPacket);
	static vector<string> AnxPacketMaker::makeMessageHeaders(OggMuxStream* inMuxStream);

};
