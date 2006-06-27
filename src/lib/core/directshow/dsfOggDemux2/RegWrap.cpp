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
#include "regwrap.h"

RegWrap::RegWrap(void)
{
}

RegWrap::~RegWrap(void)
{
}

#ifdef UNICODE
LONG RegWrap::addKeyVal(HKEY inHive, wstring inKeyName, wstring inValueName, wstring inValue) 
#else
LONG RegWrap::addKeyVal(HKEY inHive, string inKeyName, string inValueName, string inValue) 
#endif
{
	//Open or create keyname
	//Add a value called ValueName with value inValue.

	//LONG RegCreateKeyEx(
	//	HKEY hKey,
	//	LPCTSTR lpSubKey,
	//	DWORD Reserved,
	//	LPTSTR lpClass,
	//	DWORD dwOptions,
	//	REGSAM samDesired,
	//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	//	PHKEY phkResult,
	//	LPDWORD lpdwDisposition
	//);


	//LONG RegSetValueEx(
	//	HKEY hKey,
	//	LPCTSTR lpValueName,
	//	DWORD Reserved,
	//	DWORD dwType,
	//	const BYTE* lpData,
	//	DWORD cbData
	//);


	//fstream debugLog;
	//debugLog.open("G:\\reg.log", ios_base::out);
	//debugLog <<"Key = "<<inKeyName<<endl<<"ValueName = "<<inValueName<<endl<<"Value = "<<inValue<<endl;
	HKEY locKey;
	DWORD locDisp;
	LONG retVal = RegCreateKeyEx(	inHive,
									inKeyName.c_str(),
									NULL,
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_ALL_ACCESS,
									NULL,
									&locKey,
									&locDisp);

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Create Failed"<<endl;
		return retVal;
	}

	retVal = RegSetValueEx(		locKey,
								inValueName.c_str(),
								NULL,
								REG_SZ,
								(const BYTE*)inValue.c_str(),
								(DWORD)(inValue.length()+1));

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Set Value Failed"<<endl;
		return retVal;
	}
	
	RegCloseKey(locKey);

	//debugLog.close();
	return retVal;

}
#ifdef UNICODE
bool RegWrap::deleteKeyRecurse(HKEY inHive, wstring inKeyName, wstring inSubKeyToDelete) 
#else
bool RegWrap::deleteKeyRecurse(HKEY inHive, string inKeyName, string inSubKeyToDelete) 
#endif

{

#ifndef WINCE
	HKEY locKey;
	LONG retVal;

	retVal = RegOpenKeyEx(	inHive,
							inKeyName.c_str(),
							NULL,
							KEY_ALL_ACCESS,
							&locKey);

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Key not found"<<endl;
		return false;
	}

	retVal = SHDeleteKey(locKey, inSubKeyToDelete.c_str());
	RegCloseKey(locKey);
#endif
	return true;

}


#ifdef UNICODE
//NOTE::: For various reasons this is all ansi, all strings are internal, not user created.
// ---- No longer true
bool RegWrap::removeKeyVal(HKEY inHive, wstring inKeyName, wstring inValueName) 
#else
bool RegWrap::removeKeyVal(HKEY inHive, string inKeyName, string inValueName) 
#endif
{
	//LONG RegDeleteValue(
	//	HKEY hKey,
	//	LPCTSTR lpValueName
	//);

	HKEY locKey;
	LONG retVal;

	retVal = RegOpenKeyEx(	inHive,
							inKeyName.c_str(),
							NULL,
							KEY_ALL_ACCESS,
							&locKey);

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Key not found"<<endl;
		return false;
	}

	retVal = RegDeleteValue(locKey, inValueName.c_str());
	RegCloseKey(locKey);
	if (retVal != ERROR_SUCCESS) {
		return false;
	} else {
		return true;
	}
}

#ifdef UNICODE
bool RegWrap::valueExists(HKEY inHive, wstring inKeyName, wstring inValueName) 
#else
bool RegWrap::valueExists(HKEY inHive, string inKeyName, string inValueName) 
#endif
{

	//LONG RegQueryValueEx(
	//	HKEY hKey,
	//	LPCTSTR lpValueName,
	//	LPDWORD lpReserved,
	//	LPDWORD lpType,
	//	LPBYTE lpData,
	//	LPDWORD lpcbData
	//);

	//LONG RegOpenKeyEx(
	//	HKEY hKey,
	//	LPCTSTR lpSubKey,
	//	DWORD ulOptions,
	//	REGSAM samDesired,
	//	PHKEY phkResult
	//);

	//fstream debugLog;
	//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\MediaPlayer\Player\Extensions\Descriptions
	//debugLog.open("G:\\val.log", ios_base::out);
	HKEY locKey;
	LONG retVal;
	//debugLog<<"Querying : Key = "<<inKeyName<<endl<<"Value = "<<inValueName<<endl;

	retVal = RegOpenKeyEx(	inHive,
							inKeyName.c_str(),
							NULL,
							KEY_ALL_ACCESS,
							&locKey);

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Key not found"<<endl;
		return false;
	}

	retVal = RegQueryValueEx(	locKey,
								inValueName.c_str(),
								NULL,
								NULL,
								NULL,
								NULL);

	RegCloseKey(locKey);
	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Value not found"<<endl;
		return false;
	} else {
		//debugLog<<"Value found"<<endl;
		return true;
	}

}
#ifdef UNICODE
wstring RegWrap::findNextEmptyMediaPlayerDesc() 
#else
string RegWrap::findNextEmptyMediaPlayerDesc() 
#endif
{
	char locNum[6];
#ifdef UNICODE
	wstring foundNum = L"";
#else
	string foundNum = "";
#endif
	for (long i = 1; i < 24; i++) {
		_itoa(i, (char*)&locNum, 10);
#ifdef UNICODE
		string locTemp = locNum;
		wstring locNumString = StringHelper::toWStr(locTemp);
#else
		string locNumString = locNum;
#endif
		if (!RegWrap::valueExists(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\Descriptions"), /*(char*)&locNum)*/ locNumString)) {
			foundNum = locNumString; //(char*)&locNum;
			break;
		}

	}
	return foundNum;
}

bool RegWrap::removeMediaDesc() 
{
	//NOTE::: This function is deliberately all ansi
	HKEY locKey;
	LONG retVal;

	retVal = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
							TEXT("SOFTWARE\\illiminable\\oggcodecs"),
							NULL,
							KEY_ALL_ACCESS,
							&locKey);

	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Key not found"<<endl;
		return false;
	}



	DWORD locBuffSize = 64;
	char locBuff[64];

	retVal = RegQueryValueEx(	locKey,
								TEXT("MediaDescNum"),
								NULL,
								NULL,
								(BYTE*)&locBuff,
								&locBuffSize);


#ifdef UNICODE
	wstring locBuffStr = (wchar_t*)locBuff;
#else
	string locBuffStr = locBuff;
#endif

	RegCloseKey(locKey);
	if (retVal != ERROR_SUCCESS) {
		//debugLog<<"Value not found"<<endl;
		return false;
	} else {
		RegWrap::removeKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\Descriptions"), locBuffStr.c_str());
		RegWrap::removeKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\MUIDescriptions"), locBuffStr.c_str());
		RegWrap::removeKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\Types"), locBuffStr.c_str());
		RegWrap::removeKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\illiminable\\oggcodecs"), TEXT("MediaDescNum"));
		//debugLog<<"Value found"<<endl;
		return true;
		
	}


}

#ifdef UNICODE
bool RegWrap::addMediaPlayerDesc(wstring inDesc, wstring inExts) 
#else
bool RegWrap::addMediaPlayerDesc(string inDesc, string inExts) 
#endif
{
	if (!RegWrap::valueExists(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\illiminable\\oggcodecs"), TEXT("MediaDescNum"))) {
#ifdef UNICODE
	wstring locDescNum;
	wstring locFull;
#else
	string locDescNum;
	string locFull;
#endif
		locDescNum = TEXT("");
		locFull = inDesc+ TEXT(" (") + inExts + TEXT(")");
		locDescNum = RegWrap::findNextEmptyMediaPlayerDesc();
		if (locDescNum == TEXT("")) {
			return false;
		}
		RegWrap::addKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\illiminable\\oggcodecs"), TEXT("MediaDescNum"), locDescNum.c_str());
		RegWrap::addKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\Descriptions"), locDescNum, locFull.c_str());
		RegWrap::addKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\MUIDescriptions"), locDescNum, inDesc.c_str());
		RegWrap::addKeyVal(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\MediaPlayer\\Player\\Extensions\\Types"), locDescNum, inExts.c_str());
		return true;
	} else {
		return false;
	}

}