#pragma once
#include "IOggCallback.h"
#include "OggMuxStream.h"
#include <vector>
using namespace std;
class OggPageInterleaver
{
public:

	OggPageInterleaver(IOggCallback* inFileWriter);
	virtual ~OggPageInterleaver(void);

	virtual OggMuxStream* newStream();
	virtual bool isProcessable();
	virtual bool isAllEOS();
	virtual bool isAllEmpty();
	virtual void processData();

	virtual void writeLowest();

protected:
	vector<OggMuxStream*> mInputStreams;
	IOggCallback* mFileWriter;
};
