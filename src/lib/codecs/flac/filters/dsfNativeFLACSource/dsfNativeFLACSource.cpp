//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//Copyright (C) 2008, 2009 Cristian Adam
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
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
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
//===========================================================================
#include "stdafx.h"
#include <initguid.h>
#include "NativeFLACSourceFilter.h"
#include "dsfNativeFLACSource.h"
#include "common/util.h"

extern "C" BOOL WINAPI DllEntryPoint(HANDLE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        util::ConfigureLog(hModule);
    }

    return DllEntryPoint(hModule, dwReason, lpReserved);
}


//The foLlowing two functions do the registration and deregistration of the dll and it's contained com objects.
STDAPI DllRegisterServer()
{	
    HRESULT hr = AMovieDllRegisterServer2(TRUE);

    if (FAILED(hr)) 
    {    
        return hr;
    }
    
#ifndef WINCE
    CComPtr<IFilterMapper2> filterMapper;

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&filterMapper);

    if (FAILED(hr)) 
    {
        return hr;
    }
    
    hr = filterMapper->RegisterFilter(
        CLSID_NativeFLACSourceFilter,               // Filter CLSID. 
        L"Native FLAC Source Filter",               // Filter name.
        NULL,                                       // Device moniker. 
        &CLSID_LegacyAmFilterCategory,              // Direct Show general category
        NULL,                                       // Instance data. ???????
        &NativeFLACSourceFilterReg                  // Pointer to filter information.
    );
#else
    CComPtr<IFilterMapper> filterMapper;

    hr = CoCreateInstance(CLSID_FilterMapper, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper, (void **)&filterMapper);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = filterMapper->RegisterFilter(
        CLSID_NativeFLACSourceFilter,               // Filter CLSID. 
        L"Native FLAC Source Filter",               // Filter name.
        MERIT_NORMAL
        );
#endif

    return hr;
}

STDAPI DllUnregisterServer()
{
    HRESULT hr = S_OK;
        
    hr = AMovieDllRegisterServer2(FALSE);

    if (FAILED(hr)) 
    {    
        return hr;
    }

#ifndef WINCE
    CComPtr<IFilterMapper2> locFilterMapper;

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
            IID_IFilterMapper2, (void **)&locFilterMapper);

    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = locFilterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, 
            NULL, CLSID_NativeFLACSourceFilter);

#else
    CComPtr<IFilterMapper> filterMapper;

    hr = CoCreateInstance(CLSID_FilterMapper, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper, (void **)&filterMapper);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = filterMapper->UnregisterFilter(CLSID_NativeFLACSourceFilter);
#endif

    return hr;    
}
