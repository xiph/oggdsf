#include "StdAfx.h"
#include ".\StreamCommentInfo.h"
#using <mscorlib.dll>



namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

StreamCommentInfo::StreamCommentInfo(void)
{
	mNativeClass = new ::StreamCommentInfo;
}

StreamCommentInfo::~StreamCommentInfo(void)
{
	delete mNativeClass;
}




VorbisComments* StreamCommentInfo::comments() {
	return new VorbisComments(mNativeClass->comments());
}
	//void setComments(VorbisComments* inComments);

Int64 StreamCommentInfo::pageStart() {
	Int64 locNum = mNativeClass->pageStart();
	return locNum;
}
void StreamCommentInfo::setPageStart(Int64 inPageStart) {
	mNativeClass->setPageStart(inPageStart);
}

Int32 StreamCommentInfo::codecID() {
	Int32 locNum = mNativeClass->codecID();
	return locNum;
}
void StreamCommentInfo::setCodecID(Int32 inCodecID) {
	mNativeClass->setCodecID(inCodecID);
}

bool StreamCommentInfo::isDirty() {
	return mNativeClass->isDirty();
}
void StreamCommentInfo::setIsDirty(bool inIsDirty) {
	mNativeClass->setIsDirty(inIsDirty);
}

Int64 StreamCommentInfo::majorStreamNo() {
	Int64 locNum = mNativeClass->majorStreamNo();
	return locNum;
}
void StreamCommentInfo::setMajorStreamNo(Int64 inMajorStreamNo) {
	mNativeClass->setMajorStreamNo(inMajorStreamNo);
}

Int64 StreamCommentInfo::minorStreamNo() {
	Int64 locNum = mNativeClass->minorStreamNo();
	return locNum;
}
void StreamCommentInfo::setMinorStreamNo(Int64 inMinorStreamNo) {
	mNativeClass->setMinorStreamNo(inMinorStreamNo);
}

		}}}