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

bool SingleVorbisComment::parseComment(string inCommentString) {
	size_t pos = 0;
	size_t pos2 = 0;
	pos = inCommentString.find('=');
	if ((pos == string::npos) && (pos != 0)) {
		//FAILED - No equals sign
		return false;
	} else {
		pos2 = inCommentString.find('=', pos + 1);
		if (pos2 == string::npos) {
			//OK - no other equals signs
			mKey = inCommentString.substr(0, pos);
			mValue = inCommentString.substr(pos + 1);
		} else {
			//FAILED : Too many = signs
			return false;
		}

		return true;
	}


}