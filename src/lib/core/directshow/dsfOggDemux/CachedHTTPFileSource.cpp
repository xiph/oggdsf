#include "stdafx.h"
#include ".\cachedhttpfilesource.h"

CachedHTTPFileSource::CachedHTTPFileSource(void)

{
}

CachedHTTPFileSource::~CachedHTTPFileSource(void)
{
}

CachedHTTPFileSource::tMapEntry CachedHTTPFileSource::findNextHoleInData(__int64 inUpto)
{

	tRangeMap::iterator locIt = mRangeMap.upper_bound(inUpto);

	//Get the entry correspondingto this value.
	// The decrement is because of the stupid way the bound works in map.
	// The decrement of upper bound gives what you would expect a lower_bound function *should*
	//  return... ie the thing lower than the given key.
	tMapEntry locEntry = *(--locIt);
	tMapEntry locNextEntry = *(++locIt);

	//If we are in range, then we already have this
	if (inRange(locEntry, inUpto)) {
		bool locDone = false;
		
		while (locIt != mRangeMap.end()) {
			//If the end of this range equals the start of the next range, then there is no hole here !
			if (locEntry.second.first == (locNextEntry.first)) {
				locEntry = *(locIt);
				locNextEntry = *(++locIt);
			
			} else {
				//There is a hole... since the end value of the current range is not the same as the start of the next range
				// So there is a hole from here.end+1 to next.start - 1
				tMapEntry retEntry;
				retEntry.first = locEntry.second.first + 1;
				retEntry.second.first = - 1;		//The end becomes -1 until something goes in
				return retEntry;

			}
		}
	} else {
		//This upto point is not in a known range.
		tMapEntry retEntry;
		retEntry.first = locEntry.second.first + 1;
		retEntry.second.first = - 1;			//The end becomes -1 until something goes in
		return retEntry;

	}
}

bool CachedHTTPFileSource::startThread() {
	if (ThreadExists() == FALSE) {
		Create();
	}
	CallWorker(THREAD_RUN);
	return true;
}

DWORD CachedHTTPFileSource::ThreadProc(void) {
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
bool CachedHTTPFileSource::inRange(CachedHTTPFileSource::tMapEntry inTestRange, __int64 inTestValue) {

	return ((inTestRange.first <= inTestValue) && (inTestRange.second.first >= inTestValue));

	
}

unsigned long CachedHTTPFileSource::seek(unsigned long inPos) {
	//Close the socket down
	//Open up a new one to the same place.
	//Make the partial content request.
	//debugLog<<"Seeking to "<<inPos<<endl;
	if (mReadFile.readSeek(inPos)) {
		return inPos;
	} else {
		return (unsigned long) -1;
	}
	
}

unsigned long CachedHTTPFileSource::read(char* outBuffer, unsigned long inNumBytes) {
	//Reads from the buffer, will return 0 if nothing in buffer.
	// If it returns 0 check the isEOF flag to see if it was the end of file or the network is just slow.

	{ //CRITICAL SECTION - PROTECTING STREAM BUFFER
		CAutoLock locLock(mBufferLock);
		
		//debugLog<<"Read:"<<endl;
		if((mReadFile.bytesAvail() == 0) || mWasError) {
			//debugLog<<"read : Can't read is error or eof"<<endl;
			return 0;
		} else {
			//debugLog<<"Reading from buffer"<<endl;
			
			unsigned long locNumRead = mReadFile.read((unsigned char*)outBuffer, inNumBytes);
	/*		if (locNumRead == 0) {
				mStreamBuffer.clear();
			}*/

			//debugLog<<locNumRead<<" bytes read from buffer"<<endl;
			return locNumRead;
		}
	} //END CRITICAL SECTION
}

bool CachedHTTPFileSource::open(string inSourceLocation) {
	//Open network connection and start feeding data into a buffer
	//
	// In sourcelocation is a http url
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
		
		//*****************************************************
		//TODO::: Need to do something about the filename...
		//*****************************************************
		locCacheFileName += StringHelper::numToString(locRand);
		locCacheFileName += ".ogg";
		//debugLog<<"Cache file  = "<<locCacheFileName<<endl;
		if(mReadFile.open(locCacheFileName)) {
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

void CachedHTTPFileSource::clear() {
	//Reset flags.
	mIsEOF = false;
	mWasError = false;
}
bool CachedHTTPFileSource::isEOF() {
	//{ //CRITICAL SECTION - PROTECTING STREAM BUFFER
	//	CAutoLock locLock(mBufferLock);
	//	//TODO::: 
	//	unsigned long locSizeBuffed = mFileCache.bytesAvail();;
	//

	//	//debugLog<<"isEOF : Amount Buffered avail = "<<locSizeBuffed<<endl;
	//	if ((locSizeBuffed == 0) && mIsEOF) {
	//		//debugLog<<"isEOF : It is EOF"<<endl;
	//		return true;
	//	} else {
	//		//debugLog<<"isEOF : It's not EOF"<<endl;
	//		return false;
	//	}
	//} //END CRITICAL SECTION
	return false;
}
void CachedHTTPFileSource::close() {
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
void CachedHTTPFileSource::DataProcessLoop() {
	//This loop sits here filling in holes


	//WHILE still holes
	//	hole = findNextHoleInData();
	//	requestByteRange(hole)
	//WEND


}