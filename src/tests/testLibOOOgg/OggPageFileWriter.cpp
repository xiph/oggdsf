#include "StdAfx.h"
#include ".\oggpagefilewriter.h"

OggPageFileWriter::OggPageFileWriter(string inFileName)
{
	mFile.open(inFileName.c_str(), ios_base::out|ios_base::binary);
}

OggPageFileWriter::~OggPageFileWriter(void)
{
	mFile.close();
}

bool OggPageFileWriter::acceptOggPage(OggPage* inOggPage) {

	unsigned char* locPageBytes = inOggPage->createRawPageData();
	mFile.write((const char*) locPageBytes, inOggPage->pageSize());

	delete inOggPage;
	delete [] locPageBytes;
	return true;
}