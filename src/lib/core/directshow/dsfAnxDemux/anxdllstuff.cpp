#include "StdAfx.h"
#include "anxdllstuff.h"



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
    IFilterMapper2* locFilterMapper = NULL;
	
    hr = AMovieDllRegisterServer2(TRUE);
	if (FAILED(hr)) {
		
        return hr;
	}
	
	

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&locFilterMapper);

	
	if (FAILED(hr)) {
        return hr;
	}
	
	hr = locFilterMapper->RegisterFilter(
		CLSID_AnxDemuxSourceFilter,						// Filter CLSID. 
		L"Annodex Demux Source Filter",							// Filter name.
        NULL,										// Device moniker. 
        &CLSID_LegacyAmFilterCategory,				// Direct Show general category
        L"Annodex Demux Source Filter",							// Instance data. ???????
        &AnxDemuxSourceFilterReg								// Pointer to filter information.
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
	

    hr = locFilterMapper->UnregisterFilter(&CLSID_LegacyAmFilterCategory, 
            L"Annodex Demux Source Filter", CLSID_AnxDemuxSourceFilter);

	//
    locFilterMapper->Release();
    return hr;

}
