#include "StdAfx.h"
#include ".\filecomments.h"

FileComments::FileComments(void)
	:	mMinorStreamCount(0)
{
}

FileComments::~FileComments(void)
{
}

bool FileComments::acceptOggPage(OggPage* inOggPage) {
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
			
			mMinorStreamCount++;


		} else if ((strncmp((char*)locPacket->packetData(), "\201theora", 7)) == 0) {
			bool locIsOK = locVorbisComments->parseOggPacket(locPacket, 7);
			locStreamInfo->setCodecID(StreamCommentInfo::THEORA);
			locStreamInfo->setPageStart(mBytePos);
			locStreamInfo->setComments(locVorbisComments);
			locStreamInfo->setMajorStreamNo(0);   //Temp... increase for chaining
			locStreamInfo->setMinorStreamNo(mMinorStreamCount);

			mMinorStreamCount++;
		}
	}
	
	return true;

}
bool FileComments::loadFile(string inFileName) {
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
    	locOggBuff.feed(locBuff, locBytesRead);
	}

	delete locBuff;
	
}
string FileComments::fileName() {
	return mFileName;
}

bool FileComments::writeOutAll() {
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
