#pragma once

#include "oggmuxdllstuff.h"
#include "OggMuxInputPin.h"
#include "AnxMuxFilter.h"
#include "AnxPacketMaker.h"
class AnxMuxInputPin
	:	public OggMuxInputPin
{
public:
	AnxMuxInputPin(AnxMuxFilter* inOwningFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream);
	~AnxMuxInputPin(void);


	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
};
