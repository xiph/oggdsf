#include ".\oggrawaudioextractoroutputpin.h"

OggRawAudioExtractorOutputPin::OggRawAudioExtractorOutputPin(OggRawAudioExtractorFilter* inParentFilter)
	:	CTransformOutputPin(NAME("PCMOut"), inParentFilter, &mHR, L"PCM Out Pin")
	,	mDataQueue(NULL)
	,	mHR(S_OK)
{
}

OggRawAudioExtractorOutputPin::~OggRawAudioExtractorOutputPin(void)
{
	delete mDataQueue;
}
HRESULT OggRawAudioExtractorOutputPin::CompleteConnect(IPin *inReceivePin) 
{
	CAutoLock locLock(m_pLock);

	
	HRESULT locHR = S_OK;
	mDataQueue = new COutputQueue (inReceivePin, &locHR, FALSE, FALSE, 1, TRUE, 50);			//Deleted in destructor

	if (FAILED(locHR)) {
		//Handle data Q failure
		
	}
	
	return CTransformOutputPin::CompleteConnect(inReceivePin);
}

HRESULT OggRawAudioExtractorOutputPin::BreakConnect(void) 
{
	CAutoLock locLock(m_pLock);

	delete mDataQueue;
	mDataQueue = NULL;

	HRESULT locHR = CBaseOutputPin::BreakConnect();


	return locHR;
}