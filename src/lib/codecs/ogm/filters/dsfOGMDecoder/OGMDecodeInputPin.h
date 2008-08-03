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

#include "ogmdecoderdllstuff.h"
#include "IOggDecoder.h"

class OGMDecodeFilter;

class OGMDecodeInputPin
	:	public CTransformInputPin
	,	public IOggDecoder
{
public:
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	OGMDecodeInputPin(OGMDecodeFilter* inParent, HRESULT* outHR);
	virtual ~OGMDecodeInputPin(void);

	virtual STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps);
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);

	

	//IOggDecoder Interface
	virtual LOOG_INT64 __stdcall convertGranuleToTime(LOOG_INT64 inGranule);
	virtual LOOG_INT64 __stdcall mustSeekBefore(LOOG_INT64 inGranule);
	virtual IOggDecoder::eAcceptHeaderResult __stdcall showHeaderPacket(OggPacket* inCodecHeaderPacket);
	virtual string __stdcall getCodecShortName();
	virtual string __stdcall getCodecIdentString();

	enum eOGMMediaType {
		OGM_VIDEO_TYPE,
		OGM_AUDIO_TYPE,
		OGM_TEXT_TYPE,
		OGM_UNKNOWN_TYPE
	};

	eOGMMediaType getOGMMediaType()				{		return mOGMMediaType;		}
	VIDEOINFOHEADER* getVideoFormatBlock()		{		return mVideoFormatBlock;	}
	WAVEFORMATEX* getAudioFormatBlock()			{		return mAudioFormatBlock;	}
protected:
	enum eOGMSetupState {
		VSS_SEEN_NOTHING,
		VSS_SEEN_BOS,
		VSS_SEEN_COMMENT,
		VSS_ALL_HEADERS_SEEN,
		VSS_ERROR
	};

	eOGMMediaType mOGMMediaType;

	eOGMSetupState mSetupState;
	bool handleHeaderPacket(OggPacket* inHeaderPack);
	bool handleAudioHeaderPacket(OggPacket* inHeaderPack);
	bool handleVideoHeaderPacket(OggPacket* inHeaderPack);
	bool handleTextHeaderPacket(OggPacket* inHeaderPack);

	VIDEOINFOHEADER* mVideoFormatBlock;
	WAVEFORMATEX* mAudioFormatBlock;
	__int64 mGranuleRateNumerator;
	__int64 mGranuleRateDenominator;

	static const unsigned long OGM_IDENT_HEADER_SIZE = 57;
	static const unsigned long OGM_NUM_BUFFERS = 100;
	static const unsigned long OGM_BUFFER_SIZE = 1024*512*3;;

};
