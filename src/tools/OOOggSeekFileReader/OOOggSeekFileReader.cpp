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
#if 0
		LOOG_INT64 timePoint;
		unsigned long bytePos;

		fstream seekFile;
		seekFile.open(argv[1], ios_base::in | ios_base::binary);
	
		unsigned char* buff = new unsigned char[16];
		unsigned long pointCount = 0;
		while (!seekFile.eof()) {
			//Read the time
			seekFile.read((char*)buff, 8);
			if (seekFile.gcount() == 8) {
				timePoint = iLE_Math::CharArrToInt64(buff);


				seekFile.read((char*)buff, 4);
				bytePos = iLE_Math::charArrToULong(buff);

				cout << "Seek point "<<pointCount<<" : Time = "<<timePoint<<", Byte Offset = "<<bytePos<<endl;
				pointCount++;
			}
		}

		delete [] buff;
#else
		AutoOggSeekTable *locSeekTable = new AutoOggSeekTable("foo");  // Filename doesn't matter
		locSeekTable->buildTableFromFile(argv[1]);
		OggSeekTable::tSeekMap locSeekMap = locSeekTable->getSeekMap();

		for (map<LOOG_INT64, unsigned long>::iterator i = locSeekMap.begin(); i != locSeekMap.end(); i++) {
			OggSeekTable::tSeekPair locSeekPair = *i;
			LOOG_UINT64 locTimePoint = locSeekPair.first;
			unsigned long locBytePosition = locSeekPair.second;
			cout << "Seek point: time " << locTimePoint << " at byte offset " << locBytePosition << endl;
		}

		delete [] locSeekTable;

#endif
	}
	return 0;
}
