#include "StdAfx.h"
#include ".\oggfilewriter.h"

OggFileWriter::OggFileWriter(string inFilename)
{
	mFileOut.open(inFilename.c_str(), ios_base::out|ios_base::binary);
}

OggFileWriter::~OggFileWriter(void)
{
	mFileOut.close();
}

bool OggFileWriter::acceptOggPage(OggPage* inOggPage)
{
	unsigned char* locData = inOggPage->createRawPageData();
	mFileOut.write((char*)locData, inOggPage->pageSize());
	delete[] locData;
	return true;
}
void OggFileWriter::NotifyComplete()
{
	mFileOut.close();
}
