//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//Copyright (C) 2008, 2009 Cristian Adam
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

//Library Includes
#include "BasicSeekPassThrough.h"

//Forward Declararions.
class NativeFLACSourceFilter;

class NativeFLACSourcePin: public CBaseOutputPin, public BasicSeekPassThrough
{
public:
    //Constants
    static const unsigned long NUM_BUFFERS = 16;

public:
    //COM Stuff
    DECLARE_IUNKNOWN
    HRESULT __stdcall NonDelegatingQueryInterface(REFIID riid, void **ppv);

    //Constructors.
    NativeFLACSourcePin(NativeFLACSourceFilter* inParentFilter, CCritSec* inFilterLock);
    virtual ~NativeFLACSourcePin(void);

    //CBaseOutputPin virtuals
    virtual HRESULT GetMediaType(int inPosition, CMediaType* outMediaType);
    virtual HRESULT SetMediaType(const CMediaType* inMediaType);
    virtual HRESULT CheckMediaType(const CMediaType* inMediaType);
    virtual HRESULT DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest);

    //IPin virtuals
    virtual HRESULT CompleteConnect (IPin *inReceivePin);
    virtual HRESULT BreakConnect(void);
    virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    virtual HRESULT DeliverEndOfStream(void);
    virtual HRESULT DeliverEndFlush(void);
    virtual HRESULT DeliverBeginFlush(void);

    //Helper method
    HRESULT DeliverData(unsigned char* inBuff, unsigned long inBuffSize, __int64 inStart, __int64 inEnd);

    void FillMediaType(CMediaType& mediaType, bool useWaveFormatEx);

protected:
    HRESULT m_filterHR;
    
    //Member variables.
    COutputQueue* m_dataQueue;
    NativeFLACSourceFilter* m_parentFilter;

    bool m_haveDiscontinuity;
};
