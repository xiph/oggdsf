//===========================================================================
// Copyright (C) 2010 Cristian Adam
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
//- Neither the name of Cristian Adam nor the names of contributors
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
//==============================================================

#include "StdAfx.h"
#include "FilterGraph.h"

#include <InitGuid.h>
#include <uuids.h>

#include "DShowUtil.h"
#include "CustomVMR7Allocator.h"

namespace {
    const GUID CLSID_OggDemuxPacketSourceFilter =
    {0xc9361f5a, 0x3282, 0x4944, {0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb}};

    const GUID CLSID_VorbisDecodeFilter =
    {0x5a1d945, 0xa794, 0x44ef, {0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55}};

    const GUID CLSID_TheoraDecodeFilter =
    {0x5187161, 0x5c36, 0x4324, {0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d}};

    const DWORD ALLOCATOR_ID = 0xDAC10542;
}

FilterGraph::FilterGraph() :
m_customVmrAllocator(0),
m_notifyWindow(0),
m_presentImageMessage(0)
{
}

void FilterGraph::BuildGraph(const CString& videoUrl)
{
    m_videoUrl = videoUrl;

    LOG(logINFO) << __FUNCTIONW__ << " Building graph for: \"" << m_videoUrl << "\"";

    try
    {
        CHECK_HR(m_graphBuilder.CoCreateInstance(CLSID_FilterGraph));

        AddSourceFilter();
        AddDecoders();
        ConnectDecoders();
        AddRenderers();
        ConfigureVideoRenderer();
        ConnnectRenderers();

        CHECK_HR(m_graphBuilder.QueryInterface(&m_mediaControl));
        CHECK_HR(m_graphBuilder.QueryInterface(&m_mediaEvent));

        LOG(logINFO) << __FUNCTIONW__ << " Graph was successfully build.";
    }
    catch (const CAtlException& /*except*/)
    {
    }
}


FilterGraph::~FilterGraph()
{
    if (m_customVmrAllocator)
    {
        m_customVmrAllocator->Release();
    }
}

void FilterGraph::AddSourceFilter()
{
    m_oggSource = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_OggDemuxPacketSourceFilter, 
                                                L"Ogg Source Filter");

    CComQIPtr<IFileSourceFilter> fileSource = m_oggSource;
    CHECK_HR(fileSource->Load(m_videoUrl, 0));
}

void FilterGraph::AddDecoders()
{
    // Connect Vorbis audio decoder
    m_vorbisDecoder = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_VorbisDecodeFilter, 
                                                    L"Vorbis Decoder");

    // Connect Theora video decoder
    m_theoraDecoder = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_TheoraDecodeFilter,
        L"Theora Decoder");
}

void FilterGraph::ConnectDecoders()
{
    // Connect vorbis decoder
    CComPtr<IPin> vorbisDemuxOut = DShowUtil::FindPin(m_oggSource, PINDIR_OUTPUT, 1);
    CComPtr<IPin> vorbisDecoderIn = DShowUtil::FindPin(m_vorbisDecoder, PINDIR_INPUT, 0);

    CHECK_HR(m_graphBuilder->Connect(vorbisDemuxOut, vorbisDecoderIn));

    // Connect theora decoder
    CComPtr<IPin> theoraDemuxOut = DShowUtil::FindPin(m_oggSource, PINDIR_OUTPUT, 0);
    CComPtr<IPin> theoraDecoderIn = DShowUtil::FindPin(m_theoraDecoder, PINDIR_INPUT, 0);

    CHECK_HR(m_graphBuilder->Connect(theoraDemuxOut, theoraDecoderIn));
}

void FilterGraph::AddRenderers()
{
    // Add audio renderer
    m_audioRenderer = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_DSoundRender, 
                                                    L"DirectSound Renderer");
    
    // Add video renderer
    m_videoRenderer = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_VideoMixingRenderer,
                                                    L"Video Renderer");
}

void FilterGraph::ConnnectRenderers()
{
    // Connect audio renderer
    CComPtr<IPin> vorbisDecoderOut = DShowUtil::FindPin(m_vorbisDecoder, PINDIR_OUTPUT, 0);
    CComPtr<IPin> audioRendererIn = DShowUtil::FindPin(m_audioRenderer, PINDIR_INPUT, 0);

    CHECK_HR(m_graphBuilder->Connect(vorbisDecoderOut, audioRendererIn));

    // Connect video renderer
    CComPtr<IPin> theoraDecoderOut = DShowUtil::FindPin(m_theoraDecoder, PINDIR_OUTPUT, 0);
    CComPtr<IPin> videoRendererIn = DShowUtil::FindPin(m_videoRenderer, PINDIR_INPUT, 0);

    CHECK_HR(m_graphBuilder->Connect(theoraDecoderOut, videoRendererIn));
}

void FilterGraph::ConfigureVideoRenderer()
{
    CComQIPtr<IVMRFilterConfig> config = m_videoRenderer;

    if (!config)
    {
        return;
    }

    CHECK_HR(config->SetRenderingMode(VMRMode_Renderless));

    CHECK_HR(m_videoRenderer.QueryInterface(&m_surfaceNotify));

    // Connect our custom VMR7 allocator
    CComObject<CustomVMR7Allocator>::CreateInstance(&m_customVmrAllocator);
    m_customVmrAllocator->AddRef();

    ATLASSERT(GetNotifyWindow() && "SetNotifyWindow should be called first!");
    ATLASSERT(GetPresentImageMessage() && "SetPresentImageMessage should be called first!");
    m_customVmrAllocator->SetNotifyWindow(GetNotifyWindow());
    m_customVmrAllocator->SetPresentImageMessage(GetPresentImageMessage());

    CHECK_HR(m_surfaceNotify->AdviseSurfaceAllocator(ALLOCATOR_ID, m_customVmrAllocator));
    CHECK_HR(m_customVmrAllocator->CreateDefaultSurfaceAllocator());
    CHECK_HR(m_customVmrAllocator->AdviseNotify(m_surfaceNotify));

    CHECK_HR(config->SetNumberOfStreams(1));

    CComQIPtr<IVMRMixerControl> mixerControl = m_videoRenderer;

    DWORD mixingPrefs;
    CHECK_HR(mixerControl->GetMixingPrefs(&mixingPrefs));

    mixingPrefs &= ~MixerPref_RenderTargetMask;
    mixingPrefs |= MixerPref_RenderTargetRGB;

    CHECK_HR(mixerControl->SetMixingPrefs(mixingPrefs));
}

void FilterGraph::Run()
{
    LOG(logINFO) << __FUNCTIONW__;

    if (!m_mediaControl)
    {
        return;
    }

    try
    {
        CHECK_HR(m_mediaControl->Run());
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

void FilterGraph::Pause()
{
    LOG(logINFO) << __FUNCTIONW__;

    if (!m_mediaControl)
    {
        return;
    }

    try
    {
        CHECK_HR(m_mediaControl->Pause());
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

void FilterGraph::Stop()
{
    LOG(logINFO) << __FUNCTIONW__;

    if (!m_mediaControl)
    {
        return;
    }

    try
    {
        CHECK_HR(m_mediaControl->Stop());
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

HWND FilterGraph::GetNotifyWindow() const
{
    return m_notifyWindow;
}

void FilterGraph::SetNotifyWindow( HWND val )
{
    m_notifyWindow = val;
}

int FilterGraph::GetPresentImageMessage() const
{
    return m_presentImageMessage;
}

void FilterGraph::SetPresentImageMessage( int val )
{
    m_presentImageMessage = val;
}

HANDLE FilterGraph::GetMovieEventHandle()
{
    HANDLE movieEventHandle = INVALID_HANDLE_VALUE;
    
    if (!m_mediaEvent)
    {
        return movieEventHandle;
    }

    try
    {
        CHECK_HR(m_mediaEvent->GetEventHandle((OAEVENT *)&movieEventHandle));
    }
    catch (const CAtlException& /*except*/)
    {
    }

    return movieEventHandle;
}

long FilterGraph::GetMovieEventCode()
{
    long eventCode = 0;

    if (!m_mediaEvent)
    {
        return eventCode;
    }

    try
    {
        LONG_PTR  lParam1, lParam2;
        CHECK_HR(m_mediaEvent->GetEvent(&eventCode, &lParam1, &lParam2, 0));
        CHECK_HR(m_mediaEvent->FreeEventParams(eventCode, lParam1, lParam2));
    }
    catch (const CAtlException& /*except*/)
    {
    }

    return eventCode;
}
