//===========================================================================
//Copyright (C) 2004, 2005 Zentaro Kavanagh
//
//Copyright (C) 2004, 2005 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
