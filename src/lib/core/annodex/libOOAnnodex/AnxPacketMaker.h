#pragma once

#include "libOOOgg.h"
#include "OggStreamFactory.h"
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
												,	const char* inUTC
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

	
	static vector<string> AnxPacketMaker::makeMessageHeaders(OggMuxStream* inMuxStream);

};
