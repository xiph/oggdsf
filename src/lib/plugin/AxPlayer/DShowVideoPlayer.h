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
#include <gdiplus.h>
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
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    enum PlayerState
    {
        NotOpened,
        Playing,
        Paused,
        Stopped,
    };

    enum AudioState
    {
        Muted,
        UnMuted
    };

    LRESULT OnPresentImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
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

    HRESULT Mute();

    DShowVideoPlayer::PlayerState GetState() const;
    DShowVideoPlayer::AudioState GetAudioState() const;

    bool GetMouseOver() const { return m_isMouseOver; }
    void SetMouseOver(bool val) { m_isMouseOver = val; }

    void OnMouseButtonDown(long x, long y);
    void OnMouseButtonUp(long x, long y);
    void OnMouseMove(long x, long y);

    bool EnableControls() const { return m_enableControls; }
    void EnableControls(bool val) { m_enableControls = val; }

private:
    void Thread_PrepareGraph();

    void Thread_Play();
    void Thread_Pause();
    void Thread_Stop();
    void Thread_Mute();
    void Thread_DurationPosition();
    void Thread_SetPosition();
    void Thread_SetVolume();

    void Thread_ExecuteFunction();
    
    void CreateBackBufferSurface(const CSize& videoSize);
    CSize GetSurfaceSize(const CComPtr<IDirect3DSurface9>& surface);
    CComPtr<IDirect3DSurface9>& GetScalingSurface(const CSize &aSize);

    void CreateControls(const CSize& videoSize);
    void DrawControls(const CRect& rect, HDC dc);

    void CreateDevice();
    CComPtr<IDirect3DDevice9>& GetDevice();

private:
    FilterGraph m_filterGraph;

    CComPtr<IDirect3D9> m_d3d;
    CComPtr<IDirect3DDevice9> m_direct3dDevice;
    CComPtr<IDirect3DSurface9> m_backBuffer;
    CComPtr<IDirect3DSurface9> m_scalingBuffer;
    CComPtr<IDirect3DSurface9> m_presentBuffer;
    CSize m_scalingSize;

    int m_width;
    int m_height;

    CString m_src;

    HANDLE m_playbackThreadHandle;
    HANDLE m_stopPlaybackEvent;
    HANDLE m_executeFunctionEvent;
    HANDLE m_waitForFunction;

    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;

    Gdiplus::Image* m_pngPlay;
    Gdiplus::Image* m_pngPause;
    Gdiplus::Image* m_pngMute;
    Gdiplus::Image* m_pngUnmute;
    Gdiplus::Image* m_pngPositionThumb;
    Gdiplus::Image* m_pngAudioPositionThumb;

    DShowVideoPlayerCallback* m_playerCallback;

    typedef void (DShowVideoPlayer::* ExecuteFunctionOnThread)();
    ExecuteFunctionOnThread m_executeFunctionOnThread;

    bool m_isFirstFrame;
    PlayerState m_state;
    AudioState m_audioState;

    bool m_enableControls;
    bool m_isMouseOver;
    bool m_doDisplayAudioVolume;
    bool m_isMouseOverMuteButton;

    CRect m_playButtonRect;
    CRect m_muteButtonRect;
    CRect m_positionSliderRect;
    CRect m_audioVolumeSliderRect;
    CRect m_audioVolumePanel;

    unsigned long m_duration;
    unsigned long m_currentPosition;
    unsigned long m_openProgress;
    unsigned long m_setPosition;

    long m_audioVolume;
    long m_audioUnMuteVolume;
    long m_setAudioVolume;

    D3DTEXTUREFILTERTYPE m_textureFilterType;
};

#endif // DSHOWVIDEOPLAYER_H
