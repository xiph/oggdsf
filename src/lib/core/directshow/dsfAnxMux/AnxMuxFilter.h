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
#include "anxmuxdllstuff.h"
#include "OggMuxFilter.h"
#include "AnxPageInterleaver.h"
#include "AnxMuxInputPin.h"
class AnxMuxFilter
	:	public OggMuxFilter
{
public:
	AnxMuxFilter(void);
	virtual ~AnxMuxFilter(void);

	//Com Stuff
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	static CUnknown* WINAPI AnxMuxFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	enum eAnxMuxState {
		ANX_START_STATE = 0,
		WRITTEN_ANNODEX_BOS = 1,
		WRITTEN_A_CMML_ANXDATA = 2,
		WRITTEN_A_CODEC_ANXDATA = 3,
		WRITTEN_ANNODEX_EOS = 4,
		PROCEED_AS_OGG = 5


	};

	virtual HRESULT addAnotherPin();
protected:
	eAnxMuxState mAnxMuxState;

	unsigned long mAnxVersionMajor;
	unsigned long mAnxVersionMinor;
};
