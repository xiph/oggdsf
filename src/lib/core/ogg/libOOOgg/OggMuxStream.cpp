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
	,	mIsSensibleTime(true)
	,	mConvNumerator(1)
	,	mConvDenominator(1)
	,	mConvScaleFactor(1)
	,	mConvTheoraLogKeyFrameInterval(0)
{
	//debugLog.open("G:\\logs\\oggmuxstream.log", ios_base::out);
}

OggMuxStream::~OggMuxStream(void)
{
	//LEAK::: Need to delete the contents of the queue later.
}

bool OggMuxStream::acceptOggPage(OggPage* inOggPage) {		//Holds page for later... still needs deleting in destructor
	mIsEOS = false;
	mPageQueue.push_back(inOggPage);		//AOP::: Clone not required.
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
		retTime = mPageQueue.front()->header()->GranulePos();;
	}
	return retTime;
}

__int64 OggMuxStream::scaledFrontTime() {

	return convertTime(frontTime());
}

__int64 OggMuxStream::convertTime(__int64 inGranulePos) {
	__int64 retTime = INT64_MAX;
	if (inGranulePos != INT64_MAX) {
		if (mIsSensibleTime) {
			retTime = (inGranulePos * mConvScaleFactor * mConvDenominator) / mConvNumerator;
		} else {
			//Timestamp hacks start here...
			unsigned long locMod = (unsigned long)pow(2, mConvTheoraLogKeyFrameInterval);
			
			unsigned long locInterFrameNo = (inGranulePos) % locMod;
	
			__int64 locAbsFramePos = (inGranulePos >> mConvTheoraLogKeyFrameInterval) + locInterFrameNo;
	
			retTime = (locAbsFramePos * mConvScaleFactor * mConvDenominator) / mConvNumerator;
			
			
		}
	} 
	return retTime;
		
	
}

bool OggMuxStream::setConversionParams(__int64 inNumerator, __int64 inDenominator, __int64 inScaleFactor) {
	mConvNumerator = inNumerator;
	mConvDenominator = inDenominator;
	mConvScaleFactor = inScaleFactor;
	mIsSensibleTime = true;
	return true;
}

bool OggMuxStream::setConversionParams(__int64 inNumerator, __int64 inDenominator, __int64 inScaleFactor, __int64 inTheoraLogKFI) {
	mConvNumerator = inNumerator;
	mConvDenominator = inDenominator;
	mConvScaleFactor = inScaleFactor;
	mConvTheoraLogKeyFrameInterval = inTheoraLogKFI;
	mIsSensibleTime = false;
	return true;
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
	//Notify that the streams are in new state.
	mNotifier->notifyArrival();
}

bool OggMuxStream::isActive() {
	return mIsActive;
}
void OggMuxStream::setIsActive(bool inIsActive) {
	mIsActive = inIsActive;
}

