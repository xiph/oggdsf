#pragma once

#include <libOOOgg/IOggCallback.h>
#include <vector>

class OggStreamMapper:	public IOggCallback
{
public:

	enum eStreamState {
		STRMAP_READY,
		STRMAP_PARSING_BOS_PAGES,
		STRMAP_PARSING_HEADERS,
		STRMAP_DATA,
		STRMAP_FINISHED,
		STRMAP_ERROR

	};
	OggStreamMapper(OggDemuxPacketSourceFilter* inParentFilter, CCritSec* inParentFilterLock);
	virtual ~OggStreamMapper(void);

	//IOggCallback Interface
	virtual bool acceptOggPage(OggPage* inOggPage);

	eStreamState streamState();

	bool allStreamsReady();

	unsigned long numPins()				{		return mPins.size();		}
	OggDemuxPacketSourcePin* getPinByIndex(unsigned long inIndex);

protected:
	eStreamState mStreamState;
    std::vector<OggDemuxPacketSourcePin*> mPins;
	OggDemuxPacketSourceFilter* mParentFilter;
	CCritSec* mParentFilterLock;

	OggPacket* mFishHeadPacket;
	unsigned long mSkeletonSerialNo;

	bool addNewPin(OggPage* inOggPage);
	OggDemuxPacketSourcePin* getMatchingPin(unsigned long inSerialNo);

	bool handleFishHead(OggPage* inOggPage);
	bool isFishHead(OggPage* inOggPage);
};
