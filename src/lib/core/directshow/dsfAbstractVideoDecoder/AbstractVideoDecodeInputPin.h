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
#include "abstractVideodllstuff.h"
#include "BasicSeekable.h"
class AbstractVideoDecodeOutputPin;
class AbstractVideoDecodeFilter;

class ABS_VIDEO_DEC_API AbstractVideoDecodeInputPin 
	:	public CBaseInputPin
	,	public BasicSeekable
{
public:
	friend class AbstractVideoDecodeOutputPin;
	AbstractVideoDecodeInputPin(AbstractVideoDecodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoDecodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName, CMediaType* inAcceptMediaType);
	virtual ~AbstractVideoDecodeInputPin(void);
	static const short SIZE_16_BITS = 2;
	static const unsigned long HUNDRED_NANOS = 10000000;
	static const signed short SINT_MAX = 32767;
	static const signed short SINT_MIN = -32768;

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	
	//PURE VIRTUALS
	virtual long decodeData(unsigned char* inBuf, long inNumBytes, LONGLONG inStart, LONGLONG inEnd) = 0;
	virtual bool ConstructCodec() = 0;
	virtual void DestroyCodec() = 0;
	

	virtual HRESULT BreakConnect();
	virtual HRESULT CompleteConnect (IPin *inReceivePin);
	STDMETHODIMP Receive(IMediaSample *pSample);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);
	virtual HRESULT GetMediaType(int inPosition, CMediaType *outMediaType);
	virtual HRESULT SetMediaType(const CMediaType* inMediaType) = 0;

	virtual void ResetFrameCount();

	virtual STDMETHODIMP EndOfStream(void);
	virtual STDMETHODIMP BeginFlush();
	virtual STDMETHODIMP EndFlush();
	virtual STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

protected:

	virtual bool SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime);

	////fstream debugLog;

	HRESULT mHR;
	bool mBegun;
	AbstractVideoDecodeOutputPin* mOutputPin;
	//__int64 mUptoFrame;
	__int64 mLastSeenStartGranPos;
	AbstractVideoDecodeFilter* mParentFilter;
	CMediaType* mAcceptableMediaType;

	LONGLONG mFrameDuration;
	LONGLONG mFrameSize;
	LONGLONG mFrameCount;

	unsigned long mHeight;
	unsigned long mWidth;

	CCritSec* mStreamLock;


	REFERENCE_TIME mSeekTimeBase;
};
