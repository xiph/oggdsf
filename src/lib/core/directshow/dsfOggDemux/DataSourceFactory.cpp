#include "StdAfx.h"
#include "datasourcefactory.h"

DataSourceFactory::DataSourceFactory(void)
{
}

DataSourceFactory::~DataSourceFactory(void)
{
}

IFilterDataSource* DataSourceFactory::createDataSource(string inSourceLocation) {
	string locType = identifySourceType(inSourceLocation);

	if(locType.length() == 1) {
		//File...
		return new FilterFileSource;
	} else if (locType == "http") {
		//Http stream
		return NULL;
	} else {
		//Something else
		return NULL;
	}
}

string DataSourceFactory::identifySourceType(string inSourceLocation) {
	size_t locPos = inSourceLocation.find(':');
	if (locPos == string::npos) {
		//No colon... not a url or file... failure.
		return "";
	} else {
		string retStr = inSourceLocation.substr(0,locPos);
		return retStr;
	}
	
}