#pragma once
#pragma unmanaged
#include "dllstuff.h"
#include "libVorbisComment/FileComments.h"

#pragma managed
using namespace System;
using namespace illiminable::libiWrapper;
namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {


//class LIBVORBISCOMMENT_API FileComments
//	:	public IOggCallback
//{
//public:
//	FileComments(void);
//	~FileComments(void);
//
//	
//
//	bool loadFile(string inFileName);
//	string fileName();
//
//	bool writeOutAll();
//	bool writeOutStream(unsigned long inIndex);
//
//	bool addStreamComment(StreamCommentInfo* inStreamComment);
//	StreamCommentInfo* getStreamComment(unsigned long inIndex);
//
//	unsigned long streamCount();
//
//	//IOggCallback implementation
//	virtual bool acceptOggPage(OggPage* inOggPage);
//protected:
//	vector<StreamCommentInfo*> mStreams;
//	string mFileName;
//	unsigned long mMinorStreamCount;
//	__int64 mBytePos;
//};




public __gc class FileComments
{
public:
	FileComments(void);
	~FileComments(void);


	bool loadFile(String* inFileName);
	String* fileName();

	bool writeOutAll();
	bool writeOutStream(Int64 inIndex);

	bool addStreamComment(StreamCommentInfo* inStreamComment);
	StreamCommentInfo* getStreamComment(Int64 inIndex);

	Int64 streamCount();

protected:
	::FileComments* mNativeClass;
};

		}}}