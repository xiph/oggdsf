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
#include "CustomVMR9Allocator.h"

namespace {
    const GUID CLSID_OggDemuxFilter =
    {0xc9361f5a, 0x3282, 0x4944, {0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb}};

    const GUID CLSID_WebmDemuxFilter = 
    {0xed3110f8, 0x5211, 0x11df, {0x94, 0xaf, 0x00, 0x26, 0xb9, 0x77, 0xee, 0xaa}};

    const GUID CLSID_VorbisDecodeFilter =
    {0x5a1d945, 0xa794, 0x44ef, {0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55}};

    const GUID CLSID_TheoraDecodeFilter =
    {0x5187161, 0x5c36, 0x4324, {0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d}};

    const GUID CLSID_VP8DecoderFilter =
    {0xed3110f3, 0x5211, 0x11df, {0x94, 0xaf, 0x00, 0x26, 0xb9, 0x77, 0xee, 0xaa}};

    const DWORD ALLOCATOR_ID = 0xDAC10542;
}

FilterGraph::FilterGraph() :
m_customVmrAllocator(0),
m_notifyWindow(0),
m_presentImageMessage(0),
m_haveAudio(true),
m_haveVideo(true),
m_haveWebm(false)
{
}

void FilterGraph::BuildGraph(const CString& videoUrl)
{
    m_videoUrl = videoUrl;

    const CString WEBM_EXT(L".webm");
    m_haveWebm = m_videoUrl.Right(WEBM_EXT.GetLength()).MakeLower() == WEBM_EXT;

    LOG(logINFO) << __FUNCTIONW__ << " Building graph for: \"" << m_videoUrl << "\"";
    LOG(logINFO) << __FUNCTIONW__ << " Have WebM: " << std::boolalpha << m_haveWebm;

    try
    {
        CHECK_HR(m_graphBuilder.CoCreateInstance(CLSID_FilterGraph));

        AddSourceDemuxFilters();
        AddDecoders();
        ConnectDecoders();
        AddRenderers();
        ConfigureVideoRenderer();
        ConnnectRenderers();

        CHECK_HR(m_graphBuilder.QueryInterface(&m_mediaControl));
        CHECK_HR(m_graphBuilder.QueryInterface(&m_mediaEvent));
        CHECK_HR(m_graphBuilder.QueryInterface(&m_mediaSeeking));

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

void FilterGraph::AddSourceDemuxFilters()
{
    CLSID sourceCLSID = CLSID_URLReader;

    CString lowerCaseUrl(m_videoUrl);
    lowerCaseUrl.MakeLower();

    if (lowerCaseUrl.Find(L"file://") != -1)
    {
        sourceCLSID = CLSID_AsyncReader;
    }

    // Add the source filter
    m_sourceFilter = DShowUtil::AddFilterFromCLSID(m_graphBuilder, sourceCLSID, L"Source Filter");

    CHECK_HR(m_sourceFilter.QueryInterface(&m_openProgress));

    CComQIPtr<IFileSourceFilter> fileSource = m_sourceFilter;
    CHECK_HR(fileSource->Load(m_videoUrl, 0));

    CLSID clsidDemuxFilter = CLSID_OggDemuxFilter;
    if (m_haveWebm)
    {
        clsidDemuxFilter = CLSID_WebmDemuxFilter;
    }

    // Add the stream demux filter
    m_streamDemux = DShowUtil::AddFilterFromCLSID(m_graphBuilder, clsidDemuxFilter, 
        L"Stream Demux Filter");

    // Connect the source and demux filters
    CComPtr<IPin> sourceOut = DShowUtil::FindPin(m_sourceFilter, PINDIR_OUTPUT);
    CComPtr<IPin> demuxIn = DShowUtil::FindPin(m_streamDemux, PINDIR_INPUT);

    CHECK_HR(m_graphBuilder->Connect(sourceOut, demuxIn));
}

void FilterGraph::AddDecoders()
{
    // Add audio decoder
    m_audioDecoder = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_VorbisDecodeFilter, 
                                                    L"Audio Decoder");

    CLSID clsidVideoDecoder = CLSID_TheoraDecodeFilter;
    if (m_haveWebm)
    {
        clsidVideoDecoder = CLSID_VP8DecoderFilter;
    }

    // Add video decoder
    m_videoDecoder = DShowUtil::AddFilterFromCLSID(m_graphBuilder, clsidVideoDecoder,
        L"Video Decoder");
}

void FilterGraph::ConnectDecoders()
{
    // Connect audio decoder
    CComPtr<IPin> audioDemuxOut = DShowUtil::FindPin(m_streamDemux, PINDIR_OUTPUT, 1);
    CComPtr<IPin> audioDecoderIn = DShowUtil::FindPin(m_audioDecoder, PINDIR_INPUT, 0);

    if (audioDemuxOut)
    {
        CHECK_HR(m_graphBuilder->Connect(audioDemuxOut, audioDecoderIn));
    }
    else
    {
        m_haveAudio = false;
    }

    // Connect video decoder
    CComPtr<IPin> videoDemuxOut = DShowUtil::FindPin(m_streamDemux, PINDIR_OUTPUT, 0);
    CComPtr<IPin> videoDecoderIn = DShowUtil::FindPin(m_videoDecoder, PINDIR_INPUT, 0);

    if (videoDemuxOut)
    {
        CHECK_HR(m_graphBuilder->Connect(videoDemuxOut, videoDecoderIn));
    }
    else
    {
        m_haveVideo = false;
    }
}

void FilterGraph::AddRenderers()
{
    // Add audio renderer
    m_audioRenderer = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_DSoundRender, 
                                                    L"DirectSound Renderer");

    m_audioRenderer->QueryInterface(&m_basicAudio);

    // Add video renderer
    m_videoRenderer = DShowUtil::AddFilterFromCLSID(m_graphBuilder, CLSID_VideoMixingRenderer9,
                                                    L"Video Mixing Renderer 9");
}

void FilterGraph::ConnnectRenderers()
{
    // Connect audio renderer
    CComPtr<IPin> audioDecoderOut = DShowUtil::FindPin(m_audioDecoder, PINDIR_OUTPUT, 0);
    CComPtr<IPin> audioRendererIn = DShowUtil::FindPin(m_audioRenderer, PINDIR_INPUT, 0);

    if (m_haveAudio)
    {
        CHECK_HR(m_graphBuilder->Connect(audioDecoderOut, audioRendererIn));
    }

    // Connect video renderer
    CComPtr<IPin> videoDecoderOut = DShowUtil::FindPin(m_videoDecoder, PINDIR_OUTPUT, 0);
    CComPtr<IPin> videoRendererIn = DShowUtil::FindPin(m_videoRenderer, PINDIR_INPUT, 0);

    if (m_haveVideo)
    {
        CHECK_HR(m_graphBuilder->Connect(videoDecoderOut, videoRendererIn));
    }
}

void FilterGraph::ConfigureVideoRenderer()
{
    CComQIPtr<IVMRFilterConfig9> config = m_videoRenderer;

    if (!config)
    {
        return;
    }

    CHECK_HR(config->SetRenderingMode(VMR9Mode_Renderless));
    CHECK_HR(m_videoRenderer.QueryInterface(&m_surfaceNotify));

    // Connect our custom VMR9 allocator
    CComObject<CustomVMR9Allocator>::CreateInstance(&m_customVmrAllocator);
    m_customVmrAllocator->AddRef();

    m_customVmrAllocator->SetNotifyWindow(GetNotifyWindow());
    m_customVmrAllocator->SetPresentImageMessage(GetPresentImageMessage());
    m_customVmrAllocator->SetD3D(GetD3D());
    m_customVmrAllocator->SetD3DDevice(GetD3DDevice());

    CHECK_HR(m_surfaceNotify->AdviseSurfaceAllocator(ALLOCATOR_ID, m_customVmrAllocator));
    CHECK_HR(m_customVmrAllocator->AdviseNotify(m_surfaceNotify));

    CHECK_HR(config->SetNumberOfStreams(1));

    CComQIPtr<IVMRMixerControl9> mixerControl = m_videoRenderer;

    DWORD mixingPrefs;
    CHECK_HR(mixerControl->GetMixingPrefs(&mixingPrefs));

    mixingPrefs &= ~MixerPref9_RenderTargetMask;
    mixingPrefs |= MixerPref9_RenderTargetRGB;

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
    ATLASSERT(m_notifyWindow && "SetNotifyWindow should be called first!");
    return m_notifyWindow;
}

void FilterGraph::SetNotifyWindow( HWND val )
{
    m_notifyWindow = val;
}

int FilterGraph::GetPresentImageMessage() const
{
    ATLASSERT(m_presentImageMessage && "SetPresentImageMessage should be called first!");
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

CComPtr<IDirect3DDevice9> FilterGraph::GetD3DDevice() const
{
    ATLASSERT(m_d3dDevice && L"SetD3DDevice should have been called before");
    return m_d3dDevice;
}

void FilterGraph::SetD3DDevice(const CComPtr<IDirect3DDevice9>& val)
{
    m_d3dDevice = val;
}

CComPtr<IDirect3D9> FilterGraph::GetD3D() const
{
    ATLASSERT(m_d3d && L"SetD3D should have been called before");
    return m_d3d;
}

void FilterGraph::SetD3D(const CComPtr<IDirect3D9>& val)
{
    m_d3d = val;
}

long FilterGraph::GetVolume() const
{
    if (!m_haveAudio)
    {
        return MIN_VOLUME;
    }

    if (!m_basicAudio)
    {
        return MIN_VOLUME;
    }

    long volume = MIN_VOLUME;
    try
    {
        CHECK_HR(m_basicAudio->get_Volume(&volume));
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }

    return volume;
}

void FilterGraph::SetVolume(long vol)
{
    if (!m_haveAudio)
    {
        return;
    }

    if (!m_basicAudio)
    {
        return;
    }

    try
    {
        CHECK_HR(m_basicAudio->put_Volume(vol));
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }
}

unsigned long FilterGraph::GetDuration() const
{
    unsigned long duration = 0;
    try
    {
        REFERENCE_TIME rtDuration = 0;
        CHECK_HR(m_mediaSeeking->GetDuration(&rtDuration));

        duration = rtDuration / 10000;
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }

    return duration;
}

unsigned long FilterGraph::GetPosition() const
{
    unsigned long position = 0;
    try
    {
        REFERENCE_TIME rtCurrentPosition = 0;
        CHECK_HR(m_mediaSeeking->GetCurrentPosition(&rtCurrentPosition));

        position = rtCurrentPosition / 10000;
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }

    LOG(logDEBUG2) << __FUNCTIONW__ << " result: " << position;

    return position;
}


void FilterGraph::SetPosition(unsigned long position)
{
    try
    {
        REFERENCE_TIME rtPosition = position;
        rtPosition *= 10000;

        CHECK_HR(m_mediaSeeking->SetPositions(&rtPosition, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning));
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }
}

unsigned long FilterGraph::GetOpenProgress() const
{
    unsigned long percentage = 0;
    try
    {
        REFERENCE_TIME total = 0;
        REFERENCE_TIME current = 0;
        CHECK_HR(m_openProgress->QueryProgress(&total, &current));

        // This should not happen
        ATLASSERT(total != 0);
        if (total == 0)
        {
            total = 1;
        }
        percentage = static_cast<long>((current / static_cast<double>(total)) * 100);
    }
    catch (const CAtlException& except)
    {
        LOG(logERROR) << __FUNCTIONW__ << " Error code: " << std::hex << except.m_hr;
    }

    LOG(logDEBUG2) << __FUNCTIONW__ << " result: " << percentage << "%";

    return percentage;
}
