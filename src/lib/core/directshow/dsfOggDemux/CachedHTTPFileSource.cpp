#include "StdAfx.h"
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
void CachedHTTPFileSource::DataProcessLoop() {
	//This loop sits here filling in holes


	//WHILE still holes
	//	hole = findNextHoleInData();
	//	requestByteRange(hole)
	//WEND


}