#pragma once
#include "IFilterDataSource.h"
#include "FilterFileSource.h"
class DataSourceFactory
{
public:
	DataSourceFactory(void);
	~DataSourceFactory(void);

	static IFilterDataSource* createDataSource(string inSourceLocation);
	static string identifySourceType(string inSourceLocation);
};
