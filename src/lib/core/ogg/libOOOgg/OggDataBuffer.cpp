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
	,	mPrevGranPos(0)
{
	mBuffer = new CircularBuffer(MAX_OGG_PAGE_SIZE);

	//debugLog.open("G:\\logs\\OggDataBuffer.log", ios_base::out);
	pendingPage = NULL;
	mState = AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}

//Debug only
OggDataBuffer::OggDataBuffer(bool x)
	:	mBuffer(NULL)
	,	mPrevGranPos(0)
{
	mBuffer = new CircularBuffer(MAX_OGG_PAGE_SIZE);

	//debugLog.open("G:\\logs\\OggDataBufferSeek.log", ios_base::out);
	pendingPage = NULL;
	mState = AWAITING_BASE_HEADER;
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
}
//

OggDataBuffer::~OggDataBuffer(void)
{
	delete[] mBuffer;
	//debugLog.close();
	delete pendingPage;
}

bool OggDataBuffer::registerStaticCallback(fPageCallback inPageCallback) {
	mStaticCallback = inPageCallback;
	mVirtualCallback = NULL;
	
	return true;
}
//bool OggDataBuffer::registerSerialNo(SerialNoRego* inSerialRego) {
//	if (inSerialRego != NULL) {
//		mSerialNoCallList.push_back(inSerialRego);
//		return true;
//	} else {
//		return false;
//	}
//}

bool OggDataBuffer::registerVirtualCallback(IOggCallback* inPageCallback) {
	mVirtualCallback = inPageCallback;
	mStaticCallback = NULL;
	return true;
	
}


unsigned long OggDataBuffer::numBytesAvail() {
	//Returns how many bytes are available in the buffer

	unsigned long locBytesAvail = mBuffer->numBytesAvail();				//mStream.tellp() - mStream.tellg();
	////debugLog<<"Bytes avail = "<<locBytesAvail<<endl;
	return locBytesAvail;
}

OggDataBuffer::eState OggDataBuffer::state() {
	//returns the state of the stream
	return mState;
}
OggDataBuffer::eDispatchResult OggDataBuffer::dispatch(OggPage* inOggPage) {
	//TODO::: Who owns this pointer inOggPage ?
	//debugLog<<"Dispatching page..."<<endl<<endl;


	//Fire off the oggpage to whoever is registered to get it

	if (mVirtualCallback != NULL) {
		if (mVirtualCallback->acceptOggPage(inOggPage) == true) {
			return DISPATCH_OK;
		} else {
			return DISPATCH_FALSE;
		}
	} else if (mStaticCallback != NULL) {
		if (mStaticCallback(inOggPage) == true) {
			return DISPATCH_OK;	
		} else {
			return DISPATCH_FALSE;
		}
	}

	//Delete the page... if the called functions wanted a copy they should have taken one for themsselves.
	//Not any more acceptOggPage takes responsibility for the memory you pass into it. See IOggCallback.h
	//delete inOggPage;
	pendingPage = NULL;
	return DISPATCH_NO_CALLBACK;
}

OggDataBuffer::eFeedResult OggDataBuffer::feed(const unsigned char* inData, unsigned long inNumBytes) {
	if (inNumBytes != 0) {
		if (inData != NULL) {
			//Buffer is not null and there is at least 1 byte of data.
			
			//debugLog<<"Fed "<<inNumBytes<<" bytes..."<<endl;
		
			unsigned long locNumWritten = mBuffer->write(inData, inNumBytes);

			if (locNumWritten < inNumBytes) {
				//Handle this case... you lose data.
				//Buffer is full
				//debugLog<<"Feed : Could count feed in " <<inNumBytes<<" bytes"<<endl
				//		<<"Feed : ** "<<mBuffer->numBytesAvail()<<" avail, "<<mBuffer->spaceLeft()<<" space left."<<endl;
				locNumWritten = locNumWritten;
			}

			return (eFeedResult)processBuffer();
			
		} else {
			//Numbytes not equal to zero but inData point is NULL
			//debugLog<<"Feed : Fed NULL Pointer"<<endl;
			return FEED_NULL_POINTER;
		}
	} else {
		//numbytes was zero... we do nothing and it's not an error.
		//debugLog<<"Feed : Fed *zero* bytes... Not an error, do nothing, return ok."<<endl;
		return FEED_OK;
	}
		
	
}
OggDataBuffer::eProcessResult OggDataBuffer::processBaseHeader() {
		//debugLog<<"Processing base header..."<<endl;
		
		//Delete the previous page
		delete pendingPage;
		
		//Make a fresh ogg page
		pendingPage = new OggPage;

		//Make a local buffer for the header
		unsigned char* locBuff = new unsigned char[OggPageHeader::OGG_BASE_HEADER_SIZE];
		
		//debugLog<<"ProcessBaseHeader : Reading from stream..."<<endl;
		
		//STREAM ACCESS::: Read
		unsigned long locNumRead = mBuffer->read(locBuff, OggPageHeader::OGG_BASE_HEADER_SIZE);
		
		if (locNumRead < OggPageHeader::OGG_BASE_HEADER_SIZE) {
			//debugLog<<"ProcessBaseHeader : ###### Read was short."<<endl;
			//debugLog<<"ProcessBaseHeader : ** "<<mBuffer->numBytesAvail()<<" avail, "<<mBuffer->spaceLeft()<<" space left."<<endl;
			locNumRead = locNumRead;
		}

		bool locRetVal = pendingPage->header()->setBaseHeader((unsigned char*)locBuff);
		if (locRetVal == false) {
			return PROCESS_FAILED_TO_SET_HEADER;
		}
	
		//Set the number of bytes we want for next time
		mNumBytesNeeded = pendingPage->header()->NumPageSegments();

		//debugLog<<"Setting state to AWAITING_SEG_TABLE"<<endl;
		//Change the state.
		mState = AWAITING_SEG_TABLE;


		//debugLog<<"Bytes needed for seg table = "<<mNumBytesNeeded<<endl;	
		return PROCESS_OK;
}
OggDataBuffer::eProcessResult OggDataBuffer::processSegTable() {

	//Assumes a valid pending page, with numPagesegments set in the header already.
	//creates a chunk of memory size numpagesegments and stores it,.

	//debugLog<<"Processing Segment Table..."<<endl;

	//TODAY::: What happens when numpage segments is zero.

	//Save a local copy of the number of page segments - Get this from the already set header.
	unsigned char locNumSegs = pendingPage->header()->NumPageSegments();

	//debugLog<<"Num segments = "<<(int)locNumSegs<<endl;

	//Make a local buffer the size of the segment table. 0 - 255
	unsigned char* locBuff = new unsigned char[locNumSegs];
	
	////debugLog<<"ProcessSegTable : Reading from buffer..."<<endl;

	//Read the segment table from the buffer to locBuff
	//mStream.read((char*)locBuff, (std::streamsize)locNumSegs);
	unsigned long locNumRead = mBuffer->read(locBuff, (unsigned long)locNumSegs);
	
	if (locNumRead < locNumSegs) {
		//debugLog<<"ProcessSegTable : ##### Short read"<<endl;
		//debugLog<<"ProcessSegTable : ** "<<mBuffer->numBytesAvail()<<" avail, "<<mBuffer->spaceLeft()<<" space left."<<endl;
		locNumRead = locNumRead;
	}


	//Make a new segment table from the bufferd data.
	pendingPage->header()->setSegmentTable(locBuff);
	locBuff = NULL;

	
	//Set the number of bytes we want for next time - which is the size of the page data.
	
	mNumBytesNeeded = pendingPage->header()->calculateDataSize();

	//debugLog<<"Num bytes needed for data = "<< mNumBytesNeeded<<endl;

	//debugLog<<"Transition to AWAITING_DATA"<<endl;
	
	mState = AWAITING_DATA;
	return PROCESS_OK;

}

OggDataBuffer::eProcessResult OggDataBuffer::processDataSegment() {
	
	//debugLog<<"Processing Data Segment..."<<endl;
	//Make a local buffer
	
	unsigned long locPageDataSize = pendingPage->header()->dataSize();
	
	//debugLog<<"ProcessDataSegment : Page data size = "<<locPageDataSize<<endl;
	unsigned char* locBuff = NULL;// = new unsigned char[locPageDataSize];
	//unsigned long locPacketOffset = 0;

	//THis is a raw pointer into the segment table, don't delete it.
	unsigned char* locSegTable = pendingPage->header()->SegmentTable();
	unsigned int locNumSegs = pendingPage->header()->NumPageSegments();
	
	//debugLog<<"ProcessDataSegment : Num segs = "<<locNumSegs<<endl;


	unsigned long locCurrPackSize = 0;
	

	bool locIsFirstPacket = true;
	__int64 locPrevGranPos = 0;
	for (unsigned long i = 0; i < locNumSegs; i++) {
		//Packet sums the lacing values of the segment table.
		locCurrPackSize += locSegTable[i];

		//If its the last segment  in the page or if the lacing value is not 255(ie packet boundary.

		/* TRUTH TABLE:
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
			unsigned long locNumRead = mBuffer->read(locBuff, locCurrPackSize);
			
			if (locNumRead < locCurrPackSize) {
				//debugLog<<"ProcessDataSegment : ###### Short read"<<endl;
				//debugLog<<"ProcessDataSegment : ** "<<mBuffer->numBytesAvail()<<" avail, "<<mBuffer->spaceLeft()<<" space left."<<endl;
				locNumRead = locNumRead;
			}

			//debugLog<<"Adding packet - size = "<<locCurrPackSize<<endl;
			
			//A packet ends when a lacing value is not 255. So the check for != 255 means the isComplete property of the packet is not set unless the
			// lacing value is not equal to 255.
			//ERROR CHECK:::
			bool locIsContinuation = false;
			
			if (locIsFirstPacket) {
				locPrevGranPos = mPrevGranPos;
				mPrevGranPos = pendingPage->header()->GranulePos();
				locIsFirstPacket = false;
				//First packet, check if the continuation flag is set.
				if ((pendingPage->header()->HeaderFlags() & 1) == 1) {
					//Continuation flag is set.
					locIsContinuation = true;
				}
			}

			pendingPage->addPacket( new StampedOggPacket(locBuff, locCurrPackSize, (locSegTable[i] == 255), locIsContinuation, locPrevGranPos, pendingPage->header()->GranulePos(), StampedOggPacket::OGG_BOTH ) );
			
			//Reset the packet size counter.
			locCurrPackSize = 0;
		}
	}

	
	//Update the state for how many bytes are now needed
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
	
	//debugLog<<"ProcessDataSegment : num bytes needed = "<<mNumBytesNeeded<<endl;

	//Dispatch the finished pagbve
	eDispatchResult locRet = dispatch(pendingPage);
	pendingPage = NULL;   //We give away the pointer
	
	if (locRet == DISPATCH_OK) {
        //debugLog<<"ProcessDataSegment : Transition to AWAITING_BASE_HEADER"<<endl;
		mState = AWAITING_BASE_HEADER;
		return PROCESS_OK;
	} else if (locRet == DISPATCH_FALSE) {
		mState = AWAITING_BASE_HEADER;
		return PROCESS_DISPATCH_FALSE;	
	} else {
		//debugLog<<"ProcessDataSegment : Dispatch failed."<<endl;
		return PROCESS_DISPATCH_FAILED;
	}
		
}
void OggDataBuffer::clearData() {
	mBuffer->reset();
	mPrevGranPos = 0;
	//debugLog<<"ClearData : Transition back to AWAITING_BASE_HEADER"<<endl;
	
	
	mNumBytesNeeded = OggPageHeader::OGG_BASE_HEADER_SIZE;
	mState = eState::AWAITING_BASE_HEADER;

	////debugLog<<"ClearData : Num bytes needed = "<<mNumBytesNeeded<<endl;
}

OggDataBuffer::eProcessResult OggDataBuffer::processBuffer() {
		
	eProcessResult locResult = PROCESS_OK;

	while (numBytesAvail() >= mNumBytesNeeded) {
		////debugLog<<"ProcessBuffer : Bytes Needed = "<<mNumBytesNeeded<<" --- "<<"Bytes avail = "<<numBytesAvail()<<endl;
		switch (mState) {

			//QUERY:::	Should it be a bug when the if state inside the switch falls through,... potential for infinite loop.
			case eState::AWAITING_BASE_HEADER:
				//debugLog<<"ProcessBuffer : State = AWAITING_BASE_HEADER"<<endl;
				
				//If theres enough data to form the base header
				if (numBytesAvail() >= OggPageHeader::OGG_BASE_HEADER_SIZE) {
					//debugLog<<"ProcessBuffer : Enough to process..."<<endl;
					
					locResult = processBaseHeader();
                    
					if (locResult != PROCESS_OK) {
						mState = LOST_PAGE_SYNC;
						//Base header process failed
						return locResult;
					}
				}
				break;
			
			case eState::AWAITING_SEG_TABLE:
				//debugLog<<"ProcessBuffer : State = AWAITING_SEG_TABLE"<<endl;
				
				//If there is enough data to get the segt table
				if (numBytesAvail() >= pendingPage->header()->NumPageSegments()) {
					//debugLog<<"ProcessBuffer : Enough to process..."<<endl;
					
					locResult = processSegTable();
               
					if (locResult != PROCESS_OK) {
						mState = LOST_PAGE_SYNC;
						//segment table process failed
						return locResult;
					}
				}
				break;

			case eState::AWAITING_DATA:
				//debugLog<<"ProcessBuffer : State = AWAITING_DATA"<<endl;
				//If all the data segment is available
				if (numBytesAvail() >= pendingPage->header()->dataSize()) {
					//debugLog<<"ProcessBuffer : Enough to process..."<<endl;

					//FIX::: Need error check.
					locResult = processDataSegment();
					
					if (locResult == PROCESS_DISPATCH_FAILED) {
						mState = LOST_PAGE_SYNC;
						//segment table process failed
						return locResult;
					}

				}	
				break;
			case eState::LOST_PAGE_SYNC:
				//debugLog<<"ProcessBuffer : State = LOST_PAGE_SYNC"<<endl;
				return PROCESS_LOST_SYNC;
			default:
				//Do sometyhing ??
				//debugLog<<"ProcessBuffer : Ogg Buffer Error"<<endl;
				return PROCESS_UNKNOWN_INTERNAL_ERROR;
				break;
		}
	}

	//There wasn't enough data to progress if we are here.
	return locResult;

}

//Debug Only
void OggDataBuffer::debugWrite(string inString) {
	//debugLog<<inString<<endl;
}