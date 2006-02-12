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

// OOOggPageInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>

#include <iostream>
#include <fstream>

struct sOggStreamInfo {
	unsigned long mSerialNo;
	unsigned long mNumPages;
	unsigned long mMinPageSize;
	unsigned long mMaxPageSize;
	unsigned long mMinPacksPerPage;
	unsigned long mMaxPacksPerPage;
	unsigned long mTotalPacketCount;
	unsigned long mTotalStreamSize;
	unsigned long mTotalDataSize;

};

void dumpStreamInfo(sOggStreamInfo* inInfo)
{

	cout<<"=== Stream "<<inInfo->mSerialNo<<endl;
	cout<<"=================="<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;

	cout<<"Min Page Size  :  "<<inInfo->mMinPageSize<<endl;
	cout<<"Max Page Size  :  "<<inInfo->mMaxPageSize<<endl;
	cout<<"Avg Page Size  :  "<<inInfo->mTotalStreamSize / inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl;
	cout<<"Num Pages      :  "<<inInfo->mNumPages<<endl<<endl;

}
unsigned long bytePos;
//vector<unsigned long> streamSerials;
//vector<unsigned long*> maxPacks;
vector<sOggStreamInfo*> streamInfos;

//This will be called by the callback
bool pageCB(OggPage* inOggPage, void* inUserData /* ignored */) {
	bool locFoundStream = false;
	size_t locFoundPos = 0;

	unsigned long locSerialNo = inOggPage->header()->StreamSerialNo();
	for (size_t i = 0; i < streamInfos.size(); i++) {
		if (locSerialNo == streamInfos[i]->mSerialNo) {
			locFoundStream = true;
			locFoundPos = i;
			break;
		}
	}
	
	if (!locFoundStream) {
		//streamSerials.push_back(locSerialNo);
		//maxPacks.push_back(new unsigned long(0));
		sOggStreamInfo* locStreamInfo = new sOggStreamInfo;
		locStreamInfo->mMaxPacksPerPage = 0;
		locStreamInfo->mMaxPageSize = 0;
		locStreamInfo->mMinPacksPerPage = (unsigned long)-1;
		locStreamInfo->mMinPageSize = (unsigned long)-1;
		locStreamInfo->mNumPages = 0;
		locStreamInfo->mSerialNo = locSerialNo;
		locStreamInfo->mTotalDataSize = 0;
		locStreamInfo->mTotalPacketCount = 0;
		locStreamInfo->mTotalStreamSize = 0;

		streamInfos.push_back(locStreamInfo);
	}
	unsigned long locNumPacks = 0;
	for (size_t i = 0; i < streamInfos.size(); i++) {
		
		if (locSerialNo == streamInfos[i]->mSerialNo) {
			locFoundPos = i;
			
			//Fill in the stats

			if (streamInfos[i]->mMaxPacksPerPage < inOggPage->numPackets()) {
				streamInfos[i]->mMaxPacksPerPage = inOggPage->numPackets();
			}

			if (streamInfos[i]->mMaxPageSize < inOggPage->pageSize()) {
				streamInfos[i]->mMaxPageSize = inOggPage->pageSize();
			}

			if (streamInfos[i]->mMinPacksPerPage > inOggPage->numPackets()) {
				streamInfos[i]->mMinPacksPerPage = inOggPage->numPackets();
			}

			if (streamInfos[i]->mMinPageSize > inOggPage->pageSize()) {

				streamInfos[i]->mMinPageSize = inOggPage->pageSize();
			}

			streamInfos[i]->mNumPages++;
			//streamInfos[i]->mSerialNo = locSerialNo;
			streamInfos[i]->mTotalDataSize += inOggPage->dataSize();
			streamInfos[i]->mTotalPacketCount += inOggPage->numPackets();
			streamInfos[i]->mTotalStreamSize += inOggPage->pageSize();



			cout << "Stream "<<(unsigned long)i<<"  : Granule = "<<inOggPage->header()->GranulePos()<<"   - ";
			locNumPacks = 0;
			if (inOggPage->numPackets() == 0) {
				cout<<"EMPTY PAGE"<<endl;
			} else if (inOggPage->numPackets() == 1) {
				if (inOggPage->getPacket(0)->isContinuation() || inOggPage->getPacket(0)->isTruncated()) {
					cout<<" 1 Partial Packet";
				} else {
					cout<<" 1 Full Packet";
					locNumPacks = 1;
				}
			} else {
				locNumPacks = inOggPage->numPackets();

				unsigned long locPartials = 0;
				if (inOggPage->getPacket(0)->isContinuation()) {
					locNumPacks--;
					locPartials++;
				}

				if (inOggPage->getPacket(inOggPage->numPackets() - 1)->isTruncated()) {
					locNumPacks--;
					locPartials++;
				}

				if (locPartials == 1) {
					cout << " 1 Partial Packet";
				} else if (locPartials >= 2) {
					cout<<" "<<locPartials<<" Full Packets";
				}

				if (locNumPacks == 1) {
					cout<<" 1 Full Packet";
				} else if (locNumPacks >= 2) {
					cout<<" "<<locNumPacks<<" Full Packets";
				}
			}
			cout<<endl;

			break;
		}
	}

	
	//if (*maxPacks[locFoundPos] < locNumPacks) {
	//	*maxPacks[locFoundPos] = locNumPacks;
	//}

	return true;
}


#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main (int argc, char * argv[])
#endif
{
	//This program just gives some info about number of packets
	// Currently does not error checking. Check your command line carefully !
	// USAGE :: OOOggPageInfo <OggFile>
	//

	bytePos = 0;

	if (argc < 2) {
		cout<<"Usage : OOOggPageInfo <filename>"<<endl;
	} else {
		OggDataBuffer testOggBuff;
		
		testOggBuff.registerStaticCallback(&pageCB, NULL);

		fstream testFile;
		testFile.open(argv[1], ios_base::in | ios_base::binary);
		
		const unsigned short BUFF_SIZE = 8092;
		char* locBuff = new char[BUFF_SIZE];
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
		}

		cout<<endl;
		cout<<endl;

		for (size_t i = 0; i < streamInfos.size(); i++) {
			dumpStreamInfo(streamInfos[i]);
			//cout<<"Stream "<<(unsigned long)i<<" max Packets = "<<*maxPacks[i]<<endl;
		}


		for (size_t i = 0; i < streamInfos.size(); i++) {
			delete streamInfos[i];
		}

		delete[] locBuff;
	}

	return 0;
}

