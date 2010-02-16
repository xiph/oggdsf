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

#ifndef DSHOWVIDEOPLAYER_H
#define DSHOWVIDEOPLAYER_H

#pragma once

#include <atlwin.h>
#include "FilterGraph.h"

class DShowVideoPlayerCallback
{
public:
    virtual void Refresh() = 0;
    virtual void MovieSize(const CSize& movieSize) = 0;
};

class DShowVideoPlayer : public CWindowImpl<DShowVideoPlayer>
{
public:
    DShowVideoPlayer();
    virtual ~DShowVideoPlayer();

    static const int WM_PRESENT_IMAGE = WM_USER + 200;

    BEGIN_MSG_MAP(DShowVideoPlayer)
        MESSAGE_HANDLER(WM_PRESENT_IMAGE, OnPresentImage)
    END_MSG_MAP()

    enum PlayerState
    {
        NotOpened,
        Playing,
        Paused,
        Stopped,
    };

    LRESULT OnPresentImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HRESULT Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject);

    int GetWidth() const;
    void SetWidth(int val);

    int GetHeight() const;
    void SetHeight(int val);

    CString GetSrc() const;
    void SetSrc(const CString& val);

    DShowVideoPlayerCallback* GetPlayerCallback() const;
    void SetPlayerCallback(DShowVideoPlayerCallback* val);

    void InitializePlaybackThread();
    void StopPlaybackThread();
    static unsigned __stdcall PlaybackThreadFunc(void* arg);

    HRESULT Play();
    HRESULT Pause();
    HRESULT Stop();

    DShowVideoPlayer::PlayerState GetState() const;

private:
    void Thread_PrepareGraph();

    void Thread_Play();
    void Thread_Pause();
    void Thread_Stop();

    void Thread_ExecuteFunction();
    
    void CreateBackBufferSurface(const CSize& videoSize);
    CSize GetSurfaceSize(const CComPtr<IDirectDrawSurface7>& surface);
    CComPtr<IDirectDrawSurface7> GetScalingSurface(const CSize &aSize);

private:
    FilterGraph m_filterGraph;

    CComPtr<IDirectDraw7> m_directDraw7;
    CComPtr<IDirectDrawSurface7> m_backBuffer;
    CComPtr<IDirectDrawSurface7> m_scalingBuffer;
    CSize m_scalingSize;

    int m_width;
    int m_height;

    CString m_src;

    HANDLE m_playbackThreadHandle;
    HANDLE m_stopPlaybackEvent;
    HANDLE m_executeFunctionEvent;

    DShowVideoPlayerCallback* m_playerCallback;

    typedef void (DShowVideoPlayer::* ExecuteFunctionOnThread)();
    ExecuteFunctionOnThread m_executeFunctionOnThread;

    bool m_isFirstFrame;
    PlayerState m_state;
};

#endif // DSHOWVIDEOPLAYER_H
