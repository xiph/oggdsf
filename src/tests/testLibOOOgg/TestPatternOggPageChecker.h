#pragma once

#include "IOggCallback.h"
#include "TestPatternOggPacketChecker.h"
class TestPatternOggPageChecker
	:	public IOggCallback
{
public:
	TestPatternOggPageChecker(void);
	~TestPatternOggPageChecker(void);

	virtual bool acceptOggPage(OggPage* inOggPage);

protected:
	TestPatternOggPacketChecker mPacketChecker;
};
