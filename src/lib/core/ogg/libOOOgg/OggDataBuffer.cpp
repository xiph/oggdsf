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
	:	mBuffer(NULL)
{
	mBuffer = new CircularBuffer(MAX_OGG_PAGE_SIZE);

	debugLog.open("G:\\logs\\OggDataBuffer.log", ios_base::out);
	pendingPage = NULL;
	mState = AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}

//Debug only
OggDataBuffer::OggDataBuffer(bool x)
	:	mBuffer(NULL)
{
	mBuffer = new CircularBuffer(MAX_OGG_PAGE_SIZE);

	debugLog.open("G:\\logs\\OggDataBufferSeek.log", ios_base::out);
	pendingPage = NULL;
	mState = AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}
//

OggDataBuffer::~OggDataBuffer(void)
{
	delete mBuffer;
	debugLog.close();
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

	unsigned long locBytesAvail = mBuffer->numBytesAvail();				//mStream.tellp() - mStream.tellg();
	debugLog<<"Bytes avail = "<<locBytesAvail<<endl;
	return locBytesAvail;
}

OggDataBuffer::eState OggDataBuffer::state() {
	//returns the state of the stream
	return mState;
}
bool OggDataBuffer::dispatch(OggPage* inOggPage) {
	debugLog<<"Dispatching page..."<<endl;
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

OggDataBuffer::eFeedResult OggDataBuffer::feed(const unsigned char* inData, unsigned long inNumBytes) {
	if (inNumBytes != 0) {
		if (inData != NULL) {
			//Buffer is not null and there is at least 1 byte of data.
			
			debugLog<<"********** Fed "<<inNumBytes<<" bytes..."<<endl;
		
			///STREAM ACCESS::: WRite
			//Write the data into the stream buffer
			
			//mStream.write(inData, inNumBytes);
			mBuffer->write(inData, inNumBytes);


			//if(mStream.fail()) {
			//	debugLog<<"ProcessBaseHeader : Buffer write Write FAILED"<<endl;
			//	return FEED_BUFFER_WRITE_ERROR;
			//}
		
			return (eFeedResult)processBuffer();
			
		} else {
			//Numbytes not equal to zero but inData point is NULL
			debugLog<<"Feed : Fed NULL Pointer"<<endl;
			return FEED_NULL_POINTER;
		}
	} else {
		//numbytes was zero... we do nothing and it's not an error.
		debugLog<<"Feed : Fed *zero* bytes... Not an error, do nothing, return ok."<<endl;
		return FEED_OK;
	}
		
	
}
OggDataBuffer::eProcessResult OggDataBuffer::processBaseHeader() {
		debugLog<<"ProcessBaseHeader : "<<endl;
		
		//Delete the previous page
		delete pendingPage;
		
		//make a fresh one
		
		//TODAY::: verify OggPage initialises properly.
		pendingPage = new OggPage;

		//Make a local buffer for the header
		unsigned char* locBuff = new unsigned char[OggPageHeader::OGG_BASE_HEADER_SIZE];
		
		debugLog<<"ProcessBaseHeader : Reading from stream..."<<endl;
		
		//STREAM ACCESS::: Read
		//Read from the stream buffer to it
		//mStream.read((char*)locBuff, OggPageHeader::OGG_BASE_HEADER_SIZE);
		mBuffer->read(locBuff, OggPageHeader::OGG_BASE_HEADER_SIZE);

		//if(mStream.fail()) {
		//	debugLog<<"ProcessBaseHeader : File Read FAILED"<<endl;
		//	delete locBuff;
		//	return PROCESS_STREAM_READ_ERROR;
		//}

		//Set the base header into the pending page

		bool locRetVal = pendingPage->header()->setBaseHeader((unsigned char*)locBuff);
		if (locRetVal == false) {
			return PROCESS_FAILED_TO_SET_HEADER;
		}
		
		//NOTE ::: The page will delete the buffer when it's done. Don't delete it here

		//Set the number of bytes we want for next time
		mNumBytesNeeded = pendingPage->header()->NumPageSegments();

		debugLog<<"ProcessBaseHeader : Setting state to AWAITING_SEG_TABLE"<<endl;
		//Change the state.
		mState = AWAITING_SEG_TABLE;


		debugLog<<"ProcessBaseHeader : Bytes needed for seg table = "<<mNumBytesNeeded<<endl;	
		return PROCESS_OK;
}
OggDataBuffer::eProcessResult OggDataBuffer::processSegTable() {

	//Assumes a valid pending page, with numPagesegments set in the header already.
	//creates a chunk of memory size numpagesegments and stores it,.

	debugLog<<"ProcessSegTable : "<<endl;

	//TODAY::: What happens when numpage segments is zero.

	//Save a local copy of the number of page segments - Get this from the already set header.
	unsigned char locNumSegs = pendingPage->header()->NumPageSegments();

	debugLog<<"ProcessSegTable : Num segs = "<<(int)locNumSegs<<endl;

	//Make a local buffer the size of the segment table. 0 - 255
	unsigned char* locBuff = new unsigned char[locNumSegs];
	
	debugLog<<"ProcessSegTable : Reading from buffer..."<<endl;

	//Read the segment table from the buffer to locBuff
	//mStream.read((char*)locBuff, (std::streamsize)locNumSegs);
	mBuffer->read(locBuff, (std::streamsize)locNumSegs);
	//if(mStream.fail()) {
	//	debugLog<<"ProcessSegTable : Read FAILED"<<endl;
	//	delete locBuff;
	//	return PROCESS_STREAM_READ_ERROR;
	//}

	//TODAY::: Check out the page header class.

	//Make a new segment table from the bufferd data.
	OggSegmentTable* locSegTable = new OggSegmentTable();
	unsigned long locDataSize = locSegTable->setSegmentTable(locBuff, locNumSegs);

	//Set the data into the pending pages segtable... giving  the pointer away, don't use any more.
	//NOTE ::: The seg table will delete the buffer itself. Don't delete here.
	pendingPage->header()->setSegmentTable(locSegTable);
	locSegTable = NULL;

	//Set the number of bytes we want for next time - which is the size of the page data.
	mNumBytesNeeded = locDataSize;
	debugLog<<"ProcessSegTable : Num bytes needed for data = "<< mNumBytesNeeded<<endl;

	debugLog<<"ProcessSegTable : Transition to AWAITING_DATA"<<endl;
	mState = AWAITING_DATA;
	return PROCESS_OK;

}

OggDataBuffer::eProcessResult OggDataBuffer::processDataSegment() {
	
	debugLog<<"ProcessDataSegment : "<<endl;
	//Make a local buffer
	
	unsigned long locPageDataSize = pendingPage->header()->dataSize();
	
	debugLog<<"ProcessDataSegment : Page data size = "<<locPageDataSize<<endl;
	unsigned char* locBuff = NULL;// = new unsigned char[locPageDataSize];
	//unsigned long locPacketOffset = 0;

	//THis is a raw pointer into the segment table, don't delete it.
	unsigned char* locSegTable = pendingPage->header()->SegmentTable()->segmentTable();
	unsigned int locNumSegs = pendingPage->header()->SegmentTable()->numSegments();
	
	debugLog<<"ProcessDataSegment : Num segs = "<<locNumSegs<<endl;


	unsigned long locCurrPackSize = 0;
	

	//Read from the stream buffer to it

	for (unsigned long i = 0; i < locNumSegs; i++) {
		//Packet sums the lacing values of the segment table.
		locCurrPackSize += locSegTable[i];

		//If its the last segment  in the page or if the lacing value is not 255(ie packet boundary.

		/*TRUTH TABLE:
			last lacing value							lacing value is *not* 255
			=================							=========================
			true										true						}	If its the last one or a packet boundary(255 lacing value) we add it.
			true										false						}
			false										true						}
			false										false						If it is a 255 (packet continues) and it's not the last one do nothibng
			it is the last lacing value on the page


			Lacing values for a Packet never end with 255... if multiple of 255 have a next 0 lacing value.
		*/
		if ( (locSegTable[i] != 255) || (locNumSegs - 1 == i) ) {
			//If its the last lacing value or the the lacing value is not 255 (ie packet boundry)
			
			//This pointer is given to the packet... it deletes it.
			locBuff = new unsigned char[locCurrPackSize];

			//STREAM ACCESS:::
			//Read data from the stream into the local buffer.
			//mStream.read((char*)(locBuff), locCurrPackSize);
			mBuffer->read(locBuff, locCurrPackSize);


			//FIX::: check for stream failure.

			debugLog<<"ProcessDataSegment : Adding packet size = "<<locCurrPackSize<<endl;
			
			//A packet ends when a lacing value is not 255. So the check for != 255 means the isComplete property of the packet is not set unless the
			// lacing value is not equal to 255.
			//ERROR CHECK:::
			pendingPage->addPacket( new StampedOggPacket(locBuff, locCurrPackSize, (locSegTable[i] != 255), 0, pendingPage->header()->GranulePos(), StampedOggPacket::OGG_END_ONLY ) );
			
			//Reset the packet size counter.
			locCurrPackSize = 0;
		}
	}

	
	//Update the state for how many bytes are now needed
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
	
	debugLog<<"ProcessDataSegment : num bytes needed = "<<mNumBytesNeeded<<endl;

	bool locRet = dispatch(pendingPage);
	if (locRet == true) {
        debugLog<<"ProcessDataSegment : Transition to AWAITING_BASE_HEADER"<<endl;
		mState = AWAITING_BASE_HEADER;
		return PROCESS_OK;;
	} else {
		debugLog<<"ProcessDataSegment : Dispatch failed."<<endl;
		return PROCESS_DISPATCH_FAILED;
	}
		
}
void OggDataBuffer::clearData() {
	mBuffer->reset();
	//mStream.clear();
	//mStream.flush();
	//mStream.seekg(0, ios_base::beg);
	//mStream.seekp(0, ios_base::beg);

	debugLog<<"ClearData : Transition back to AWAITING_BASE_HEADER"<<endl;
	
	
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
	mState = eState::AWAITING_BASE_HEADER;

	debugLog<<"ClearData : Num bytes needed = "<<mNumBytesNeeded<<endl;
}

OggDataBuffer::eProcessResult OggDataBuffer::processBuffer() {
	debugLog<<"ProcessBuffer :"<<endl;
	
	while (numBytesAvail() >= mNumBytesNeeded) {
		debugLog<<"ProcessBuffer : Bytes Needed = "<<mNumBytesNeeded<<" --- "<<"Bytes avail = "<<numBytesAvail()<<endl;
		switch (mState) {

			//QUERY:::	Should it be a bug when the if state inside the switch falls through,... potential for infinite loop.
			case eState::AWAITING_BASE_HEADER:
				debugLog<<"ProcessBuffer : State = AWAITING_BASE_HEADER"<<endl;
				
				//If theres enough data to form the base header
				if (numBytesAvail() >= OggPageHeader::OGG_BASE_HEADER_SIZE) {
					debugLog<<"ProcessBuffer : Enough to process..."<<endl;
					
					eProcessResult locResult = processBaseHeader();
                    
					if (locResult != PROCESS_OK) {
						mState = LOST_PAGE_SYNC;
						//Base header process failed
						return locResult;
					}
				}
				break;
			
			case eState::AWAITING_SEG_TABLE:
				debugLog<<"ProcessBuffer : State = AWAITING_SEG_TABLE"<<endl;
				
				//If there is enough data to get the segt table
				if (numBytesAvail() >= pendingPage->header()->NumPageSegments()) {
					debugLog<<"ProcessBuffer : Enough to process..."<<endl;
					
					eProcessResult locResult = processSegTable();
               
					if (locResult != PROCESS_OK) {
						mState = LOST_PAGE_SYNC;
						//segment table process failed
						return locResult;
					}
				}
				break;

			case eState::AWAITING_DATA:
				debugLog<<"ProcessBuffer : State = AWAITING_DATA"<<endl;
				//If all the data segment is available
				if (numBytesAvail() >= pendingPage->header()->dataSize()) {
					debugLog<<"ProcessBuffer : Enough to process..."<<endl;

					//FIX::: Need error check.
					return processDataSegment();
				}	
				break;
			case eState::LOST_PAGE_SYNC:
				debugLog<<"ProcessBuffer : State = LOST_PAGE_SYNC"<<endl;
				return PROCESS_LOST_SYNC;
			default:
				//Do sometyhing ??
				debugLog<<"ProcessBuffer : Ogg Buffer Error"<<endl;
				return PROCESS_UNKNOWN_INTERNAL_ERROR;
				break;
		}
	}

	//There wasn't enough data to progress if we are here.
	return PROCESS_OK;

}

//Debug Only
void OggDataBuffer::debugWrite(string inString) {
	debugLog<<inString<<endl;
}