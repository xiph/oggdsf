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
#include "httpfilesource.h"

HTTPFileSource::HTTPFileSource(void)
	:	mBufferLock(NULL)
{
	mBufferLock = new CCritSec;
	//debugLog.open("G:\\logs\\httpdebug.log", ios_base::out | ios_base::ate | ios_base::app);
	//debugLog<<"==========================================="<<endl;
	//fileDump.open("G:\\filedump.ogg", ios_base::out|ios_base::binary);




}

HTTPFileSource::~HTTPFileSource(void)
{
	//debugLog<<"About to close socket"<<endl;
	close();
	//debugLog<<"Winsock ended"<<endl;
	//debugLog.close();
	//fileDump.close();
	delete mBufferLock;
	
}

void HTTPFileSource::DataProcessLoop() {
	//debugLog<<"DataProcessLoop: "<<endl;
	int locNumRead = 0;
	char* locBuff = NULL;
	DWORD locCommand = 0;
	const unsigned long RECV_BUFF_SIZE = 1024;
	locBuff = new char[RECV_BUFF_SIZE];
	while(true) {
		if(CheckRequest(&locCommand) == TRUE) {
			//debugLog<<"Thread Data Process loop received breakout signal..."<<endl;
			delete[] locBuff;
			return;
		}
		//debugLog<<"About to call recv"<<endl;
		locNumRead = recv(mSocket, locBuff, RECV_BUFF_SIZE, 0);
		//debugLog<<"recv complete"<<endl;
		if (locNumRead == SOCKET_ERROR) {
			int locErr = WSAGetLastError();
			//debugLog<<"Socket error receiving - Err No = "<<locErr<<endl;
			mWasError = true;
			break;
		}

		if (locNumRead == 0) {
			//debugLog<<"Read last bytes..."<<endl;
			mIsEOF = true;
			delete[] locBuff;
			return;
		}

		{//CRITICAL SECTION - PROTECTING BUFFER STATE
			CAutoLock locLock(mBufferLock);
			//debugLog <<"Num Read = "<<locNumRead<<endl;
			if (mSeenResponse) {
				//Add to buffer
				bool locWriteOK = mFileCache.write((const unsigned char*)locBuff, locNumRead);
				if (locWriteOK) {
					//debugLog<<"Added to cache "<<locNumRead<<" bytes."<<endl;
				} else {
					//debugLog<<"Write to cache failed.."<<endl;
				}
				//Dump to file
				//fileDump.write(locBuff, locNumRead);
			} else {
				string locTemp = locBuff;
				//debugLog<<"Binary follows... "<<endl<<locTemp<<endl;
				size_t locPos = locTemp.find("\r\n\r\n");
				if (locPos != string::npos) {
					//Found the break
					//debugLog<<"locPos = "<<locPos<<endl;
					mSeenResponse = true;
					mLastResponse = locTemp.substr(0, locPos);
					char* locBuff2 = locBuff + locPos + 4;  //View only - don't delete.
					locTemp = locBuff2;
					//debugLog<<"Start of data follows"<<endl<<locTemp<<endl;
					bool locWriteOK = mFileCache.write((const unsigned char*)locBuff2, (locNumRead - (locPos + 4)));

					//Dump to file
					//fileDump.write(locBuff2, locNumRead - (locPos + 4));
					

					if(locWriteOK) {
						//debugLog<<"Added to Buffer "<<locNumRead - (locPos+4)<<" bytes... first after response."<<endl;
						
					} else {
						//debugLog<<"Buffering failure..."<<endl;
					}

					//size_t locG, locP;
					//locG = mStreamBuffer.tellg();
					//locP = mStreamBuffer.tellp();

					//debugLog << "Get : "<<locG<<" ---- Put : "<<locP<<endl;
					
				}
			}
		} //END CRITICAL SECTION
	}

	delete[] locBuff;


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
		//mStreamBuffer.flush();
		//mStreamBuffer.clear();
		//mStreamBuffer.seekg(0, ios_base::beg);
		//mStreamBuffer.seekp(0, ios_base::beg);

		//TODO::: Get rid of this path.
		
		//string locCacheFileName = "filecache.dat";
		
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
	mIsEOF = false;
	mWasError = false;
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
			//debugLog<<"Reading from buffer"<<endl;
			
			unsigned long locNumRead = mFileCache.read((unsigned char*)outBuffer, inNumBytes);
	/*		if (locNumRead == 0) {
				mStreamBuffer.clear();
			}*/

			//debugLog<<locNumRead<<" bytes read from buffer"<<endl;
			return locNumRead;
		}
	} //END CRITICAL SECTION
}
