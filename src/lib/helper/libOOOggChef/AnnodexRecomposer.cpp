#include "stdafx.h"

#include <libOOOggChef/AnnodexRecomposer.h>

#include <libOOOgg/libOOOgg.h>
#include <libOOOggSeek/AutoAnxSeekTable.h>

#include <assert.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


#undef DEBUG


AnnodexRecomposer::AnnodexRecomposer(string inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData)
	:	mFilename(inFilename)
	,	mDemuxState(SEEN_NOTHING)
	,	mDemuxParserState(LOOK_FOR_HEADERS)
	,	mRequestedTime(0)
	,	mBufferWriter(inBufferWriter)
	,	mBufferWriterUserData(inBufferWriterUserData)
{
}

AnnodexRecomposer::~AnnodexRecomposer(void)
{
}


void AnnodexRecomposer::recomposeStreamFrom(double inStartingTimeOffset,
	const vector<const string>* inWantedMIMETypes)
{
	mWantedMIMETypes = inWantedMIMETypes;

#ifdef DEBUG
	mDebugFile.open("G:\\Logs\\AnnodexRecomposer.log", ios_base::out);
	mDebugFile << "AnnodexRecomposer 1 " << endl;
#endif

	static const size_t BUFF_SIZE = 8192;

	// Open the file and prepare the OggDataBuffer to receive pages
	fstream mFile;
	mFile.open(mFilename.c_str(), ios_base::in | ios_base::binary);

	// Build a seek table from the file, so we can find out the end location of
	// the stream headers, and the byte position of the user's requested start
	// time
	AutoAnxSeekTable *locSeekTable = new AutoAnxSeekTable(mFilename);
	locSeekTable->buildTable();
	
	// Find out where the non-header packets (i.e. the stream body) starts

	// n.b. We should be using the following line of code to do this:
	//
	//  unsigned long locStartOfBodyOffset = locSeekTable->getStartPos(0).second;
	//
	// ... since that should return the body offset, but there's a bug in
	// AutoAnxSeekTable (or AutoOggSeekTable) which makes that sometimes
	// return 0, which makes it useless for our purposes.  So, we force feed
	// the first 640K of the file, which should be enough to detect any headers ;)

	unsigned long locStartOfBodyOffset = 640 * 1024;

#ifdef DEBUG
	mDebugFile << "Filename: " << mFilename << endl;
	mDebugFile << "locStartOfBodyOffset: " << locStartOfBodyOffset << endl;
#endif

	// Grab the headers from the stream
	mDemuxParserState = LOOK_FOR_HEADERS;
	{
		OggDataBuffer locOggBuffer;
		locOggBuffer.registerVirtualCallback(this);

		unsigned long locBytesRead = 0;
		char *locBuffer = new char[BUFF_SIZE];
		while (locBytesRead < locStartOfBodyOffset)
		{
			// MIN is defined in OggPaginator.h (and in about three zillion other
			// projects)
			unsigned long locBytesToRead =
				MIN(locStartOfBodyOffset - locBytesRead, BUFF_SIZE);
			mFile.read(locBuffer, locBytesToRead);
			unsigned long locBytesReadThisIteration = mFile.gcount();
			if (locBytesReadThisIteration <= 0) {
				break;
			}
			locOggBuffer.feed((unsigned char *) locBuffer, locBytesReadThisIteration);
		}
	}

	// Grab data from the stream at offset
	LOOG_UINT64 locRequestedStartTime =
		(LOOG_UINT64) inStartingTimeOffset * 10000000;
	unsigned long locRequestedStartTimeOffset =
		locSeekTable->getStartPos(locRequestedStartTime).second;

	// Re-open the file, to avoid any fallout from reading the headers
	mFile.clear();
	mFile.seekg(locRequestedStartTimeOffset);

#ifdef DEBUG
	mDebugFile << "locRequestedStartTime: " << locRequestedStartTime << endl;
	mDebugFile << "locRequestedStartTimeOffset: " << locRequestedStartTimeOffset << endl;
	mDebugFile << "Current position: " << mFile.tellg() << endl;
#endif

	mDemuxParserState = LOOK_FOR_BODY;
	{
		OggDataBuffer locOggBuffer;
		locOggBuffer.registerVirtualCallback(this);

		char *locBuffer = new char[BUFF_SIZE];
		for (;;)
		{
			mFile.read(locBuffer, BUFF_SIZE);
			unsigned long locBytesReadThisIteration = mFile.gcount();
			if (locBytesReadThisIteration <= 0) {
				break;
			}
			locOggBuffer.feed((unsigned char *) locBuffer, locBytesReadThisIteration);
		}
	}

	// Tidy up
	mFile.close();

#ifdef DEBUG
	mDebugFile << "----------------" << endl;
	mDebugFile.close();
#endif

}

bool isAnnodexBOSPage (OggPage *inOggPage)
{
	return (
			inOggPage->numPackets() == 1
		&&	inOggPage->header()->isBOS()
		&&	strncmp((char*)inOggPage->getPacket(0)->packetData(),
		            "Annodex\0", 8) == 0
		   );
}

bool isAnxDataPage (OggPage *inOggPage)
{
	return (
			inOggPage->numPackets() == 1
		&&	inOggPage->header()->isBOS()
		&&	strncmp((char*)inOggPage->getPacket(0)->packetData(),
		            "AnxData\0", 8) == 0
		   );
}

bool isAnnodexEOSPage (OggPage *inOggPage, unsigned long locAnnodexSerialNumber)
{
	return (
			inOggPage->header()->isEOS()
		&&	inOggPage->header()->StreamSerialNo() == locAnnodexSerialNumber
		   );
}

unsigned long secondaryHeaders(OggPacket* inPacket)
{
	const unsigned short NUM_SEC_HEADERS_OFFSET = 24;

	return iLE_Math::charArrToULong(inPacket->packetData() +
									NUM_SEC_HEADERS_OFFSET);

}

string mimeType(OggPacket* inPacket)
{
	const unsigned short CONTENT_TYPE_OFFSET = 28;

	if (_strnicmp((char *) inPacket->packetData() + CONTENT_TYPE_OFFSET,
		          "Content-Type: ", 14) == 0)
	{
		const unsigned short MIME_TYPE_OFFSET = 28 + 14;
		const unsigned short MAX_MIME_TYPE_LENGTH = 256;
		char *locMimeType = new char[MAX_MIME_TYPE_LENGTH];
		sscanf((char *) inPacket->packetData() + MIME_TYPE_OFFSET, "%s\r\n", locMimeType);
		return locMimeType;
	} else {
		return NULL;
	}
}

bool wantOnlyPacketBody(const vector<const string>* inWantedMIMETypes)
{
	// TODO: This should check for packet bodies generally, not text/x-cmml

	return (	inWantedMIMETypes->size() == 1
			&&	inWantedMIMETypes->at(0) == "text/x-cmml");
}

bool AnnodexRecomposer::acceptOggPage(OggPage* inOggPage)
{
	if (mDemuxParserState == LOOK_FOR_HEADERS) {

		switch (mDemuxState) {

			case SEEN_NOTHING:
				if (isAnnodexBOSPage(inOggPage)) {
					mDemuxState = SEEN_ANNODEX_BOS;

					// Remember the Annodex stream's serial number, so we can output it later
					mAnnodexSerialNumber = inOggPage->header()->StreamSerialNo();
					mWantedStreamSerialNumbers.push_back(make_pair<unsigned long, unsigned long>(mAnnodexSerialNumber, 0));

					if (!wantOnlyPacketBody(mWantedMIMETypes)) {
						// Send out the page
						mBufferWriter(inOggPage->createRawPageData(),
							inOggPage->pageSize(), mBufferWriterUserData);
					}
				} else {
					// The Annodex BOS page should always be the very first page of
					// the stream, so if we don't see it, the stream's invalid
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_BOS:
				if (isAnxDataPage(inOggPage)) {
					unsigned long locSerialNumber = inOggPage->header()->StreamSerialNo();
					string locMimeType = mimeType(inOggPage->getPacket(0));

					for (unsigned int i = 0; i < mWantedMIMETypes->size(); i++) {
						if (	mWantedMIMETypes->at(i) == locMimeType
							||	mWantedMIMETypes->at(i) == "*/*" /* accept all */) {
							// Create an association of serial no and num headers
							tSerial_HeadCountPair locMap;
							locMap.first = locSerialNumber;
							locMap.second = secondaryHeaders(inOggPage->getPacket(0));

							// Add the association to our stream list
							mWantedStreamSerialNumbers.push_back(locMap);

							if (!wantOnlyPacketBody(mWantedMIMETypes)) {
								mBufferWriter(inOggPage->createRawPageData(),
									inOggPage->pageSize(), mBufferWriterUserData);
							}

							break;
						}
					}
				} else if (isAnnodexEOSPage(inOggPage, mAnnodexSerialNumber)) {
					mDemuxState = SEEN_ANNODEX_EOS;
					if (!wantOnlyPacketBody(mWantedMIMETypes)) {
						mBufferWriter(inOggPage->createRawPageData(),
							inOggPage->pageSize(), mBufferWriterUserData);
					}
				} else {
					// We didn't spot either an AnxData page or the Annodex EOS: WTF?
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_EOS:
				{
					// Only output headers for the streams that the user wants
					// in their request
					for (unsigned int i = 0; i < mWantedStreamSerialNumbers.size(); i++) {
						if (mWantedStreamSerialNumbers[i].first == inOggPage->header()->StreamSerialNo()) {
							if (mWantedStreamSerialNumbers[i].second >= 1) {
								mWantedStreamSerialNumbers[i].second--;
								if (wantOnlyPacketBody(mWantedMIMETypes)) {
									OggPacket* locPacket = inOggPage->getPacket(0);
									mBufferWriter(locPacket->packetData(),
										locPacket->packetSize(), mBufferWriterUserData);
								} else {
									mBufferWriter(inOggPage->createRawPageData(),
										inOggPage->pageSize(), mBufferWriterUserData);
								}
							} 
#if 0
							else {
								mDemuxState = INVALID;
							}
#endif
						}
					}

					bool allEmpty = true;
					for (unsigned int i = 0; i < mWantedStreamSerialNumbers.size(); i++) {
						if (mWantedStreamSerialNumbers[i].second != 0) {
							allEmpty = false;
						}
					}

					if (allEmpty) {
						mDemuxState = SEEN_ALL_CODEC_HEADERS;
					}
				}
				break;
			case SEEN_ALL_CODEC_HEADERS:
				// Seen all the headers: don't do anything
				break;
			case INVALID:
				break;
			default:
				break;
		}

	} else if (mDemuxParserState == LOOK_FOR_BODY) {

		switch (mDemuxState) {

			case SEEN_NOTHING:
			case SEEN_ANNODEX_BOS:
			case SEEN_ANNODEX_EOS:
#ifdef DEBUG
			mDebugFile << "Looking for body, state is " << mDemuxState << endl;
#endif		
			break;

			case SEEN_ALL_CODEC_HEADERS:
				{
					// Only output streams which the user requested
					for (unsigned int i = 0; i < mWantedStreamSerialNumbers.size(); i++) {
						if (	mWantedStreamSerialNumbers[i].first
							==	inOggPage->header()->StreamSerialNo()) {
							if (wantOnlyPacketBody(mWantedMIMETypes)) {
								for (unsigned long j = 0; j < inOggPage->numPackets(); j++) {
									OggPacket* locPacket = inOggPage->getPacket(j);
									if (locPacket->packetSize() > 0) {
										mBufferWriter(locPacket->packetData(),
											locPacket->packetSize(), mBufferWriterUserData);
									}
								}
							} else {
								mBufferWriter(inOggPage->createRawPageData(),
									inOggPage->pageSize(), mBufferWriterUserData);
							}
						}
					}
				}
				break;
			case INVALID:
				break;
			default:
				break;
		}

	} else {
		// Should never get here!
		assert(0);
	}

	if (mDemuxState == INVALID) {
	}

	// Tidy up
	delete inOggPage;
	inOggPage = NULL;

	return true;
}

