// testLibOOOgg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "TestPatternOggPacketGenerator.h"
#include "TestPatternOggPageChecker.h"


#include <iostream>
using namespace std;

bool testRandomPacketSizes(string inFileName, unsigned long inNumPackets) {

	TestPatternOggPacketGenerator locPackGen;
	TestPatternOggPageChecker locPageChecker;
	OggPaginator locPaginator;

	OggPaginatorSettings*  locSettings = new OggPaginatorSettings;
	;
	locSettings->mMaxPageSize = 20000;
	locSettings->mMinPageSize = 4096;
	locSettings->mTargetPageSize = 10000;
	locSettings->mSerialNo = 77;
	locSettings->mNumHeaders = 6;
	locPaginator.setParameters(locSettings);

	locPaginator.setPageCallback(&locPageChecker);

	const unsigned long MAX_PACK_SIZE = 100000;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locTestPacket;

	for (int i = 0; i < inNumPackets; i++) {
		locPacketSize = rand() % MAX_PACK_SIZE;
		locTestPacket = locPackGen.generatePacket(locPacketSize, i);
		locPaginator.acceptStampedOggPacket(locTestPacket);
	}
	locPaginator.finishStream();
	return true;

}


bool testSmallPacketSizes(string inFileName, unsigned long inNumPackets) {

	TestPatternOggPacketGenerator locPackGen;
	TestPatternOggPageChecker locPageChecker;
	OggPaginator locPaginator;

	OggPaginatorSettings*  locSettings = new OggPaginatorSettings;
	
	locSettings->mMaxPageSize = 20000;
	locSettings->mMinPageSize = 4096;
	locSettings->mTargetPageSize = 8192;
	locSettings->mSerialNo = 77;
	locSettings->mNumHeaders = 6;
	locPaginator.setParameters(locSettings);

	locPaginator.setPageCallback(&locPageChecker);

	const unsigned long MAX_PACK_SIZE = 200;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locTestPacket;

	for (int i = 0; i < inNumPackets; i++) {
		locPacketSize = rand() % MAX_PACK_SIZE;
		locTestPacket = locPackGen.generatePacket(locPacketSize, i);
		locPaginator.acceptStampedOggPacket(locTestPacket);
	}
	locPaginator.finishStream();
	return true;

}

//bool testPagination(string inFileName) {
//	TestPatternOggPacketGenerator locPackGen;
//	TestPatternOggPageChecker locPageChecker;
//	OggPaginator locPaginator;
//	locPaginator.setPageCallback(&locPageChecker);
//
//	for (int i = 0; i <
//
//}
int _tmain(int argc, _TCHAR* argv[])
{
	//testRandomPacketSizes("", 100);
	testSmallPacketSizes("", 10000);
	return 0;
}

