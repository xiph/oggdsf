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
#include "dsfDiracDecodeSource.h"
#include "DiracDecodeSourceFilter.h"
#include <fstream>
using namespace std;
class DiracDecodeSourcePin
	:	public CBaseOutputPin
{
public:

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	DiracDecodeSourcePin(	DiracDecodeSourceFilter* inParentFilter, CCritSec* inFilterLock);
					
	virtual ~DiracDecodeSourcePin(void);

	static const unsigned long BUFFER_SIZE = 65536;			//What should this be ????
	static const unsigned long NUM_BUFFERS = 10;

	//CBaseOutputPin virtuals
	virtual HRESULT GetMediaType(int inPosition, CMediaType* outMediaType);
	virtual HRESULT CheckMediaType(const CMediaType* inMediaType);
	virtual HRESULT DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest);


	//IPin
	virtual HRESULT CompleteConnect (IPin *inReceivePin);
	virtual HRESULT BreakConnect(void);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverEndFlush(void);
	virtual HRESULT DeliverBeginFlush(void);
protected:
	//fstream debugLog;
	HRESULT mFilterHR;
	COutputQueue* mDataQueue;
	
};
