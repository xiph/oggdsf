#include "StdAfx.h"
#include "cmmldecoderdllstuff.h"

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
		CLSID_CMMLDecodeFilter,						// Filter CLSID. 
		L"CMML Decode Filter",							// Filter name.
        NULL,										// Device moniker. 
        &CLSID_LegacyAmFilterCategory,				// Direct Show general category
        L"CMML Decode Filter",							// Instance data. ???????
        &CMMLDecodeFilterReg								// Pointer to filter information.
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
	

    hr = locFilterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, L"CMML Decode Filter", CLSID_CMMLDecodeFilter);


	//
    locFilterMapper->Release();
    return hr;

}
