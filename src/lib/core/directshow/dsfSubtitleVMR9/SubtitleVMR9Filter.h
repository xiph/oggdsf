//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

#pragma once
#include "subtitledllstuff.h"
#include <d3d9.h>
#include <vmr9.h>
#include <tchar.h>
#include <string>
//Debug only
#include <fstream>
//
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

	//IMediaFilter OVerride - This lets us tell the graph we may not produce data in pause state so don't block.
	virtual STDMETHODIMP GetState(DWORD dw, FILTER_STATE *pState);


	//AKLSJFDLAKSJDL:ASKJD:LKAJSDL:KJSAD:KLJS
	//This method is disabling the filter...
	HRESULT SubtitleVMR9Filter::Receive(IMediaSample *pMediaSample);
	//asdfasdfasdfaskldjf;lkasjdfl;kasjdf;kl


protected:
	void SetColorRef(VMR9AlphaBitmap& bmpInfo);
	HRESULT mHR;
	//SubtitleVMR9OutputPin* mOutputPin;
	virtual HRESULT SetSubtitle(wstring inSubtitle);
	IVideoWindow* mVideoWindow;
	IVMRMixerBitmap9* mBitmapMixer;
	IVMRWindowlessControl9* mWindowLess; 
	VMR9AlphaBitmap mAlphaBitmap;

	//debug only
	//fstream debugLog;
	//
};
