#pragma once
#include "subtitledllstuff.h"
class SubtitleVMR9InputPin
	:	public CRendererInputPin
{
public:
	SubtitleVMR9InputPin(SubtitleVMR9Filter* inParentFilter, HRESULT* inHR, LPCWSTR inPinName);
	virtual ~SubtitleVMR9InputPin(void);

	HRESULT SubtitleVMR9InputPin::CheckMediaType(const CMediaType *pmt);
protected:
	SubtitleVMR9Filter* mParentFilter;
};
