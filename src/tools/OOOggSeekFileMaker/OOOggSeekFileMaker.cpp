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
#include <libOOOggSeek/AutoOggSeekTable.h>
#include <libOOOggSeek/AutoAnxSeekTable.h>


#include <iostream>
#include <fstream>

#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{
	int x;
	cin >> x;


	if (argc < 3) {
		cout<<"Usage : OOOggSeekFileMaker <in_ogg_file> <out_seek_table_file>"<<endl;
	} else {

		string inFileName = argv[1];
		AutoOggSeekTable* locSeekTable = NULL;
		if (inFileName.find(".anx") != string::npos) {
			locSeekTable = new AutoAnxSeekTable(argv[1]);
		} else {
			locSeekTable = new AutoOggSeekTable(argv[1]);
		}
		locSeekTable->buildTable();

		fstream outputFile;
		outputFile.open(argv[2], ios_base::out | ios_base::binary);

		unsigned char* locBuff = new unsigned char[locSeekTable->serialisedSize()];
		locSeekTable->serialiseInto(locBuff, locSeekTable->serialisedSize());

		outputFile.write((char*)locBuff, locSeekTable->serialisedSize());

		outputFile.close();

		delete [] locBuff;
		delete locSeekTable;
	}

	return 0;
}

