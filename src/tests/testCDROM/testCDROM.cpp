// testCDROM.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
#include "CDROM.h"
#include <string>
#include <iostream>
using namespace std;
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	CDROM testCD;

	string locDrive = "h:";
	int locRes = testCD.initDrive(locDrive);

	locRes = testCD.readTOC();

	CDROM_TOC* locTOC = testCD.getTOC();

	cout<<"Drive "<<locDrive<<endl;
	cout<<"Header Length  - "<< locTOC->Length<<endl;
	cout<<"Num Tracks     - "<<locTOC->LastTrack - locTOC->FirstTrack<<endl;
	DISK_GEOMETRY* locGeom = testCD.getDiskGeom();

	
	cout<<"---------------------------"<<endl;
	cout<<"Bytes/Sector   - "<<locGeom->BytesPerSector<<endl;
	cout<<"Num. Cyylinders- "<<locGeom->Cylinders.QuadPart<<endl;
	cout<<"Sectors/Track  - "<<locGeom->SectorsPerTrack<<endl;
	cout<<"Track/Cyl      - "<<locGeom->TracksPerCylinder<<endl;
	cout<<"Media type     - "<<locGeom->MediaType<<endl;
	cout<<"---------------------------"<<endl;
	
	unsigned long locAddress = 0;
	for (int i = locTOC->FirstTrack - 1; i < locTOC->LastTrack; i++) {
		cout<<"Track "<<i+1<<endl;
		cout<<"======="<<endl;
		locAddress = 0;
		for (int j = 0; j < 4; j++) {
			locAddress <<= 8;
			locAddress += locTOC->TrackData[i].Address[j];
			
		}
		cout<<"Adress     - "<<locAddress<<endl;
		cout<<"Adr        - "<<(int)locTOC->TrackData[i].Adr<<endl;
		cout<<"Control    - "<<(int)locTOC->TrackData[i].Control<<endl;
		cout<<"Track No   - "<<(int)locTOC->TrackData[i].TrackNumber<<endl<<endl;
	}



	int x;
	cin >> x;

	return 0;
}

