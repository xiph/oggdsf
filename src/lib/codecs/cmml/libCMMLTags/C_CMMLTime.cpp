#include "StdAfx.h"
#include ".\c_cmmltime.h"

C_CMMLTime::C_CMMLTime(void)
{
}

C_CMMLTime::~C_CMMLTime(void)
{
}

//Accessors
string C_CMMLTime::time() {
	return mTime;
}

//Mutators
void C_CMMLTime::setTime(string inTime) {
	mTime = inTime;
}
