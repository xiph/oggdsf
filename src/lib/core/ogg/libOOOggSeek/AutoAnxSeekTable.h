#pragma once

#include "libOOOggSeek.h"

#include "AutoOggSeekTable.h"

#include <string>

using namespace std;

class LIBOOOGGSEEK_API AutoAnxSeekTable
	:	public AutoOggSeekTable
{
public:
	AutoAnxSeekTable(string inFileName);
	virtual ~AutoAnxSeekTable(void);

	//virtual bool buildTable();

	//IOggCallback interface
	virtual bool acceptOggPage(OggPage* inOggPage);

protected:
	unsigned long mAnxPackets;
	bool mSeenAnything;
	unsigned long mAnnodexSerialNo;
	bool mReadyForOgg;
	bool mSkippedCMML;

	
};
