#include "stdafx.h"
//#include ".\oggdemuxer.h"
//
//OggDemuxer::OggDemuxer(void)
//	:	mDataSource(NULL)
//	,	mWorkingBuffer(NULL)
//	,	mWorkingBufferSize(0)
//{
//	mWorkingBufferSize = 4096;
//	mWorkingBuffer = new unsigned char[mWorkingBufferSize];
//
//}
//
//OggDemuxer::~OggDemuxer(void)
//{
//	delete [] mWorkingBuffer;
//}
//
//bool OggDemuxer::load(string inSourceName) {
//	mSourceName = inSourceName;
//
//	mDataSource = DataSourceFactory::createDataSource(inSourceName);
//}
//
//__int64 OggDemuxer::seek(__int64 inSeekTime) {
//
//}
//
//
//bool OggDemuxer::registerVirtualCallback(IOggCallback* inCallback) {
//	mDataBuffer.registerVirtualCallback(inCallback);
//}
//
//bool OggDemuxer::processChunk(unsigned long inNumBytes) {
//	ASSERT(inNumBytes <= mWorkingBufferSize);
//	unsigned long locNumRead = mDataSource->read(mWorkingBuffer, inNumBytes);
//	if (locNumRead > 0) {
//		mDataBuffer.feed(mWorkingBuffer, locNumRead);
//		return true;
//	} else {
//		return false;
//	}
//}
//
//
