//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
//Copyright (C) 2009 Cristian Adam
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
//Include Files
#include "speexdecoderdllstuff.h"
#include "AbstractTransformFilter.h"
#include <libilliCore/iLE_Math.h>

//Forward Declarations
struct SPEEXFORMAT;
class SpeexDecodeInputPin;
class SpeexDecodeOutputPin;

//Class Interface
class SpeexDecodeFilter
	//Base Classes
	:	public AbstractTransformFilter
{
public:
	//Friends
	friend class SpeexDecodeInputPin;
	friend class SpeexDecodeOutputPin;

	//Constructors and Destructors
	SpeexDecodeFilter(void);
	virtual ~SpeexDecodeFilter(void);

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	///COM CreateInstance Function
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

    static const wchar_t* NAME;
    static const AMOVIESETUP_MEDIATYPE m_inputMediaTypes;
    static const AMOVIESETUP_MEDIATYPE m_outputMediaTypes;
    static const AMOVIESETUP_PIN m_pinReg[];
    static const AMOVIESETUP_FILTER m_filterReg;
	
	//TODO::: Do we need these ? Aren't they all friends ?? Should set be public?

	///Gets the speex format block
	virtual SPEEXFORMAT* getSpeexFormatBlock();

	///Sets the speex format block
	virtual void setSpeexFormat(BYTE* inFormatBlock);

#ifdef WINCE
	virtual LPAMOVIESETUP_FILTER GetSetupData();
#endif

protected:
	//Pure Virtuals from AbstracttransformFilter
	virtual bool ConstructPins();

	//Format Block
	SPEEXFORMAT* mSpeexFormatInfo;
};
