#include "StdAfx.h"
#include "vorbiscomments.h"

VorbisComments::VorbisComments(void)
{
}

VorbisComments::~VorbisComments(void)
{
}

string VorbisComments::vendorString() {
	return mVendorString;
}
bool VorbisComments::setVendorString(string inVendorString) {
	//FIX::: Validation needed
	mVendorString = inVendorString;
	return true;
}

unsigned long VorbisComments::numUserComments() {
	return mCommentList.size();
}
SingleVorbisComment VorbisComments::getUserComment(unsigned long inIndex) {
	//FIX::: Bounds checking
	return mCommentList[inIndex];
}
	
vector<SingleVorbisComment> VorbisComments::getCommentsByKey(string inKey) {
	//FIX::: Probably faster not to iterate... but who cares for now.. there aren't many.
	vector<SingleVorbisComment> retComments;
	SingleVorbisComment locCurrComment;

	for (int i = 0; i < mCommentList.size(); i++) {
		locCurrComment = mCommentList[i];
		//FIX::: Need to upcase everything
		if (locCurrComment.key() == inKey) {
			retComments.push_back(locCurrComment);
		}
	}
	return retComments;
}

bool VorbisComments::addComment(SingleVorbisComment inComment) {
	mCommentList.push_back(inComment);
	return true;
}
bool VorbisComments::addComment(string inKey, string inValue) {
	SingleVorbisComment locComment;
	locComment.setKey(inKey);
	locComment.setValue(inValue);
	mCommentList.push_back(locComment);
	return true;
}