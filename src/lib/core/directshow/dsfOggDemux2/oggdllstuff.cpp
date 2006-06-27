//===========================================================================
//Copyright (C) 2003, 2004, 2005 Zentaro Kavanagh
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

#include "oggdllstuff.h"
#include "RegWrap.h"

//#define DONT_TOUCH_REGISTRY


extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}


//The folowing two functions do the registration and deregistration of the dll and it's contained com objects.
STDAPI DllRegisterServer()
{
	
	//TO DO::: Should we be releasing the filter mapper even when we return early ?
    HRESULT hr;

	//MessageBox(NULL, L"Ogg", L"Ogg", MB_OK);
    
	
#ifdef WINCE
	hr = AMovieDllRegisterServer();//AMovieDLLRegisterServer(TRUE);
#else
    hr = AMovieDllRegisterServer2(TRUE);
#endif
	if (FAILED(hr)) {
		
        return hr;
	}
	
#ifdef WINCE

#else
	IFilterMapper2* locFilterMapper = NULL;
    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&locFilterMapper);

	
	if (FAILED(hr)) {
        return hr;
	}
	
	hr = locFilterMapper->RegisterFilter(
		CLSID_OggDemuxPacketSourceFilter,						// Filter CLSID. 
		L"Ogg Demux Packet Source Filter",							// Filter name.
        NULL,										// Device moniker. 
        &CLSID_LegacyAmFilterCategory,				// Direct Show general category
        NULL,							// Instance data. ???????
        &OggDemuxPacketSourceFilterReg								// Pointer to filter information.
    );

#if (!defined(DONT_TOUCH_REGISTRY))
	//Only call once... if you need multiple you have to fix the hack job in RegWrap !
	RegWrap::addMediaPlayerDesc(TEXT("Ogg File"),  TEXT("*.ogg;*.ogv;*.oga;*.spx"));
	RegWrap::deleteKeyRecurse(HKEY_CLASSES_ROOT, TEXT(".OGG"), TEXT("ShellEx"));
#endif




    locFilterMapper->Release();
#endif //WINCE
    return hr;

}

STDAPI DllUnregisterServer()
{
#if (!defined(DONT_TOUCH_REGISTRY))
	//This is not a general purpose function.
	RegWrap::removeMediaDesc();
#endif
   HRESULT hr;
    

#ifdef WINCE
	hr = AMovieDllUnregisterServer();//AMovieDLLRegisterServer(TRUE);
#else
    hr = AMovieDllRegisterServer2(TRUE);
#endif
	if (FAILED(hr)) {
		
        return hr;
	}
#ifdef WINCE

#else
	IFilterMapper2* locFilterMapper = NULL;
    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
            IID_IFilterMapper2, (void **)&locFilterMapper);

	if (FAILED(hr)) {
        return hr;
	}
	

    hr = locFilterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, 
            NULL, CLSID_OggDemuxPacketSourceFilter);

	//
    locFilterMapper->Release();
#endif //WINCE
    return hr;

}
