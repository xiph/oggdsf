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
#include "stdafx.h"
#include "oggstreammapper.h"

OggStreamMapper::OggStreamMapper(OggDemuxSourceFilter* inOwningFilter)
	:	mOwningFilter(inOwningFilter)
	,	mDataStartsAt(0)
	
{

}

OggStreamMapper::~OggStreamMapper(void)
{
	for (size_t i = 0; i < mStreamList.size(); i++) {
		//This is to deal with a circular reference. The pin has a ref on the filter and a filter one on the pin.
		//If they are both allowed to normal reference count, neither are deleted and both are left with 1 ref each.

		//If the pin just doesn't hold a ref on the filter, an infinite destruction loop occurs.
		//So to combat this... the pin does not have a ref on the filter... the filter controls the pins lifetime.
		// And when this destructor is called (which is in the destructor of the filter), we are just going to NULL
		// out the seekdelegates pointer (which is a apointer to the filter)... since we don't hold a ref on the filter
		// this isn't a leak.
		mStreamList[i]->getPin()->SetDelegate(NULL);
		delete mStreamList[i];
	}
	mStreamList.empty();
	mStreamList.clear();
}

//Sends the page to *only one* stream if it matches the serial number.
bool OggStreamMapper::dispatchPage(OggPage* inOggPage)				//Gives away or deletes page.
{
	//debugLog<<"Dispatch page..."<<endl;
	for (unsigned long i = 0; i < mStreamList.size(); i++) {
		if (mStreamList[i]->serialNo() == inOggPage->header()->StreamSerialNo()) {
			//This is the correct stream
			//DbgLog((LOG_TRACE, 2, TEXT("Mapper : Dispatching page to serial %u",inOggPage->header()->StreamSerialNo())));
			//debugLog<<"Dispatch to stream "<<i<<endl;
			return mStreamList[i]->acceptOggPage(inOggPage);		//Give away page.
			
		}
	}
	//debugLog<<"Shouldn't be here $$$$$$"<<endl;

	//::: CHAINING SUPPORT HACKED IN HERE !!!
	//===================================
	//We only get here if the serial number wasn't in a header... this means either
	// a) It's a chain, and we are going to hack around it
	// b) The file is invalid.

	//Only attempt a chain for a single stream (probably vorbis only)
	if (mStreamList.size() == 1) {
		mStreamList[0]->setSerialNo(inOggPage->header()->StreamSerialNo());
		return mStreamList[0]->acceptOggPage(inOggPage);		//Give away page.
	}
	//return false;
	delete inOggPage;			//Delete page.
	return true;
}

unsigned long OggStreamMapper::startOfData() {
	return mDataStartsAt;
}
bool OggStreamMapper::acceptOggPage(OggPage* inOggPage)			//Gives away page.
{
	
	//FIXED::: Data starts a 0.
	//if(!isReady()) {
	//	mDataStartsAt += inOggPage->pageSize();
	//}

	if (!isReady() && inOggPage->header()->isBOS()) {
		bool locAllowSeekThrough = false;

		//We only want one of the pins to delegate their seek to us.
		//if (mStreamList.size() == 0) {
		//	locAllowSeekThrough = true;
		//}

		locAllowSeekThrough = true;
		//Above code is changed... due to WMP9 and 10 's non-adherence to directshows standard... it
		// requires all streams to be seekable, in contradiction to directshow which specifies that
		// only 1 or more is required.


			
		//If the page is a BOS we need to start a new stream
		OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter, locAllowSeekThrough);
		//FIX::: Need to check for NULL
		if (locStream != NULL) {
			mStreamList.push_back(locStream);
		}
		return true;
		//TODO::: Shuold verify the mapper doesn';t already have a stream with this number !
	} else {
		return dispatchPage(inOggPage);			//Gives away page.
	}
}
bool OggStreamMapper::toStartOfData() {
	if (isReady()) {  //CHECK::: Should check for allow dsipatch ???
		for (unsigned long i = 0; i < mStreamList.size(); i++) {
			//Flush each stream, then ignore the codec headers.
			mStreamList[i]->flush((unsigned short)mStreamList[i]->numCodecHeaders());
		}	
		return true;
	} else {
		return false;
	}
}

void OggStreamMapper::setAllowDispatch(bool inAllowDispatch) {
	//debugLog<<"Setting allow dispatch..."<<endl;
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