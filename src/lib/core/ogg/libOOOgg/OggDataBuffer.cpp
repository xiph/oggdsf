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
#include ".\oggdatabuffer.h"

OggDataBuffer::OggDataBuffer(void)
{
	pendingPage = NULL;
	mState = AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}

OggDataBuffer::~OggDataBuffer(void)
{
	delete pendingPage;
}

bool OggDataBuffer::registerPageCallback(OggCallbackRego* inPageCB) {
	if (inPageCB != NULL) {
		mAlwaysCallList.push_back(inPageCB);
		return true;
	} else {
		return false;
	}
}
bool OggDataBuffer::registerSerialNo(SerialNoRego* inSerialRego) {
	if (inSerialRego != NULL) {
		mSerialNoCallList.push_back(inSerialRego);
		return true;
	} else {
		return false;
	}
}

bool OggDataBuffer::registerVirtualCallback(IOggCallback* inCBInterface) {
	if (inCBInterface != NULL) {
		mVirtualCallbackList.push_back(inCBInterface);
		return true;
	} else {
		return false;
	}
}


unsigned long OggDataBuffer::numBytesAvail() {
	//Returns how many bytes are available in the buffer
	return mStream.tellp() - mStream.tellg();
}

OggDataBuffer::eState OggDataBuffer::state() {
	//returns the state of the stream
	return mState;
}
bool OggDataBuffer::dispatch(OggPage* inOggPage) {
	bool locIsOK;

	//Fire off the oggpage to whoever is registered to get it

	for (unsigned long i = 0; i < mAlwaysCallList.size(); i++) {
		mAlwaysCallList[i]->dispatch(inOggPage);
	}

	//Fire off the oggpage to those that registered for a particular seriao number.
	//CHECK::: Does this actually even check for serial number matches ??
	for (unsigned long i = 0; i < mSerialNoCallList.size(); i++) {
		mSerialNoCallList[i]->dispatch(inOggPage);
	}

	//The above callbacks will only call back to global functions or static members. They won't match the callback
	// function specification if they are bound memebr functions
	//Any class that implements the IOggCallback interface can pass a point to themselves into this class
	// and then a call back can be delivered to a function in a specific instance of an object.
	for (unsigned long i = 0; i < mVirtualCallbackList.size(); i++) {
		locIsOK = mVirtualCallbackList[i]->acceptOggPage(inOggPage);
		if (!locIsOK) {
			//Somethings happened deeper in the stack like we are being asked to stop.
			return false;
		}
	}

	//Delete the page... if the called functions wanted a copy they should have taken one for themsselves.
	delete inOggPage;
	pendingPage = NULL;
	return true;
}

bool OggDataBuffer::feed(const char* inData, unsigned long inNumBytes) {
	
	mStream.write(inData, inNumBytes);
	//FIX ::: Need error checking.

	return processBuffer();


}
void OggDataBuffer::processBaseHeader() {
		//Delete the previous page
		delete pendingPage;
		//make a fresh one
		pendingPage = new OggPage;

		//Make a local buffer
		unsigned char* locBuff = new unsigned char[OggPageHeader::OGG_BASE_HEADER_SIZE];
		//Read from the stream buffer to it
		mStream.read((char*)locBuff, OggPageHeader::OGG_BASE_HEADER_SIZE);
		//Set the base header into the pending page
		pendingPage->header()->setBaseHeader((unsigned char*)locBuff);
		//NOTE ::: The page will delete the buffer when it's done. Don't delete it here

		//Change the state.
		mState = AWAITING_SEG_TABLE;

		//Set the number of bytes we want for next time
		mNumBytesNeeded = pendingPage->header()->NumPageSegments();
}
void OggDataBuffer::processSegTable() {
		//Make a local buffer
		unsigned char* locBuff = new unsigned char[pendingPage->header()->NumPageSegments()];
		//Read from the stream buffer to it
		mStream.read((char*)locBuff, pendingPage->header()->NumPageSegments());
		//Set the data into the pending pages segtable
		pendingPage->header()->setSegmentTable(locBuff);
		//NOTE ::: The seg table will delete the buffer itself. Don't delete here.

		mState = AWAITING_DATA;

		//Set the number of bytes we want for next time
		mNumBytesNeeded = pendingPage->header()->dataSize();
}

bool OggDataBuffer::processDataSegment() {
	//Make a local buffer
	
	unsigned long locPageDataSize = pendingPage->header()->dataSize();
	unsigned char* locBuff = NULL;// = new unsigned char[locPageDataSize];
	//unsigned long locPacketOffset = 0;

	unsigned char* locSegTable = pendingPage->header()->SegmentTable()->segmentTable();
	unsigned int locNumSegs = pendingPage->header()->SegmentTable()->numSegments();
	
	unsigned long locCurrPackSize = 0;
	
	bool locIsLastSeg = false;

	//Read from the stream buffer to it

	for (unsigned long i = 0; i < locNumSegs; i++) {
		locCurrPackSize += locSegTable[i];
		locIsLastSeg = (locNumSegs - 1 == i);
		if ( (locSegTable[i] != 255) || locIsLastSeg ) {

			locBuff = new unsigned char[locCurrPackSize];
			mStream.read((char*)(locBuff), locCurrPackSize);
			pendingPage->addPacket( new StampedOggPacket(locBuff, locCurrPackSize, (locSegTable[i] != 255), 0, pendingPage->header()->GranulePos()->value(), StampedOggPacket::OGG_END_ONLY ) );
			//locPacketOffset += locCurrPackSize;
			locCurrPackSize = 0;
		}
	}

	mState = AWAITING_BASE_HEADER;

	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
	
	return dispatch(pendingPage);
	
}
void OggDataBuffer::clearData() {
	mStream.clear();
	mStream.flush();
	mStream.seekg(0, ios_base::beg);
	mStream.seekp(0, ios_base::beg);

	if (numBytesAvail() != 0) {
		int i = i;
	}
	mState = eState::AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}
bool OggDataBuffer::processBuffer() {
	bool locErr;
	while (numBytesAvail() >= mNumBytesNeeded) {
		switch (mState) {
			case eState::AWAITING_BASE_HEADER:
				//If theres enough data to form the base header
				if (numBytesAvail() >= OggPageHeader::OGG_BASE_HEADER_SIZE) {
					processBaseHeader();
				}
				break;
			
			case eState::AWAITING_SEG_TABLE:
				//If there is enough data to get the segt table
				if (numBytesAvail() >= pendingPage->header()->NumPageSegments()) {
					processSegTable();
				}
				break;

			case eState::AWAITING_DATA:
				//If all the data segment is available
				if (numBytesAvail() >= pendingPage->header()->dataSize()) {
					locErr = processDataSegment();
					if (!locErr) {
						return false;
					}
				}	
				break;
			default:
				//Do sometyhing ??
				cerr<<"Ogg Buffer Error"<<endl;
				break;
		}
	}
	return true;

}