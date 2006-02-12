#pragma once



#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>

#include <iostream>
#include <fstream>
class OggFileWriter
	:	public IOggCallback
	,	public INotifyComplete
{
public:
	OggFileWriter(string inFilename);
	virtual ~OggFileWriter(void);

	virtual bool acceptOggPage(OggPage* inOggPage);
	virtual void NotifyComplete();

private:
	fstream mFileOut;
};
