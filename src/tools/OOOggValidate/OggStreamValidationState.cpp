#include "stdafx.h"
#include "OggStreamValidationState.h"

OggStreamValidationState::OggStreamValidationState(void)
	:	mSerialNo(0)
	,	mGranulePosUpto(0)
	,	mSequenceNoUpto(0)
	,	mSeenBOS(false)
	,	mSeenEOS(false)
	,	mSeenAnything(false)
	,	mErrorCount(0)
	,	mWarningCount(0)
	,	mState(VS_SEEN_NOTHING)
{
}

OggStreamValidationState::~OggStreamValidationState(void)
{
}
