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

#ifndef CUSTOMVMR9ALLOCATOR_H
#define CUSTOMVMR9ALLOCATOR_H

#pragma once

#include <strmif.h>
#include <d3d9.h>
#include <vmr9.h>

class CustomVMR9Allocator: 
    public CComObjectRootEx<CComSingleThreadModel>,
    public IVMRSurfaceAllocator9, 
    public IVMRImagePresenter9
{
public:
    
    BEGIN_COM_MAP(CustomVMR9Allocator)
        COM_INTERFACE_ENTRY(IVMRSurfaceAllocator9)
        COM_INTERFACE_ENTRY(IVMRImagePresenter9)
    END_COM_MAP()

    CustomVMR9Allocator();
    virtual ~CustomVMR9Allocator();

    int GetPresentImageMessage() const;
    void SetPresentImageMessage(int val);

    HWND GetNotifyWindow() const;
    void SetNotifyWindow(HWND val);

    CComPtr<IDirect3DDevice9> GetD3DDevice() const;
    void SetD3DDevice(const CComPtr<IDirect3DDevice9>& val);

    CComPtr<IDirect3D9> GetD3D() const;
    void SetD3D(const CComPtr<IDirect3D9>& val);

    // IVMRSurfaceAllocator9 Methods
    virtual HRESULT __stdcall InitializeDevice(DWORD_PTR dwUserID, 
        VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers);

    virtual HRESULT __stdcall TerminateDevice(DWORD_PTR dwID);

    virtual HRESULT __stdcall GetSurface(DWORD_PTR dwUserID,
        DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface);

    virtual HRESULT __stdcall AdviseNotify(IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify);

    // IVMRImagePresenter9  Methods
    virtual HRESULT __stdcall StartPresenting(DWORD_PTR dwUserID);
    virtual HRESULT __stdcall StopPresenting(DWORD_PTR dwUserID);
    virtual HRESULT __stdcall PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo);

private:
    
    CComPtr<IVMRSurfaceAllocator9> m_defaultAllocator;
    CComPtr<IVMRSurfaceAllocatorNotify9> m_surfaceAllocatorNotify;

    CComPtr<IDirect3DDevice9> m_d3dDevice;
    CComPtr<IDirect3D9> m_d3d;
    typedef std::vector<IDirect3DSurface9*> SurfaceList;
    SurfaceList m_surfacesList;

    HWND m_notifyWindow;
    int m_presentImageMessage;

    struct AutoLock
    {
        CComAutoCriticalSection& m_lockObject;

        AutoLock(CComAutoCriticalSection& lockObject) : 
        m_lockObject(lockObject)
        {
            m_lockObject.Lock();
        }

        ~AutoLock()
        {
            m_lockObject.Unlock();
        }
    };

    CComAutoCriticalSection m_lockObject;

};

#endif // CUSTOMVMR7ALLOCATOR_H
