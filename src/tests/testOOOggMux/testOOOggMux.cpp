// testOOOggMux.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libOOOgg.h>
#include <dllstuff.h>

#include <iostream>
#include <fstream>

unsigned long bytePos;

//This will be called by the callback
bool pageCB(OggPage* inOggPage) {
	cout<<"Page Location : "<<bytePos;
	bytePos += inOggPage->pageSize();
	cout<<" to "<<bytePos<<endl;
	OggPacket* locPack = NULL;
	cout << inOggPage->header()->toString();
	cout << "Num Packets : " << inOggPage->numPackets() << endl;;
	for (unsigned long i = 0; i < inOggPage->numPackets(); i++) {
		locPack = inOggPage->getPacket(i);
		cout << "------ Packet  " << i << " (" << locPack->packetSize() << " bytes) -------";
		
		if (locPack->isContinuation()) {
			cout<<"  ** CONT **";
		}
		if (locPack->isTruncated()) {
			cout<<" ** TRUNC **";
		}
		cout<<endl;
		cout << locPack->toPackDumpString();
	}
	
	return true;
}


int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	//This program just dumps the pages out of a file in ogg format.
	// Currently does not error checking. Check your command line carefully !
	// USAGE :: OggDump <OggFile>
	//

	bytePos = 0;

	
	
	

		
		const unsigned short BUFF_SIZE = 8092;
		char* locBuff = new char[BUFF_SIZE];
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
		}

		delete[] locBuff;
	}

	return 0;
}

