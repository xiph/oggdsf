//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "flacencoderdllstuff.h"

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}


//The folowing two functions do the registration and deregistration of the dll and it's contained com objects.
STDAPI DllRegisterServer()
{

    HRESULT hr;
    IFilterMapper2* locFilterMapper = NULL;

    hr = AMovieDllRegisterServer2(TRUE);

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&locFilterMapper);

	hr = locFilterMapper->RegisterFilter(
		CLSID_FLACEncodeFilter,						// Filter CLSID. 
		L"Xiph.Org FLAC Encoder",					// Filter name.
        NULL,										// Device moniker. 
        &CLSID_LegacyAmFilterCategory,				// Direct Show general category
        NULL,							// Instance data. ???????
        &FLACEncodeFilterReg								// Pointer to filter information.
    );

    hr = locFilterMapper->RegisterFilter(
        CLSID_FLACEncodeFilter,						// Filter CLSID. 
        L"Xiph.Org FLAC Encoder",					// Filter name.
        NULL,										// Device moniker. 
        &CLSID_AudioCompressorCategory,				// Direct Show general category
        NULL,							// Instance data. ???????
        &FLACEncodeFilterReg								// Pointer to filter information.
        );

    locFilterMapper->Release();

    return hr;

}

STDAPI DllUnregisterServer()
{
   HRESULT hr;
    IFilterMapper2* locFilterMapper = NULL;

    hr = AMovieDllRegisterServer2(FALSE);
	if (FAILED(hr)) {
		
        return hr;
	}
 
    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
            IID_IFilterMapper2, (void **)&locFilterMapper);

	if (FAILED(hr)) {
        return hr;
	}

    hr = locFilterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, NULL, CLSID_FLACEncodeFilter);
    hr = locFilterMapper->UnregisterFilter(&CLSID_AudioCompressorCategory, NULL, CLSID_FLACEncodeFilter);
    
    locFilterMapper->Release();
    return hr;

}
