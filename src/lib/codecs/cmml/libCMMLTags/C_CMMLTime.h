#pragma once
#include <string>

using namespace std;

class LIBCMMLTAGS_API C_CMMLTime
{
public:
	C_CMMLTime(void);
	~C_CMMLTime(void);

	//Accessors
	wstring time();

	//Mutators
	void setTime(wstring inTime);


//Temporarily just a string container
protected:
	wstring mTime;
};
