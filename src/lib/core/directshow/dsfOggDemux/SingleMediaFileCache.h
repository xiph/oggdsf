#pragma once

//This class will be a cache of a single media file.
//It will only allow a single chunk of data to be cached...
// ie you can't cache bytes 0-1000 and 2000-3000...
// only consecutive blocks for now.
//
//Data can be read randomly... but only written sequentially.
//Will act as a buffer so that data read off the network can be put straight
// into the file and then read as needed.

#include <string>
#include <fstream>
using namespace std;
class SingleMediaFileCache
{
public:
	SingleMediaFileCache(void);
	~SingleMediaFileCache(void);

	bool open(string inFileName);
	void close();
	bool write(const unsigned char* inBuff, unsigned long inBuffSize);
	unsigned long read(unsigned char* outBuff, unsigned long inBuffSize);
	bool readSeek(unsigned long inSeekPos);
	unsigned long totalBytes();
	unsigned long bytesAvail();

protected:
	fstream mLocalFile;

	unsigned long mBytesWritten;
	unsigned long mReadPtr;
	
	fstream debugLog;
	bool mIsComplete;
};
