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
#include "oggstreammapper.h"

OggStreamMapper::OggStreamMapper(OggDemuxSourceFilter* inOwningFilter)
	:	mOwningFilter(inOwningFilter)
	,	mDataStartsAt(0)
	
{

}

OggStreamMapper::~OggStreamMapper(void)
{
	for (size_t i = 0; i < mStreamList.size(); i++) {
		delete mStreamList[i];
	}
}

//Sends the page to *only one* stream if it matches the serial number.
bool OggStreamMapper::dispatchPage(OggPage* inOggPage) 
{
	for (unsigned long i = 0; i < mStreamList.size(); i++) {
		if (mStreamList[i]->serialNo() == inOggPage->header()->StreamSerialNo()) {
			//This is the correct stream
			//DbgLog((LOG_TRACE, 2, TEXT("Mapper : Dispatching page to serial %u",inOggPage->header()->StreamSerialNo())));
			return mStreamList[i]->acceptOggPage(inOggPage);
			
		}
	}
	//return false;
	return true;
}

unsigned long OggStreamMapper::startOfData() {
	return mDataStartsAt;
}
bool OggStreamMapper::acceptOggPage(OggPage* inOggPage) 
{
	
	//FIXED::: Data starts a 0.
	//if(!isReady()) {
	//	mDataStartsAt += inOggPage->pageSize();
	//}

	if (!isReady() && inOggPage->header()->isBOS()) {
		bool locAllowSeekThrough = false;

		//We only want one of the pins to delegate their seek to us.
		if (mStreamList.size() == 0) {
			locAllowSeekThrough = true;
		}
			
		//If the page is a BOS we need to start a new stream
		OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter, locAllowSeekThrough);
		//FIX::: Need to check for NULL
		if (locStream != NULL) {
			mStreamList.push_back(locStream);
		}
		return true;
		//TODO::: Shuold verify the mapper doesn';t already have a stream with this number !
	} else {
		return dispatchPage(inOggPage);
	}
}
bool OggStreamMapper::toStartOfData() {
	if (isReady()) {  //CHECK::: Should check for allow dsipatch ???
		for (unsigned long i = 0; i < mStreamList.size(); i++) {
			//Flush each stream, then ignore the codec headers.
			mStreamList[i]->flush(mStreamList[i]->numCodecHeaders());
		}	
		return true;
	} else {
		return false;
	}
}

void OggStreamMapper::setAllowDispatch(bool inAllowDispatch) {
	for (unsigned long i = 0; i < mStreamList.size(); i++) {
		mStreamList[i]->setAllowDispatch(inAllowDispatch);;
	}
}
bool OggStreamMapper::isReady() {
	bool retVal = true;
	bool locWasAny = false;
	for (unsigned long i = 0; i < mStreamList.size(); i++) {
		locWasAny = true;
		retVal = retVal && mStreamList[i]->streamReady();
	}
	return locWasAny && retVal;
}

unsigned long OggStreamMapper::numStreams() {
	return (unsigned long)mStreamList.size();
}

OggStream* OggStreamMapper::getOggStream(unsigned long inPinNo) {
	if (inPinNo < mStreamList.size()) {
		return mStreamList[inPinNo];
	} else {
		return NULL;
	}
	
}