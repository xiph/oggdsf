#pragma once


//This class really shouldn't be part of this project, but i got lazy !
//This is a pretty inefficient way to do it but its easy.
class OGG_DEMUX_API RegWrap
{
public:
	RegWrap(void);
	~RegWrap(void);

	static LONG addKeyVal(HKEY inHive, string inKeyName, string inValueName, string inValue);
	static bool valueExists(HKEY inHive, string inKeyName, string inValueName);
	static string findNextEmptyMediaPlayerDesc();
	static bool addMediaPlayerDesc(string inDesc, string inExts);
};
