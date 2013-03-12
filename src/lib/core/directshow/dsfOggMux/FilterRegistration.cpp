//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//          (C) 2013 Cristian Adam
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

#include "Precompiled.h"
#include "FilterRegistration.h"
#include "common/util.h"

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    util::ConfigureLogSettings((HMODULE)hModule);
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

STDAPI DllRegisterServer()
{	
    HRESULT hr = S_OK;
    try
    {
        CComPtr<IFilterMapper2> filterMapper;
	
        CHECK_HR( AMovieDllRegisterServer2(TRUE) );
	
        CHECK_HR( CoCreateInstance(CLSID_FilterMapper2, NULL, 
                                   CLSCTX_INPROC_SERVER, IID_IFilterMapper2, 
                                   (void **)&filterMapper) );

		CHECK_HR( filterMapper->RegisterFilter(
		                        CLSID_OggMuxFilter,
		                        L"Xiph.Org Ogg Muxer",
                                NULL,
                                &CLSID_LegacyAmFilterCategory,
                                NULL,
                                &OggMuxFilterReg) );
    }
    catch (const CAtlException& ex)
    {
        hr = ex;
    }

    return hr;
}

STDAPI DllUnregisterServer()
{
    HRESULT hr = S_OK;
    try
    {
        CComPtr<IFilterMapper2> filterMapper;

        CHECK_HR( AMovieDllRegisterServer2(FALSE) );
 
        CHECK_HR( CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
                IID_IFilterMapper2, (void **)&filterMapper) );
	
        CHECK_HR( filterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, 
                NULL, CLSID_OggMuxFilter) );
    } 
    catch (const CAtlException& ex)
    {
        hr = ex;
    }

    return hr;
 }
