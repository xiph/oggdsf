#include "StdAfx.h"
#include "singlevorbiscomment.h"

SingleVorbisComment::SingleVorbisComment(void)
{
}

SingleVorbisComment::~SingleVorbisComment(void)
{
}


string SingleVorbisComment::key() {
	return mKey;
}
bool SingleVorbisComment::setKey(string inKey) {
	//FIX::: Need to do a check here for invalid chars
	mKey = inKey;
	return true;
}

string SingleVorbisComment::value() {
	return mValue;
}
bool SingleVorbisComment::setValue(string inValue) {
	//FIX::: Need to do a check here for invalid chars
	mValue = inValue;
	return true;
}

string SingleVorbisComment::toString() {
	string retStr = mKey+"="+mValue;
	return retStr;
}

unsigned long SingleVorbisComment::length() {
	//FIX::: This would be faster to add them manually.
	return toString().length();
}