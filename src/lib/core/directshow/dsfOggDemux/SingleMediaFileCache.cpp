#include "StdAfx.h"
#include ".\singlemediafilecache.h"

SingleMediaFileCache::SingleMediaFileCache(void)
{
}

SingleMediaFileCache::~SingleMediaFileCache(void)
{
}

bool SingleMediaFileCache::open(string inFileName) {
	mLocalFile.open(inFileName.c_str(), ios_base::in|ios_base::out|ios_base::binary);
	return mLocalFile.is_open();
}
void SingleMediaFileCache::close() {
	mLocalFile.close();
	
}
bool SingleMediaFileCache::write(const unsigned char* inBuff, unsigned long inBuffSize) {
	if (inBuffSize != 0) {
		mLocalFile.write((const char*)inBuff, inBuffSize);
		mBytesWritten += inBuffSize;
	}

	return mLocalFile.fail();
}
unsigned long SingleMediaFileCache::read(unsigned char* outBuff, unsigned long inBuffSize) {
	mLocalFile.read((char*)outBuff, inBuffSize);
	return mLocalFile.gcount();
}
bool SingleMediaFileCache::readSeek(unsigned long inSeekPos) {
	if (inSeekPos < mBytesWritten) {
		mLocalFile.seekg(inSeekPos);
		return true;
	} else {
		return false;
	}
}