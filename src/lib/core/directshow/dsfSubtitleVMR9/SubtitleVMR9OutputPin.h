#pragma once
#include "subtitledllstuff.h"
#include <d3d9.h>
#include <vmr9.h>
#include <tchar.h>
#include <string>
using namespace std;

#define TRANSPARENCY_VALUE   (0.8f)

#define PURE_WHITE          RGB(255,255,255)
#define ALMOST_WHITE        RGB(250,250,250)

#define BLEND_TEXT          TEXT("This is a demonstration of alpha-blended dynamic text.\0")
#define DYNAMIC_TEXT_SIZE   255

#define DEFAULT_FONT_NAME   TEXT("Impact\0")
#define DEFAULT_FONT_STYLE  TEXT("Regular\0")
#define DEFAULT_FONT_SIZE   12
#define DEFAULT_FONT_COLOR  RGB(250,250,250)
#define MAX_FONT_SIZE		25


class SubtitleVMR9OutputPin
	:	public CBasePin
{
public:
	SubtitleVMR9OutputPin(SubtitleVMR9Filter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName);
	virtual ~SubtitleVMR9OutputPin(void);



	virtual HRESULT CheckConnect(IPin *pPin);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual STDMETHODIMP BeginFlush();
	virtual STDMETHODIMP EndFlush();

	virtual HRESULT SetSubtitle(wstring inSubtitle);

	HRESULT SubtitleVMR9OutputPin::GetMediaType(int inPosition, CMediaType *outMediaType);
	bool SubtitleVMR9OutputPin::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer);
	void SubtitleVMR9OutputPin::FillMediaType(CMediaType* outMediaType);
	
	

protected:
	void SetColorRef(VMR9AlphaBitmap& bmpInfo);
	SubtitleVMR9Filter* mParentFilter;
	HRESULT mHR;
	IVMRMixerBitmap9* mBitmapMixer;
	VMR9AlphaBitmap mAlphaBitmap;
};
