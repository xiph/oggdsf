#include "StdAfx.h"
#include ".\singlevorbiscomment.h"
#using <mscorlib.dll>

namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {
SingleVorbisComment::SingleVorbisComment(void)
	:	mNativeClass(NULL)
{
	mNativeClass = new ::SingleVorbisComment;
}

SingleVorbisComment::SingleVorbisComment(::SingleVorbisComment* inNativeClass) {
	mNativeClass = inNativeClass;
}
SingleVorbisComment::~SingleVorbisComment(void)
{
	delete mNativeClass;
}



String* SingleVorbisComment::key() {
	return Wrappers::CStrToNetStr(mNativeClass->key().c_str());
}
bool SingleVorbisComment::setKey(String* inKey) {
	char* locCS = Wrappers::netStrToCStr(inKey);
	mNativeClass->setKey(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

String* SingleVorbisComment::value() {
	return Wrappers::CStrToNetStr(mNativeClass->value().c_str());
}
bool SingleVorbisComment::setValue(String* inValue) {
	char* locCS = Wrappers::netStrToCStr(inValue);
	mNativeClass->setValue(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

String* SingleVorbisComment::toString() {
	return Wrappers::CStrToNetStr(mNativeClass->toString().c_str());
}

Int64 SingleVorbisComment::length() {
	Int64 locLength = mNativeClass->length();
	return locLength;
}






}}}