#include "stdafx.h"
#include ".\DNsinglevorbiscomment.h"
#using <mscorlib.dll>

namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {
DNSingleVorbisComment::DNSingleVorbisComment(void)
	:	mNativeClass(NULL)
{
	mNativeClass = new ::SingleVorbisComment;
}

DNSingleVorbisComment::DNSingleVorbisComment(::SingleVorbisComment* inNativeClass) {
	mNativeClass = inNativeClass;
}
DNSingleVorbisComment::~DNSingleVorbisComment(void)
{
	delete mNativeClass;
}



String* DNSingleVorbisComment::key() {
	return Wrappers::CStrToNetStr(mNativeClass->key().c_str());
}
bool DNSingleVorbisComment::setKey(String* inKey) {
	char* locCS = Wrappers::netStrToCStr(inKey);
	mNativeClass->setKey(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

String* DNSingleVorbisComment::value() {
	return Wrappers::CStrToNetStr(mNativeClass->value().c_str());
}
bool DNSingleVorbisComment::setValue(String* inValue) {
	char* locCS = Wrappers::netStrToCStr(inValue);
	mNativeClass->setValue(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

String* DNSingleVorbisComment::toString() {
	return Wrappers::CStrToNetStr(mNativeClass->toString().c_str());
}

Int64 DNSingleVorbisComment::length() {
	Int64 locLength = mNativeClass->length();
	return locLength;
}






}}}