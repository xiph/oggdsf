// CMMLRip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	if ((argc != 3) || (argc != 4)) {
		cout << "Usage : CMMLRip <Annodex File> <CMML File> [<Ogg File>>]"<<endl;
		return 1;
	}

	string locAnxFileName = argv[1];
	string locCMMLFileName = argv[2];
	string locOggFileName = "";
	bool locKeepOgg = false;
	if (argc == 4) {
		locOggFileName = argv[3];
		locKeepOgg = true;
	}

	fstream locAnxFile;
	locAnxFile.open(locAnxFileName, ios_base::in|ios_base::binary);
	if (!locAnxFile.is_open()) {
		cout<<"Cannot open annodex file ("<<locAnxFileName<<")"<<endl;
		return 2;
	}

	fstream locCMMLFile;
	locCMMLFile.open(locCMMLFileName, ios:base::out|ios_base::binary);


	return 0;
}

