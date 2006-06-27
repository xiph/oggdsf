// OOOggSeekFileReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libilliCore/illicoreconfig.h>
#include <libilliCore/iLE_Math.h>

#include <libOOOggSeek/AutoOggSeekTable.h>

#include <iostream>
#include <fstream>

using namespace std;

#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{

	if (argc < 2) {
		cout << "Usage : OOOggSeekFileReader <seek_table_file>"<<endl;

	} else {
		AutoOggSeekTable *locSeekTable = new AutoOggSeekTable(TEXT("foo"));  // Filename doesn't matter
		locSeekTable->buildTableFromFile(argv[1]);
		OggSeekTable::tSeekMap locSeekMap = locSeekTable->getSeekMap();

		for (map<LOOG_INT64, unsigned long>::iterator i = locSeekMap.begin(); i != locSeekMap.end(); i++) {
			OggSeekTable::tSeekPair locSeekPair = *i;
			LOOG_UINT64 locTimePoint = locSeekPair.first;
			unsigned long locBytePosition = locSeekPair.second;
			cout << "Seek point: time " << locTimePoint << " at byte offset " << locBytePosition << endl;
		}

		delete [] locSeekTable;
	}
	return 0;
}
