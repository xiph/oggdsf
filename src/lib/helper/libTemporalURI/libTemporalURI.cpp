// libTemporalURI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "libTemporalURI.h"
//These ignore unreferenced formal parameter (lpReserved and hModule since they are unused)
#pragma warning( push )
#pragma warning( disable : 4100 )
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#pragma warning( pop )