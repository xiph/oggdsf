// testLibOOOgg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "TestPatternOggPacketGenerator.h"
#include "TestPatternOggPageChecker.h"
#include "OggPageFileWriter.h"


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



bool test255MultPacketSizes(string inFileName, unsigned long inNumPackets) {

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

	const unsigned long MAX_MULTIPLE_SIZE = 100;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locTestPacket;

	for (int i = 0; i < inNumPackets; i++) {
		locPacketSize = (rand() % MAX_MULTIPLE_SIZE) * 255;
		locTestPacket = locPackGen.generatePacket(locPacketSize, i);
		locPaginator.acceptStampedOggPacket(locTestPacket);
	}
	locPaginator.finishStream();
	return true;

}



bool test255MultPacketSizesToFile(string inFileName, unsigned long inNumPackets) {

	TestPatternOggPacketGenerator locPackGen;
	//TestPatternOggPageChecker locPageChecker;
	OggPageFileWriter locFileWriter(inFileName);
	
	OggPaginator locPaginator;

	OggPaginatorSettings*  locSettings = new OggPaginatorSettings;
	
	locSettings->mMaxPageSize = 20000;
	locSettings->mMinPageSize = 4096;
	locSettings->mTargetPageSize = 8192;
	locSettings->mSerialNo = 77;
	locSettings->mNumHeaders = 6;
	locPaginator.setParameters(locSettings);

	locPaginator.setPageCallback(&locFileWriter);

	const unsigned long MAX_MULTIPLE_SIZE = 100;
	unsigned long locPacketSize = 0;
	StampedOggPacket* locTestPacket;

	for (int i = 0; i < inNumPackets; i++) {
		locPacketSize = (rand() % MAX_MULTIPLE_SIZE) * 255;
		locTestPacket = locPackGen.generatePacket(locPacketSize, i);
		locPaginator.acceptStampedOggPacket(locTestPacket);
	}
	locPaginator.finishStream();
	return true;

}

int _tmain(int argc, _TCHAR* argv[])
{
	int x;
	cin>>x;
	//testRandomPacketSizes("", 100);
	//testSmallPacketSizes("", 10000);
	test255MultPacketSizesToFile("d:\\Zen\\tests\\test.ogg", 500);
	return 0;
}

