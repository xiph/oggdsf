#pragma once

#include "OggStreamMapper.h"
class AnxStreamMapper
	:	public OggStreamMapper
{
public:
	AnxStreamMapper(void);
	AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter);
	virtual ~AnxStreamMapper(void);

	virtual bool acceptOggPage(OggPage* inOggPage);

protected:
	vector<unsigned long> mSeenStreams;
	bool mSeenAnnodexBOS;
	bool mReadyForCodecs;
	unsigned long mAnnodexSerial;
};
