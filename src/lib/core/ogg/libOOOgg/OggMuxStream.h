#pragma once

#include "OggPage.h"
#include "IOggCallback.h"
#include <deque>
using namespace std;
class OggMuxStream
	:	public IOggCallback
{
public:
	OggMuxStream(void);
	virtual ~OggMuxStream(void);
	static const __int64 INT64_MAX = 9223372036854775807;

	virtual bool acceptOggPage(OggPage* inOggPage);
	virtual OggPage* popFront();
	virtual const OggPage* peekfront();
	virtual __int64 frontTime();
	virtual bool isEmpty();
	virtual bool isEOS();
	virtual bool isProcessable();
	virtual void setIsEOS(bool inIsEOS);
	
protected:
	bool mIsEOS;
	deque<OggPage*> mPageQueue;

};
