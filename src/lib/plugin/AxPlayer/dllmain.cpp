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
    util::GetHModule() = (HMODULE)hInstance;

	return _AtlModule.DllMain(dwReason, lpReserved); 
}
