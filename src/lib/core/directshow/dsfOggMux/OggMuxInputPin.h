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
#include "OggMuxFilter.h"
#include <libOOOgg/OggPaginator.h>
#include <libOOOgg/OggMuxStream.h>
#include "BasicSeekPassThrough.h"
#include "FLACMetadataSplitter.h"
#include <time.h>
#include <fstream>
#include <windows.h>
using namespace std;

class OggMuxFilter;

class OGG_MUX_API OggMuxInputPin
	:	public CBaseInputPin
	,	public BasicSeekPassThrough
{
public:
	OggMuxInputPin(OggMuxFilter* inParentFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream);
	virtual ~OggMuxInputPin(void);

	//COM Setup
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	/// Gets an indexed media type, that this pin will accept.
	virtual HRESULT GetMediaType(int inPosition, CMediaType* outMediaType);

	/// Checks whether this pin will accepted the proposed media type.
	virtual HRESULT CheckMediaType(const CMediaType* inMediaType);

	/// Notification that this media type has been selected for the connection.
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	/// Receives a sample from an upstream filter.
	STDMETHODIMP Receive(IMediaSample* inSample);

	/// Notification that the stream has ended
	virtual STDMETHODIMP EndOfStream();

	/// Notification that the output pin of an upstream filter has connected.
	virtual HRESULT CompleteConnect(IPin* inReceivePin);

	/// Notification the output pin of an upstream filter has been disconnected.
	virtual HRESULT BreakConnect();
	
	//virtual HRESULT DeliverEndFlush(void);
	//virtual HRESULT DeliverBeginFlush(void);

protected:
	OggMuxFilter* mParentFilter;

	bool mNeedsFLACHeaderTweak;
	bool mNeedsFLACHeaderCount;
	
	OggPaginator mPaginator;
	OggMuxStream* mMuxStream;
	fstream debugLog;
};
