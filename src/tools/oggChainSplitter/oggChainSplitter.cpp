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

// oggChainSplitter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libOOOgg.h>
#include <dllstuff.h>
#include "VorbisComments.h"
#include <iostream>
#include <fstream>
#include <vector>

unsigned long bytePos = 0;
bool needComments = false;
bool inStream = false;
fstream outFile;
string inFileName;
string outFileName;
VorbisComments currentComment;
unsigned long chainCount = 0;
bool isOK = true;

bool renameCurrentFile() {
	cout<<"Renaming File..."<<endl;
	if ((currentComment.numUserComments() > 0)) {
		vector<SingleVorbisComment*> locArtists = currentComment.getCommentsByKey("artist");
		vector<SingleVorbisComment*> locTitles = currentComment.getCommentsByKey("title");
		string locArtist = "Unknown_Artist";
		string locTitle = "Unknown_Title";

		cout<<"Basics done..."<<endl;
		if (locArtists.size() > 0) {
			locArtist = locArtists[0]->value();
		}

		if (locTitles.size() > 0) {
			locTitle = locTitles[0]->value();
		}

		string locPartialName = locArtist+"_"+locTitle;

		size_t locSlashPos = outFileName.find_last_of('\\');
		if (locSlashPos == string::npos) {
			locSlashPos == -1;
		}
		size_t locDotPos = outFileName.find_last_of('.');

		cout<<"Before substitution..."<<endl;
		string locNewName = outFileName.substr(0,locSlashPos + 1) + locPartialName + outFileName.substr(locDotPos);
		cout<<"After subst."<<endl;
		cout<<"Renaming "<<outFileName<<" to "<<locNewName<<endl;
#ifdef WIN32
		MoveFile(outFileName.c_str(), locNewName.c_str());
#else  /* assume POSIX */
		rename(outFileName.c_str(), locNewName.c_str());
#endif                
	}

	return true;

}
bool writePage(OggPage* inOggPage) {
	//cout << "Writing page "<<endl;
	unsigned char* pageBuff = inOggPage->createRawPageData();
	//cout <<pageBuff<<endl;
	outFile.write((char*)pageBuff, inOggPage->pageSize());
	//cout<<"Written..."<<endl;
	delete[] pageBuff;
	//cout<<"After delete"<<endl;
	return true;
}
//This will be called by the callback
bool pageCB(OggPage* inOggPage) {
	bool retVal = false;
	if (inStream == false) {
		//Not in the middle of a stream
		if (inOggPage->header()->isBOS()) {
			//Case 1 : Not in the middle of a stream and we found a BOS... start a new file.
			char* locNum = new char[32];
                        sprintf(locNum, "%d", chainCount);
			// ^ is the same as: itoa(chainCount, locNum, 10);
			outFileName = inFileName + "__" + locNum + ".ogg";
			cout<<"New output file "<<outFileName<<endl;
		
			delete locNum;
			locNum = NULL;
			outFile.open(outFileName.c_str(), ios_base::out | ios_base::binary);
			retVal = writePage(inOggPage);
			inStream = retVal;
			needComments = true;
			
		} else {
			//Case 2 : Not in middle of stream and not BOS - ERROR
			cout << "BOS Page expected at "<<bytePos<<endl;
			retVal = false;
		}

	} else {
		//In the middle of a stream
		if (inOggPage->header()->isEOS()) {
			//Case 3 : It's the last page of the current stream... write it out and switch state.
			retVal = writePage(inOggPage);
			outFile.close();
			renameCurrentFile();
			cout<<"Closing file number "<<chainCount<<endl;
			chainCount++;
			inStream = false;
			
		} else {
			//Case 4 : It's just a normal page write it out.
			if (needComments) {
				currentComment.parseOggPacket(inOggPage->getPacket(0), 7);
			}
			retVal = writePage(inOggPage);
		}
	}

	isOK = retVal;
	delete inOggPage;
	return retVal;

}

#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{
	//This program just dumps the pages out of a file in ogg format.
	// Currently does not error checking. Check your command line carefully !
	// USAGE :: oggChainSplitter <OggFile>
	//

	bytePos = 0;

	if (argc < 2) {
		cout<<"Usage : oggChainSplitter <filename>"<<endl;
		cout<<"Only splits vorbis chained streams... careful with command line... no error checks"<<endl;
	} else {
		cout << "Starting..."<<endl;
		OggDataBuffer testOggBuff;
		
		testOggBuff.registerStaticCallback(&pageCB);

		fstream testFile;
		inFileName = argv[1];
		testFile.open(argv[1], ios_base::in | ios_base::binary);
		
		const unsigned short BUFF_SIZE = 8092;
		char* locBuff = new char[BUFF_SIZE];
		while ((!testFile.eof()) && (isOK)) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
		}

		delete[] locBuff;
	}

	return 0;
}

