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

// OOOggStat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "libOOOgg.h"
#include "dllstuff.h"
#include "OggPageFileWriter.h"
#include <iostream>


#include <fstream>

const BUFF_SIZE = 8092;


int testPagination(string inOutputFileName) {
	int NUM_PACKS = 100;
	int PACK_BUFF_SIZE = 100000;
	unsigned char* locBuff = NULL;


	OggPaginatorSettings sets;
	sets.mMaxPageSize = 8192;
	sets.mMinPageSize = 4096;
	sets.mSerialNo = 8877;
	sets.mTargetPageSize = 4096;

	OggPageFileWriter* writer = new OggPageFileWriter(inOutputFileName);

	OggPaginator pager;
	pager.setParameters(&sets);
	pager.setPageCallback(writer);
	
	StampedOggPacket* pack = NULL;
	
	for (int i=0; i < NUM_PACKS; i++) {
		locBuff = new unsigned char[	PACK_BUFF_SIZE];
		memset((void*)locBuff, 99, PACK_BUFF_SIZE);

		locBuff[0] = 88;
		locBuff[PACK_BUFF_SIZE-1] = 77;
		pack = new StampedOggPacket(locBuff, PACK_BUFF_SIZE, false, false, i, i, StampedOggPacket::OGG_BOTH);
		pager.acceptStampedOggPacket(pack);


	}
	return 0;
}


int testPageWriter(string inReadFile, string inWriteFile) {
	//Reads a file, demuxes it at page level and then writes it out again
	OggDataBuffer testOggBuff;

	//Set up the file writer and set the callback on the demux buffer
	OggPageFileWriter testOggPageWriter(inWriteFile);
	testOggBuff.registerVirtualCallback(&testOggPageWriter);

	//Source File read and let it all fire away.
	fstream testFile;
	testFile.open(inReadFile.c_str(), ios_base::in | ios_base::binary);
	char* locBuff = new char[BUFF_SIZE];
	while (!testFile.eof()) {
		testFile.read(locBuff, BUFF_SIZE);
		unsigned long locBytesRead = testFile.gcount();
    	testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
	}

	delete[] locBuff;

	return 0;
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{

	//if (argc != 3) {
	//	cout<<"Usage : testMuxDemux <inputFilename> <outputFilename>"<<endl;
	//	return 1;
	//} else {
		cout<<"Testing libOOOgg Mux and Demux..."<<endl;
		cout<<"================================="<<endl;
		unsigned long numTests = 0;

		testPagination("G:\\logs\\dumpage.out");
		//cout<<numTests<<" : "<<"Testing page level demux chained to page file writer..."<<endl;
		//testPageWriter(argv[1], argv[2]);
		//cout<<numTests<<" : "<<"Complete."<<endl;
		return 0;
	//}
}


