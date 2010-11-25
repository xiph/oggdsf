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
#include "theoraencoderdllstuff.h"
#include "TheoraEncoder.h"


//Mmmmm macrolicious !
//#define INT_FLOOR(num,scale) (num - (num % scale))
#define CLIP3(x,y,z) ((z < x) ? x : ((z > y) ? y : z))
//
//

class TheoraEncodeOutputPin;
class TheoraEncodeInputPin
	:	public AbstractTransformInputPin
{
public:
	TheoraEncodeInputPin(AbstractTransformFilter* inParentFilter, CCritSec* inFilterLock, AbstractTransformOutputPin* inOutputPin, vector<CMediaType*> inAcceptableMediaTypes);
	virtual ~TheoraEncodeInputPin(void);
	
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
	//

	theora_info* theoraInfo();

    bool GetFlipImageVerticaly() const;
    void SetFlipImageVerticaly(bool val);

    StampedOggPacket** GetCodecHeaders(); 

protected:

	//PURE VIRTUALS
	virtual long TransformData(unsigned char* inBuf, long inNumBytes);
	virtual bool ConstructCodec();
	virtual void DestroyCodec();

	HRESULT deliverData(LONGLONG inStart, LONGLONG inEnd, unsigned char* inBuf, unsigned long inNumBytes);

	long encodeYV12ToYV12(unsigned char* inBuf, long inNumBytes);
	long encodeYUY2ToYV12(unsigned char* inBuf, long inNumBytes);
	long encodeAYUVtoYV12(unsigned char* inBuf, long inNumBytes);
	long encodeRGB24toYV12(unsigned char* inBuf, long inNumBytes);
	long encodeRGB32toYV12(unsigned char* inBuf, long inNumBytes);
	long encodeUYVYToYV12(unsigned char* inBuf, long inNumBytes);
	long encodeYVYUToYV12(unsigned char* inBuf, long inNumBytes);
	long encodeIYUVToYV12(unsigned char* inBuf, long inNumBytes);

	HRESULT encodeMoreFrames();

protected:
	HRESULT m_hr;

	TheoraEncoder m_theoraEncoder;
	theora_info m_theoraInfo;
	yuv_buffer m_yuv;

	unsigned long m_xOffset;
	unsigned long m_yOffset;

	unsigned long m_height;
	unsigned long m_width;
	REFERENCE_TIME	m_averageTimePerFrame;

	unsigned __int64 m_uptoFrame;

	CMediaType m_pinInputType;

	bool m_hasBegun;

    bool m_flipImageVerticaly;
	/* To adapt theora frame rate to variable directshow clock */
	unsigned __int64 m_numFrames;

	//DEBUG ONLY
	//fstream debugLog;
};
