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
#include "subtitlevmr9outputpin.h"

SubtitleVMR9OutputPin::SubtitleVMR9OutputPin(SubtitleVMR9Filter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName)
	:	CBasePin(inObjectName, inParentFilter, inFilterLock, &mHR, inPinDisplayName, PINDIR_OUTPUT)
	,	mParentFilter(inParentFilter)
	,	mBitmapMixer(NULL)
{
}

SubtitleVMR9OutputPin::~SubtitleVMR9OutputPin(void)
{
}

HRESULT SubtitleVMR9OutputPin::CheckConnect(IPin* inPin) {
	//Query for the video mixing bitmap interface
    //HRESULT locHR = inPin->QueryInterface(IID_IVMRMixerBitmap9, (void**)&mBitmapMixer);
	//if (locHR == S_OK) {
		return S_OK;
	//} else {
	//	return S_FALSE;
	//}


}
void SubtitleVMR9OutputPin::FillMediaType(CMediaType* outMediaType) {
	outMediaType->SetType(&MEDIATYPE_Video);
	outMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
	outMediaType->SetFormatType(&FORMAT_VideoInfo);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(0);

}
HRESULT SubtitleVMR9OutputPin::GetMediaType(int inPosition, CMediaType *outMediaType) {

	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if (inPosition == 0) {
		FillMediaType(outMediaType);
		VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
		FillVideoInfoHeader(locVideoFormat);
		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}
}

bool SubtitleVMR9OutputPin::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer) {
	

	inFormatBuffer->AvgTimePerFrame = (UNITS * 1) / 25;
	inFormatBuffer->dwBitRate = 0;
	
	inFormatBuffer->bmiHeader.biBitCount = 12;   //12 bits per pixel
	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth
	inFormatBuffer->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
	inFormatBuffer->bmiHeader.biHeight = 0;   //Not sure
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	inFormatBuffer->bmiHeader.biSizeImage = 0;    //Size in bytes of image ??
	inFormatBuffer->bmiHeader.biWidth = 0;
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = 0;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = 0;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = 0;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = 0;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}
HRESULT SubtitleVMR9OutputPin::CheckMediaType(const CMediaType *pmt) {
	//Fix this up after testing.
	return S_OK;
}

STDMETHODIMP SubtitleVMR9OutputPin::BeginFlush() {
	return S_OK;
}
STDMETHODIMP SubtitleVMR9OutputPin::EndFlush() {
	return S_OK;
}

HRESULT SubtitleVMR9OutputPin::SetSubtitle(wstring inSubtitle) {
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
    //hr = pWC->GetNativeVideoSize(&cx, &cy, NULL, NULL);
    //if (FAILED(hr))
    //{
    //    Msg(TEXT("GetNativeVideoSize FAILED!  hr=0x%x\r\n"), hr);
    //    return hr;
    //}

    // Create a device context compatible with the current window
	hr = mBitmapMixer->GetAlphaBitmapParameters(&mAlphaBitmap);
    HDC hdc = mAlphaBitmap.hdc;
    HDC hdcBmp = CreateCompatibleDC(hdc);

    // Write with a known font by selecting it into our HDC
    HFONT hOldFont = (HFONT) SelectObject(hdcBmp, g_hFont);

    // Determine the length of the string, then determine the
    // dimensions (in pixels) of the character string using the
    // currently selected font.  These dimensions are used to create
    // a bitmap below.
    int nLength, nTextBmpWidth, nTextBmpHeight;
    SIZE sz={0};
    nLength = (int) inSubtitle.size();
	TCHAR* szNewText = _T("Blah blah");//inSubtitle.c_str();
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
    TextOut(hdcBmp, 0, 0, szNewText, nLength);

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
    bmpInfo.rDest.bottom = 1.0f - Y_EDGE_BUFFER;
    bmpInfo.rSrc = rcText;

    // Transparency value 1.0 is opaque, 0.0 is transparent.
    bmpInfo.fAlpha = TRANSPARENCY_VALUE;

    // Set the COLORREF so that the bitmap outline will be transparent
    SetColorRef(bmpInfo);

    // Give the bitmap to the VMR for display
    hr = mBitmapMixer->SetAlphaBitmap(&bmpInfo);
    if (FAILED(hr))
        //Msg(TEXT("SetAlphaBitmap FAILED!  hr=0x%x\r\n\r\n%s\0"), hr,
          //  STR_VMR_DISPLAY_WARNING);

    // Select the initial objects back into our device context
    DeleteObject(SelectObject(hdcBmp, hbmOld));
    SelectObject(hdc, hOldFont);

    // Clean up resources
    DeleteObject(hbm);
    DeleteDC(hdcBmp);

    return hr;
}

void SubtitleVMR9OutputPin::SetColorRef(VMR9AlphaBitmap& bmpInfo)
{
    // Set the COLORREF so that the bitmap outline will be transparent
    bmpInfo.clrSrcKey = RGB(255, 255, 255);  // Pure white
    bmpInfo.dwFlags |= VMRBITMAP_SRCCOLORKEY;
}