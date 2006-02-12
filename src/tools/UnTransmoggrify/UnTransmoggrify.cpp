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
#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
#include <libilliCore/iBE_Math.h>
#include <libilliCore/iLE_Math.h>

#include "OggFileWriter.h"
#include <iostream>
#include <fstream>

unsigned long bytePos;

OggPageInterleaver* interleaver;

OggFileWriter* fileWriter;

OggPage* pendingPage = NULL;
OggMuxStream* pendingStream = NULL;
bool isFirstPage = true;
bool hasPageWaiting = false;

unsigned long granFixCount = 0;

struct sStreamInfo {
	unsigned long mSerialNo;
	unsigned long mUptoSeqNo;
	OggMuxStream* mMuxStream;

};

vector<sStreamInfo*> streamList;


bool fixifyPage(OggPage* inOggPage, sStreamInfo* inStreamInfo)
{
	//Fix the granule pos bug
	if (inOggPage->numPackets() == 1) {
		if ((inOggPage->getPacket(0)->isTruncated()) && (inStreamInfo->mUptoSeqNo < inStreamInfo->mMuxStream->numHeaders())) {

			inOggPage->header()->setGranulePos(-1);
		}
	}

	//Fix sequence number bug
	inOggPage->header()->setPageSequenceNo(inStreamInfo->mUptoSeqNo++);

	//Recompute checksum
	inOggPage->computeAndSetCRCChecksum();

	return true;
}


bool setMuxState(OggPage* inOggPage, OggMuxStream* inMuxStream) 
{
		if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "\001vorbis", 7) == 0) {
			inMuxStream->setConversionParams(iLE_Math::charArrToULong(inOggPage->getPacket(0)->packetData() + 12), 1, 10000000);
			inMuxStream->setNumHeaders(3);
			return true;
		} else if ((strncmp((char*)inOggPage->getPacket(0)->packetData(), "\200theora", 7)) == 0){
			//mGranulePosShift = (((inOggPage->getPacket(0)->packetData()[40]) % 4) << 3) + ((inOggPage->getPacket(0)->packetData()[41]) >> 5);
			inMuxStream->setConversionParams(
						iBE_Math::charArrToULong(inOggPage->getPacket(0)->packetData() + 22) 
					,	iBE_Math::charArrToULong(inOggPage->getPacket(0)->packetData() + 26)
					,	10000000
					,	(((inOggPage->getPacket(0)->packetData()[40]) % 4) << 3) + ((inOggPage->getPacket(0)->packetData()[41]) >> 5));
			inMuxStream->setNumHeaders(3);
			return true;
		} else {
			return false;
		}

}

//This will be called by the callback
bool pageCB(OggPage* inOggPage, void*  /* inUserData  ignored */) 
{
	sStreamInfo* locCurrentInfo = NULL;
	if (inOggPage->header()->isBOS()) {
		sStreamInfo* locStreamInfo = new sStreamInfo;
		locStreamInfo->mSerialNo = inOggPage->header()->StreamSerialNo();
		locStreamInfo->mUptoSeqNo = 0;
		locStreamInfo->mMuxStream = interleaver->newStream();
		locStreamInfo->mMuxStream->setIsActive(true);
		streamList.push_back(locStreamInfo);

		if (!setMuxState(inOggPage, locStreamInfo->mMuxStream)) {
			cout<<"Only handles theora and vorbis... ABORT ABORT ABORT!!"<<endl;
			throw 0;
		}

		locCurrentInfo = locStreamInfo;

		
	} else {
		for (size_t i = 0; i < streamList.size(); i++) {
			if (streamList[i]->mSerialNo == inOggPage->header()->StreamSerialNo()) {
				locCurrentInfo = streamList[i];
				break;
			}	
		}
	}

	if (locCurrentInfo == NULL) {
		throw 0;
	}

	fixifyPage(inOggPage, locCurrentInfo);



	if (isFirstPage) {
		if (strncmp((const char*)inOggPage->getPacket(0)->packetData(), "\001vorbis", 7) == 0) {
			//First page is vorbis.

			//Hold it for later.
			pendingPage = inOggPage;
			pendingStream = locCurrentInfo->mMuxStream;
			isFirstPage = false;
			hasPageWaiting = true;
		}
	} else {
		if (hasPageWaiting) {
			if ((strncmp((char*)inOggPage->getPacket(0)->packetData(), "\200theora", 7)) == 0) {
				//Was vorbis, then theora... need to send the theora first, then the vorbis... then carry on
				locCurrentInfo->mMuxStream->acceptOggPage(inOggPage);
				pendingStream->acceptOggPage(pendingPage);
			} else {
				//Was vorbis... but no theora followed, send the pending page... and carry on as normal
				pendingStream->acceptOggPage(pendingPage);
				locCurrentInfo->mMuxStream->acceptOggPage(inOggPage);
			}

			hasPageWaiting = false;
			pendingPage = NULL;
		} else {
			locCurrentInfo->mMuxStream->acceptOggPage(inOggPage);
		}
	}






	//cout<<"Page Location : "<<bytePos;
	//bytePos += inOggPage->pageSize();
	//cout<<" to "<<bytePos<<endl;
	//OggPacket* locPack = NULL;
	//cout << inOggPage->header()->toString();
	//cout << "Num Packets : " << inOggPage->numPackets() << endl;;
	//for (unsigned long i = 0; i < inOggPage->numPackets(); i++) {
	//	locPack = inOggPage->getPacket(i);
	//	cout << "------ Packet  " << i << " (" << locPack->packetSize() << " bytes) -------";
	//	
	//	if (locPack->isContinuation()) {
	//		cout<<"  ** CONT **";
	//	}
	//	if (locPack->isTruncated()) {
	//		cout<<" ** TRUNC **";
	//	}
	//	cout<<endl;
	//	cout << locPack->toPackDumpString();
	//}
	
	return true;
}


#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main (int argc, char * argv[])
#endif
{
	//This program just dumps the pages out of a file in ogg format.
	// Currently does not error checking. Check your command line carefully !
	// USAGE :: OggDump <OggFile>
	//


	bytePos = 0;

	if (argc < 3) {
		cout<<"Usage : OOOggDump <filename>"<<endl;
	} else {

		int x;
		cin>>x;

		string locOutFilename = argv[2];
		fileWriter = new OggFileWriter(locOutFilename);
		interleaver = new OggPageInterleaver(fileWriter, fileWriter);

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

		delete[] locBuff;

		delete interleaver;
		delete fileWriter;

	}

	return 0;
}

