#pragma once
#include "libVorbisComment.h"
#include <string>
#include <vector>

using namespace std;
#include "SingleVorbisComment.h"

class LIBVORBISCOMMENT_API VorbisComments
{
public:
	VorbisComments(void);
	~VorbisComments(void);

	string vendorString();
	bool setVendorString(string inVendorString);

	unsigned long numUserComments();
	SingleVorbisComment getUserComment(unsigned long inIndex);
	
	vector<SingleVorbisComment> getCommentsByKey(string inKey);

	bool addComment(SingleVorbisComment inComment);
	bool addComment(string inKey, string inValue);
protected:
	string mVendorString;
	vector<SingleVorbisComment> mCommentList;
};
