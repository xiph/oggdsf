#include "stdafx.h"
#include "anxdemuxsourcepin.h"

AnxDemuxSourcePin::AnxDemuxSourcePin(AnxDemuxSourceFilter* inParentFilter, CCritSec* inLock)
	:	OggDemuxSourcePin(NAME("AnxDemuxSourcePin"), inParentFilter, 
{
}

AnxDemuxSourcePin::~AnxDemuxSourcePin(void)
{
}
