#pragma once
#include "anxmuxdllstuff.h"
#include "OggMuxFilter.h"
class AnxMuxFilter
	:	public OggMuxFilter
{
public:
	AnxMuxFilter(void);
	~AnxMuxFilter(void);

	static CUnknown* WINAPI AnxMuxFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	enum eAnxMuxState {
		ANX_START_STATE = 0,
		WRITTEN_ANNODEX_BOS = 1,
		WRITTEN_A_CMML_ANXDATA = 2,
		WRITTEN_A_CODEC_ANXDATA = 3,
		WRITTEN_ANNODEX_EOS = 4,
		PROCEED_AS_OGG = 5


	};
protected:
	eAnxMuxState mAnxMuxState;
};
