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

#include "abstractaudioencodedllstuff.h"

//Debug Only
//#include <fstream>
//
//using namespace std;

class AbstractAudioEncodeOutputPin;
class AbstractAudioEncodeFilter;

class ABS_AUDIO_ENC_API AbstractAudioEncodeInputPin 
	:	public CBaseInputPin
	//,	public BasicSeekable
{
public:
		
	friend class AbstractAudioEncodeOutputPin;
	AbstractAudioEncodeInputPin(AbstractAudioEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractAudioEncodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName);
	virtual ~AbstractAudioEncodeInputPin(void);
	static const short SIZE_16_BITS = 2;
	static const unsigned long HUNDRED_NANOS = 10000000;
	static const signed short SINT_MAX = 32767;
	static const signed short SINT_MIN = -32768;

	
	

	//PURE VIRTUALS
	virtual long encodeData(unsigned char* inBuf, long inNumBytes) = 0;
	virtual bool ConstructCodec() = 0;
	virtual void DestroyCodec() = 0;
	

	STDMETHODIMP Receive(IMediaSample *pSample);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);
	virtual HRESULT GetMediaType(int inPosition, CMediaType *outMediaType);
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	virtual void ResetFrameCount();

	virtual STDMETHODIMP EndOfStream(void);
	virtual STDMETHODIMP BeginFlush();
	virtual STDMETHODIMP EndFlush();
	virtual STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

protected:

	virtual bool SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime);


	HRESULT mHR;
	bool mBegun;
	AbstractAudioEncodeOutputPin* mOutputPin;
	__int64 mUptoFrame;
	AbstractAudioEncodeFilter* mParentFilter;
	CMediaType* mAcceptableMediaType;

	//Debug only
	//fstream debugLog;
	//

	//unsigned long mFrameSize;
	//unsigned long mNumChannels;
	//unsigned long mSampleRate;

	WAVEFORMATEX* mWaveFormat;

};
