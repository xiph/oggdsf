#include "stdafx.h"
#include "oggdemuxfilter.h"

OggDemuxFilter::OggDemuxFilter(void)
{
}

OggDemuxFilter::~OggDemuxFilter(void)
{
}


CBasePin* OggDemuxFilter::GetPin(int inPinNo) {
	if ((inPinNo < 0) || (inPinNo >= mPinList.size())) {
		//Out of range
		return NULL;
	} else if (inPinNo == 0) {
		return mInputPin;
	} else {
		return mOutputPinList[inPinNo];
	}
}
int OggDemuxFilter::GetPinCount() {
	return mPinList.size() + 1;
}
