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
#include "httpfilesource.h"

//#define OGGCODECS_LOGGING
HTTPFileSource::HTTPFileSource(void)
	:	mBufferLock(NULL)
	,	mIsChunked(false)
	,	mIsFirstChunk(true)
	,	mChunkRemains(0)
	,	mNumLeftovers(0)
{
	mBufferLock = new CCritSec;
#ifdef OGGCODECS_LOGGING
	debugLog.open("d:\\zen\\logs\\htttp.log", ios_base::out | ios_base::app);
	fileDump.open("d:\\zen\\logs\\filedump.ogg", ios_base::out|ios_base::binary);
	rawDump.open("D:\\zen\\logs\\rawdump.out", ios_base::out|ios_base::binary);
#endif
	mInterBuff = new unsigned char[RECV_BUFF_SIZE* 2];

}

HTTPFileSource::~HTTPFileSource(void)
{
	//debugLog<<"About to close socket"<<endl;
	close();
	//debugLog<<"Winsock ended"<<endl;
#ifdef OGGCODECS_LOGGING
	debugLog.close();
	fileDump.close();
	rawDump.close();
#endif
	delete mBufferLock;
	delete mInterBuff;
}

void HTTPFileSource::unChunk(unsigned char* inBuff, unsigned long inNumBytes) 
{

	//This method is a bit rough and ready !!
	ASSERT(inNumBytes > 2);
	rawDump.write((char*)inBuff, inNumBytes);
	debugLog<<"UnChunk"<<endl;
	unsigned long locNumBytesLeft = inNumBytes;

	memcpy((void*)(mInterBuff + mNumLeftovers), (const void*)inBuff, inNumBytes);
	locNumBytesLeft +=  mNumLeftovers;
	mNumLeftovers = 0;
	unsigned char* locWorkingBuffPtr = mInterBuff;

	//debugLog<<"inNumBytes = "<<inNumBytes<<endl;

	while (locNumBytesLeft > 8) {
		//debugLog<<"---"<<endl;
		//debugLog<<"Bytes left = "<<locNumBytesLeft<<endl;
		//debugLog<<"ChunkRemaining = "<<mChunkRemains<<endl;

		if (mChunkRemains == 0) {
			//debugLog<<"Zero bytes of chunk remains"<<endl;

			//Assign to a string for easy manipulation of the hex size
			string locTemp;
		
			if (mIsFirstChunk) {
				//debugLog<<"It's the first chunk"<<endl;
				mIsFirstChunk = false;
				locTemp = (char*)locWorkingBuffPtr;
			} else {
				//debugLog<<"Not the first chunk"<<endl;
				//debugLog<<"Skip bytes = "<<(int)locWorkingBuffPtr[0]<<(int)locWorkingBuffPtr[1]<<endl;
				locTemp = (char*)(locWorkingBuffPtr + 2);
				locWorkingBuffPtr+=2;
				locNumBytesLeft -= 2;
			}

	/*		if (mLeftOver != "") {
				debugLog<<"Sticking the leftovers back together..."<<endl;
				locTemp = mLeftOver + locTemp;
				mLeftOver = "";
			}*/

			size_t locChunkSizePos = locTemp.find("\r\n");
			
			
			if (locChunkSizePos != string::npos) {
				//debugLog<<"Found the size bytes "<<endl;
				//Get a string representation of the hex string that tells us the size of the chunk
				string locChunkSizeStr = locTemp.substr(0, locChunkSizePos);
				//debugLog<<"Sizingbuytes " << locChunkSizeStr<<endl;
				char* locDummyPtr = NULL;

				//Convert it to a number
				mChunkRemains = strtol(locChunkSizeStr.c_str(), &locDummyPtr, 16);

				//debugLog<<"Chunk reamining "<<mChunkRemains<<endl;
				//The size of the crlf 's and the chunk size value
				unsigned long locGuffSize = (unsigned long)(locChunkSizeStr.size() + 2);
				locWorkingBuffPtr +=  locGuffSize;
				locNumBytesLeft -= locGuffSize;
			} else {
				//debugLog<<"************************************** "<<endl;
			

			}
		}

		//This is the end of file
		if (mChunkRemains == 0) {
			//debugLog<<"EOF"<<endl;
			return;
		}

		//If theres less bytes than the remainder of the chunk
		if (locNumBytesLeft < mChunkRemains) {
			//debugLog<<"less bytes remain than the chunk needs"<<endl;
			
			mFileCache.write((const unsigned char*)locWorkingBuffPtr, locNumBytesLeft );
			fileDump.write((char*)locWorkingBuffPtr, locNumBytesLeft);
			locWorkingBuffPtr += locNumBytesLeft;
			mChunkRemains -= locNumBytesLeft;
			locNumBytesLeft = 0;
		} else {
			//debugLog<<"more bytes remain than the chunk needs"<<endl;
			mFileCache.write((const unsigned char*)locWorkingBuffPtr, mChunkRemains );
			fileDump.write((char*)locWorkingBuffPtr, mChunkRemains);
			locWorkingBuffPtr += mChunkRemains;
			locNumBytesLeft -= mChunkRemains;
			mChunkRemains = 0;
		}

	}

	if (locNumBytesLeft != 0) {
		//debugLog<<"There is a non- zero amount of bytes leftover... buffer them up for next time..."<<endl;
		memcpy((void*)mInterBuff, (const void*)locWorkingBuffPtr, locNumBytesLeft);
		mNumLeftovers = locNumBytesLeft;
	}
}
void HTTPFileSource::DataProcessLoop() {
	//debugLog<<"DataProcessLoop: "<<endl;
	int locNumRead = 0;
	char* locBuff = NULL;
	DWORD locCommand = 0;
	bool locSeenAny = false;
	debugLog<<"Starting dataprocessloop"<<endl;

	locBuff = new char[RECV_BUFF_SIZE];

	while(true) {
		if(CheckRequest(&locCommand) == TRUE) {
			debugLog<<"Thread Data Process loop received breakout signal..."<<endl;
			delete[] locBuff;
			return;
		}
		//debugLog<<"About to call recv"<<endl;
		locNumRead = recv(mSocket, locBuff, RECV_BUFF_SIZE, 0);
		//debugLog<<"recv complete"<<endl;
		if (locNumRead == SOCKET_ERROR) {
			int locErr = WSAGetLastError();
			debugLog<<"Socket error receiving - Err No = "<<locErr<<endl;
			mWasError = true;
			break;
		}

		if (locNumRead == 0) {
			debugLog<<"Read last bytes..."<<endl;
			mIsEOF = true;
			delete[] locBuff;
			return;
		}

		{//CRITICAL SECTION - PROTECTING BUFFER STATE
			CAutoLock locLock(mBufferLock);
			//debugLog <<"Num Read = "<<locNumRead<<endl;
			if (mSeenResponse) {
				//Add to buffer

				if (mIsChunked) {
					unChunk((unsigned char*)locBuff, locNumRead);
				} else {
					mFileCache.write((const unsigned char*)locBuff, locNumRead);
				}
			
				//Dump to file
				//fileDump.write(locBuff, locNumRead);
			} else {
				//if (!locSeenAny) {
				//	locSeenAny = true;
				//	//Start of response
				//	if (locBuff[0] != '2') {
				//		mWasError = true;
				//		delete[] locBuff;
				//		return;
				//	}
				//}
				string locTemp = locBuff;
				//debugLog<<"Binary follows... "<<endl<<locTemp<<endl;
				size_t locPos = locTemp.find("\r\n\r\n");
				if (locPos != string::npos) {
					//Found the break
					//debugLog<<"locPos = "<<locPos<<endl;
					mSeenResponse = true;
					mLastResponse = locTemp.substr(0, locPos);
					debugLog<<"HTTP Response:"<<endl;
					debugLog<<mLastResponse<<endl;

					unsigned short locResponseCode = getHTTPResponseCode(mLastResponse);

					mRetryAt = "";
					if (locResponseCode == 301) {
						size_t locLocPos = mLastResponse.find("Location: ");
						if (locLocPos != string::npos) {
							locLocPos += 10;
							size_t locEndPos = mLastResponse.find("\r", locLocPos);
							if (locEndPos != string::npos) {
								if (locEndPos > locLocPos) {
									mRetryAt = mLastResponse.substr(locLocPos, locEndPos - locLocPos);
									debugLog<<"Retry URL = "<<mRetryAt<<endl;
								}
							}
						}

						debugLog<<"Setting error to true"<<endl;
						mIsEOF = true;
						mWasError = true;
						//close();
					} else if (locResponseCode >= 300) {
						debugLog<<"Setting error to true"<<endl;
						mIsEOF = true;
						mWasError = true;
						//close();
					}

					if (locTemp.find("Transfer-Encoding: chunked") != string::npos) {
						mIsChunked = true;
					}

					char* locBuff2 = locBuff + locPos + 4;  //View only - don't delete.
					locTemp = locBuff2;

					if (mIsChunked) {
						if (locNumRead - locPos - 4 > 0) {
							unChunk((unsigned char*)locBuff2, locNumRead - locPos - 4);
						}
					} else {
                        //debugLog<<"Start of data follows"<<endl<<locTemp<<endl;
						if (locNumRead - locPos - 4 > 0) {
							mFileCache.write((const unsigned char*)locBuff2, (locNumRead - (locPos + 4)));
						}
					}
				}
			}
		} //END CRITICAL SECTION
	}

	delete[] locBuff;
}

unsigned short HTTPFileSource::getHTTPResponseCode(string inHTTPResponse)
{
	size_t locPos = inHTTPResponse.find(" ");
	if (locPos != string::npos) {
		string locCodeString = inHTTPResponse.substr(locPos + 1, 3);
		try {
			unsigned short locCode = (unsigned short)StringHelper::stringToNum(locCodeString);
			return locCode;
		} catch(...) {
			return 0;
		}
	} else {
		return 0;
	}
}
string HTTPFileSource::shouldRetryAt()
{
	return mRetryAt;
}

DWORD HTTPFileSource::ThreadProc(void) {
	//debugLog<<"ThreadProc:"<<endl;
	while(true) {
		DWORD locThreadCommand = GetRequest();
		
		switch(locThreadCommand) {
			case THREAD_EXIT:
				
				Reply(S_OK);
				return S_OK;

			case THREAD_RUN:
				
				Reply(S_OK);
				DataProcessLoop();
				break;
		}


	}
	return S_OK;
}
unsigned long HTTPFileSource::seek(unsigned long inPos) {
	//Close the socket down
	//Open up a new one to the same place.
	//Make the partial content request.
	//debugLog<<"Seeking to "<<inPos<<endl;
	if (mFileCache.readSeek(inPos)) {
		return inPos;
	} else {
		return (unsigned long) -1;
	}
	
}


void HTTPFileSource::close() {
	//Killing thread
	//debugLog<<"HTTPFileSource::close()"<<endl;
	if (ThreadExists() == TRUE) {
		//debugLog<<"Calling Thread to EXIT"<<endl;
		CallWorker(THREAD_EXIT);
		//debugLog<<"Killing thread..."<<endl;
		Close();
		//debugLog<<"After Close called on CAMThread"<<endl;
	}


	
	//debugLog<<"Closing socket..."<<endl;
	//Close the socket down.
	closeSocket();
}

bool HTTPFileSource::startThread() {
	if (ThreadExists() == FALSE) {
		Create();
	}
	CallWorker(THREAD_RUN);
	return true;
}
bool HTTPFileSource::open(string inSourceLocation) {
	//Open network connection and start feeding data into a buffer
	//
	mSeenResponse = false;
	mLastResponse = "";
	//debugLog<<"Open: "<<inSourceLocation<<endl;

	{ //CRITICAL SECTION - PROTECTING STREAM BUFFER
		CAutoLock locLock(mBufferLock);
		
		//Init rand number generator
		LARGE_INTEGER locTicks;
		QueryPerformanceCounter(&locTicks);
		srand((unsigned int)locTicks.LowPart);

		int locRand = rand();

		string locCacheFileName = getenv("TEMP");
		//debugLog<<"Temp = "<<locCacheFileName<<endl;
		locCacheFileName += "\\filecache";
		
		locCacheFileName += StringHelper::numToString(locRand);
		locCacheFileName += ".ogg";
		//debugLog<<"Cache file  = "<<locCacheFileName<<endl;
		if(mFileCache.open(locCacheFileName)) {
			//debugLog<<"OPEN : Cach file opened"<<endl;
		}
	} //END CRITICAL SECTION

	bool locIsOK = setupSocket(inSourceLocation);

	if (!locIsOK) {
		//debugLog<<"Setup socket FAILED"<<endl;
		closeSocket();
		return false;
	}

	//debugLog<<"Sending request..."<<endl;

	//How is filename already set ??
	httpRequest(assembleRequest(mFileName));
	//debugLog<<"Socket ok... starting thread"<<endl;
	locIsOK = startThread();


	return locIsOK;
}
void HTTPFileSource::clear() {
	//Reset flags.
	debugLog<<"Setting error to false";
	mIsEOF = false;
	mWasError = false;
	mRetryAt = "";
}
bool HTTPFileSource::isError()
{
	return mWasError;
}
bool HTTPFileSource::isEOF() {
	{ //CRITICAL SECTION - PROTECTING STREAM BUFFER
		CAutoLock locLock(mBufferLock);
		unsigned long locSizeBuffed = mFileCache.bytesAvail();;
	
		//debugLog<<"isEOF : Amount Buffered avail = "<<locSizeBuffed<<endl;
		if ((locSizeBuffed == 0) && mIsEOF) {
			//debugLog<<"isEOF : It is EOF"<<endl;
			return true;
		} else {
			//debugLog<<"isEOF : It's not EOF"<<endl;
			return false;
		}
	} //END CRITICAL SECTION

}
unsigned long HTTPFileSource::read(char* outBuffer, unsigned long inNumBytes) {
	//Reads from the buffer, will return 0 if nothing in buffer.
	// If it returns 0 check the isEOF flag to see if it was the end of file or the network is just slow.

	{ //CRITICAL SECTION - PROTECTING STREAM BUFFER
		CAutoLock locLock(mBufferLock);
		
		//debugLog<<"Read:"<<endl;
		if((mFileCache.bytesAvail() == 0) || mWasError) {
			//debugLog<<"read : Can't read is error or eof"<<endl;
			return 0;
		} else {
			debugLog<<"Reading from buffer"<<endl;
			
			unsigned long locNumRead = mFileCache.read((unsigned char*)outBuffer, inNumBytes);

			//debugLog<<locNumRead<<" bytes read from buffer"<<endl;
			return locNumRead;
		}
	} //END CRITICAL SECTION
}
