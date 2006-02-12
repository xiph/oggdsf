//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
#include "cmmldecoderdllstuff.h"
#include "CMMLDecodeInputPin.h"
#include <string>
#include <fstream>
#include <libilliCore/StringHelper.h>
#include <libCMMLParse/libCMMLParse.h>
#include <libCMMLParse/CMMLParser.h>
#include <libCMMLTags/libCMMLTags.h>
#include "ICMMLAppControl.h"
using namespace std;

class OGMDecodeInputPin;
class CMMLDecodeFilter
	:	public CTransformFilter
	,	public ICMMLAppControl
{
public:
	CMMLDecodeFilter(void);
	virtual ~CMMLDecodeFilter(void);
	//COM Creator Function
	DECLARE_IUNKNOWN
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//Implement the Tranform filter interface
	HRESULT CheckInputType(const CMediaType* inInputMediaType);
	HRESULT CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType);
	HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest);
	HRESULT GetMediaType(int inPosition, CMediaType* outMediaType);
	HRESULT Transform(IMediaSample* inSample, IMediaSample* outSample);

	CBasePin* CMMLDecodeFilter::GetPin(int inPinNo);

	//Implement ICMMLAppControl
	virtual STDMETHODIMP_(bool) setCallbacks(ICMMLCallbacks* inCallbacks);
	virtual STDMETHODIMP_(ICMMLCallbacks*) getCallbacks();
protected:
	wstring toWStr(string inString);
	CMMLParser* mCMMLParser;
	bool mSeenHead;
	C_HeadTag* mHeadTag;
	ICMMLCallbacks* mCMMLCallbacks;

	CMMLDecodeInputPin* mInputPin;

	//fstream debugLog;
	

};
