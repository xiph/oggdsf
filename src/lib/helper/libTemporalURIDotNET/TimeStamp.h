#pragma once


//Original public members

	//enum eTimeStampType {
	//	TS_NONE,
	//	TS_NPT_SECS,
	//	TS_NPT_FULL,
	//	TS_SMPT
	//};

	//struct sFourPartTime {
	//	LOOG_UINT64 hours;
	//	short minutes;
	//	short seconds;
	//	LOOG_UINT64 partials;
	//};
	//bool parseTimeStamp(double inTimeStampInSeconds);
	//bool parseTimeStamp(string inTimeStamp);

	//string toString();
	//LOOG_INT64 toHunNanos();
#pragma unmanaged
#include <libTemporalURI/libTemporalURI.h>
#include <libTemporalURI/C_TimeStamp.h>
#include <libilliCore/StringHelper.h>

#pragma managed
using namespace System;
#using "libiWrapper.dll"
using namespace illiminable::libiWrapper;






namespace illiminable {
namespace libTemporalURIDotNET {
public __gc class TimeStamp
{
public:
	TimeStamp(void);
	~TimeStamp(void);
	Int64 toHunNanos();
	bool parseTimeStamp(String* inTimeStamp);

protected:
	C_TimeStamp* mTimeStamp;
};

}
}
