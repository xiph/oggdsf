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

DShowVideoPlayer::DShowVideoPlayer() :
m_isFirstFrame(false),
m_executeFunctionOnThread(0),
m_playerCallback(0),
m_state(NotOpened)
{
    m_stopPlaybackEvent = ::CreateEvent(0, FALSE, FALSE, 0);
    m_executeFunctionEvent = ::CreateEvent(0 , FALSE, FALSE, 0);
}

DShowVideoPlayer::~DShowVideoPlayer()
{
    ::CloseHandle(m_stopPlaybackEvent);
    ::CloseHandle(m_executeFunctionEvent);
}

void DShowVideoPlayer::CreateBackBufferSurface(const CSize& videoSize)
{
    try
    {
        CHECK_HR(DirectDrawCreateEx(0, (void**)&m_directDraw7, IID_IDirectDraw7, 0));
        CHECK_HR(m_directDraw7->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL + DDSCL_MULTITHREADED));
     
        // Create back buffer for conversion to RGB.
        // Buffer will by default be in a format compatible to the primary surface.
        LPDIRECTDRAWSURFACE7 surface = NULL;
        DDSURFACEDESC2 descriptor = {0};
        ZeroMemory(&descriptor, sizeof(DDSURFACEDESC2));

        descriptor.dwSize = sizeof(DDSURFACEDESC2);
        descriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        descriptor.dwWidth = videoSize.cx;
        descriptor.dwHeight = videoSize.cy;

        descriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

        CHECK_HR(m_directDraw7->CreateSurface(&descriptor, &surface, 0));
        m_backBuffer = surface;
    }
    catch (const CAtlException& /*except*/)
    {
    }
}


CComPtr<IDirectDrawSurface7> DShowVideoPlayer::GetScalingSurface(const CSize &aSize)
{
    if (m_scalingBuffer && aSize != m_scalingSize) 
    {
        LOG(logDEBUG3) << __FUNCTIONW__ << " Releasing old scaling surface";

        m_scalingBuffer = 0;
    }

    if (!m_scalingBuffer) 
    {
        LOG(logDEBUG3) << __FUNCTIONW__ << " Creating new scaling surface";

        // Create a new back buffer surface
        DDSURFACEDESC2 descriptor = {0};
        descriptor.dwSize = sizeof(DDSURFACEDESC2);
        descriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        descriptor.dwWidth = aSize.cx;
        descriptor.dwHeight = aSize.cy;
        descriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

        CHECK_HR(m_directDraw7->CreateSurface(&descriptor, &m_scalingBuffer, 0));

        m_scalingSize = aSize;
    }

    return m_scalingBuffer;
}

HRESULT DShowVideoPlayer::Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject)
{
//    CRect rect(rcBounds.left, rcBounds.top, rcBounds.left + m_width, rcBounds.top + m_height);
    CRect rect(rcBounds);

    HRESULT hr = S_OK;
    try
    {
        CComPtr<IDirectDrawSurface> destSurface;

        destSurface = reinterpret_cast<IDirectDrawSurface*>(pvDrawObject);

        DDSURFACEDESC descriptor = {};
        descriptor.dwSize = sizeof(DDSURFACEDESC);

        CHECK_HR(destSurface->GetSurfaceDesc(&descriptor));

        LOG(logDEBUG4) << __FUNCTIONW__ << " DirectDrawSurface: " << destSurface;
        LOG(logDEBUG4) << "SurfaceDescFlags: " << DDrawUtil::GetSurfaceDescFlags(descriptor.dwFlags).c_str();
        LOG(logDEBUG4) << "Pixel Format: " << std::endl
            << "\tSize: " << descriptor.ddpfPixelFormat.dwSize << std::endl
            << "\tFlags: " << DDrawUtil::GetPixelFormatFlags(descriptor.ddpfPixelFormat.dwFlags).c_str() << std::endl
            << "\tFourCC: " << descriptor.ddpfPixelFormat.dwFourCC << std::endl
            << "\tRGBBitCount: " << descriptor.ddpfPixelFormat.dwRGBBitCount << std::endl;

        if (m_backBuffer)
        {
            // Use a third buffer to scale the picture
            CComPtr<IDirectDrawSurface7> scaledSurface = GetScalingSurface(CSize(rect.Width(), rect.Height()));
            CHECK_HR(scaledSurface->Blt(NULL, m_backBuffer, NULL, DDBLT_WAIT, NULL));

            HDC destDC;
            CHECK_HR(destSurface->GetDC(&destDC));

            HDC scaledDC;
            CHECK_HR(scaledSurface->GetDC(&scaledDC));

            // Do not paint when the current displayed line is passing
            // our rect, when it does and we draw we get tearing.
            for(; ;)
            {
                DWORD scanLine;
                hr = m_directDraw7->GetScanLine(&scanLine);

                if (hr ==  DDERR_VERTICALBLANKINPROGRESS)
                {
                    break;
                }

                if (FAILED(hr))
                {
                    break;
                }

                if (scanLine >= (DWORD)rect.top && scanLine <= (DWORD)rect.bottom)
                {
                    continue;
                }

                break;
            }

            ::BitBlt(destDC, rect.left, rect.top, rect.Width(), rect.Height(), scaledDC, 0, 0, SRCCOPY);

            CHECK_HR(scaledSurface->ReleaseDC(scaledDC));
            CHECK_HR(destSurface->ReleaseDC(destDC));
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

    m_isFirstFrame = true;
    m_filterGraph.BuildGraph(GetSrc());
    
    m_filterGraph.Pause();
    m_state = Paused;
}


unsigned DShowVideoPlayer::PlaybackThreadFunc(void* arg)
{
    util::ComInitializer comInit;

    DShowVideoPlayer* self = reinterpret_cast<DShowVideoPlayer*>(arg);

    self->Thread_PrepareGraph();

    HANDLE events[3];
    events[0] = self->m_stopPlaybackEvent;
    events[1] = self->m_executeFunctionEvent;
    events[2] = self->m_filterGraph.GetMovieEventHandle();

    bool exit;
    do
    {
        exit = false;
        DWORD result = ::WaitForMultipleObjects(sizeof(events) / sizeof HANDLE, events, FALSE, 10);
        
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
    } while (!exit);

    return 0;
}

CSize DShowVideoPlayer::GetSurfaceSize(const CComPtr<IDirectDrawSurface7>& surface)
{
    DDSURFACEDESC2 descriptor;
    ZeroMemory(&descriptor, sizeof(DDSURFACEDESC2));
    descriptor.dwSize = sizeof(DDSURFACEDESC2);
    
    surface->GetSurfaceDesc(&descriptor);
    return CSize(descriptor.dwWidth, descriptor.dwHeight);
}

LRESULT DShowVideoPlayer::OnPresentImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    VMRPRESENTATIONINFO* frame = reinterpret_cast<VMRPRESENTATIONINFO*>(lParam);

    if (m_isFirstFrame)
    {
        CSize videoSize = GetSurfaceSize(frame->lpSurf);
        CreateBackBufferSurface(videoSize);

        if (m_playerCallback)
        {
            m_playerCallback->MovieSize(videoSize);
        }
        m_isFirstFrame = false;
    }

    if (!m_backBuffer)
    {
        return 0;
    }

    HRESULT hr = m_backBuffer->BltFast(0, 0, frame->lpSurf, NULL, DDBLTFAST_DONOTWAIT);

    LOG(logDEBUG2) << __FUNCTIONW__ << " Start: " << ReferenceTime(frame->rtStart) 
        << " End: " << ReferenceTime(frame->rtEnd) << " result: 0x" << std::hex << hr;

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
