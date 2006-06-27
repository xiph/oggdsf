#include "StdAfx.h"
#include "CustomOggChainGranuleSeekTable.h"

CustomOggChainGranuleSeekTable::CustomOggChainGranuleSeekTable(IFilterDataSource* inDataSource)
	:	AutoOggChainGranuleSeekTable(TEXT(""))
	,	mCustomSource(inDataSource)
{

}

CustomOggChainGranuleSeekTable::~CustomOggChainGranuleSeekTable(void)
{
}

bool CustomOggChainGranuleSeekTable::buildTable()
{
	//if (mFilename.find("http") != 0) {
	//	
	//	//mSeekMap.clear();
	//	//addSeekPoint(0, 0);

	//	mFile.open(mFilename.c_str(), ios_base::in | ios_base::binary);
	//	//TODO::: Error check
	//	const unsigned long BUFF_SIZE = 4096;
	//	unsigned char* locBuff = new unsigned char[BUFF_SIZE];		//Deleted this function.
	//	while (!mFile.eof()) {
	//		mFile.read((char*)locBuff, BUFF_SIZE);
	//		mOggDemux->feed((const unsigned char*)locBuff, mFile.gcount());
	//	}
	//	delete[] locBuff;

	//	mFile.close();
	//	mIsEnabled = true;
	//	
	//} else {
	//	mIsEnabled = false;
	//}
	//return true;

	mCustomSource->seek(0);

	if (mCustomSource->isError() || mCustomSource->isEOF()) {
		return false;
	} else {
		const unsigned long BUFF_SIZE = 4096;
		unsigned char* locBuff = new unsigned char[BUFF_SIZE];		//Deleted this function.
		unsigned long locReadCount = 0;
		while (!mCustomSource->isEOF()) {
			locReadCount = mCustomSource->read((char*)locBuff, BUFF_SIZE);
			mOggDemux->feed((const unsigned char*)locBuff, locReadCount);
		}
		delete[] locBuff;


		mIsEnabled = true;	
	
		mCustomSource->clear();
		mCustomSource->seek(0);
		return true;
	}

}