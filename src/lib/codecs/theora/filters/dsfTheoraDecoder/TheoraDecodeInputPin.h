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


//----------------------
//OLD IMPLEMENTATION
//----------------------
//
//#include "Theoradecoderdllstuff.h"
////#include "AbstractVideoDecodeInputPin.h"
////#include "TheoraDecodeInputPin.h"
//
////#include "TheoraDecodeFilter.h"
//#include "DSStringer.h"
//#include "TheoraDecoder.h"
//#include <math.h>
//#include <fstream>
//using namespace std;
//
//
//class TheoraDecodeOutputPin;
//
//class TheoraDecodeInputPin 
//	:	public AbstractVideoDecodeInputPin
//{
//public:
//	TheoraDecodeInputPin(AbstractVideoDecodeFilter* inFilter, CCritSec* inFilterLock, AbstractVideoDecodeOutputPin* inOutputPin, CMediaType* inAcceptMediaType);
//	virtual ~TheoraDecodeInputPin(void);
//	int TheoraDecoded (yuv_buffer* inYUVBuffer);
//
//		DECLARE_IUNKNOWN
//	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
//
//	HRESULT SetMediaType(const CMediaType* inMediaType);
//
//	//VIRTUAL FUNCTIONS - AbstractAudioDecodeInputPin
//	//FIX:::These should be protected.
//	virtual bool ConstructCodec();
//	virtual void DestroyCodec();
//
//	long decodeData(BYTE* inBuf, long inNumBytes, LONGLONG inStart, LONGLONG inEnd) ;
//
//protected:
//	fstream debugLog;
//	//FishSound* mFishSound;
//	//FishSoundInfo mFishInfo; 
//	TheoraDecoder* mTheoraDecoder;
//	unsigned long mXOffset;
//	unsigned long mYOffset;
//	
//
//
//};
