#pragma once

//#include "dllstuff.h"
#include "libOOOgg.h"
#include <fstream>
using namespace std;
//#include "IOggCallback.h"
class OggPageFileWriter
	:	public IOggCallback
{
public:
	OggPageFileWriter(string inFileName);
	~OggPageFileWriter(void);

	virtual bool acceptOggPage(OggPage* inOggPage);

protected:
	fstream mFile;
};
