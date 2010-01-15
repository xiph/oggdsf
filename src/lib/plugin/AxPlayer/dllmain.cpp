// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "AxPlayer_i.h"
#include "dllmain.h"
#include "dlldatax.h"

#include "common/util.h"

CAxPlayerModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
    util::GetHModule() = (HMODULE)hInstance;

	return _AtlModule.DllMain(dwReason, lpReserved); 
}
