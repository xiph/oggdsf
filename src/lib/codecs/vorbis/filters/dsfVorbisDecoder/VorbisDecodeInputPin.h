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
#include <vector>
#include <fstream>
using namespace std;
#include "vorbisdecoderdllstuff.h"
#include "AbstractTransformInputPin.h"
#include "VorbisDecodeInputPin.h"

#include "VorbisDecodeFilter.h"

extern "C" {
//#include <fishsound/fishsound.h>
#include "fish_cdecl.h"
}

class VorbisDecodeOutputPin;

class VorbisDecodeInputPin 
	:	public AbstractTransformInputPin
{
public:

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	VorbisDecodeInputPin(AbstractTransformFilter* inFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~VorbisDecodeInputPin(void);
	static int __cdecl VorbisDecoded (FishSound* inFishSound, float** inPCM, long inFrames, void* inThisPointer);

	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	virtual STDMETHODIMP NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate);


protected:
	fstream debugLog;



	//Implementation of virtuals from AbstractTransform Filter
	virtual bool ConstructCodec();
	virtual void DestroyCodec();
	virtual HRESULT TransformData(unsigned char* inBuf, long inNumBytes);

	HRESULT mHR;
	bool mBegun;

	FishSound* mFishSound;
	FishSoundInfo mFishInfo; 

	int mNumChannels;
	int mFrameSize;
	int mSampleRate;
	unsigned int mUptoFrame;

};


//Old imp
//************************************************************
//#pragma once
//#include <fstream>
//using namespace std;
//#include "vorbisdecoderdllstuff.h"
//#include "AbstractAudioDecodeInputPin.h"
//#include "VorbisDecodeInputPin.h"
//
//#include "VorbisDecodeFilter.h"
//
//extern "C" {
////#include <fishsound/fishsound.h>
//#include "fish_cdecl.h"
//}
//
//class VorbisDecodeOutputPin;
//
//class VorbisDecodeInputPin 
//	:	public AbstractAudioDecodeInputPin
//{
//public:
//
//	DECLARE_IUNKNOWN
//	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
//	VorbisDecodeInputPin(AbstractAudioDecodeFilter* inFilter, CCritSec* inFilterLock, AbstractAudioDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType);
//	virtual ~VorbisDecodeInputPin(void);
//	static int __cdecl VorbisDecoded (FishSound* inFishSound, float** inPCM, long inFrames, void* inThisPointer);
//
//	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
//
//	//VIRTUAL FUNCTIONS - AbstractAudioDecodeInputPin
//	//FIX:::These should be protected.
//	virtual bool ConstructCodec();
//	virtual void DestroyCodec();
//
//	long decodeData(unsigned char* inBuf, long inNumBytes);
//
//protected:
//
//	HRESULT mHR;
//	bool mBegun;
//
//	FishSound* mFishSound;
//	FishSoundInfo mFishInfo; 
//
//};
