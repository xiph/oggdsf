#include "StdAfx.h"
#include ".\oggvalidationstate.h"

OggValidationState::OggValidationState(void)
	:	mNumBOS(0)
	,	mNumEOS(0)
	,	mExpectingBOS(false)
	,	mIsValid(false)
	,	mState(FVS_SEEN_NOTHING)

{
}

OggValidationState::~OggValidationState(void)
{
}


OggStreamValidationState* OggValidationState::getStreamBySerialNo(unsigned long inSerialNo) {
	//Find this stream.... or make a new one.

	for (size_t i = 0; i < mStreams.size(); i++) {
		if (mStreams[i]->mSerialNo == inSerialNo) {
			return mStreams[i];
		}
	}

	OggStreamValidationState* retStream = new OggStreamValidationState;

	retStream->mSerialNo = inSerialNo;
	mStreams.push_back(retStream);

	return retStream;
}
unsigned long OggValidationState::numStreams() {
	return mStreams.size();
}

OggStreamValidationState* OggValidationState::getStream(unsigned long inIndex) {
	//gets the indexed string from the vector

	if (inIndex < mStreams.size()) {
        return mStreams[inIndex];
	} else {
		return NULL;
	}

}

bool OggValidationState::notify(bool isBOS, bool isEOS) {
	bool locOK = true;
	if (mState == FVS_SEEN_NOTHING) {
		mState = FVS_AWAITING_BOS;
		//throw 0;
	}

	mIsValid = false;

	if (isBOS) {
		mNumBOS++;
		if (mState == FVS_AWAITING_BOS) {
			//Valid case.
			
		} else {
			//Unexpected BOS page.
			mState = FVS_INVALID;
			locOK = false;
		}

		if (mNumBOS == mNumEOS) {
			//Start of chain
		}
	}

	if (isEOS) {
		mNumEOS++;
		if (mState == FVS_AWAITING_BOS || mState == FVS_AWAITING_EOS) {
			
			if (mNumBOS == mNumEOS) {
				//Valid case. Chain.
				//We are here because we've seen an equal number of EOS and BOS and at least 1 EOS(this one)
				// This happens whenever a complete chain of streams complete and/or the end of a complete physical bitstream.
				// 
				//This is the only place it's possible for isValid to become true.
				// It is true when an equal number of eos and bos pages are seen,
				//  and all streams are also considered fully valid.

				//If this EOS balanced the eos = bos then we can accept bos pages again.
				mState = FVS_AWAITING_BOS;
				
	
				if (mStreams.size() != 0) {
					mIsValid = true;
					for (int i = 0; i < mStreams.size(); i++) {
						mIsValid &= (mStreams[i]->mState == OggStreamValidationState::VS_FULLY_VALID);
					}
				}
			} else {
				mState = FVS_AWAITING_EOS;
			}
		} else {
			mState = FVS_INVALID;
			locOK = false;
		}

		//Check for entire stream validity
		

	}

	if (!isBOS && !isEOS) {
		//This is used because BOS pages must follow each other, after a non BOS page, no more BOS
		// pages can occur until all currently open streams have seen an EOS.
		if (mState == FVS_AWAITING_BOS || mState == FVS_AWAITING_EOS) {
			//Valid case.
			//This happens the first non-bos, non-eos page, when in awaiting BOS state.

			mState = FVS_AWAITING_EOS;
		} else {

			mState = FVS_INVALID;
			locOK = false;
		}
	}

	return mIsValid;

}

bool OggValidationState::isValid() {
	return mIsValid;
}
bool OggValidationState::acceptOggPage(OggPage* inOggPage) {
	//Validate the page header
	
	
	OggStreamValidationState* locStreamState = getStreamBySerialNo(inOggPage->header()->StreamSerialNo());

	OggPageHeader* locHeader = inOggPage->header();

	//If this stream previously was valid, it should have no more data... so it's no longer valid.
	if (locStreamState->mState == OggStreamValidationState::VS_FULLY_VALID) {
		error_write(OVE_DATA_AFTER_VALID_STREAM,inOggPage,  locStreamState);
		mIsValid = false;
		locStreamState->mState = OggStreamValidationState::VS_INVALID;
		//Should return ?
	}

	const int UNUSED_HEADER_MASK =  248; //11111000
	const int OGG_FLAG_CONTINUATION = 1;
	if ((locHeader->HeaderFlags() & UNUSED_HEADER_MASK) != 0) {
		//Unused header bits are set.
		error_write(OVE_UNUSED_HEADER_BITS_SET, inOggPage, locStreamState);
		mIsValid = false;
		locStreamState->mState = OggStreamValidationState::VS_INVALID;
	}

	if (locStreamState->mState == OggStreamValidationState::VS_WAITING_FOR_CONTINUATION) {
		//Last page had an incomplete packet, so expecting a continuation flag on this page
		if ((locHeader->HeaderFlags() & OGG_FLAG_CONTINUATION) != 0) {
			//Valid case.

			//Got the continuation we were expecting.
			locStreamState->mState = OggStreamValidationState::VS_SEEN_BOS;
		} else {
			//There should have been a continuation flag here.
			error_write(OVE_CONTINUATION_FLAG_EXPECTED, inOggPage, locStreamState);
			mIsValid = false;
			locStreamState->mState = OggStreamValidationState::VS_INVALID;
		}
	} else {
		if ((locHeader->HeaderFlags() & OGG_FLAG_CONTINUATION) != 0) {
			//We have a continuation flag we weren't expecting.
			error_write(OVE_UNEXPECTED_CONTINUATION_FLAG, inOggPage, locStreamState);
			mIsValid = false;
			locStreamState->mState = OggStreamValidationState::VS_INVALID;

		}
	}
	
	
	//----------------------------------------------------------------------
	//Verify structure version is 0 (RFC 3533 Sec. 6.1)
	if (locHeader->StructureVersion() != 0) {
		error_write(OVE_STRUCTURE_VERSION_NOT_ZERO, inOggPage, locStreamState);
		mIsValid = false;
	}
	//----------------------------------------------------------------------
	//Verify BOS integrity.
	
	if (locHeader->isBOS()) {
		
		

		if (locStreamState->mState == OggStreamValidationState::VS_SEEN_NOTHING) {
			//Valid case.
			locStreamState->mState = OggStreamValidationState::VS_SEEN_BOS;
		}



		//Check is BOS is first page in stream
		if (locStreamState->mSeenAnything == true) {
			error_write(OVE_BOS_PAGE_NOT_AT_START,inOggPage,  locStreamState);
			mIsValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenBOS) {
			error_write(OVE_MORE_THAN_ONE_BOS,inOggPage, locStreamState);
			mIsValid = false;
		}

		locStreamState->mSeenBOS = true;

	}

	//----------------------------------------------------------------------
	//Verify EOS integrity
	if (locHeader->isEOS()) {
		
		
		
		if (locStreamState->mState == OggStreamValidationState::VS_SEEN_BOS) {
			//Valid case.
			locStreamState->mState = OggStreamValidationState::VS_FULLY_VALID;
		}


		//Check is BOS has been seen before this EOS
		if (locStreamState->mSeenBOS == false) {
			error_write(OVE_EOS_BEFORE_BOS,inOggPage, locStreamState);
			mIsValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenEOS) {
			error_write(OVE_MORE_THAN_ONE_EOS, inOggPage, locStreamState);
			mIsValid = false;
		}
	
		locStreamState->mSeenEOS = true;
	}

	//----------------------------------------------------------------------
	//Verify Granule pos
	if (locHeader->GranulePos() != -1) {
		//Check if the granule pos decreased.
		if (locHeader->GranulePos() < locStreamState->mGranulePosUpto) {
			error_write(OVE_GRANULE_POS_DECREASED, inOggPage, locStreamState);
			mIsValid = false;
		}

		
		locStreamState->mGranulePosUpto = locHeader->GranulePos();

	} else {
		//No packets end here
	}

    	
	//----------------------------------------------------------------------

	//Verify sequence No
	if (locHeader->PageSequenceNo() == locStreamState->mSequenceNoUpto) {
		//error_write(OVE_SEQUENCE_NO_REPEATED, inOggPage, locStreamState);
		//mIsValid = false;
	} else if (locHeader->PageSequenceNo() < locStreamState->mSequenceNoUpto) {
		error_write(OVE_SEQUENCE_NO_DECREASED, inOggPage, locStreamState);
		mIsValid = false;
	} else {
		locStreamState->mSequenceNoUpto = locHeader->PageSequenceNo();
	}
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------

	//Check for incomplete last packet
	if (inOggPage->numPackets() > 0) {
		if (inOggPage->getPacket(inOggPage->numPackets() - 1)->isComplete() == false)  {
			//Valid case
			//If the last packet is incomplete.
			if (OggStreamValidationState::VS_SEEN_BOS) {
				locStreamState->mState = OggStreamValidationState::VS_WAITING_FOR_CONTINUATION;
			} else {
				//Does this mean anythign ?
			}
		}
	}
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------


	notify(inOggPage->header()->isBOS(), inOggPage->header()->isEOS());
	return true;
}

OggValidationState::eOggValidationState OggValidationState::validationState() {
	return mState;
}
void OggValidationState::error_write(short int inErrNo, OggPage* inOggPage, OggStreamValidationState* inStreamState) {

	string locErrorString = "";
	switch (inErrNo) {
		case OVE_OK:
			//Do nothing
			return;
			break;
		case OVE_STRUCTURE_VERSION_NOT_ZERO:
			locErrorString = (int)inOggPage->header()->StructureVersion();
			locErrorString = "Structure version(" + locErrorString +") is not 0, (RFC 3533 Sec. 6.1)";
			break;
		case OVE_MORE_THAN_ONE_BOS:
			locErrorString = "More than one BOS page in stream.";
			break;
		case OVE_MORE_THAN_ONE_EOS:
			locErrorString = "More than one EOS page in stream.";
			break;
		case OVE_BOS_PAGE_NOT_AT_START:
			locErrorString = "BOS found not as first page in stream.";
			break;
		case OVE_EOS_BEFORE_BOS:
			locErrorString = "EOS page found before BOS page";
			break;
		case OVE_DATA_AFTER_VALID_STREAM:
			locErrorString = "More data was received for a stream that was previously complete and valid.";
			break;
		case OVE_UNUSED_HEADER_BITS_SET:
			locErrorString = "One or more unused header flags (XXXXXebc) are set.";
			break;
		case OVE_CONTINUATION_FLAG_EXPECTED:
			locErrorString = "Page did not set continuation flag after incomplete packet.";
			break;
		case OVE_UNEXPECTED_CONTINUATION_FLAG:
			locErrorString = "Continuation flag with no previous incomplete packet.";
			break;

		//Process errors
		case OggDataBuffer::PROCESS_FAILED_TO_SET_HEADER:
			locErrorString = "Lost Sync.";
			break;
		default:
			locErrorString = "Unnamed Error"; 
			break;

	}

	if (inStreamState != NULL) {
        inStreamState->mErrorCount++;
	}

	cout << "ERROR "<<(int)inErrNo<<":  "<<locErrorString<<endl;

}


