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

#ifndef FILTERGRAPH_H
#define FILTERGRAPH_H

#pragma once

#include <dshow.h>
#include <strmif.h>
#include <d3d9.h>
#include <vmr9.h>

class CustomVMR9Allocator;

class FilterGraph
{
public:
    FilterGraph();
    virtual ~FilterGraph();

    void BuildGraph(const CString& videoUrl);

    void Run();
    void Pause();
    void Stop();

    HWND GetNotifyWindow() const;
    void SetNotifyWindow(HWND val);

    int GetPresentImageMessage() const;
    void SetPresentImageMessage(int val);

    CComPtr<IDirect3DDevice9> GetD3DDevice() const;
    void SetD3DDevice(const CComPtr<IDirect3DDevice9>& val);

    CComPtr<IDirect3D9> GetD3D() const;
    void SetD3D(const CComPtr<IDirect3D9>& val);

    HANDLE GetMovieEventHandle();
    long GetMovieEventCode();

private:
    void AddSourceDemuxFilters();
    void AddDecoders();
    void ConnectDecoders();
    void AddRenderers();
    void ConfigureVideoRenderer();
    void ConnnectRenderers();

private:
    CString m_videoUrl;

    CComPtr<IFilterGraph2> m_graphBuilder;
    
    CComPtr<IBaseFilter> m_sourceFilter;
    CComPtr<IBaseFilter> m_streamDemux;
    CComPtr<IBaseFilter> m_audioDecoder;
    CComPtr<IBaseFilter> m_videoDecoder;

    CComPtr<IBaseFilter> m_audioRenderer;
    CComPtr<IBaseFilter> m_videoRenderer;

    CComPtr<IVMRSurfaceAllocatorNotify9> m_surfaceNotify;
    CComObject<CustomVMR9Allocator>* m_customVmrAllocator;

    CComPtr<IMediaControl> m_mediaControl;
    CComPtr<IMediaEvent> m_mediaEvent;
    
    HWND m_notifyWindow;
    int m_presentImageMessage;
    CComPtr<IDirect3D9> m_d3d;
    CComPtr<IDirect3DDevice9> m_d3dDevice;

    bool m_haveAudio;
    bool m_haveVideo;
    bool m_haveWebm;
};

#endif // FILTERGRAPH_H
