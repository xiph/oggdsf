#pragma once

#include "OggStreamMapper.h"
#include "CMMLStream.h"
class AnxStreamMapper
	:	public OggStreamMapper
{
public:
	AnxStreamMapper(void);
	AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter);
	virtual ~AnxStreamMapper(void);

	virtual bool acceptOggPage(OggPage* inOggPage);

	virtual bool AnxStreamMapper::isReady();
protected:
	vector<unsigned long> mSeenStreams;
	bool mSeenAnnodexBOS;
	bool mReadyForCodecs;
	bool mSeenCMML;
	unsigned long mAnnodexSerial;
};
