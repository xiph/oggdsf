#pragma once

#pragma unmanaged
#include "dllstuff.h"
#include "libVorbisComment/VorbisComments.h"
#include "libVorbisComment/StreamCommentInfo.h"

#pragma managed
#include "DNVorbisComments.h"
using namespace System;
using namespace illiminable::libiWrapper;
namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

//NATIVE CLASS
//class LIBVORBISCOMMENT_API StreamCommentInfo
//{
//public:
//	StreamCommentInfo(void);
//	~StreamCommentInfo(void);
//
//	enum eCodecIDs {
//		NO_CODEC = 0,
//		VORBIS = 1,
//		THEORA = 2
//
//	};
//	VorbisComments* comments();
//	void setComments(VorbisComments* inComments);
//
//	__int64 pageStart();
//	void setPageStart(__int64 inPageStart);
//
//	unsigned short codecID();
//	void setCodecID(unsigned short inCodecID);
//
//	bool isDirty();
//	void setIsDirty(bool inIsDirty);
//
//	unsigned long majorStreamNo();
//	void setMajorStreamNo(unsigned long inMajorStreamNo);
//
//	unsigned long minorStreamNo();
//	void setMinorStreamNo(unsigned long inMinorStreamNo);
//
//protected:
//	VorbisComments* mComments;
//	__int64 mPageStart;
//	unsigned short mCodecID;
//
//	bool mIsDirty;
//
//	unsigned long mMajorStreamNo;
//	unsigned long mMinorStreamNo;
//};






public __gc class DNStreamCommentInfo
{
public:
	DNStreamCommentInfo(void);
	DNStreamCommentInfo(::StreamCommentInfo* inNativeClass);
	~DNStreamCommentInfo(void);



	/*const Int32	NO_CODEC = 0;
	const Int32 VORBIS = 1;
	const Int32 THEORA = 2;*/

	DNVorbisComments* comments();
	//void setComments(VorbisComments* inComments);

	Int64 pageStart();
	void setPageStart(Int64 inPageStart);

	Int32 codecID();
	void setCodecID(Int32 inCodecID);

	bool isDirty();
	void setIsDirty(bool inIsDirty);

	Int64 majorStreamNo();
	void setMajorStreamNo(Int64 inMajorStreamNo);

	Int64 minorStreamNo();
	void setMinorStreamNo(Int64 inMinorStreamNo);

protected:
	::StreamCommentInfo* mNativeClass;
};



		}}}