#pragma once
#include "IOggCallback.h"
#include "IStampedOggPacketSink.h"
#include "OggPage.h"

//Debug only
#include <fstream>
using namespace std;
//

//TODO::: Loose mode controls
class LIBOOOGG_API OggPacketiser
	:	public IOggCallback
{
public:
	//Constants
	enum ePacketiserState {
		PKRSTATE_OK,
		PKRSTATE_AWAITING_CONTINUATION,
		PKRSTATE_INVALID_STREAM
	};

	//Constructors
	OggPacketiser(void);
	OggPacketiser(IStampedOggPacketSink* inPacketSink);
	virtual ~OggPacketiser(void);

	//IOggCallback implementation
	virtual bool acceptOggPage(OggPage* inOggPage);

	//Packet sink control
	void setPacketSink(IStampedOggPacketSink* inPacketSink);
	IStampedOggPacketSink* packetSink();

	//Ignore packets controls.
	void setNumIgnorePackets(unsigned long inNumIgnorePackets);
	unsigned long numIgnorePackets();

	//Packetiser state control
	bool reset();

protected:
    IStampedOggPacketSink* mPacketSink;
	StampedOggPacket* mPendingPacket;

	virtual bool dispatchStampedOggPacket(StampedOggPacket* inPacket);

	bool mLooseMode;
	unsigned long mNumIgnorePackets;
	__int64 mPrevGranPos;
	__int64 mCurrentGranPos;
	bool processPage(OggPage* inOggPage, bool inIncludeFirst, bool inIncludeLast);
	ePacketiserState mPacketiserState;

	//fstream debugLog;
};
