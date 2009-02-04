// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define FLAC__NO_DLL

// Fixing compiler error C2039: 'wcsftime' : is not a member of '`global namespace''
#if defined(_WIN32_WCE)
size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm*);
#endif
