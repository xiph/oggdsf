#pragma once

#include "OggPageInterleaver.h"
class AnxPageInterleaver
	:	public OggPageInterleaver
{
public:
	AnxPageInterleaver(unsigned long inVersionMajor, unsigned long inVersionMinor);
	virtual ~AnxPageInterleaver(void);

	
	virtual void processData();


protected:
	bool mIsAnxSetup;
	
	unsigned long mVersionMajor;
	unsigned long mVersionMinor;
};
