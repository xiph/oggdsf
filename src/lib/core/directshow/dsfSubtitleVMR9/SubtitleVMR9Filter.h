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
#define DEFAULT_FONT_COLOR  RGB(255,0,0)
#define MAX_FONT_SIZE		25

class SubtitleVMR9OutputPin;

class SubtitleVMR9Filter
	:	public CBaseRenderer
{
public:
	DECLARE_IUNKNOWN
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
	SubtitleVMR9Filter(void);
	virtual ~SubtitleVMR9Filter(void);

	//Base Filter Implementation
	virtual int GetPinCount(void);
	virtual CBasePin* GetPin(int inPinNo);

	HRESULT CheckMediaType(const CMediaType* inMediaType);
	HRESULT DoRenderSample(IMediaSample *pMediaSample);






protected:
	void SetColorRef(VMR9AlphaBitmap& bmpInfo);
	HRESULT mHR;
	//SubtitleVMR9OutputPin* mOutputPin;
	virtual HRESULT SetSubtitle(string inSubtitle);
	IVideoWindow* mVideoWindow;
	IVMRMixerBitmap9* mBitmapMixer;
	IVMRWindowlessControl9* mWindowLess; 
	VMR9AlphaBitmap mAlphaBitmap;
};
