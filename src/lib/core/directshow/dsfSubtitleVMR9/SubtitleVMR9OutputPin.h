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
