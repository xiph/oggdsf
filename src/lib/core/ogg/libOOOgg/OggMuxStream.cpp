//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================
#include "StdAfx.h"
#include "oggmuxstream.h"

OggMuxStream::OggMuxStream(INotifyArrival* inNotifier)
	:	mIsEOS(false)
	,	mIsActive(false)
	,	mNotifier(inNotifier)
{
}

OggMuxStream::~OggMuxStream(void)
{
	//Need to delete the contents of the queue later.
}

bool OggMuxStream::acceptOggPage(OggPage* inOggPage) {
	mIsEOS = false;
	mPageQueue.push_back(inOggPage->clone());
	mNotifier->notifyArrival();
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
OggPage* OggMuxStream::peekFront() {
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

bool OggMuxStream::isProcessable() {
	if (isEmpty() &&  !isEOS() && isActive()) {
		return false;
	} else {
		return true;
	}
}
void OggMuxStream::setIsEOS(bool inIsEOS) {
	mIsEOS = inIsEOS;
}

bool OggMuxStream::isActive() {
	return mIsActive;
}
void OggMuxStream::setIsActive(bool inIsActive) {
	mIsActive = inIsActive;
}