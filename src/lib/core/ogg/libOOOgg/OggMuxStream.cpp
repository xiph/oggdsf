#include "StdAfx.h"
#include "oggmuxstream.h"

OggMuxStream::OggMuxStream(void)
	:	mIsEOS(true)
{
}

OggMuxStream::~OggMuxStream(void)
{
	//Need to delete the contents of the queue later.
}

bool OggMuxStream::acceptOggPage(OggPage* inOggPage) {
	mIsEOS = false;
	mPageQueue.push_back(inOggPage);
	return true;
}

OggPage* OggMuxStream::popFront() {
	OggPage* retPage = NULL;
	if (!mPageQueue.empty()) {
		retPage = mPageQueue.front();
		mPageQueue.pop_front();
	}
	return retPage;
}
const OggPage* OggMuxStream::peekfront() {
	OggPage* retPage = NULL;
	if (!mPageQueue.empty()) {
		retPage = mPageQueue.front();
		
	}
	return retPage;
}
__int64 OggMuxStream::frontTime() {
	__int64 retTime = INT64_MAX;
	if (!mPageQueue.empty()) {
		retTime = mPageQueue.front()->header()->GranulePos()->value();
	}
	return retTime;
}

bool OggMuxStream::isEmpty() {
	return mPageQueue.empty();
}
bool OggMuxStream::isEOS() {
	return mIsEOS;
}

void OggMuxStream::setIsEOS(bool inIsEOS) {
	mIsEOS = inIsEOS;
}