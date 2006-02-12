#pragma once
#include "oggrawaudioextractordllstuff.h"
class OggRawAudioExtractorOutputPin
	:	public CTransformOutputPin
{
public:
	friend class OggRawAudioExtractorFilter;
	OggRawAudioExtractorOutputPin(OggRawAudioExtractorFilter* inParentFilter);
	virtual ~OggRawAudioExtractorOutputPin(void);

	HRESULT CompleteConnect(IPin *inReceivePin);
	HRESULT BreakConnect(void);

protected:
	COutputQueue* mDataQueue;
	HRESULT mHR;
};
