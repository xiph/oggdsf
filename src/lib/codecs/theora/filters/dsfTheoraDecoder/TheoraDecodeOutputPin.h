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
#include "Theoradecoderdllstuff.h"
#include "BasicSeekable.h"

#include <fstream>
using namespace std;
class TheoraDecodeOutputPin 
	:	public CTransformOutputPin
	//,	public BasicSeekable
{
public:
	//COnstructors
	TheoraDecodeOutputPin(CTransformFilter* inParentFilter, HRESULT* outHR);
	virtual ~TheoraDecodeOutputPin();

	//COM Guff
	//DECLARE_IUNKNOWN
	//STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//Overrides
	//virtual HRESULT BreakConnect();
	//virtual HRESULT CompleteConnect (IPin *inReceivePin);

	//fstream debugLog;
};
//----------------------
//OLD IMPLEMENTATION
//----------------------
//#include <d3d9.h>
//#include <vmr9.h>
//#include "Theoradecoderdllstuff.h"
//#include "AbstractVideoDecodeOutputPin.h"
//
//
//class TheoraDecodeFilter;
//class TheoraDecodeOutputPin 
//	:	public AbstractVideoDecodeOutputPin
//	//,	public IStreamBuilder
//{
//public:
//	friend class TheoraDecodeInputPin;
//
//		DECLARE_IUNKNOWN
//	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
//	TheoraDecodeOutputPin(TheoraDecodeFilter* inParentFilter,CCritSec* inFilterLock);
//	virtual ~TheoraDecodeOutputPin(void);
//
//	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q);
//
//	//Implements IStreamBuilder to force the pin to connect to VMR9
//	//STDMETHODIMP Render(IPin* inOutputPin, IGraphBuilder* inGraphBuilder);
//	//STDMETHODIMP Backout(IPin* inOutputPin, IGraphBuilder* inGraphBuilder);
//
//
//
//
//	
//
//
//	//virtual bool FillWaveFormatExBuffer(WAVEFORMATEX* inFormatBuffer);
//	virtual bool FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer);
//	
//
//};
//
//
