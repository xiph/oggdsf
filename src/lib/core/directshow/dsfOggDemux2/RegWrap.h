#pragma once
#include <shlwapi.h>

//This class really shouldn't be part of this project, but i got lazy !
//This is a pretty inefficient way to do it but its easy.
#include <stdlib.h>
#include <string>
using namespace std;
class OGG_DEMUX2_API RegWrap
{
public:
	RegWrap(void);
	~RegWrap(void);

#ifdef UNICODE
	static LONG		addKeyVal(HKEY inHive, wstring inKeyName, wstring inValueName, wstring inValue);
	static bool		valueExists(HKEY inHive, wstring inKeyName, wstring inValueName);
	static wstring	findNextEmptyMediaPlayerDesc();
	static bool		addMediaPlayerDesc(wstring inDesc, wstring inExts);
	static bool		removeKeyVal(HKEY inHive, wstring inKeyName, wstring inValueName);
	static bool		deleteKeyRecurse(HKEY inHive, wstring inKeyName, wstring inSubKeyToDelete);

#else
	static LONG		addKeyVal(HKEY inHive, string inKeyName, string inValueName, string inValue);
	static bool		valueExists(HKEY inHive, string inKeyName, string inValueName);
	static string	findNextEmptyMediaPlayerDesc();
	static bool		addMediaPlayerDesc(string inDesc, string inExts);
	static bool		removeKeyVal(HKEY inHive, string inKeyName, string inValueName);
	static bool		deleteKeyRecurse(HKEY inHive, string inKeyName, string inSubKeyToDelete);
#endif

	static bool		removeMediaDesc();
};
