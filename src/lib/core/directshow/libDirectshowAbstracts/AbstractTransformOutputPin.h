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
#include "directshowabstractsdllstuff.h"

//External Includes
#include "BasicSeekable.h"

//STL Includes
#include <vector>
using namespace std;

//Forward Declarations
class AbstractTransformFilter;

class AbstractTransformOutputPin 
	//Base Classes
	:	public CBaseOutputPin
			//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directshow/htm/cbaseoutputpinclass.asp
	,	public BasicSeekable
{
public:
	//COM Initialisation
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//Constructors
	AbstractTransformOutputPin(AbstractTransformFilter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName, int inBuffSize, int inNumBuffs, vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~AbstractTransformOutputPin(void);

	//Buffer control method
	virtual HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES *inReqAllocProps);

	//Media Type control methods.
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);
	virtual HRESULT GetMediaType(int inPosition, CMediaType *outMediaType);

	//Pure virtuals for codec specific format data
	virtual HRESULT CreateAndFillFormatBuffer(CMediaType* outMediaType, int inPosition) = 0;

	//Virtuals for data queue delegation
	virtual HRESULT BreakConnect(void);
	virtual HRESULT CompleteConnect (IPin *inReceivePin);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate);
	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverEndFlush(void);
	virtual HRESULT DeliverBeginFlush(void);
	
protected:
	//Helper methods
	void FillMediaType(CMediaType* outMediaType, int inPosition);
	//Member data
	AbstractTransformFilter* mParentFilter;
	COutputQueue* mDataQueue;

	vector<CMediaType*> mAcceptableMediaTypes;

	int mDesiredBufferSize;
	int mDesiredBufferCount;
	int mActualBufferSize;
	int mActualBufferCount;

	HRESULT mHR;
};


