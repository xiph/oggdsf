#pragma once

class OggStreamValidationState
{
public:
	OggStreamValidationState(void);
	~OggStreamValidationState(void);

	unsigned long mSerialNo;
	__int64 mGranulePosUpto;
	unsigned long mSequenceNoUpto;

	bool mSeenAnything;
	unsigned long mSeenBOS;
	unsigned long mSeenEOS;

	unsigned long mErrorCount;
	unsigned long mWarningCount;

};
