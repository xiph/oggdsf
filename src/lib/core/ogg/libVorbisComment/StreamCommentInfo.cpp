#include "StdAfx.h"
#include ".\streamcommentinfo.h"

StreamCommentInfo::StreamCommentInfo(void)
	:	mIsDirty(false)
	,	mPageStart(0)
	,	mCodecID(NO_CODEC)
	,	mComments(NULL)
{
}

StreamCommentInfo::~StreamCommentInfo(void)
{
	delete mComments;
}


VorbisComments* StreamCommentInfo::comments() {
	return mComments;
}
void StreamCommentInfo::setComments(VorbisComments* inComments) {
	mComments = inComments;
}
__int64 StreamCommentInfo::pageStart() {
	return mPageStart;
	
}
void StreamCommentInfo::setPageStart(__int64 inPageStart) {
	mPageStart = inPageStart;
}
unsigned short StreamCommentInfo::codecID() {
	return mCodecID;
}
void StreamCommentInfo::setCodecID(unsigned short inCodecID) {
	mCodecID = inCodecID;
}

bool StreamCommentInfo::isDirty() {
	return mIsDirty;
}
void StreamCommentInfo::setIsDirty(bool inIsDirty) {
	mIsDirty = inIsDirty;
}

unsigned long StreamCommentInfo::majorStreamNo() {
	return mMajorStreamNo;
}
void StreamCommentInfo::setMajorStreamNo(unsigned long inMajorStreamNo) {
	mMajorStreamNo = inMajorStreamNo;
}

unsigned long StreamCommentInfo::minorStreamNo() {
	return mMinorStreamNo;
}
void StreamCommentInfo::setMinorStreamNo(unsigned long inMinorStreamNo) {
	mMinorStreamNo = inMinorStreamNo;
}
