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
#include "oggdllstuff.h"
#include "StreamHeaders.h"
#include <libOOOgg/OggPage.h>
#include <libOOOgg/IOggCallback.h>
#include "BasicSeekPassThrough.h"
#include <fstream>
#include "OggDemuxSourceFilter.h"
using namespace std;

class OggDemuxSourceFilter;

class OGG_DEMUX_API OggDemuxSourcePin
	:	public CBaseOutputPin
	,	public BasicSeekPassThrough
		
{
public:

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	OggDemuxSourcePin(		TCHAR* inObjectName, 
							OggDemuxSourceFilter* inParentFilter,
							CCritSec* inFilterLock,
							StreamHeaders* inHeaderSource, 
							CMediaType* inMediaType,
							wstring inPinName,
							bool inAllowSeek,
							unsigned long inNumBuffers,
							unsigned long inBufferSize);
	


	virtual ~OggDemuxSourcePin(void);
	static const unsigned long BUFFER_SIZE = 1024*512;			//What should this be ????
	static const unsigned long NUM_BUFFERS = 125;


	virtual bool deliverOggPacket(StampedOggPacket* inOggPacket);

	//CSourceStream virtuals
	virtual HRESULT GetMediaType(int inPosition, CMediaType* outMediaType);
	virtual HRESULT CheckMediaType(const CMediaType* inMediaType);
	virtual HRESULT DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest);


	//IOggCallback
	//virtual bool acceptOggPage(OggPage* inOggPage);


	//IPin
	virtual HRESULT CompleteConnect (IPin *inReceivePin);
	virtual HRESULT BreakConnect(void);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverEndFlush(void);
	virtual HRESULT DeliverBeginFlush(void);
protected:
	OggDemuxSourceFilter* mParentFilter;
	StreamHeaders* mHeaders;
	CMediaType* mMediaType;
	StampedOggPacket* mPartialPacket;
	HRESULT mFilterHR;
	COutputQueue* mDataQueue;
	fstream debugLog;
	bool mFirstRun;

	unsigned long mBufferSize;
	unsigned long mNumBuffers;
};
