//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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

#include "StdAfx.h"
#include "theoradecodeoutputpin.h"

TheoraDecodeOutputPin::TheoraDecodeOutputPin(TheoraDecodeFilter* inParentFilter, CCritSec* inFilterLock)
	: AbstractVideoDecodeOutputPin(inParentFilter, inFilterLock,NAME("TheoraDecodeOutputPin"), L"YV12 Out")
{

		
}
TheoraDecodeOutputPin::~TheoraDecodeOutputPin(void)
{
	
	
}
STDMETHODIMP TheoraDecodeOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	}

	return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}


STDMETHODIMP TheoraDecodeOutputPin::Notify(IBaseFilter *pSelf, Quality q) {
	return S_OK;

}

bool TheoraDecodeOutputPin::FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer) {
	TheoraDecodeFilter* locFilter = ((TheoraDecodeFilter*)m_pFilter);

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->mTheoraFormatInfo->frameRateDenominator) / locFilter->mTheoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->mTheoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = 12;   //12 bits per pixel
	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth
	inFormatBuffer->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
	inFormatBuffer->bmiHeader.biHeight = locFilter->mTheoraFormatInfo->height;   //Not sure
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	inFormatBuffer->bmiHeader.biSizeImage = ((locFilter->mTheoraFormatInfo->height * locFilter->mTheoraFormatInfo->width) * 3)/2;    //Size in bytes of image ??
	inFormatBuffer->bmiHeader.biWidth = locFilter->mTheoraFormatInfo->width;
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->mTheoraFormatInfo->height;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->mTheoraFormatInfo->width;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->mTheoraFormatInfo->height;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->mTheoraFormatInfo->width;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}