#include "StdAfx.h"
#include "anxstreammapper.h"

AnxStreamMapper::AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter)
	:	OggStreamMapper(inOwningFilter)
	,	mAnnodexSerial(0)
	,	mSeenAnnodexBOS(false)
	,	mReadyForCodecs(false)
{
}

AnxStreamMapper::~AnxStreamMapper(void)
{
}

bool AnxStreamMapper::acceptOggPage(OggPage* inOggPage) 
{
	
	if(!isReady()) {
		mDataStartsAt += inOggPage->pageSize();
	}

	if (!mReadyForCodecs) {
		if (inOggPage->header()->isBOS()) {
			if (!mSeenAnnodexBOS) {
				//This is an annodex BOS page
				//Need to verify it's an annodex page too
				mAnnodexSerial = inOggPage->header()->StreamSerialNo();
				mSeenAnnodexBOS = true;
				
				//Need to save the data from the header here.
			} else {
				//This is anxdata

				//Need to save header data here.
				mSeenStreams.push_back(inOggPage->header()->StreamSerialNo());
			}
		}


		if (inOggPage->header()->isEOS()) {
			if (mSeenAnnodexBOS) {
				//This is the annodex EOS page
				mReadyForCodecs = true;
			} else {
				//ERROR... got an EOS before we've seen the annodex BOS
				return false;
			}
		}
	} else {
		vector<unsigned long>::iterator it;
		int i;
		for(i = 0, it = mSeenStreams.begin(); it != mSeenStreams.end(); i++, ++it) {
		//for (int i = 0; i < mSeenStreams.size(); i++) {
			if (mSeenStreams[i] == inOggPage->header()->StreamSerialNo()) {
				//If the page is a BOS we need to start a new stream
				OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter);
				//FIX::: Need to check for NULL
				if (locStream != NULL) {
					mStreamList.push_back(locStream);
				}
				mSeenStreams.erase(it);
				return true;
			}
		}

		//If we are here, the stream is not in the list.
		//At the moment we assume it's because it's been seen, and removed... this is a bad assumption !

		return dispatchPage(inOggPage);
	}
}