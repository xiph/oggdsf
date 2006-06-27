//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
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

#include "stdafx.h"
#include "FileComments.h"

FileComments::FileComments(void)
	:	mMinorStreamCount(0)
{
}

FileComments::~FileComments(void)
{
}

bool FileComments::acceptOggPage(OggPage* inOggPage) {		//Correctly deletes page.
	//Get a callback... check whether we have a comment.
	VorbisComments* locVorbisComments = NULL;
	StreamCommentInfo* locStreamInfo = NULL;
	for (unsigned long i = 0; i < inOggPage->numPackets(); i++) {
		OggPacket* locPacket = NULL;
		locPacket = inOggPage->getPacket(i);
		if (strncmp((const char*)locPacket->packetData(), "\003vorbis", 7) == 0) {
			//Comment Packet
			locVorbisComments = new VorbisComments;
			locStreamInfo = new StreamCommentInfo;
			bool locIsOK = locVorbisComments->parseOggPacket(locPacket, 7);
			
			locStreamInfo->setCodecID(StreamCommentInfo::VORBIS);
			locStreamInfo->setPageStart(mBytePos);
			locStreamInfo->setComments(locVorbisComments);
			locStreamInfo->setMajorStreamNo(0);   //Temp... increase for chaining
			locStreamInfo->setMinorStreamNo(mMinorStreamCount);
			mStreams.push_back(locStreamInfo);
			mMinorStreamCount++;


		} else if ((strncmp((char*)locPacket->packetData(), "\201theora", 7)) == 0) {
			locVorbisComments = new VorbisComments;
			locStreamInfo = new StreamCommentInfo;
			bool locIsOK = locVorbisComments->parseOggPacket(locPacket, 7);
			locStreamInfo->setCodecID(StreamCommentInfo::THEORA);
			locStreamInfo->setPageStart(mBytePos);
			locStreamInfo->setComments(locVorbisComments);
			locStreamInfo->setMajorStreamNo(0);   //Temp... increase for chaining
			locStreamInfo->setMinorStreamNo(mMinorStreamCount);
			mStreams.push_back(locStreamInfo);

			mMinorStreamCount++;
		}
	}
	mBytePos += inOggPage->pageSize();
	
	delete inOggPage;
	return true;

}

#ifdef UNICODE
bool FileComments::loadFile(wstring inFileName) {
#else
bool FileComments::loadFile(string inFileName) {
#endif
	mBytePos = 0;
	const unsigned long BUFF_SIZE = 4096;
	OggDataBuffer locOggBuff;
	
	locOggBuff.registerVirtualCallback(this);

	fstream locFile;
	locFile.open(inFileName.c_str(), ios_base::in | ios_base::binary);
	if (locFile.is_open() != true) {
		return false;
	}
	char* locBuff = new char[BUFF_SIZE];
	unsigned long locBytesRead = 0;
	while (!locFile.eof()) {
		locFile.read(locBuff, BUFF_SIZE);
		locBytesRead = locFile.gcount();
    	locOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
	}

	delete[] locBuff;
	return true;
	
}
string FileComments::fileName() {
	return mFileName;
}

bool FileComments::writeOutAll(string inFileName) {

	fstream locFile;
	locFile.open(inFileName.c_str(), ios_base::out | ios_base::in | ios_base::binary);

	if (locFile.is_open()) {
				
		
	}
	return false;
}
bool FileComments::writeOutStream(unsigned long inIndex) {
	return false;
}

bool FileComments::addStreamComment(StreamCommentInfo* inStreamComment) {
	mStreams.push_back(inStreamComment);
	return true;
}
StreamCommentInfo* FileComments::getStreamComment(unsigned long inIndex) {
	if (inIndex < mStreams.size()) {
		return mStreams[inIndex];
	} else {
		return NULL;
	}
}

unsigned long FileComments::streamCount() {
	return (unsigned long)mStreams.size();
}
