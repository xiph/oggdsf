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
#include "CustomVMR7Allocator.h"
#include <uuids.h>

CustomVMR7Allocator::CustomVMR7Allocator():
m_notifyWindow(0),
m_presentImageMessage(0)
{
}

CustomVMR7Allocator::~CustomVMR7Allocator()
{
}


HRESULT CustomVMR7Allocator::CreateDefaultSurfaceAllocator()
{
    return m_defaultAllocator.CoCreateInstance(CLSID_AllocPresenter, 0, CLSCTX_INPROC_SERVER);
}

// IVMRSurfaceAllocator Methods
HRESULT __stdcall CustomVMR7Allocator::AllocateSurface(DWORD_PTR dwUserID, VMRALLOCATIONINFO *lpAllocInfo, 
                                                       DWORD *lpdwActualBuffers, LPDIRECTDRAWSURFACE7 *lplpSurface)
{
    ATLASSERT(m_defaultAllocator && "Missing default allocator");
    return m_defaultAllocator->AllocateSurface(dwUserID, lpAllocInfo, lpdwActualBuffers, lplpSurface);
}

HRESULT __stdcall CustomVMR7Allocator::FreeSurface(DWORD_PTR dwID)
{
    ATLASSERT(m_defaultAllocator && "Missing default allocator");
    return m_defaultAllocator->FreeSurface(dwID);
}

HRESULT __stdcall CustomVMR7Allocator::PrepareSurface(DWORD_PTR dwUserID,LPDIRECTDRAWSURFACE7 lpSurface, 
                                                      DWORD dwSurfaceFlags)
{
    ATLASSERT(m_defaultAllocator && "Missing default allocator");
    return m_defaultAllocator->PrepareSurface(dwUserID, lpSurface, dwSurfaceFlags);
}

HRESULT __stdcall CustomVMR7Allocator::AdviseNotify(IVMRSurfaceAllocatorNotify *lpIVMRSurfAllocNotify)
{
   ATLASSERT(m_defaultAllocator && "Missing default allocator");
   return m_defaultAllocator->AdviseNotify(lpIVMRSurfAllocNotify);
}

// IVMRImagePresenter Methods
HRESULT __stdcall CustomVMR7Allocator::StartPresenting(DWORD_PTR /*dwUserID*/)
{
    return S_OK;
}

HRESULT __stdcall CustomVMR7Allocator::StopPresenting(DWORD_PTR /*dwUserID*/)
{
    return S_OK;
}

HRESULT __stdcall CustomVMR7Allocator::PresentImage(DWORD_PTR dwUserID, VMRPRESENTATIONINFO *lpPresInfo)
{
    ATLASSERT(m_notifyWindow && "SetNotifyWindow must be called first!");
    ATLASSERT(m_presentImageMessage && "SetPresentImageMessage must be called first!");

    ::SendMessage(m_notifyWindow, m_presentImageMessage, dwUserID, reinterpret_cast<LPARAM>(lpPresInfo));
    return S_OK;
}

int CustomVMR7Allocator::GetPresentImageMessage() const
{
    return m_presentImageMessage;
}

void CustomVMR7Allocator::SetPresentImageMessage( int val )
{
    m_presentImageMessage = val;
}

HWND CustomVMR7Allocator::GetNotifyWindow() const
{
    return m_notifyWindow;
}

void CustomVMR7Allocator::SetNotifyWindow( HWND val )
{
    m_notifyWindow = val;
}
