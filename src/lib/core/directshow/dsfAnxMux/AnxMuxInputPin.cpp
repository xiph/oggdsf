#include "StdAfx.h"
#include ".\anxmuxinputpin.h"

AnxMuxInputPin::AnxMuxInputPin(void)
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

		mPaginator.addPacketToPage(AnxPacketMaker::makeAnxData(mMuxStream));
	}

	return locHR;


}