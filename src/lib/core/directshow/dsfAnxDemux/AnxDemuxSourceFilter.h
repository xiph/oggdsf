#pragma once
#include "anxdllstuff.h"
#include "OggDemuxSourceFilter.h"
#include "AnxStreamMapper.h"
#include "CMMLStream.h"
#include <fstream>
using namespace std;

class OggDemuxSourceFilter;

class AnxDemuxSourceFilter
	:	public OggDemuxSourceFilter
{
public:
	friend class CMMLStream;
	AnxDemuxSourceFilter(void);
	~AnxDemuxSourceFilter(void);

	static CUnknown* WINAPI AnxDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

protected:
	fstream anxDebug;
};
