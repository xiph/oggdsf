#pragma once
#include <string>

using namespace std;

class LIBCMMLTAGS_API C_CMMLTime
{
public:
	C_CMMLTime(void);
	~C_CMMLTime(void);

	//Accessors
	string time();

	//Mutators
	void setTime(string inTime);


//Temporarily just a string container
protected:
	string mTime;
};
