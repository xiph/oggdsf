//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

// OggDump.cpp : Defines the entry point for the console application.
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
		
		if (!locPack->isComplete()) {
			cout<<"  ** INCOMPLETE **";
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

	if (argc < 2) {
		cout<<"Usage : OOOggDump <filename>"<<endl;
	} else {
		OggDataBuffer testOggBuff;
		
		testOggBuff.registerStaticCallback(&pageCB);

		fstream testFile;
		testFile.open(argv[1], ios_base::in | ios_base::binary);
		
		const unsigned short BUFF_SIZE = 8092;
		char* locBuff = new char[BUFF_SIZE];
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
		}

		delete locBuff;
	}

	return 0;
}

