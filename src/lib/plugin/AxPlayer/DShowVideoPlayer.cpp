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
#include "DShowVideoPlayer.h"
#include <ddraw.h>
#include "DDrawUtil.h"
#include "DShowUtil.h"
#include "resource.h"

namespace 
{
    const int CONTROLS_HEIGHT = 28;
    const int CONTROLS_WIDTH = 28;
    const int AUDIO_SLIDER_HEIGHT = 75;

    const int AUDIO_PADDING = 8;
    const int AUDIO_PADDING_LEFT = 10;
    const int AUDIO_PADDING_RIGHT = 9;

    using Gdiplus::Color;

    const Color CONTROLS_BACKGROUND_COLOR = Color(192, 35, 31, 32);
    const Color TEXT_COLOR = Color(192, 255, 255, 255);
    const Color PLAYED_COLOR = Color(192, 255, 255, 255);
    const Color OPEN_COLOR = Color(192, 192, 192, 192);
    const Color TO_BE_PLAYED_COLOR = Color(192, 128, 128, 128);

    const int UPDATE_DURATION = 1;

    const CString TEXT_FONT = L"Arial";
    const int TEXT_SIZE = 9;
    const int TEXT_WIDTH = 80;

    const int SLIDER_TOP = 9;
    const int SLIDER_BOTTOM = 10;

    Gdiplus::Image* LoadImage(UINT resourceId)
    {
        Gdiplus::Image* image = 0;

        HRSRC hResource = ::FindResource(util::GetHModule(), MAKEINTRESOURCE(resourceId), RT_RCDATA);
        if (!hResource)
        {
            return image;
        }

        DWORD imageSize = ::SizeofResource(util::GetHModule(), hResource);
        if (!imageSize)
        {
            image;
        }

        const void* pResourceData = ::LockResource(::LoadResource(util::GetHModule(), hResource));
        if (!pResourceData)
        {
            return image;
        }

        HGLOBAL buffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
        if (!buffer)
        {
            return image;
        }
        void* pBuffer = ::GlobalLock(buffer);
        if (!pBuffer)
        {
            ::GlobalFree(buffer);
            return image;
        }
        CopyMemory(pBuffer, pResourceData, imageSize);

        CComPtr<IStream> pStream = NULL;
        if (::CreateStreamOnHGlobal(buffer, FALSE, &pStream) == S_OK)
        {
            image = Gdiplus::Bitmap::FromStream(pStream);
        }
        ::GlobalUnlock(buffer);
        ::GlobalFree(buffer);

        return image;
    }

    using Gdiplus::Rect;

    Rect GetControlsRect(const CRect& displayRect)
    {
        return Rect(0, displayRect.Height() - CONTROLS_HEIGHT, displayRect.Width(), CONTROLS_HEIGHT);
    }

    Rect GetPlayButtonRect(const CRect& displayRect)
    {
        return Rect(0, displayRect.Height() - CONTROLS_HEIGHT, CONTROLS_WIDTH, CONTROLS_HEIGHT);
    }

    Rect GetMuteButtonRect(const CRect& displayRect)
    {
        return Rect(displayRect.Width() - CONTROLS_WIDTH, displayRect.Height() - CONTROLS_HEIGHT, 
            CONTROLS_WIDTH, CONTROLS_HEIGHT);
    }

    std::pair<Rect, Rect> GetPositionSliderRects(const CRect& displayRect, unsigned long thumbPercent)
    {
        unsigned long thumbPosition = thumbPercent * displayRect.Width() / 100;

        Rect first(displayRect.left, displayRect.top, thumbPosition, displayRect.Height());
        Rect second(displayRect.left + thumbPosition, displayRect.top, displayRect.Width() - thumbPosition, displayRect.Height());

        return std::make_pair(first, second);
    }

    std::pair<Rect, Rect> GetAudioSliderRects(const CRect& displayRect, unsigned long thumbPercent)
    {
        unsigned long thumbPosition = thumbPercent * displayRect.Height() / 100;

        Rect first(displayRect.left, displayRect.top, displayRect.Width(), thumbPosition);
        Rect second(displayRect.left, displayRect.top + thumbPosition, displayRect.Width(), 
            displayRect.Height() - thumbPosition);

        return std::make_pair(first, second);
    }
}

DShowVideoPlayer::DShowVideoPlayer() :
m_isFirstFrame(false),
m_executeFunctionOnThread(0),
m_playerCallback(0),
m_state(NotOpened),
m_audioState(UnMuted),
m_textureFilterType(D3DTEXF_NONE),
m_isMouseOver(false),
m_audioVolume(FilterGraph::MIN_VOLUME),
m_audioUnMuteVolume(FilterGraph::MIN_VOLUME),
m_setAudioVolume(FilterGraph::MIN_VOLUME),
m_duration(0),
m_currentPosition(0),
m_openProgress(0),
m_isMouseOverMuteButton(false),
m_doDisplayAudioVolume(false)
{
    m_stopPlaybackEvent = ::CreateEvent(0, FALSE, FALSE, 0);
    m_executeFunctionEvent = ::CreateEvent(0 , FALSE, FALSE, 0);
    m_waitForFunction = ::CreateEvent(0, FALSE, FALSE, 0);

    m_d3d.Attach(Direct3DCreate9(D3D_SDK_VERSION));

    Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, 0);

    m_pngPlay = LoadImage(IDI_PNG_PLAY);
    m_pngPause = LoadImage(IDI_PNG_PAUSE);
    m_pngMute = LoadImage(IDI_PNG_MUTE);
    m_pngUnmute = LoadImage(IDI_PNG_UNMUTE);
    m_pngPositionThumb = LoadImage(IDI_PNG_POSITION_THUMB);
    m_pngAudioPositionThumb = LoadImage(IDI_PNG_VOLUME_THUMB);
}

DShowVideoPlayer::~DShowVideoPlayer()
{
    ::CloseHandle(m_stopPlaybackEvent);
    ::CloseHandle(m_executeFunctionEvent);
    ::CloseHandle(m_waitForFunction);

    delete m_pngPlay;
    delete m_pngPause;
    delete m_pngMute;
    delete m_pngUnmute;
    delete m_pngPositionThumb;
    delete m_pngAudioPositionThumb;

    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void DShowVideoPlayer::CreateBackBufferSurface(const CSize& videoSize)
{
    try
    {
        D3DDISPLAYMODE displayMode = {};
        CHECK_HR(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode));

        CHECK_HR(GetDevice()->CreateRenderTarget(videoSize.cx, videoSize.cy, displayMode.Format, 
            D3DMULTISAMPLE_NONE, 0, FALSE, &m_backBuffer, 0));
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

void DShowVideoPlayer::CreateDevice()
{
    ATLASSERT(m_d3d && "Direct3D should be created!");
    if (!m_d3d)
    {
        return;
    }

    try
    {
        D3DDISPLAYMODE displayMode = {};
        CHECK_HR(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode));

        // CreateDevice fails on 0x0 windows;
        MoveWindow(0, 0, 1, 1, false);

        D3DPRESENT_PARAMETERS presentParameters = {};
        presentParameters.Windowed = TRUE;
        presentParameters.hDeviceWindow = m_hWnd;
        presentParameters.SwapEffect = D3DSWAPEFFECT_COPY;
        presentParameters.BackBufferFormat = displayMode.Format;

        CHECK_HR(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, 
            D3DCREATE_MULTITHREADED | D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentParameters, &m_direct3dDevice));

        D3DCAPS9 d3dCaps = {};
        CHECK_HR(m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps));
        if (d3dCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)
        {
            m_textureFilterType = D3DTEXF_POINT;
        }
        if (d3dCaps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
        {
            m_textureFilterType = D3DTEXF_LINEAR;
        }
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

CComPtr<IDirect3DDevice9>& DShowVideoPlayer::GetDevice()
{
    if (!m_direct3dDevice)
    {
        CreateDevice();
    }

    return m_direct3dDevice;
}

CComPtr<IDirect3DSurface9>& DShowVideoPlayer::GetScalingSurface(const CSize &aSize)
{
    if (m_scalingBuffer && m_presentBuffer && aSize != m_scalingSize) 
    {
        LOG(logDEBUG3) << __FUNCTIONW__ << " Releasing old scaling surfaces";

        m_scalingBuffer = 0;
        m_presentBuffer = 0;
    }

    if (!m_scalingBuffer && !m_presentBuffer) 
    {
        LOG(logDEBUG3) << __FUNCTIONW__ << " Creating new scaling surfaces";

        D3DDISPLAYMODE displayMode = {};
        CHECK_HR(m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode));

        CHECK_HR(GetDevice()->CreateRenderTarget(aSize.cx, aSize.cy, displayMode.Format, 
            D3DMULTISAMPLE_NONE, 0, FALSE, &m_scalingBuffer, 0));

        CHECK_HR(GetDevice()->CreateOffscreenPlainSurface(aSize.cx, aSize.cy, displayMode.Format,
            D3DPOOL_SYSTEMMEM, &m_presentBuffer, 0));

        m_scalingSize = aSize;
        CreateControls(m_scalingSize);
    }

    return m_scalingBuffer;
}

HRESULT DShowVideoPlayer::Draw(RECT rcBounds, RECT rcWindow, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject)
{
    CRect rect(rcBounds);

    LOG(logDEBUG4) << __FUNCTIONW__ 
        << " [" << rect.left << "," << rect.top << " x " << rect.right << "," << rect.bottom << "] ->"
        << " [" << rcWindow.left << "," << rcWindow.top << " x " << rcWindow.right  << "," << rcWindow.bottom << "]";

    HRESULT hr = S_OK;
    try
    {
        if (m_backBuffer)
        {
            // Use a third buffer to scale the picture
            CComPtr<IDirect3DSurface9> scaledSurface = GetScalingSurface(CSize(rect.Width(), rect.Height()));
            CHECK_HR(GetDevice()->StretchRect(m_backBuffer, 0, scaledSurface, 0, m_textureFilterType));
            CHECK_HR(GetDevice()->GetRenderTargetData(scaledSurface, m_presentBuffer));

            HDC presentBufferDC;
            CHECK_HR(m_presentBuffer->GetDC(&presentBufferDC));

            // Do not paint when the current displayed line is passing
            // our rect, when it does and we draw we get tearing.
            for(; ;)
            {
                D3DRASTER_STATUS rasterStatus;
                hr = GetDevice()->GetRasterStatus(0, &rasterStatus);

                if (FAILED(hr))
                {
                    break;
                }

                if (rasterStatus.InVBlank)
                {
                    break;
                }

                if (rasterStatus.ScanLine >= (DWORD)rcWindow.top && rasterStatus.ScanLine <= (DWORD)rcWindow.bottom)
                {
                    Sleep(1);
                    continue;
                }

                break;
            }

            DrawControls(rect, presentBufferDC);

            ::BitBlt(hdc, rect.left, rect.top, rect.Width(), rect.Height(), presentBufferDC, 0, 0, SRCCOPY);

            CHECK_HR(m_presentBuffer->ReleaseDC(presentBufferDC));
        }
    }
    catch (const CAtlException& except)
    {
        hr = except.m_hr;
    }

    return hr;
}

int DShowVideoPlayer::GetWidth() const
{
    return m_width;
}

void DShowVideoPlayer::SetWidth(int val)
{
    m_width = val;
}

int DShowVideoPlayer::GetHeight() const
{
    return m_height;
}

void DShowVideoPlayer::SetHeight(int val)
{
    m_height = val;
}

CString DShowVideoPlayer::GetSrc() const
{
    return m_src;
}

void DShowVideoPlayer::SetSrc(const CString& val)
{
    m_src = val;
}

HRESULT DShowVideoPlayer::Play()
{
    m_executeFunctionOnThread = &DShowVideoPlayer::Thread_Play;
    ::SetEvent(m_executeFunctionEvent);
    m_state = Playing;

    return S_OK;
}

HRESULT DShowVideoPlayer::Pause()
{
    m_executeFunctionOnThread = &DShowVideoPlayer::Thread_Pause;
    ::SetEvent(m_executeFunctionEvent);
    m_state = Paused;

    return S_OK;
}

HRESULT DShowVideoPlayer::Stop()
{
    m_executeFunctionOnThread = &DShowVideoPlayer::Thread_Stop;
    ::SetEvent(m_executeFunctionEvent);
    m_state = Stopped;

    return S_OK;
}

HRESULT DShowVideoPlayer::Mute()
{
    m_executeFunctionOnThread = &DShowVideoPlayer::Thread_Mute;
    ::SetEvent(m_executeFunctionEvent);

    return S_OK;
}

void DShowVideoPlayer::InitializePlaybackThread()
{
    unsigned int threadId;
    m_playbackThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, 
        PlaybackThreadFunc, this, 0, &threadId));
}

void DShowVideoPlayer::StopPlaybackThread()
{
    ::SetEvent(m_stopPlaybackEvent);
    AtlWaitWithMessageLoop(m_playbackThreadHandle);
}

void DShowVideoPlayer::Thread_PrepareGraph()
{
    m_filterGraph.SetNotifyWindow(m_hWnd);
    m_filterGraph.SetPresentImageMessage(WM_PRESENT_IMAGE);
    m_filterGraph.SetD3D(m_d3d);
    m_filterGraph.SetD3DDevice(GetDevice());

    m_isFirstFrame = true;
    m_filterGraph.BuildGraph(GetSrc());
    
    m_filterGraph.Pause();

    m_audioVolume = m_filterGraph.GetVolume();
    m_audioState = (m_audioVolume != FilterGraph::MIN_VOLUME) ? DShowVideoPlayer::UnMuted : DShowVideoPlayer::Muted;

    m_state = Paused;
}


unsigned DShowVideoPlayer::PlaybackThreadFunc(void* arg)
{
    const int WAITTIME = 20;

    util::ComInitializer comInit;

    DShowVideoPlayer* self = reinterpret_cast<DShowVideoPlayer*>(arg);

    self->Thread_PrepareGraph();

    std::vector<HANDLE> events;
    events.push_back(self->m_stopPlaybackEvent);
    events.push_back(self->m_executeFunctionEvent);

    HANDLE movieEventHandle = self->m_filterGraph.GetMovieEventHandle();
    if (movieEventHandle != INVALID_HANDLE_VALUE)
    {
        events.push_back(movieEventHandle);
    }

    bool exit;
    do
    {
        exit = false;
        DWORD result = ::WaitForMultipleObjects(events.size(), &*events.begin(), FALSE, WAITTIME);
        
        if (result == WAIT_OBJECT_0)
        {
            exit = true;
        }
        else if (result == WAIT_OBJECT_0 + 1)
        {
            self->Thread_ExecuteFunction();
        }
        else if (result == WAIT_OBJECT_0 + 2)
        {
            long eventCode = self->m_filterGraph.GetMovieEventCode();
            LOG(logINFO) << __FUNCTIONW__ << " FilterGraph event code: " << DShowUtil::GetEventCodeString(eventCode);

            switch(eventCode)
            {
            case EC_COMPLETE:
                self->Thread_Pause();
                break;
            case EC_USERABORT:
            case EC_ERRORABORT:
                self->Thread_Stop();
                break;
            }
        }
        else if (result == WAIT_FAILED)
        {
            LOG(logERROR) << __FUNCTIONW__ << " WaitFailed, GetLastError: 0x" << std::hex << ::GetLastError();
            ::Sleep(WAITTIME);
        }

    } while (!exit);

    return 0;
}

CSize DShowVideoPlayer::GetSurfaceSize(const CComPtr<IDirect3DSurface9>& surface)
{
    D3DSURFACE_DESC descriptor;
    surface->GetDesc(&descriptor);

    return CSize(descriptor.Width, descriptor.Height);
}

LRESULT DShowVideoPlayer::OnPresentImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    VMR9PresentationInfo* frame = reinterpret_cast<VMR9PresentationInfo*>(lParam);

    if (m_isFirstFrame)
    {
        CSize videoSize = GetSurfaceSize(frame->lpSurf);
        CreateBackBufferSurface(videoSize);
        CreateControls(videoSize);

        if (m_playerCallback)
        {
            m_playerCallback->MovieSize(videoSize);
        }
        m_isFirstFrame = false;

        SetTimer(UPDATE_DURATION, 1000);
    }

    if (!m_backBuffer)
    {
        return 0;
    }

    HRESULT hr = GetDevice()->StretchRect(frame->lpSurf, 0, m_backBuffer, 0, D3DTEXF_NONE);

    LOG(logDEBUG2) << __FUNCTIONW__ << " Start: " << ReferenceTime(frame->rtStart) 
        << " End: " << ReferenceTime(frame->rtEnd) << " StretchRect result: 0x" << std::hex << hr;

    if (m_playerCallback)
    {
        m_playerCallback->Refresh();
    }

    return 0;
}

void DShowVideoPlayer::Thread_Play()
{
    m_filterGraph.Run();
}

void DShowVideoPlayer::Thread_Pause()
{
    m_filterGraph.Pause();
}

void DShowVideoPlayer::Thread_Stop()
{
    m_filterGraph.Stop();
}

void DShowVideoPlayer::Thread_Mute()
{
    long currentVolume = m_filterGraph.GetVolume();
    if (m_audioState == DShowVideoPlayer::UnMuted)
    {
        m_audioUnMuteVolume = currentVolume;
        m_filterGraph.SetVolume(FilterGraph::MIN_VOLUME);

        m_audioState = DShowVideoPlayer::Muted;
    }
    else
    {
        m_filterGraph.SetVolume(m_audioUnMuteVolume);
        m_audioState = DShowVideoPlayer::UnMuted;
    }

    m_audioVolume = m_filterGraph.GetVolume();
}


void DShowVideoPlayer::Thread_DurationPosition()
{
    m_duration = m_filterGraph.GetDuration();
    m_currentPosition = m_filterGraph.GetPosition();
    m_openProgress = m_filterGraph.GetOpenProgress();
}

void DShowVideoPlayer::Thread_SetPosition()
{
    m_filterGraph.SetPosition(m_setPosition);
    long percent = static_cast<unsigned long>((m_setPosition / static_cast<double>(m_duration)) * 100.0);

    LOG(logDEBUG2) << __FUNCTIONW__ << " Percent: " << percent << "%" << " Duration: " << m_duration << " Position: " << m_setPosition;

    m_currentPosition = m_filterGraph.GetPosition();

    ::SetEvent(m_waitForFunction);
}

void DShowVideoPlayer::Thread_SetVolume()
{
    m_filterGraph.SetVolume(m_setAudioVolume);

    LOG(logDEBUG2) << __FUNCTIONW__ << " Volume: " << m_setAudioVolume;

    m_audioVolume = m_filterGraph.GetVolume();
    m_audioState = (m_audioVolume != FilterGraph::MIN_VOLUME) ? DShowVideoPlayer::UnMuted : DShowVideoPlayer::Muted;

    ::SetEvent(m_waitForFunction);
}

void DShowVideoPlayer::Thread_ExecuteFunction()
{
    if (m_executeFunctionOnThread)
    {
        (this->*m_executeFunctionOnThread)();
    }
}

DShowVideoPlayerCallback* DShowVideoPlayer::GetPlayerCallback() const
{
    return m_playerCallback;
}

void DShowVideoPlayer::SetPlayerCallback( DShowVideoPlayerCallback* val )
{
    m_playerCallback = val;
}

DShowVideoPlayer::PlayerState DShowVideoPlayer::GetState() const
{
    return m_state;
}


DShowVideoPlayer::AudioState DShowVideoPlayer::GetAudioState() const
{
    return m_audioState;
}

void DShowVideoPlayer::CreateControls(const CSize& videoSize)
{
    m_playButtonRect.SetRect(0, videoSize.cy - CONTROLS_HEIGHT, CONTROLS_WIDTH, videoSize.cy);
    m_muteButtonRect.SetRect(videoSize.cx - CONTROLS_WIDTH, videoSize.cy - CONTROLS_HEIGHT,
        videoSize.cx, videoSize.cy);

    m_positionSliderRect.SetRect(CONTROLS_WIDTH, videoSize.cy - CONTROLS_HEIGHT,
        videoSize.cx - CONTROLS_WIDTH - TEXT_WIDTH, videoSize.cy);

    m_audioVolumePanel.SetRect(videoSize.cx - CONTROLS_WIDTH, 
        videoSize.cy - AUDIO_SLIDER_HEIGHT - CONTROLS_HEIGHT,
        videoSize.cx, 
        videoSize.cy - CONTROLS_HEIGHT);

    m_audioVolumeSliderRect.SetRect(
        videoSize.cx - CONTROLS_WIDTH + AUDIO_PADDING_LEFT, 
        videoSize.cy - AUDIO_SLIDER_HEIGHT - CONTROLS_HEIGHT + AUDIO_PADDING,
        videoSize.cx - AUDIO_PADDING_RIGHT, 
        videoSize.cy - CONTROLS_HEIGHT - AUDIO_PADDING);
}

void DShowVideoPlayer::DrawControls(const CRect& rect, HDC dc)
{
    if (!GetMouseOver())
    {
        return;
    }

    using namespace Gdiplus;
    Graphics graphics(dc);
    graphics.SetInterpolationMode(InterpolationModeHighQuality);

    // Draw the background
    SolidBrush backgroundBrush(CONTROLS_BACKGROUND_COLOR);
    graphics.FillRectangle(&backgroundBrush, GetControlsRect(rect));

    // Draw the play/pause button
    if (GetState() == DShowVideoPlayer::Paused ||
        GetState() == DShowVideoPlayer::Stopped)
    {
        graphics.DrawImage(m_pngPlay, GetPlayButtonRect(rect));
    }
    else if (GetState() == DShowVideoPlayer::Playing)
    {
        graphics.DrawImage(m_pngPause, GetPlayButtonRect(rect));
    }

    // Draw the mute/unmute button
    if (GetAudioState() == DShowVideoPlayer::UnMuted)
    {
        graphics.DrawImage(m_pngMute, GetMuteButtonRect(rect));
    }
    else
    {
        graphics.DrawImage(m_pngUnmute, GetMuteButtonRect(rect));
    }

    // Draw the duration
    CString durationText;

    if ((m_currentPosition / 1000) / 3600 == 0)
    {
        durationText.Format(L"%d:%.2d / %d:%.2d", 
            (m_currentPosition / 1000) / 60, (m_currentPosition / 1000) % 60,
            (m_duration / 1000) / 60, (m_duration / 1000) % 60);
    }
    else
    {
        durationText.Format(L"%d%.2d:%.2d / %d:%.2d:%.2d", 
            (m_currentPosition / 1000) / 3600, ((m_currentPosition / 1000) % 3600) / 60, ((m_currentPosition / 1000) % 3600) % 60,
            (m_duration / 1000) / 3600, ((m_duration / 1000) % 3600) / 60, ((m_duration / 1000) % 3600) % 60);
    }

    Font durationFont(TEXT_FONT, TEXT_SIZE);
    SolidBrush durationBrush(TEXT_COLOR);

    RectF durationRect(rect.Width() - CONTROLS_HEIGHT - TEXT_WIDTH, rect.Height() - CONTROLS_HEIGHT, 
        TEXT_WIDTH, CONTROLS_HEIGHT);

    StringFormat* durationStringFormat = StringFormat::GenericDefault()->Clone();
    durationStringFormat->SetAlignment(StringAlignmentFar);
 
    RectF measuredBox;
    graphics.MeasureString(durationText, -1, &durationFont, durationRect, &measuredBox);

    durationRect.X = rect.Width() - CONTROLS_WIDTH - TEXT_WIDTH;
    durationRect.Y = rect.Height() - CONTROLS_HEIGHT + (CONTROLS_HEIGHT - measuredBox.Height) / 2;
    durationRect.Width  = TEXT_WIDTH;
    durationRect.Height = measuredBox.Height;

    graphics.DrawString(durationText, -1, &durationFont, durationRect, durationStringFormat, &durationBrush);
    delete durationStringFormat;

    // Draw the position
    long positionSliderWidth = rect.Width() - CONTROLS_HEIGHT - CONTROLS_HEIGHT - durationRect.Width - 
                                m_pngPositionThumb->GetWidth();

    long positionThumb = CONTROLS_HEIGHT;
    if (m_duration)
    {
        positionThumb += (long)(((float)m_currentPosition / m_duration) * positionSliderWidth);
    }

    // Draw the rectangle until the position thumb
    SolidBrush playedBrush(PLAYED_COLOR);
    Gdiplus::GraphicsPath path;

    graphics.FillRectangle(&playedBrush, CONTROLS_HEIGHT, rect.Height() - CONTROLS_HEIGHT + SLIDER_TOP, 
        positionThumb - CONTROLS_HEIGHT, SLIDER_BOTTOM);

    // Draw the rectangle until the end of movie
    SolidBrush toBePlayedBrush(TO_BE_PLAYED_COLOR);

    graphics.FillRectangle(&toBePlayedBrush, positionThumb, rect.Height() - CONTROLS_HEIGHT + SLIDER_TOP, 
        positionSliderWidth - positionThumb + CONTROLS_HEIGHT + m_pngPositionThumb->GetWidth(), SLIDER_BOTTOM);

    // Draw the rectangle until the end of loaded movie
    SolidBrush loadedBrush(OPEN_COLOR);
    long openPosition = positionSliderWidth * m_openProgress / 100;

    graphics.FillRectangle(&loadedBrush, positionThumb, rect.Height() - CONTROLS_HEIGHT + SLIDER_TOP, 
         openPosition - positionThumb + CONTROLS_HEIGHT + m_pngPositionThumb->GetWidth(), SLIDER_BOTTOM);

    graphics.DrawImage(m_pngPositionThumb, positionThumb, 
        rect.Height() - CONTROLS_HEIGHT + (CONTROLS_HEIGHT - m_pngPositionThumb->GetHeight()) / 2,
        m_pngPositionThumb->GetWidth(), m_pngPositionThumb->GetHeight());

    // Draw the audio volume slider
    if (m_doDisplayAudioVolume)
    {
        graphics.FillRectangle(&backgroundBrush, m_audioVolumePanel.left, m_audioVolumePanel.top,
            m_audioVolumePanel.Width(), m_audioVolumePanel.Height());

        unsigned long audioPositionPercent = static_cast<unsigned long>(static_cast<double>(m_audioVolume) / FilterGraph::MIN_VOLUME * 100);
        std::pair<Rect, Rect> rects = GetAudioSliderRects(m_audioVolumeSliderRect, audioPositionPercent);

        // Draw the rectangle until the position thumb
        SolidBrush playedBrush(PLAYED_COLOR);
        graphics.FillRectangle(&playedBrush, rects.first);

        // Draw the rectangle until the end of movie
        SolidBrush toBePlayedBrush(TO_BE_PLAYED_COLOR);
        graphics.FillRectangle(&playedBrush, rects.second);

        CSize thumbPadding((m_pngAudioPositionThumb->GetWidth() - rects.second.Width) / 2, 0);

        graphics.DrawImage(m_pngAudioPositionThumb, rects.second.X - thumbPadding.cx, 
            rects.second.Y - thumbPadding.cy,
            m_pngAudioPositionThumb->GetWidth(), m_pngAudioPositionThumb->GetHeight());
    }
}

void DShowVideoPlayer::OnMouseButtonDown(long x, long y)
{
    if (m_positionSliderRect.PtInRect(CPoint(x, y)))
    {
        m_setPosition = static_cast<unsigned long>(((x - m_positionSliderRect.left) / static_cast<double>(m_positionSliderRect.Width())) * m_duration);

        m_executeFunctionOnThread = &DShowVideoPlayer::Thread_SetPosition;
        ::SetEvent(m_executeFunctionEvent);

        AtlWaitWithMessageLoop(m_waitForFunction);
        if (m_playerCallback)
        {
            m_playerCallback->Refresh();
        }
    }
    
    if (m_audioVolumeSliderRect.PtInRect(CPoint(x, y)))
    {
        double volume = (y - m_audioVolumeSliderRect.top) / static_cast<double>(m_audioVolumeSliderRect.Height());

        m_setAudioVolume = volume * FilterGraph::MIN_VOLUME;

        m_executeFunctionOnThread = &DShowVideoPlayer::Thread_SetVolume;
        ::SetEvent(m_executeFunctionEvent);

        AtlWaitWithMessageLoop(m_waitForFunction);
        if (m_playerCallback)
        {
            m_playerCallback->Refresh();
        }
    }
}

void DShowVideoPlayer::OnMouseButtonUp(long x, long y)
{
    bool dirty = false;
    if (m_playButtonRect.PtInRect(CPoint(x, y)))
    {
        if (GetState() == DShowVideoPlayer::Paused ||
            GetState() == DShowVideoPlayer::Stopped)
        {
            Play();
            dirty = true;
        }
        else if (GetState() == DShowVideoPlayer::Playing)
        {
            Pause();
            dirty = true;
        }
    }

    if (m_muteButtonRect.PtInRect(CPoint(x, y)))
    {
        Mute();
        dirty = true;
    }

    if (dirty && m_playerCallback)
    {
        m_playerCallback->Refresh();
    }
}

void DShowVideoPlayer::OnMouseMove(long x, long y)
{
    bool dirty = false;
    if (m_muteButtonRect.PtInRect(CPoint(x, y)))
    {
        m_isMouseOverMuteButton = true;
        m_doDisplayAudioVolume = true;
        dirty = true;
    }
    else if (m_isMouseOverMuteButton && !m_audioVolumePanel.PtInRect(CPoint(x, y)))
    {
        m_isMouseOverMuteButton = false;
        if (m_doDisplayAudioVolume)
        {
            m_doDisplayAudioVolume = false;
            dirty = true;
        }
    }

    if (dirty && m_playerCallback)
    {
        m_playerCallback->Refresh();
    }
}

LRESULT DShowVideoPlayer::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (wParam == UPDATE_DURATION)
    {
        m_executeFunctionOnThread = &DShowVideoPlayer::Thread_DurationPosition;
        ::SetEvent(m_executeFunctionEvent);

        if (m_state == Paused && m_openProgress != 100 && m_playerCallback)
        {
            m_playerCallback->Refresh();
        }
    }

    return 0;
}

LRESULT DShowVideoPlayer::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    KillTimer(UPDATE_DURATION);
    return 0;
}
