#include "StdAfx.h"
#include ".\timestamp.h"

namespace illiminable {
namespace libTemporalURIDotNET {
TimeStamp::TimeStamp(void)
{
	mTimeStamp = new C_TimeStamp();
}

TimeStamp::~TimeStamp(void)
{
	delete mTimeStamp;
	mTimeStamp = NULL;
}


bool TimeStamp::parseTimeStamp(String* inTimeStamp) {

	delete mTimeStamp;
	mTimeStamp = new C_TimeStamp();

	char* locCStr = Wrappers::netStrToCStr(inTimeStamp);
	string locStr = locCStr;

	bool retVal = mTimeStamp->parseTimeStamp(locStr);
	Wrappers::releaseCStr(locCStr);
	return retVal;
	
}

Int64 TimeStamp::toHunNanos() {
	Int64 locTime = mTimeStamp->toHunNanos();
	return locTime;
}

}

}