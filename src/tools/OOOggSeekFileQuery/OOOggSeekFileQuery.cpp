// OOOggSeekFileQuery.cpp : Defines the entry point for the console application.
//
// OOOggSeekFileReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libilliCore/illicoreconfig.h>
#include <libilliCore/iLE_Math.h>
#include <libilliCore/StringHelper.h>
#include <libOOOggSeek/OggSeekTable.h>
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
		cout << "Usage : OOOggSeekFileQuery <seek_table_file>"<<endl;

	} else {
		LOOG_INT64 timePoint;
		unsigned long bytePos;

		fstream seekFile;
		seekFile.open(argv[1], ios_base::in | ios_base::binary);
	
		unsigned char* buff = new unsigned char[16];
		unsigned long pointCount = 0;

		OggSeekTable seekTable;
		while (!seekFile.eof()) {
			//Read the time
			seekFile.read((char*)buff, 8);
			if (seekFile.gcount() == 8) {
				timePoint = iLE_Math::CharArrToInt64(buff);


				seekFile.read((char*)buff, 4);
				bytePos = iLE_Math::charArrToULong(buff);

				seekTable.addSeekPoint(timePoint, bytePos);
				pointCount++;
			}
		}

		delete [] buff;

		cout<<"Constructed seek table with "<<pointCount<<" points."<<endl;
		cout<<endl;
		string query = "";
		LOOG_INT64 queryInt;
		OggSeekTable::tSeekPair response;
		while (true) {
			cout<<"Enter a time : ";
			cin>>query;
			
			if (query == "") {
				break;
			}

			queryInt = StringHelper::stringToNum(query);
			cout<<endl;
			response = seekTable.getStartPos(queryInt);
			cout<<"Query - "<<queryInt<<" : closest time  "<<response.first<<" maps to byte position "<<response.second<<endl;

		}
	}
	return 0;
}
