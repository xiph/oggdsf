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

#include "abstractvideodllstuff.h"
#include "BasicSeekable.h"

#include <fstream>

using namespace std;

class AbstractVideoDecodeInputPin;
class AbstractVideoDecodeOutputPin;

class ABS_VIDEO_DEC_API AbstractVideoDecodeFilter 
	:	public CBaseFilter
	,	public BasicSeekable
{
public:
	friend class AbstractVideoDecodeInputPin;
	friend class AbstractVideoDecodeOutputPin;
	AbstractVideoDecodeFilter(TCHAR* inFilterName, REFCLSID inFilterGUID, unsigned short inVideoFormat );
	virtual ~AbstractVideoDecodeFilter(void);
	
	static const long NUM_PINS = 2;
	enum eVideoFormat {
		NONE = 0,
		THEORA = 1,
		OTHER = 1000
	};

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//PURE VIRTUALS
	virtual bool ConstructPins() = 0;
	virtual void DestroyPins();

	//CBaseFilter overrides
	CBasePin* GetPin(int n);
	int GetPinCount(void);

	virtual STDMETHODIMP Stop();
	virtual STDMETHODIMP Pause();
	virtual STDMETHODIMP Run(REFERENCE_TIME tStart);

	
	unsigned short mVideoFormat;

protected:

	//fstream debugLog;

	AbstractVideoDecodeInputPin* mInputPin;
	AbstractVideoDecodeOutputPin* mOutputPin;

};
