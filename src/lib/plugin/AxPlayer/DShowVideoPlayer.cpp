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
m_state(NotOpened),
m_textureFilterType(D3DTEXF_NONE)
{
    m_stopPlaybackEvent = ::CreateEvent(0, FALSE, FALSE, 0);
    m_executeFunctionEvent = ::CreateEvent(0 , FALSE, FALSE, 0);

    m_d3d.Attach(Direct3DCreate9(D3D_SDK_VERSION));
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
