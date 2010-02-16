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

#ifndef CUSTOMVMR7ALLOCATOR_H
#define CUSTOMVMR7ALLOCATOR_H

#pragma once

#include <ddraw.h>
#include <strmif.h>

class CustomVMR7Allocator: 
    public CComObjectRootEx<CComSingleThreadModel>,
    public IVMRSurfaceAllocator, 
    public IVMRImagePresenter
{
public:
    
    BEGIN_COM_MAP(CustomVMR7Allocator)
        COM_INTERFACE_ENTRY(IVMRSurfaceAllocator)
        COM_INTERFACE_ENTRY(IVMRImagePresenter)
    END_COM_MAP()

    CustomVMR7Allocator();
    virtual ~CustomVMR7Allocator();

    int GetPresentImageMessage() const;
    void SetPresentImageMessage(int val);

    HWND GetNotifyWindow() const;
    void SetNotifyWindow(HWND val);

    HRESULT CreateDefaultSurfaceAllocator();

    // IVMRSurfaceAllocator Methods
    virtual HRESULT __stdcall AllocateSurface(DWORD_PTR dwUserID, VMRALLOCATIONINFO *lpAllocInfo,
                                      DWORD *lpdwActualBuffers, LPDIRECTDRAWSURFACE7 *lplpSurface);
    virtual HRESULT __stdcall FreeSurface(DWORD_PTR dwID);
    virtual HRESULT __stdcall PrepareSurface(DWORD_PTR dwUserID, LPDIRECTDRAWSURFACE7 lpSurface,
                                     DWORD dwSurfaceFlags);
    virtual HRESULT __stdcall AdviseNotify(IVMRSurfaceAllocatorNotify *lpIVMRSurfAllocNotify);

    // IVMRImagePresenter Methods
    virtual HRESULT __stdcall StartPresenting(DWORD_PTR dwUserID);
    virtual HRESULT __stdcall StopPresenting(DWORD_PTR dwUserID);
    virtual HRESULT __stdcall PresentImage(DWORD_PTR dwUserID, VMRPRESENTATIONINFO *lpPresInfo);

protected:
    
    CComPtr<IVMRSurfaceAllocator> m_defaultAllocator;
    HWND m_notifyWindow;
    int m_presentImageMessage;
};

#endif // CUSTOMVMR7ALLOCATOR_H
