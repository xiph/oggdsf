#pragma once

#include <libOOOggChef/libOOOggChef.h>

#include <string>
#include <vector>

using namespace std;

class LIBOOOGGCHEF_API IRecomposer
{
public:
	IRecomposer(void);
	virtual ~IRecomposer(void);

	virtual void recomposeStreamFrom(double inStartingTimeOffset, const vector<const string>* inWantedMIMETypes) = 0;

};
