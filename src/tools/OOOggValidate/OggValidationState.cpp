#include "StdAfx.h"
#include ".\oggvalidationstate.h"

OggValidationState::OggValidationState(void)

{
}

OggValidationState::~OggValidationState(void)
{
}


OggStreamValidationState* OggValidationState::getStreamBySerialNo(unsigned long inSerialNo) {
	for (size_t i = 0; i < mStreams.size(); i++) {
		if (mStreams[i]->mSerialNo == inSerialNo) {
			return mStreams[i];
		}
	}
	return NULL;
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