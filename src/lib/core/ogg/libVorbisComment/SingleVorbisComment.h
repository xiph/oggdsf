#pragma once
#include "libVorbisComment.h"
#include <string>

using namespace std;

class LIBVORBISCOMMENT_API SingleVorbisComment
{
public:
	SingleVorbisComment(void);
	~SingleVorbisComment(void);

	string key();
	bool setKey(string inKey);

	string value();
	bool setValue(string inValue);

	string toString();

	unsigned long length();

protected:
	string mKey;
	string mValue;
};
