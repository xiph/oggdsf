#include "StdAfx.h"
#include ".\anxmuxinputpin.h"
#include "AnxMuxFilter.h"
AnxMuxInputPin::AnxMuxInputPin(AnxMuxFilter* inOwningFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream)
:	OggMuxInputPin(inOwningFilter, inFilterLock, inHR, inMuxStream)
{
}

AnxMuxInputPin::~AnxMuxInputPin(void)
{
}

HRESULT AnxMuxInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//This pushes an anxdata packet into the stream.
	HRESULT locHR = OggMuxInputPin::SetMediaType(inMediaType);

	if (locHR == S_OK) {

		mPaginator.acceptStampedOggPacket(AnxPacketMaker::makeAnxData_2_0(mMuxStream, &mPaginator));
	}

	return locHR;


}