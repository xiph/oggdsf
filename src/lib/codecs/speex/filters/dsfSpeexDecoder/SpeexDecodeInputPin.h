//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "IOggDecoder.h"
#include "AbstractTransformInputPin.h"
#include "SpeexDecodeInputPin.h"

#include "SpeexDecodeFilter.h"

#include "SpeexDecoder.h"

//Forward declaration
class SpeexDecodeOutputPin;

class SpeexDecodeInputPin 
	:	public AbstractTransformInputPin
	,	public IOggDecoder
{
public:
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	SpeexDecodeInputPin(		AbstractTransformFilter* inFilter
							,	CCritSec* inFilterLock
							,	AbstractTransformOutputPin* inOutputPin
							,	vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~SpeexDecodeInputPin(void);
	
	///Called when the connection type is determined
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	///Called to check if the media type is acceptable to this pin
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);

	///New segment messages pass downstream to inform a new segment is about to play
	virtual STDMETHODIMP NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate);

	///Called at the end of a flush operation
	virtual STDMETHODIMP EndFlush();

	///Called by the connecting pin to ask this pins buffer requirements
	virtual STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps);

	///Receive is the main data path, data travels downstream through successive Receive's
	virtual STDMETHODIMP Receive(IMediaSample* inSample);

	///Called by upstream(demux) to get a conversion from granules to time
	virtual LOOG_INT64 __stdcall convertGranuleToTime(LOOG_INT64 inGranule);

	///Called by upstream to determine preroll data, ie how far back to seek to decode the given granule
	virtual LOOG_INT64 __stdcall mustSeekBefore(LOOG_INT64 inGranule);

	///Called during pin setup to prime the decoder
	virtual IOggDecoder::eAcceptHeaderResult __stdcall showHeaderPacket(OggPacket* inCodecHeaderPacket);

	///Returns the short name for this codec "speex"
	virtual string __stdcall getCodecShortName();

	///Returns the codec ident string, could include version, build etc info. Purely for display purposes.
	virtual string __stdcall getCodecIdentString();


protected:
	static const unsigned long DECODED_BUFFER_SIZE = 1<<20;		//1 Meg buffer
	static const unsigned long SPEEX_IDENT_HEADER_SIZE = 80;
	static const unsigned long SPEEX_NUM_BUFFERS = 75;
	static const unsigned long SPEEX_BUFFER_SIZE = 65536;

	//Implementation of pure virtuals from AbstractTransformInputPin
	virtual bool ConstructCodec();
	virtual void DestroyCodec();

	///Internal method for doing decoding into the main buffer
	virtual HRESULT TransformData(unsigned char* inBuf, long inNumBytes);

	unsigned char* mDecodedBuffer;
	unsigned long mDecodedByteCount;

	SpeexDecoder mSpeexDecoder;

	unsigned long mNumChannels;
	unsigned long mSampleFrameSize;
	unsigned long mSpeexFrameSize;
	unsigned long mSampleRate;

	//TODO::: NEEDED???
	unsigned int mUptoFrame;
	bool mBegun;

	enum eSpeexSetupState {
		VSS_SEEN_NOTHING,
		VSS_SEEN_BOS,
		VSS_SEEN_COMMENT,
		VSS_ALL_HEADERS_SEEN,
		VSS_ERROR
	};

	eSpeexSetupState mSetupState;

	__int64 mRateNumerator;
	static const __int64 RATE_DENOMINATOR = 65536;

};
