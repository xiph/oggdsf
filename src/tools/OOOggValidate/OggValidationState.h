#pragma once
#include <vector>
using namespace std;

#include <libOOOgg/libOOOgg.h>
#include "OggStreamValidationState.h"
class OggValidationState
	:	public IOggCallback
{
public:
	OggValidationState(void);
	virtual ~OggValidationState(void);
	enum eOggValidationErrors {
		OVE_OK,

		OVE_STRUCTURE_VERSION_NOT_ZERO,

		OVE_GRANULE_POS_DECREASED,
		
		OVE_MORE_THAN_ONE_BOS,
		OVE_BOS_PAGE_NOT_AT_START,
		
		OVE_MORE_THAN_ONE_EOS,
		OVE_EOS_BEFORE_BOS,

		OVE_DATA_AFTER_VALID_STREAM,
		
		OVE_SEQUENCE_NO_REPEATED,
		OVE_SEQUENCE_NO_DECREASED,

		OVE_UNUSED_HEADER_BITS_SET,
		OVE_CONTINUATION_FLAG_EXPECTED,
		OVE_UNEXPECTED_CONTINUATION_FLAG,

		OVE_BOS_
	};
	enum eOggValidationState {
		FVS_OK,
		FVS_SEEN_NOTHING,
		FVS_AWAITING_BOS,
		FVS_AWAITING_EOS,
		FVS_INVALID
	};

	

	//IOggCallback
	virtual bool acceptOggPage(OggPage* inOggPage);

	OggStreamValidationState* getStream(unsigned long inIndex);
	OggStreamValidationState* getStreamBySerialNo(unsigned long inSerialNo);
	unsigned long numStreams();
	vector<OggStreamValidationState*> mStreams;

	bool notify(bool inIsBOS, bool inIsEOS);
	void error_write(short int inErrNo, OggPage* inOggPage, OggStreamValidationState* inStreamState);
	
	eOggValidationState validationState();
	
	bool isValid();
protected:
	bool mExpectingBOS;
	unsigned long mNumBOS;
	unsigned long mNumEOS;
	bool mIsValid;
	eOggValidationState mState;
};
