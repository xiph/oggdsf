#pragma once
#include "IOggCallback.h"
#include "IStampedOggPacketSink.h"
#include "OggPage.h"
class LIBOOOGG_API OggPacketiser
	:	public IOggCallback
{
public:

	enum ePacketiserState {
		PKRSTATE_OK,
		PKRSTATE_AWAITING_CONTINUATION,
		PKRSTATE_INVALID_STREAM
	};

	OggPacketiser(void);
	OggPacketiser(IStampedOggPacketSink* inPacketSink);
	virtual ~OggPacketiser(void);

	void setPacketSink(IStampedOggPacketSink* inPacketSink);
	//void setPacketSink(IStampedOggPacketSink* inPacketSink):
	IStampedOggPacketSink* packetSink();
	//void setPacketSink(IStampedOggPacketSink* inPacketSink):

	//IOggCallback implementation
	virtual bool acceptOggPage(OggPage* inOggPage);

	bool reset();
protected:
    IStampedOggPacketSink* mPacketSink;
	StampedOggPacket* mPendingPacket;

	bool processPage(OggPage* inOggPage, bool inIncludeFirst, bool inIncludeLast);
	ePacketiserState mPacketiserState;
};
