#pragma once
#include "OggDemuxSourceFilter.h"
class AnxDemuxSourceFilter
	:	public OggDemuxSourceFilter
{
public:
	AnxDemuxSourceFilter(void);
	~AnxDemuxSourceFilter(void);

	STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType)
};
