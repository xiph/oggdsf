#include "stdafx.h"
#include ".\DNFileComments.h"
#using <mscorlib.dll>


namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

DNFileComments::DNFileComments(void)
{
	mNativeClass = new ::FileComments;
}

DNFileComments::~DNFileComments(void)
{
	delete mNativeClass;
}



bool DNFileComments::loadFile(String* inFileName) {
	char* locCS = Wrappers::netStrToCStr(inFileName);
	string locStr = locCS;
	bool locRet = mNativeClass->loadFile(locStr);
	Wrappers::releaseCStr(locCS);
	return locRet;
}
String* DNFileComments::fileName() {
	return Wrappers::CStrToNetStr(mNativeClass->fileName().c_str());
}

bool DNFileComments::writeOutAll(String* inFileName) {
	char* locCS = Wrappers::netStrToCStr(inFileName);
	string locStr = locCS;
	bool locRet = mNativeClass->writeOutAll(locStr);
	Wrappers::releaseCStr(locCS);
	return locRet;
	
}
bool DNFileComments::writeOutStream(Int64 inIndex) {
	return mNativeClass->writeOutStream((unsigned long)inIndex);
}

//bool DNFileComments::addStreamComment(DNStreamCommentInfo* inStreamComment) {
//
//}
DNStreamCommentInfo* DNFileComments::getStreamComment(Int64 inIndex) {
	return new illiminable::Ogg::libVorbisCommentDotNET::DNStreamCommentInfo(mNativeClass->getStreamComment((unsigned long)inIndex));
}

Int64 DNFileComments::streamCount() {
	return mNativeClass->streamCount();
}


		}}}