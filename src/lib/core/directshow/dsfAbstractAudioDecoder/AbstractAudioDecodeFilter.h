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

//Local Includes
#include "abstractaudiodllstuff.h"
#include "BasicSeekable.h"

//Forward Declarations
class AbstractAudioDecodeInputPin;
class AbstractAudioDecodeOutputPin;

//*************************************************************************************************
class ABS_AUDIO_DEC_API AbstractAudioDecodeFilter
	//Parent Classes
	:	public CBaseFilter
	,	public BasicSeekable
{
public:
	//Friends
	friend class AbstractAudioDecodeInputPin;
	friend class AbstractAudioDecodeOutputPin;

	//COM Setup
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//Constants and Enumerations
	static const long NUM_PINS = 2;
	enum eAudioFormat {
		NONE = 0,
		VORBIS = 1,
		SPEEX = 2,
		FLAC = 3,
		OTHER = 1000
	};

	//Constructors
	AbstractAudioDecodeFilter(TCHAR* inFilterName, REFCLSID inFilterGUID, unsigned short inAudioFormat );
	virtual ~AbstractAudioDecodeFilter(void);
	
	//Pin Methods
	CBasePin* GetPin(int n);
	int GetPinCount(void);

	virtual bool ConstructPins() = 0;
	virtual void DestroyPins();


	//Media Control Methods
	virtual STDMETHODIMP Stop();
	
	//Helper Methods
	void SetStartToNow();					//QUERY::: Do we even still use this ?

	unsigned short mAudioFormat;			//TODO::: Make this private at some point

	//static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
protected:
	//Member Data
	AbstractAudioDecodeInputPin* mInputPin;
	AbstractAudioDecodeOutputPin* mOutputPin;

	
};


