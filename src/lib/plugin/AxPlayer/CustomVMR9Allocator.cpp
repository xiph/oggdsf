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
#include "CustomVMR9Allocator.h"
#include <uuids.h>
#include <algorithm>

namespace {
    struct ReleaseObject
    {
        template <typename T>
        void operator () (T* arg)
        {
            arg->Release();
        }
    };
}

CustomVMR9Allocator::CustomVMR9Allocator():
m_notifyWindow(0),
m_presentImageMessage(0)
{
}

CustomVMR9Allocator::~CustomVMR9Allocator()
{
    std::for_each(m_surfacesList.begin(), m_surfacesList.end(), ReleaseObject());
    m_surfacesList.clear();
}

// IVMRSurfaceAllocator9 Methods
HRESULT CustomVMR9Allocator::InitializeDevice(DWORD_PTR /*dwUserID*/, 
                                              VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers)
{
    if (!lpNumBuffers)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    try
    {
        std::for_each(m_surfacesList.begin(), m_surfacesList.end(), ReleaseObject());
        m_surfacesList.resize(*lpNumBuffers);

        CHECK_HR(m_surfaceAllocatorNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, &*m_surfacesList.begin()));
    }
    catch (const CAtlException& except)
    {
        hr = except.m_hr;
    }

    return hr;
}

HRESULT CustomVMR9Allocator::TerminateDevice(DWORD_PTR /*dwID*/)
{
    AutoLock lock(m_lockObject);
    std::for_each(m_surfacesList.begin(), m_surfacesList.end(), ReleaseObject());
    m_surfacesList.clear();
    
    return S_OK;
}

HRESULT CustomVMR9Allocator::GetSurface(DWORD_PTR /*dwUserID*/, DWORD surfaceIndex, DWORD /*surfaceFlags*/, 
                                        IDirect3DSurface9 **lplpSurface)
{
    if (!lplpSurface)
    {
        return E_POINTER;
    }

    AutoLock lock(m_lockObject);
    *lplpSurface = m_surfacesList[surfaceIndex];
    (*lplpSurface)->AddRef();

    return S_OK;
}

HRESULT CustomVMR9Allocator::AdviseNotify(IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{
    AutoLock locker(m_lockObject);

    HRESULT hr = S_OK;
    try
    {
        m_surfaceAllocatorNotify = lpIVMRSurfAllocNotify;
        
        HMONITOR defaultMonitor = GetD3D()->GetAdapterMonitor(D3DADAPTER_DEFAULT);
        CHECK_HR(m_surfaceAllocatorNotify->SetD3DDevice(GetD3DDevice(), defaultMonitor));
    }
    catch (const CAtlException& except)
    {
        hr = except.m_hr;
    }
    
    return hr;
}

// IVMRImagePresenter Methods
HRESULT __stdcall CustomVMR9Allocator::StartPresenting(DWORD_PTR /*dwUserID*/)
{
    return S_OK;
}

HRESULT __stdcall CustomVMR9Allocator::StopPresenting(DWORD_PTR /*dwUserID*/)
{
    return S_OK;
}

HRESULT __stdcall CustomVMR9Allocator::PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo)
{
    ATLASSERT(m_notifyWindow && "SetNotifyWindow must be called first!");
    ATLASSERT(m_presentImageMessage && "SetPresentImageMessage must be called first!");

    ::SendMessage(m_notifyWindow, m_presentImageMessage, dwUserID, reinterpret_cast<LPARAM>(lpPresInfo));
    return S_OK;
}

int CustomVMR9Allocator::GetPresentImageMessage() const
{
    return m_presentImageMessage;
}

void CustomVMR9Allocator::SetPresentImageMessage( int val )
{
    m_presentImageMessage = val;
}

HWND CustomVMR9Allocator::GetNotifyWindow() const
{
    return m_notifyWindow;
}

void CustomVMR9Allocator::SetNotifyWindow(HWND val)
{
    m_notifyWindow = val;
}

CComPtr<IDirect3DDevice9> CustomVMR9Allocator::GetD3DDevice() const
{
    ATLASSERT(m_d3dDevice && L"SetD3DDevice should have been called before");
    return m_d3dDevice;
}

void CustomVMR9Allocator::SetD3DDevice(const CComPtr<IDirect3DDevice9>& val)
{
    m_d3dDevice = val;
}

CComPtr<IDirect3D9> CustomVMR9Allocator::GetD3D() const
{
    ATLASSERT(m_d3d && L"SetD3D should have been called before");
    return m_d3d;
}

void CustomVMR9Allocator::SetD3D(const CComPtr<IDirect3D9>& val)
{
    m_d3d = val;
}
