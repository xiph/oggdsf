#include "StdAfx.h"
#include "subtitlevmr9inputpin.h"

SubtitleVMR9InputPin::SubtitleVMR9InputPin(SubtitleVMR9Filter* inParentFilter, HRESULT* inHR, LPCWSTR inPinName)
	:	CRendererInputPin(inParentFilter, inHR, inPinName)
	,	mParentFilter(inParentFilter)
{
}

SubtitleVMR9InputPin::~SubtitleVMR9InputPin(void)
{
}

HRESULT SubtitleVMR9InputPin::CheckMediaType(const CMediaType *pmt) {
	//Fix this up after testing.
	return S_OK;
}
