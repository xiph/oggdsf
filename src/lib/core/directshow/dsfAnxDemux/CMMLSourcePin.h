#pragma once
#include "anxdllstuff.h"
#include "OggDemuxSourcePin.h"
class CMMLSourcePin
	:	public OggDemuxSourcePin
	,	public IStreamBuilder
{
public:

		DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	CMMLSourcePin(	TCHAR* inObjectName, 
										OggDemuxSourceFilter* inParentFilter,
										CCritSec* inFilterLock,
										StreamHeaders* inHeaderSource, 
										CMediaType* inMediaType,
										wstring inPinName);
	~CMMLSourcePin(void);

	//Implements IStreamBuilder to force the pin tothe cmml filter
	STDMETHODIMP Render(IPin* inOutputPin, IGraphBuilder* inGraphBuilder);
	STDMETHODIMP Backout(IPin* inOutputPin, IGraphBuilder* inGraphBuilder);
};
