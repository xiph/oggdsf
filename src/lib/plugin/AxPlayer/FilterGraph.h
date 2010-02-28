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

class CustomVMR7Allocator;

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

    HANDLE GetMovieEventHandle();
    long GetMovieEventCode();

private:
    void AddSourceFilter();
    void AddDecoders();
    void ConnectDecoders();
    void AddRenderers();
    void ConfigureVideoRenderer();
    void ConnnectRenderers();

private:
    CString m_videoUrl;

    CComPtr<IFilterGraph2> m_graphBuilder;
    
    CComPtr<IBaseFilter> m_oggSource;
    CComPtr<IBaseFilter> m_vorbisDecoder;
    CComPtr<IBaseFilter> m_theoraDecoder;

    CComPtr<IBaseFilter> m_audioRenderer;
    CComPtr<IBaseFilter> m_videoRenderer;

    CComPtr<IVMRSurfaceAllocatorNotify> m_surfaceNotify;
    CComObject<CustomVMR7Allocator>* m_customVmrAllocator;

    CComPtr<IMediaControl> m_mediaControl;
    CComPtr<IMediaEvent> m_mediaEvent;
    
    HWND m_notifyWindow;
    int m_presentImageMessage;

    bool m_haveAudio;
    bool m_haveVideo;
};

#endif // FILTERGRAPH_H
