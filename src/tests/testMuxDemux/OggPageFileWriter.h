#pragma once

//#include <libOOOgg/dllstuff.h>
#include <libOOOgg/libOOOgg.h>
#include <fstream>
using namespace std;
//#include <libOOOgg/IOggCallback.h>
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
