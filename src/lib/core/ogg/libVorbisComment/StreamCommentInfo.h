#pragma once
#include "VorbisComments.h"
class StreamCommentInfo
{
public:
	StreamCommentInfo(void);
	~StreamCommentInfo(void);

	enum eCodecIDs {
		NO_CODEC = 0,
		VORBIS = 1,
		THEORA = 2

	};
	VorbisComments* comments();
	void setComments(VorbisComments* inComments);

	__int64 pageStart();
	void setPageStart(__int64 inPageStart);

	unsigned short codecID();
	void setCodecID(unsigned short inCodecID);

	bool isDirty();
	void setIsDirty(bool inIsDirty);

	unsigned long majorStreamNo();
	void setMajorStreamNo(unsigned long inMajorStreamNo);

	unsigned long minorStreamNo();
	void setMinorStreamNo(unsigned long inMinorStreamNo);

protected:
	VorbisComments* mComments;
	__int64 mPageStart;
	unsigned short mCodecID;

	bool mIsDirty;

	unsigned long mMajorStreamNo;
	unsigned long mMinorStreamNo;
};
