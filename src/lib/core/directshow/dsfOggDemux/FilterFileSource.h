#pragma once

#include <fstream>

using namespace std;
#include "IFilterDataSource.h"
class FilterFileSource
	:	public IFilterDataSource
{
public:
	FilterFileSource(void);
	~FilterFileSource(void);

	//IFilterDataSource Interface
	virtual unsigned long seek(unsigned long inPos);
	virtual void close();
	virtual bool open(string inSourceLocation);
	virtual void clear();
	virtual bool isEOF();
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);
	//

protected:
	fstream mSourceFile;
};
