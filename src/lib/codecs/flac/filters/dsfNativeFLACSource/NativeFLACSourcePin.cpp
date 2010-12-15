//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//Copyright (C) 2008, 2010 Cristian Adam
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
#include "stdafx.h"
#include "NativeFLACSourcePin.h"
#include "NativeFLACSourceFilter.h"

#include <sstream>

NativeFLACSourcePin::NativeFLACSourcePin(NativeFLACSourceFilter* inParentFilter, CCritSec* inFilterLock):   
CBaseOutputPin(NAME("Native FLAC Source Pin"), inParentFilter, inFilterLock, &m_filterHR, L"PCM Out"),
m_parentFilter(inParentFilter),
m_dataQueue(NULL),
m_haveDiscontinuity(true)
{
    //Subvert COM and do this directly... this way, the source filter won't expose the interface to the
    // graph but we can still delegate to it.
    IMediaSeeking* locSeeker = NULL;
    locSeeker = (IMediaSeeking*)m_parentFilter;
    SetDelegate(locSeeker);
}

NativeFLACSourcePin::~NativeFLACSourcePin(void)
{
    SetDelegate(NULL);      //Avoid infinite destructor loop.
    delete m_dataQueue;
    m_dataQueue = NULL;
}

HRESULT __stdcall NativeFLACSourcePin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    if (riid == IID_IMediaSeeking) 
    {
        return GetInterface((IMediaSeeking*) this, ppv);
    }
    
    return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT NativeFLACSourcePin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
    LOG(logINFO) << "DeliverNewSegment: start " << ReferenceTime(tStart) << ", end " << ReferenceTime(tStop);

    if (m_dataQueue != 0)
    {
        m_dataQueue->NewSegment(tStart, tStop, dRate);
    }

    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverEndOfStream()
{
    if (m_dataQueue != 0)
    {
        m_dataQueue->EOS();
    }

    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverEndFlush()
{
    if (m_dataQueue != 0)
    {
        m_dataQueue->EndFlush();
    }
    
    m_haveDiscontinuity = true;

    return S_OK;
}

HRESULT NativeFLACSourcePin::DeliverBeginFlush()
{
    if (m_dataQueue != 0)
    {
        m_dataQueue->BeginFlush();
    }

    return S_OK;
}

HRESULT NativeFLACSourcePin::CompleteConnect (IPin *inReceivePin)
{
    m_filterHR = S_OK;
    //Deleted in destructor
    m_dataQueue = new COutputQueue (inReceivePin, &m_filterHR, FALSE, TRUE, 1, TRUE, NUM_BUFFERS);
    
    if (FAILED(m_filterHR)) 
    {
        //TODO::: Probably should handle this !
        //CHECK::: See if it ever silently reports failure but actually does work before bailing here.
    }
    
    return CBaseOutputPin::CompleteConnect(inReceivePin);
}

HRESULT NativeFLACSourcePin::BreakConnect() 
{
    delete m_dataQueue;
    m_dataQueue = NULL;
    return CBaseOutputPin::BreakConnect();
}


HRESULT NativeFLACSourcePin::SetMediaType(const CMediaType* inMediaType)
{
    return CBaseOutputPin::SetMediaType(inMediaType);
}

void NativeFLACSourcePin::FillMediaType(CMediaType& mediaType, bool useWaveFormatEx)
{
    if (useWaveFormatEx)
    {
        // Windows CE doesn't have ksmedia.h, or at least Windows CE 5.0
#ifndef WINCE
        mediaType.SetType(&MEDIATYPE_Audio);
        mediaType.SetSubtype(&MEDIASUBTYPE_PCM);
        mediaType.SetFormatType(&FORMAT_WaveFormatEx);
        mediaType.SetTemporalCompression(FALSE);
        mediaType.SetSampleSize(0);

        WAVEFORMATEXTENSIBLE* formatEx = (WAVEFORMATEXTENSIBLE*)mediaType.AllocFormatBuffer(sizeof(WAVEFORMATEXTENSIBLE));

        formatEx->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;

        formatEx->Format.nChannels = m_parentFilter->m_numChannels;
        formatEx->Format.nSamplesPerSec = m_parentFilter->m_sampleRate;

        formatEx->Samples.wValidBitsPerSample = m_parentFilter->m_significantBitsPerSample;

        switch (formatEx->Format.nChannels)
        {
        case 1:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_MONO;
            break;
        case 2:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_STEREO;
            break;
        case 3:
            formatEx->dwChannelMask = SPEAKER_FRONT_LEFT
                                    | SPEAKER_FRONT_RIGHT
                                    | SPEAKER_FRONT_CENTER;
            break;
        case 4:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_QUAD;
            break;
        case 5:
            formatEx->dwChannelMask = SPEAKER_FRONT_LEFT
                                    | SPEAKER_FRONT_RIGHT
                                    | SPEAKER_FRONT_CENTER
                                    | SPEAKER_BACK_LEFT
                                    | SPEAKER_BACK_RIGHT;
            break;
        case 6:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
            break;
        case 7:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_5POINT1_SURROUND 
                                    | SPEAKER_BACK_CENTER;
            break;
        case 8:
            formatEx->dwChannelMask = KSAUDIO_SPEAKER_7POINT1_SURROUND;
            break;
        default:
            formatEx->dwChannelMask = 0;
            break;
        }

        formatEx->Format.wBitsPerSample = m_parentFilter->m_bitsPerSample;
        formatEx->Format.nBlockAlign = m_parentFilter->m_numChannels * (m_parentFilter->m_bitsPerSample >> 3);
        formatEx->Format.nAvgBytesPerSec = formatEx->Format.nBlockAlign * m_parentFilter->m_sampleRate;
        formatEx->Format.cbSize = 22; //sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
        formatEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
#endif
    }
    else
    {
        mediaType.SetType(&MEDIATYPE_Audio);
        mediaType.SetSubtype(&MEDIASUBTYPE_PCM);
        mediaType.SetFormatType(&FORMAT_WaveFormatEx);
        mediaType.SetTemporalCompression(FALSE);
        mediaType.SetSampleSize(0);

        WAVEFORMATEX* format = (WAVEFORMATEX*)mediaType.AllocFormatBuffer(sizeof(WAVEFORMATEX));
        format->wFormatTag = WAVE_FORMAT_PCM;

        format->nChannels = m_parentFilter->m_numChannels;
        format->nSamplesPerSec =  m_parentFilter->m_sampleRate;
        format->wBitsPerSample = m_parentFilter->m_bitsPerSample;
        format->nBlockAlign = m_parentFilter->m_numChannels * (m_parentFilter->m_bitsPerSample >> 3);
        format->nAvgBytesPerSec = format->nBlockAlign * m_parentFilter->m_sampleRate;
        format->cbSize = 0;
    }
}

HRESULT NativeFLACSourcePin::GetMediaType(int inPosition, CMediaType* outMediaType) 
{
    if (outMediaType == 0)
    {
        return E_POINTER;
    }

    HRESULT result = VFW_S_NO_MORE_ITEMS;

#ifdef WINCE
    if (inPosition == 0)
    {
        FillMediaType(*outMediaType, false);
        result = S_OK;
    }
#else
    if (inPosition == 0) 
    {
        FillMediaType(*outMediaType, true);
        
        result = S_OK;
    } 
    else if (inPosition == 1) 
    {
        FillMediaType(*outMediaType, false);
    
        result = S_OK;
    }
#endif
    
    return result;
}

HRESULT NativeFLACSourcePin::CheckMediaType(const CMediaType* inMediaType) 
{
    //WFE::: Do check for extensible type
    if (inMediaType->majortype == MEDIATYPE_Audio &&
        inMediaType->subtype == MEDIASUBTYPE_PCM &&
        inMediaType->formattype == FORMAT_WaveFormatEx) 
    {
        return S_OK;
    } 
    else 
    {
        return E_FAIL;
    }
}

HRESULT NativeFLACSourcePin::DecideBufferSize(IMemAllocator* inoutAllocator, ALLOCATOR_PROPERTIES* inoutInputRequest) 
{
    HRESULT hr = S_OK;

    ALLOCATOR_PROPERTIES reqAlloc;
    ALLOCATOR_PROPERTIES actualAlloc;

    reqAlloc.cbAlign = 1;
    reqAlloc.cbBuffer = m_parentFilter->m_numChannels * (m_parentFilter->m_bitsPerSample >> 3) * m_parentFilter->m_sampleRate;
    reqAlloc.cbPrefix = 0;
    reqAlloc.cBuffers = NUM_BUFFERS;

    hr = inoutAllocator->SetProperties(&reqAlloc, &actualAlloc);

    if (hr != S_OK) 
    {
        return hr;
    }
    
    hr = inoutAllocator->Commit();

    return hr;
}

//This method is responsible for deleting the incoming buffer.
HRESULT NativeFLACSourcePin::DeliverData(unsigned char* inBuff, unsigned long inBuffSize, __int64 inStart, __int64 inEnd) 
{
    //Locks !!
    
    IMediaSample* pSample = NULL;
    REFERENCE_TIME locStart = inStart;
    REFERENCE_TIME locStop = inEnd;
    
    HRESULT locHR = GetDeliveryBuffer(&pSample, &locStart, &locStop, NULL);
    
    //Error checks
    if (locHR != S_OK) 
    {
        delete[] inBuff;
        return locHR;
    }

    pSample->SetTime(&locStart, &locStop);
    
    pSample->SetSyncPoint(TRUE);

    // Create a pointer for the samples buffer
    BYTE* locBuffer = NULL;
    pSample->GetPointer(&locBuffer);

    unsigned long sampleSize = pSample->GetSize();

    if (m_haveDiscontinuity)
    {
        m_haveDiscontinuity = false;

        pSample->SetDiscontinuity(true);

        DeliverNewSegment(locStart, locStop, 1.0);
    }

    if (sampleSize >= inBuffSize) 
    {
        memcpy((void*)locBuffer, (const void*)inBuff, inBuffSize);
        pSample->SetActualDataLength(inBuffSize);

        locHR = m_dataQueue->Receive(pSample);

        LOG(logDEBUG3) << "Deliver Data: size " << inBuffSize << ", hr: 0x" << std::hex << locHR
            << ", Start: " << ToString(locStart) << ", Stop: " << ToString(locStop);

        delete [] inBuff;
        return locHR;
    } 
    else 
    {
        delete[] inBuff;
        throw 0;
    }
}

