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

#pragma once
#include "oggmuxdllstuff.h"
#include "OggMuxInputPin.h"
#include "BasicSeekable.h"

#include <string>

#include <fstream>
#include "IOggCallback.h"
using namespace std;
#include "StringHelper.h"
class OggMuxInputPin;

class OggMuxFilter
	:	public IFileSinkFilter
	,	public CBaseFilter
	,	public IOggCallback
	,	public IAMFilterMiscFlags
	,	public BasicSeekable
{
public:
	OggMuxFilter(void);
	virtual ~OggMuxFilter(void);


	//Com Stuff
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//IAMFilterMiscFlags Implementation
	ULONG STDMETHODCALLTYPE GetMiscFlags(void);

	//IOggCallback Implementation
	virtual bool acceptOggPage(OggPage* inOggPage);
	//IFileSinkFilter Implementation
	STDMETHODIMP SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);
	STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);

	//IPin Interface (i Think ?? From CBAseFilter)
	virtual int GetPinCount();
	virtual CBasePin* GetPin(int inPinNo);

	//Streaming MEthods
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

protected:

	bool SetupOutput();
	bool CloseOutput();

	wstring mFileName;
	OggMuxInputPin* mInputPin;

	fstream mOutputFile;
	fstream debugLog;
	HRESULT mHR;


};
