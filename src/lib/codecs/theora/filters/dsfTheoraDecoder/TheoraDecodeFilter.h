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

#include <fstream>
using namespace std;
class TheoraDecodeFilter 
	:	public CVideoTransformFilter

{
public:
	TheoraDecodeFilter(void);
	virtual ~TheoraDecodeFilter(void);

	//COM Creator Function
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//CTransfrom filter pure virtuals
	virtual HRESULT CheckInputType(const CMediaType* inMediaType);
	virtual HRESULT CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType);
	virtual HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest);
	virtual HRESULT GetMediaType(int inPosition, CMediaType* outOutputMediaType);
	virtual HRESULT Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample);

protected:
	void FillMediaType(CMediaType* outMediaType, unsigned long inSampleSize);
	bool FillVideoInfoHeader(VIDEOINFOHEADER* inFormatBuffer);
	//Format Block
	sTheoraFormatBlock* mTheoraFormatInfo;
	fstream debugLog;
};
//---------------------------------------
//OLD IMPLOEMENTATION....
//---------------------------------------
////Include Files
//#include "Theoradecoderdllstuff.h"
//#include "AbstractVideoDecodeFilter.h"
//
////Forward Declarations
//struct sTheoraFormatBlock;
//class TheoraDecodeInputPin;
//class TheoraDecodeOutputPin;
//
////Class Interface
//class TheoraDecodeFilter
//	//Base Classes
//	:	public AbstractVideoDecodeFilter
//{
//public:
//	//Friends
//	friend class TheoraDecodeInputPin;
//	friend class TheoraDecodeOutputPin;
//
//	
//	
//
//	//Constructors and Destructors
//	TheoraDecodeFilter(void);
//	virtual ~TheoraDecodeFilter(void);
//
//	//COM Creator Function
//	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
//
//	//IMediaFilter OVerride - This lets us tell the graph we may not produce data in pause state so don't block.
//	virtual STDMETHODIMP GetState(DWORD dw, FILTER_STATE *pState);
//
//	
//
//	//VIRTUAL FUNCTIONS - AbstractAudioDecodeFilter
//	virtual bool ConstructPins();
//
//	//FIX::: Do we need these ? Aren't they all friends ??
//	virtual sTheoraFormatBlock* getTheoraFormatBlock();
//	virtual void setTheoraFormat(sTheoraFormatBlock* inFormatBlock);
//
//protected:
//	//Format Block
//	sTheoraFormatBlock* mTheoraFormatInfo;
//};
