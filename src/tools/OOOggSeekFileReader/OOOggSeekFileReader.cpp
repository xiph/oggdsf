// OOOggSeekFileReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "illicoreconfig.h"
#include "iLE_Math.h"
#include <iostream>
#include <fstream>
using namespace std;

#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{

	if (argc < 2) {
		cout << "Usage : OOOggSeekFileReader <seek_table_file>"<<endl;

	} else {
		LOOG_INT64 timePoint;
		unsigned long bytePos;

		fstream seekFile;
		seekFile.open(argv[1], ios_base::in | ios_base::binary);
	
		unsigned char* buff = new unsigned char[16];
		unsigned long pointCount = 0;
		while (!seekFile.eof()) {
			//Read the time
			seekFile.read((char*)buff, 8);
			timePoint = iLE_Math::CharArrToInt64(buff);

			//Read the byte offset
			seekFile.read((char*)buff, 4);
			bytePos = iLE_Math::charArrToULong(buff);

			cout << "Seek point "<<pointCount<<" : Time = "<<timePoint<<", Byte Offset = "<<bytePos<<endl;


		}

		delete buff;
	}
	return 0;
}

