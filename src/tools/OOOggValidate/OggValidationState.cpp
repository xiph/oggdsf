#include "StdAfx.h"
#include ".\oggvalidationstate.h"

OggValidationState::OggValidationState(void)

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