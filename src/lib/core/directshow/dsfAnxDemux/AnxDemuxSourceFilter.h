#pragma once
#include "anxdllstuff.h"
#include "OggDemuxSourceFilter.h"
#include "AnxStreamMapper.h"
#include <fstream>
using namespace std;

class OggDemuxSourceFilter;

class AnxDemuxSourceFilter
	:	public OggDemuxSourceFilter
{
public:
	AnxDemuxSourceFilter(void);
	~AnxDemuxSourceFilter(void);

	static CUnknown* WINAPI AnxDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

protected:
	fstream anxDebug;
};
