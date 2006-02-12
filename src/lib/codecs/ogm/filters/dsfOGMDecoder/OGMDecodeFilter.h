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
//Include Files
#include "ogmdecoderdllstuff.h"
#include "OGMDecodeInputPin.h"
#include <libilliCore/iLE_Math.h>

//Forward Declarations
struct sOGMFormatBlock;
class OGMDecodeInputPin;
class OGMDecodeOutputPin;

//Class Interface
class OGMDecodeFilter
	//Base Classes
	:	public CTransformFilter
{
public:
	//Friends
	//friend class OGMDecodeInputPin;
	//friend class OGMDecodeOutputPin;

	//Constructors and Destructors
	OGMDecodeFilter(void);
	virtual ~OGMDecodeFilter(void);

	//COM Creator Function
	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	virtual HRESULT CheckInputType(const CMediaType* inMediaType);
	virtual HRESULT CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType);
	virtual HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropInputRequest);
	virtual HRESULT GetMediaType(int iPosition, CMediaType* outMediaType);
	virtual HRESULT Transform(IMediaSample* inInputSample, IMediaSample* inOutputSample);

	virtual HRESULT Receive(IMediaSample* inSample);

	virtual HRESULT NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate);

	virtual CBasePin* GetPin(int inPinNo);


protected:
	void deleteBufferedPackets();

	OGMDecodeInputPin* mInputPin;
	CTransformOutputPin* mOutputPin;
	struct sSimplePack {
		unsigned char* mBuff;
		unsigned long mLength;
		unsigned long mDuration;
		unsigned long mHeaderSize;
		bool mIsKeyframe;
		
	};


	
	unsigned long mOGMGranulesBuffered;

	__int64 mSegStart;
	__int64 mSegEnd;
	double mSegRate;

	vector<sSimplePack> mPacketBuffer;
};
