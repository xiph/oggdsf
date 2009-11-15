// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <streams.h>
#include <pullpin.h>

#ifndef WINCE
//Kernel streaming header for KSDATA_FORMAT_SUBTYPE_PCM
#include <ks.h>
#include <ksmedia.h>
#include <initguid.h>
#endif

#include <string>
#include <set>

#include "common/Log.h"

#include <atlbase.h>
#include <atlcom.h>

#define FLAC__NO_DLL
