#pragma once
#include "oggseektable.h"

class OggBinarySeeker :
	public OggSeekTable
{
public:
	OggBinarySeeker(void);
	virtual ~OggBinarySeeker(void);

	tSeekPair getStartPos(__int64 inTime);
};
