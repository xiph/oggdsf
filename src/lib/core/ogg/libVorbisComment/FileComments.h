#pragma once
#include "IOggCallback.h"
#include "StreamCommentInfo.h"
#include "OggPage.h"
#include "OggDataBuffer.h"
#include <string>
#include <vector>

using namespace std;

class FileComments
	:	public IOggCallback
{
public:
	FileComments(void);
	~FileComments(void);

	

	bool loadFile(string inFileName);
	string fileName();

	bool writeOutAll();
	bool writeOutStream(unsigned long inIndex);

	bool addStreamComment(StreamCommentInfo* inStreamComment);
	StreamCommentInfo* getStreamComment(unsigned long inIndex);

	//IOggCallback implementation
	virtual bool acceptOggPage(OggPage* inOggPage);
protected:
	vector<StreamCommentInfo*> mStreams;
	string mFileName;
	unsigned long mMinorStreamCount;
	__int64 mBytePos;
};
