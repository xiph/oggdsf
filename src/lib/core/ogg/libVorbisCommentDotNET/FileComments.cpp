#include "StdAfx.h"
#include ".\FileComments.h"
#using <mscorlib.dll>


namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

FileComments::FileComments(void)
{
	mNativeClass = new ::FileComments;
}

FileComments::~FileComments(void)
{
	delete mNativeClass;
}



bool FileComments::loadFile(String* inFileName) {
	char* locCS = Wrappers::netStrToCStr(inFileName);
	bool locRet = mNativeClass->loadFile(locCS);
	Wrappers::releaseCStr(locCS);
	return locRet;
}
String* FileComments::fileName() {
	return Wrappers::CStrToNetStr(mNativeClass->fileName().c_str());
}

bool FileComments::writeOutAll() {
	return mNativeClass->writeOutAll();
}
bool FileComments::writeOutStream(Int64 inIndex) {
	return mNativeClass->writeOutStream(inIndex);
}

//bool FileComments::addStreamComment(StreamCommentInfo* inStreamComment) {
//
//}
StreamCommentInfo* FileComments::getStreamComment(Int64 inIndex) {
	return new StreamCommentInfo(mNativeClass->getStreamComment(inIndex));
}

Int64 FileComments::streamCount() {
	return mNativeClass->streamCount();
}


		}}}