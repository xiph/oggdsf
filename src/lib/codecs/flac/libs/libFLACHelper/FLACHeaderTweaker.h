#pragma once
#include "dllstuff.h"
#include "OggPacket.h"
#include <vector>
#include <fstream>
using namespace std;
class FLACHeaderTweaker
{
public:
	FLACHeaderTweaker(void);
	~FLACHeaderTweaker(void);

	enum eFLACAcceptHeaderResult {
		HEADER_ACCEPTED = 0,
		LAST_HEADER_ACCEPTED = 1,
		HEADER_ERROR = 100,
		ALL_HEADERS_ALREADY_SEEN = 101
	};

	 eFLACAcceptHeaderResult acceptHeader(OggPacket* inHeader);

	 unsigned long numNewHeaders();
	 OggPacket* getHeader(unsigned long inHeaderNo);
protected:
	 bool createNewHeaderList();
	 void deleteOldHeaders();
	 void deleteNewHeaders();


	vector<OggPacket*> mOldHeaderList;
	vector<OggPacket*> mNewHeaderList;


	fstream debugLog;
	bool mSeenAllHeaders;
};
