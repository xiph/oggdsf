#include "StdAfx.h"
#include ".\anxmuxinputpin.h"
#include "AnxMuxFilter.h"
AnxMuxInputPin::AnxMuxInputPin(AnxMuxFilter* inOwningFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream)
:	OggMuxInputPin(inOwningFilter, inFilterLock, inHR, inMuxStream)
{
	debugLog.open("g:\\logs\\anxmuxinputpin.log", ios_base::out);
}

AnxMuxInputPin::~AnxMuxInputPin(void)
{
}

HRESULT AnxMuxInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//This pushes an anxdata packet into the stream.
	HRESULT locHR = OggMuxInputPin::SetMediaType(inMediaType);

	if (locHR == S_OK) {
		debugLog<<"Set media type ok in base class"<<endl;
		mPaginator.acceptStampedOggPacket(AnxPacketMaker::makeAnxData_2_0(mMuxStream, &mPaginator));
	}

	return locHR;


}