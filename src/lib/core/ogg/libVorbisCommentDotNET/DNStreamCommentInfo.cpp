#include "StdAfx.h"
#include ".\DNStreamCommentInfo.h"
#using <mscorlib.dll>



namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

DNStreamCommentInfo::DNStreamCommentInfo(void)
{
	mNativeClass = new ::StreamCommentInfo;
}

DNStreamCommentInfo::DNStreamCommentInfo(::StreamCommentInfo* inNativeClass) {
	mNativeClass = inNativeClass;
}

DNStreamCommentInfo::~DNStreamCommentInfo(void)
{
	delete mNativeClass;
}




DNVorbisComments* DNStreamCommentInfo::comments() {
	
	return new illiminable::Ogg::libVorbisCommentDotNET::DNVorbisComments((::VorbisComments*)mNativeClass->comments());
}
	//void setComments(VorbisComments* inComments);

Int64 DNStreamCommentInfo::pageStart() {
	Int64 locNum = mNativeClass->pageStart();
	return locNum;
}
void DNStreamCommentInfo::setPageStart(Int64 inPageStart) {
	mNativeClass->setPageStart(inPageStart);
}

Int32 DNStreamCommentInfo::codecID() {
	Int32 locNum = mNativeClass->codecID();
	return locNum;
}
void DNStreamCommentInfo::setCodecID(Int32 inCodecID) {
	mNativeClass->setCodecID(inCodecID);
}

bool DNStreamCommentInfo::isDirty() {
	return mNativeClass->isDirty();
}
void DNStreamCommentInfo::setIsDirty(bool inIsDirty) {
	mNativeClass->setIsDirty(inIsDirty);
}

Int64 DNStreamCommentInfo::majorStreamNo() {
	Int64 locNum = mNativeClass->majorStreamNo();
	return locNum;
}
void DNStreamCommentInfo::setMajorStreamNo(Int64 inMajorStreamNo) {
	mNativeClass->setMajorStreamNo(inMajorStreamNo);
}

Int64 DNStreamCommentInfo::minorStreamNo() {
	Int64 locNum = mNativeClass->minorStreamNo();
	return locNum;
}
void DNStreamCommentInfo::setMinorStreamNo(Int64 inMinorStreamNo) {
	mNativeClass->setMinorStreamNo(inMinorStreamNo);
}

		}}}