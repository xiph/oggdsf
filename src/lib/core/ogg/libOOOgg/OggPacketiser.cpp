#include "StdAfx.h"
#include ".\oggpacketiser.h"

OggPacketiser::OggPacketiser(void) 
	:	mPacketSink(NULL)
	,	mPendingPacket(NULL)
	,	mPacketiserState(PKRSTATE_OK)
	,	mLooseMode(true)						//FIX::: This affects the validator.
	,	mNumIgnorePackets(0)
	,	mPrevGranPos(0)
	,	mCurrentGranPos(0)
{

}
OggPacketiser::OggPacketiser(IStampedOggPacketSink* inPacketSink)
	:	mPacketSink(inPacketSink)
	,	mPendingPacket(NULL)
	,	mPacketiserState(PKRSTATE_OK)
	,	mLooseMode(true)						//FIX::: This affects the validator.
	,	mNumIgnorePackets(0)
	,	mPrevGranPos(0)
	,	mCurrentGranPos(0)
{

}

OggPacketiser::~OggPacketiser(void)
{
	//Don't delete the packet sink
}

IStampedOggPacketSink* OggPacketiser::packetSink() {
	return mPacketSink;
}
void OggPacketiser::setPacketSink(IStampedOggPacketSink* inPacketSink) {
	mPacketSink = inPacketSink;
}
bool OggPacketiser::reset() {
	delete mPendingPacket;
	mPendingPacket = NULL;
	mNumIgnorePackets = 0;
	mPacketiserState = PKRSTATE_OK;
	mPrevGranPos = 0;
	mCurrentGranPos = 0;
	return true;
}
bool OggPacketiser::acceptOggPage(OggPage* inOggPage) {
	//All callers to acceptOggPage give away their pointer
	// to this function. All functions implementing this interface
	// are responsible for deleting this page. All callers
	// should NULL their pointer immediately after calling
	// to avoid reusing them.
	// 

	//If the page isn't a -1 page and it's got a different granpos save it.
	if ( (inOggPage->header()->GranulePos() != -1) && (inOggPage->header()->GranulePos() != mCurrentGranPos)) {
		mPrevGranPos = mCurrentGranPos;

		//If the previous is higher than the
		if (mPrevGranPos > mCurrentGranPos) {
			mPrevGranPos = -1;
		}
		mCurrentGranPos = inOggPage->header()->GranulePos();
	}

	//If the page header says its a continuation page...
	if ((inOggPage->header()->HeaderFlags() & 1) == 1) {
		///... and there is at least 1 packet...
		if (inOggPage->numPackets() > 0) {
			//... and we were expecting a continuation...
			if (mPacketiserState == PKRSTATE_AWAITING_CONTINUATION) {
				//... and the first packet is marked as a continuation...
				if (inOggPage->getStampedPacket(0)->isContinuation()) {
					//... merge this packet into our pending page.
					//ASSERT when mPacketiserState = PKRSTATE_AWAITING_CONTINUATION, mPending page != NULL
					mPendingPacket->merge(inOggPage->getStampedPacket(0));
					
					//If even after merging this packet is still truncated...
					if (mPendingPacket->isTruncated()) {
						//Packet still not full. special case full page.
						//===
						// The only way the the pending packet can be truncated is if
						//  the first packet in the page is truncated, and the first
						//  packet in a page can only be truncated if it's also the 
						//  only packet on the page.
						//Considering it is incomplete ending a page the granule pos
						// will be -1.
						//This is a special type of page :
						// 1 incomlpete packet on the page
						// Continuation flag set
						// No complete packets end on this page
						// Granule pos is -1


						//We are still waiting for another continuation...
						mPacketiserState = PKRSTATE_AWAITING_CONTINUATION;		//This should be redundant, we should already be in this state.
						//First packet on page is now merged into pending packet.
					} else {
						//... the pending packet is now complete.
						
						//TODO::: Static alternative here ?
						
						//Deliver the packet to the packet sink...
						if (dispatchStampedOggPacket(mPendingPacket) == false) {
							return false;
						}
						
						//Go back to OK state
						mPacketiserState = PKRSTATE_OK;
						mPendingPacket = NULL;
						//First packet on page is merged and delivered.
					}
					//Send every packet except the first and last to the packet sink.
					processPage(inOggPage, false, false);
				} else {
					//Header flag says continuation but first packet is not continued.
					mPacketiserState = PKRSTATE_INVALID_STREAM;
					throw 0;
				}
			} else {
				if (mLooseMode == true) {
					//Just ignore when we get continuation pages, just drop the broken bit of packet.

					mPendingPacket = NULL;   //MEMCHECK::: Did i just leak memory ?
					mPacketiserState = PKRSTATE_OK;

					//TODO::: Should really return false here if this returns false.
					if( processPage(inOggPage, false, false) == false) {
						//TODO::: State change ???
						return false;
					}
				} else {
					//Unexpected continuation
					mPacketiserState = PKRSTATE_INVALID_STREAM;
					throw 0;
				}
			}
		} else {
			//Is this something ?
			//UNKNOWN CASE::: Header continuation flag set, but no packets on page.
			mPacketiserState = PKRSTATE_INVALID_STREAM;
			throw 0;
		}
	} else {
		//Normal page, no continuations... just dump the packets, except the last one
		if (inOggPage->numPackets() == 1) {
			if (processPage(inOggPage, true, true) == false ) {			//If there was only one pack process it.
				//TODO::: State change
				return false;
			}
		} else {
			if (processPage(inOggPage, true, false) == false ) {			//If there was only one packet, no packets would be written
				//TODO::: State change
				return false;			
			}
		}
		
		//The first packet is delivered.
	}

	//ASSERT: By this point something has been done with the first packet.

	// It was either merged with pending page and possibly delivered
	// or it was delivered by process page.
	//Code following assumes the first packet is dealt with already.
	
	//Now we deal with the last packet...
	//ASSERT : The last packet has only been sent if there was 1 or less packets.

	//If there is at least two packet on the page... ie at least one more packet we haven't processed.
	if (inOggPage->numPackets() > 1) {
		//... and we are in the OK state
		if (mPacketiserState == PKRSTATE_OK) {
			//If the last packet is truncated.
			if (inOggPage->getPacket(inOggPage->numPackets() - 1)->isTruncated()) {
				//The last packet is truncated. Save it and await continuation.
				
				mPacketiserState = PKRSTATE_AWAITING_CONTINUATION;
			
				//ASSERT when mPacketiserState = OK, mPendingPacket = NULL
				mPendingPacket = (StampedOggPacket*)(inOggPage->getStampedPacket(inOggPage->numPackets() - 1)->clone());
				//This packet is not delivered, it waits for a continuation.
			} else {
				//We are in the OK state, with no pending packets, and the last packet is not truncated.

				//Deliver to the packet sink.
				if ( dispatchStampedOggPacket( (StampedOggPacket*)(inOggPage->getStampedPacket(inOggPage->numPackets() - 1)->clone()) ) == false ) {
					//TODO::: State change ?
					return false;
				}
				//The last packet is complete. So send it.
			}
		} else if (mPacketiserState == PKRSTATE_AWAITING_CONTINUATION) {
			//FIX::: This case should never occur.
			
			//Packetiser state is not ok... what to do abo8ut it.
			
			//See special page case above.
			//This can only happen when we went through the special case above, and kept
			// the state in  the continuation state. But by definition it is impossible
			// for a subsequent packet on this page to be a continuation packet
			// as continuation packets can only be the first packet on the page.
			//This is more likely to be due to inconsistency of state code than invalidaity
			// of file.
			mPacketiserState = PKRSTATE_INVALID_STREAM;
			throw 0;
		} else {
			//Shouldn't be here
			mPacketiserState = PKRSTATE_INVALID_STREAM;
			throw 0;
		}
	} else {
		//Zero packets on page.
	}
	
	return true;
}

bool OggPacketiser::processPage(OggPage* inOggPage, bool inIncludeFirst, bool inIncludeLast) {
	//Returns false only if one of the acceptStampedOggPacket calls return false... means we should stop sending stuff and return.
	bool locIsOK = true;

	//Adjusts the loop parameters so that only packets excluding those specified are written.
	for (	int i = ((inIncludeFirst) ? 0 : 1); 
			i < ((int)inOggPage->numPackets()) - ((inIncludeLast) ? 0 : 1);
			i++) 
	{
			
				locIsOK = (locIsOK && dispatchStampedOggPacket(inOggPage->getStampedPacket(i)));
				if (!locIsOK) {
					//TODO::: State change ???
					return false;
				}
	}
	return true;

}

bool OggPacketiser::dispatchStampedOggPacket(StampedOggPacket* inPacket) {
	if (mNumIgnorePackets > 0) {
		//Ignore this packet.
		mNumIgnorePackets--;

		//MEMCHECK::: Should probably delete this packet here.
		return true;
	} else {
		//Modify the header packet to include the gran pos of previous page.
		if (mPrevGranPos != -1) {
			inPacket->setTimeStamp(mPrevGranPos, mCurrentGranPos, StampedOggPacket::eStampType::OGG_BOTH);
		}
		//Dispatch it.
		return mPacketSink->acceptStampedOggPacket(inPacket);
	}
}

void OggPacketiser::setNumIgnorePackets(unsigned long inNumIgnorePackets) {
	mNumIgnorePackets = inNumIgnorePackets;
}
unsigned long OggPacketiser::numIgnorePackets() {
	return mNumIgnorePackets;
}