#pragma once

#include <libilliCore/StringHelper.h>
#include <libTemporalURI/libTemporalURI.h>

#include <string>

using namespace std;

class LIBTEMPORALURI_API C_TimeStamp
{
public:
	C_TimeStamp(void);
	
	virtual ~C_TimeStamp(void);


	enum eTimeStampType {
		TS_NONE,
		TS_NPT_SECS,
		TS_NPT_FULL,
		TS_SMPT
	};

	struct sFourPartTime {
		LOOG_UINT64 hours;
		short minutes;
		short seconds;
		LOOG_UINT64 partials;
	};
	bool parseTimeStamp(double inTimeStampInSeconds);
	bool parseTimeStamp(string inTimeStamp);

	string toString();
	LOOG_INT64 toHunNanos();
	

protected:
	

	bool parseNPT(string inTimeStamp, sFourPartTime* inFPT);
	bool parseThreePartTime(string inTimeStamp, sFourPartTime* inFPT, string* outLeftOver);
	bool parseSMPT(string inTimeStamp, sFourPartTime* inFPT);
	bool parseSecsOnly(string inTimeStamp);

	string mTimeStamp;

	sFourPartTime mFPT;
	eTimeStampType mStampType;

	LOOG_UINT64 mSecs;
	LOOG_UINT64 mHuns;





};
