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
#include "stdafx.h"
#include "subtitlevmr9filter.h"


//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"Subtitle VMR9 Filter",					// Name
	    &CLSID_SubtitleVMR9Filter,				// CLSID
	    SubtitleVMR9Filter::CreateInstance,		// Method to create an instance of Speex Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

CUnknown* WINAPI SubtitleVMR9Filter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	SubtitleVMR9Filter *pNewObject = new SubtitleVMR9Filter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

//***********************&!*(#&$()*&!@()*#&!@()*#&()!@*&#)(!@*&#()*!@&#)(*!@&#)*(!@
//This esentially disables the filter...
HRESULT SubtitleVMR9Filter::Receive(IMediaSample *pMediaSample) {
	return S_OK;
}
//&)(*Q&#$()*!@&#$()!*@&#)(*!@&#)(*!@&#)(*&!@)#(*&!@)(#*&()!@*#&)(!*@&#()!*@&#()!@*&#

SubtitleVMR9Filter::SubtitleVMR9Filter(void)
	:	CBaseRenderer(CLSID_SubtitleVMR9Filter, NAME("Subtitle VMR9 Filter") ,NULL,&mHR)
	,	mBitmapMixer(NULL)
	,	mVideoWindow(NULL)
	,	mWindowLess(NULL)
{
	//debugLog.open("G:\\logs\\vmr9_subs.log", ios_base::out);
	//mOutputPin = new SubtitleVMR9OutputPin(this, m_pLock, NAME("SubtitleVMR9OutputPin"), L"Subtitle Out");
}

SubtitleVMR9Filter::~SubtitleVMR9Filter(void)
{
	//debugLog<<"Destructor for subtitle renderer"<<endl;
	//debugLog.close();

	if (mBitmapMixer != NULL) {
		mBitmapMixer->Release();
	}

	if (mVideoWindow != NULL) {
		mVideoWindow->Release();
	}

	if (mWindowLess != NULL) {
		mWindowLess->Release();
	}
}

STDMETHODIMP SubtitleVMR9Filter::GetState(DWORD dw, FILTER_STATE *pState)
{
    CheckPointer(pState, E_POINTER);
    *pState = m_State;
	if (m_State == State_Paused) {
        return VFW_S_CANT_CUE;
	} else {
        return S_OK;
	}
}
int SubtitleVMR9Filter::GetPinCount(void) {
	return 1;
}
CBasePin* SubtitleVMR9Filter::GetPin(int inPinNo) {
	if (inPinNo == 0) {
		return CBaseRenderer::GetPin(inPinNo);
	} else {
		return NULL;
	}
}

HRESULT SubtitleVMR9Filter::CheckMediaType(const CMediaType* inMediaType) {
	return S_OK;
}
HRESULT SubtitleVMR9Filter::DoRenderSample(IMediaSample* inMediaSample) {
	//debugLog<<"DoRenderSample : "<<endl;
	LONGLONG locStart, locEnd;
	inMediaSample->GetTime(&locStart, &locEnd);
	//debugLog<<"DoRenderSample : Time = "<<locStart<<" to "<<locEnd<<endl;
	inMediaSample->GetMediaTime(&locStart, &locEnd);
	//debugLog<<"DoRenderSample : Media Time = "<<locStart<<" to "<<locEnd<<endl;
	static int c = 0;
	const int hm = 50;
	if (mBitmapMixer == NULL) {
		IFilterGraph* locFilterGraph = NULL;
		locFilterGraph = GetFilterGraph();

	
		IBaseFilter* locVMR9 = NULL;
		HRESULT locHR = locFilterGraph->FindFilterByName(L"Video Mixing Renderer 9", &locVMR9);
		if (locVMR9 != NULL) {
			//debugLog<<"DoRenderSample : Getting VMR9 pointer..."<<endl;
			HRESULT locHR = locVMR9->QueryInterface(IID_IVMRMixerBitmap9, (void**)&mBitmapMixer);
			locHR = locVMR9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&mWindowLess);

			locHR = locVMR9->QueryInterface(IID_IVideoWindow, (void**)&mVideoWindow);
			locVMR9->Release();
		} else {
			return S_OK;
		}
	} else {
		/*c++;
		string x;
		switch ((c / hm) % 4) {
			case 0:
                x = "One";
				SetSubtitle(x);
				break;
			case 1:
                x = "Two";
				SetSubtitle(x);
				break;

			case 2:
                x = "Three";
				SetSubtitle(x);
				break;
			case 3:
                x = "Four";
				SetSubtitle(x);
				break;
			default:
				break;


		}*/
		
		if (inMediaSample->GetActualDataLength() > 0) {
			wchar_t* locStr = NULL;
			BYTE* locBuff = NULL;
			inMediaSample->GetPointer(&locBuff);
			locStr = new wchar_t[inMediaSample->GetActualDataLength() / sizeof(wchar_t)];
			memcpy((void*)locStr, (const void*) locBuff, inMediaSample->GetActualDataLength());
			wstring x = locStr;
			//debugLog<<"DoRenderSample : Subtile = "<<x<<endl;
			SetSubtitle(x);
			//debugLog<<"DoRenderSample : SetSubtitle Returns"<<endl<<endl;
			delete locStr;
		} else {
			//debugLog<<"DoRenderSample : Clearing Subtitle..."<<endl;
			SetSubtitle(L"");
		}


		return S_OK;
	}
	//debugLog<<"DoRenderSample : NEVER SHOULD BE HERE !!!!!!"<<endl;
}




HRESULT SubtitleVMR9Filter::SetSubtitle(wstring inSubtitle) {
 //   LONG cx, cy;
 //   HRESULT locHR;

 //   // Read the default video size
 //   //hr = pWC->GetNativeVideoSize(&cx, &cy, NULL, NULL);
 //   //if (FAILED(hr))
 //   //{
 //   //    Msg(TEXT("GetNativeVideoSize FAILED!  hr=0x%x\r\n"), hr);
 //   //    return hr;
 //   //}

 //   // Create a device context compatible with the current window

	//locHR = mBitmapMixer->GetAlphaBitmapParameters(&mAlphaBitmap);
 //   //HDC hdc = GetDC(hwndApp);
 //   HDC locHDCBitmap = CreateCompatibleDC(mAlphaBitmap.hdc);

 //   // Write with a known font by selecting it into our HDC
 //   HFONT locFontHandle = (HFONT) SelectObject(locHDCBitmap, g_hFont);

 //   // Determine the length of the string, then determine the
 //   // dimensions (in pixels) of the character string using the
 //   // currently selected font.  These dimensions are used to create
 //   // a bitmap below.
 //   int locLength, locTextBmpWidth, locTextBmpHeight;
 //   SIZE sz={0};
 //   locLength = inSubtitle.size();
 //   GetTextExtentPoint32(locHDCBitmap, inSubtitle.c_str(), locLength, &sz);
 //   locTextBmpHeight = sz.cy;
 //   locTextBmpWidth  = sz.cx;

 //   // Create a new bitmap that is compatible with the current window
 //   HBITMAP locBitmapHandle = CreateCompatibleBitmap(mAlphaBitmap.hdc, locTextBmpWidth, locTextBmpHeight);
 //   //ReleaseDC(hwndApp, hdc);

 //   // Select our bitmap into the device context and save the old one
 //   BITMAP locBitmap;
 //   HBITMAP locOldBitmapHandle;
 //   GetObject(locBitmapHandle, sizeof(locBitmap), &locBitmap);
 //   locOldBitmapHandle = (HBITMAP)SelectObject(locBitmapHandle, locBitmapHandle);

 //   // Set initial bitmap settings
 //   RECT rcText;
 //   SetRect(&rcText, 0, 0, nTextBmpWidth, nTextBmpHeight);
 //   SetBkColor(locBitmapHandle, RGB(255, 255, 255)); // Pure white background
 //   SetTextColor(locBitmapHandle, g_rgbColors);      // Write text with requested color

 //   // Draw the requested text string onto the bitmap
 //   TextOut(locBitmapHandle, 0, 0, inSubtitle.c_str(), locLength);

 //   // Configure the VMR's bitmap structure
 //   //VMR9AlphaBitmap bmpInfo;
 //   ZeroMemory(&mAlphaBitmap, sizeof(mAlphaBitmap) );
 //   mAlphaBitmap.dwFlags = VMRBITMAP_HDC;
 //   mAlphaBitmap.hdc = locHDCBitmap;  // DC which has selected our bitmap

 //   // Remember the width of this new bitmap
 //   g_nImageWidth = bm.bmWidth;

 //   // Save the ratio of the bitmap's width to the width of the video file.
 //   // This value is used to reposition the bitmap in composition space.
 //   g_fBitmapCompWidth = (float)g_nImageWidth / (float)cx;

 //   // Display the bitmap in the bottom right corner.
 //   // rSrc specifies the source rectangle in the GDI device context 
 //   // rDest specifies the destination rectangle in composition space (0.0f to 1.0f)
 //   mAlphaBitmap.rDest.left  = 0.0f + X_EDGE_BUFFER;
 //   mAlphaBitmap.rDest.right = 1.0f - X_EDGE_BUFFER;
 //   mAlphaBitmap.rDest.top = (float)(cy - bm.bmHeight) / (float)cy - Y_EDGE_BUFFER;
 //   bmAlphaBitmap.rDest.bottom = 1.0f - Y_EDGE_BUFFER;
 //   mAlphaBitmap.rSrc = rcText;

 //   // Transparency value 1.0 is opaque, 0.0 is transparent.
 //   mAlphaBitmap.fAlpha = TRANSPARENCY_VALUE;

 //   // Set the COLORREF so that the bitmap outline will be transparent
 //   SetColorRef(mAlphaBitmap);

 //   // Give the bitmap to the VMR for display
 //   locHR = mBitmapMixer->SetAlphaBitmap(&mAlphaBitmap);
 //   if (FAILED(hr))
 //       Msg(TEXT("SetAlphaBitmap FAILED!  hr=0x%x\r\n\r\n%s\0"), hr,
 //           STR_VMR_DISPLAY_WARNING);

 //   // Select the initial objects back into our device context
 //   DeleteObject(SelectObject(locBitmapHDC,locOldBitmapHandle ));
 //   SelectObject(locHDCBitmap, locFont);

 //   // Clean up resources
 //   DeleteObject(hbm);
 //   DeleteDC(hdcBmp);

 //   return hr;


	// Text font information
	HFONT g_hFont=0;
	LONG g_lFontPointSize   = DEFAULT_FONT_SIZE;
	COLORREF g_rgbColors    = DEFAULT_FONT_COLOR;
	TCHAR g_szFontName[100] = {DEFAULT_FONT_NAME};
	TCHAR g_szFontStyle[32] = {DEFAULT_FONT_STYLE};

	LONG cx, cy;
    HRESULT hr;

    // Read the default video size
	if (mWindowLess != NULL) {
		hr = mWindowLess->GetNativeVideoSize(&cx, &cy, NULL, NULL);
	} else {
		mVideoWindow->get_Width(&cx);
		mVideoWindow->get_Height(&cy);
	}
    //if (FAILED(hr))
    //{
    //    Msg(TEXT("GetNativeVideoSize FAILED!  hr=0x%x\r\n"), hr);
    //    return hr;
    //}

    // Create a device context compatible with the current window
	hr = mBitmapMixer->GetAlphaBitmapParameters(&mAlphaBitmap);
	//HWND hwndApp;
	//OAHWND oah;
	//mVideoWindow->get_Owner(&oah);
	HDC hdc = GetDC(NULL);
    //HDC hdc = mAlphaBitmap.hdc;
    HDC hdcBmp = CreateCompatibleDC(hdc);

    // Write with a known font by selecting it into our HDC
    HFONT hOldFont = (HFONT) SelectObject(hdcBmp, g_hFont);

    // Determine the length of the string, then determine the
    // dimensions (in pixels) of the character string using the
    // currently selected font.  These dimensions are used to create
    // a bitmap below.
    int nLength, nTextBmpWidth, nTextBmpHeight;
    SIZE sz={0};
    
	TCHAR* szNewText = (TCHAR*)inSubtitle.c_str();//_T("Annodex Me Baby !");//inSubtitle.c_str();
	nLength = _tcslen(szNewText); ;//(int) inSubtitle.size();

    GetTextExtentPoint32(hdcBmp, szNewText, nLength, &sz);
    nTextBmpHeight = sz.cy;
    nTextBmpWidth  = sz.cx;

    // Create a new bitmap that is compatible with the current window
    HBITMAP hbm = CreateCompatibleBitmap(hdc, nTextBmpWidth, nTextBmpHeight);
    //ReleaseDC(hwndApp, hdc);

    // Select our bitmap into the device context and save the old one
    BITMAP bm;
    HBITMAP hbmOld;
    GetObject(hbm, sizeof(bm), &bm);
    hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);

    // Set initial bitmap settings
    RECT rcText;
    SetRect(&rcText, 0, 0, nTextBmpWidth, nTextBmpHeight);
    SetBkColor(hdcBmp, RGB(255, 255, 255)); // Pure white background
    SetTextColor(hdcBmp, g_rgbColors);      // Write text with requested color

    // Draw the requested text string onto the bitmap
    BOOL ret = TextOut(hdcBmp, 0, 0, szNewText, nLength);

    // Configure the VMR's bitmap structure
    VMR9AlphaBitmap bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
    bmpInfo.dwFlags = VMRBITMAP_HDC;
    bmpInfo.hdc = hdcBmp;  // DC which has selected our bitmap

    // Remember the width of this new bitmap
    //g_nImageWidth = bm.bmWidth;

    // Save the ratio of the bitmap's width to the width of the video file.
    // This value is used to reposition the bitmap in composition space.
    //g_fBitmapCompWidth = (float)g_nImageWidth / (float)cx;

    // Display the bitmap in the bottom right corner.
    // rSrc specifies the source rectangle in the GDI device context 
    // rDest specifies the destination rectangle in composition space (0.0f to 1.0f)

	const float X_EDGE_BUFFER=0.05f; // Pixel buffer between bitmap and window edge
                                 // (represented in composition space [0 - 1.0f])
	const float Y_EDGE_BUFFER=0.05f;

    bmpInfo.rDest.left  = 0.0f + X_EDGE_BUFFER;
    bmpInfo.rDest.right = 1.0f - X_EDGE_BUFFER;
    bmpInfo.rDest.top = (float)(cy - bm.bmHeight) / (float)cy - Y_EDGE_BUFFER;
	//bmpInfo.rDest.top = (float)(200.0f - bm.bmHeight) / (float)200.0f - Y_EDGE_BUFFER;
	//bmpInfo.rDest.top = 0.8f;//(float)(200.0f - bm.bmHeight) / (float)200.0f - Y_EDGE_BUFFER;
    bmpInfo.rDest.bottom = 1.0f - Y_EDGE_BUFFER;
    bmpInfo.rSrc = rcText;

    // Transparency value 1.0 is opaque, 0.0 is transparent.
    bmpInfo.fAlpha = TRANSPARENCY_VALUE;

    // Set the COLORREF so that the bitmap outline will be transparent
    SetColorRef(bmpInfo);

    // Give the bitmap to the VMR for display
    hr = mBitmapMixer->SetAlphaBitmap(&bmpInfo);
    //if (FAILED(hr))
        //Msg(TEXT("SetAlphaBitmap FAILED!  hr=0x%x\r\n\r\n%s\0"), hr,
          //  STR_VMR_DISPLAY_WARNING);

    // Select the initial objects back into our device context
    DeleteObject(SelectObject(hdcBmp, hbmOld));
    SelectObject(hdc, hOldFont);

    // Clean up resources
    DeleteObject(hbm);
    DeleteDC(hdcBmp);

	//debugLog<<"SetSubtitle : End of method: hr = "<<hr<<endl;
    return hr;
}

void SubtitleVMR9Filter::SetColorRef(VMR9AlphaBitmap& bmpInfo)
{
    // Set the COLORREF so that the bitmap outline will be transparent
    bmpInfo.clrSrcKey = RGB(255, 255, 255);  // Pure white
    bmpInfo.dwFlags |= VMRBITMAP_SRCCOLORKEY;
}