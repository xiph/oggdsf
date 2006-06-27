//===========================================================================
//Copyright (C) 2005 Zentaro Kavanagh
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
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

#include <libOOOggChef/AnnodexRecomposer.h>
#include <libOOOggChef/CMMLRecomposer.h>
#include <libOOOggChef/utils.h>

#include <libOOOgg/libOOOgg.h>
#include <libOOOggSeek/AutoAnxSeekTable.h>

#include <assert.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


#undef DEBUG

/** You may optionally ask
	AnnodexRecomposer to use a cached representation of the seek table (which is
	computationally expensive to build) by passing a filename in the
	inCachedSeekTableFilename parameter.  If the file does not exist,
	AnnodexRecomposer will write out the constructed seek table to the filename
	given.  (If the file cannot be written for any reason, you will receive no
	warning.  Yell at me if this is a serious issue.)
 */

#ifdef UNICODE
AnnodexRecomposer::AnnodexRecomposer(wstring inFilename,
									 BufferWriter inBufferWriter,
									 void* inBufferWriterUserData,
									 wstring inCachedSeekTableFilename)

#else
AnnodexRecomposer::AnnodexRecomposer(string inFilename,
									 BufferWriter inBufferWriter,
									 void* inBufferWriterUserData,
									 string inCachedSeekTableFilename)
#endif
	:	mDemuxState(SEEN_NOTHING)
	,	mDemuxParserState(LOOK_FOR_HEADERS)
	,	mBufferWriter(inBufferWriter)
	,	mBufferWriterUserData(inBufferWriterUserData)
{
	// These need to go in the constructor body, because we can't assign
	// strings in the initialiser list
	
	mFilename = inFilename;
	mCachedSeekTableFilename = inCachedSeekTableFilename;
}

AnnodexRecomposer::~AnnodexRecomposer(void)
{
}

/** The starting time offset's units is in seconds, while the wanted MIME types
    is a vector of strings, which will be matched against the MIME type in the
	AnxData header of the logical bitstream.
  */
bool AnnodexRecomposer::recomposeStreamFrom(double inStartingTimeOffset,
	const vector<string>* inWantedMIMETypes)
{
	mWantedMIMETypes = inWantedMIMETypes;

#ifdef DEBUG
	mDebugFile.open("G:\\Logs\\AnnodexRecomposer.log", ios_base::out);
	mDebugFile << "AnnodexRecomposer 1 " << endl;
#endif

	static const size_t BUFF_SIZE = 8192;

	// Optimisation: If the client only wants CMML, and a file with .cmml
	// exists, recompose from that instead of the original .anx file, which
	// will be orders of magnitudes faster!
#ifdef UNICODE
	wstring locCMMLFilename;
#else
	string locCMMLFilename;
#endif
	locCMMLFilename = mFilename + TEXT(".cmml");
	if (wantOnlyCMML(mWantedMIMETypes) && fileExists(locCMMLFilename)) {
#ifdef DEBUG
		mDebugFile << "Client wants CMML: " + locCMMLFilename + " exists" << endl;
#endif
		CMMLRecomposer *locCMMLRecomposer = new CMMLRecomposer(locCMMLFilename,
			mBufferWriter, mBufferWriterUserData);
		return locCMMLRecomposer->recomposeStreamFrom(inStartingTimeOffset,
			inWantedMIMETypes);
	}

	// Turn the starting time offset into DirectSeconds
	mRequestedStartTime = (LOOG_UINT64) inStartingTimeOffset * 10000000;

	// Open the file and prepare the OggDataBuffer to receive pages
	fstream locFile;
	locFile.open(mFilename.c_str(), ios_base::in | ios_base::binary);

	// Build a seek table from the file, so we can find out the end location of
	// the stream headers, and the byte position of the user's requested start
	// time
	AutoAnxSeekTable *locSeekTable = new AutoAnxSeekTable(mFilename);
	if ((mCachedSeekTableFilename != TEXT("")) && fileExists(mCachedSeekTableFilename)) {
		locSeekTable->buildTableFromFile(mCachedSeekTableFilename);
	} else {
		locSeekTable->buildTable();
	}

	if ((mCachedSeekTableFilename != TEXT("")) && !fileExists(mCachedSeekTableFilename)) {
		locSeekTable->serialiseInto(mCachedSeekTableFilename);
	}
	
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

	// Output CMML preamble if the user wants it
	if (wantOnlyCMML(mWantedMIMETypes)) {
		const string CMML_PREAMBLE = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<cmml>\n";
		mBufferWriter((unsigned char *) CMML_PREAMBLE.c_str(), (unsigned long) CMML_PREAMBLE.length(), mBufferWriterUserData);
	}

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
			locFile.read(locBuffer, locBytesToRead);
			unsigned long locBytesReadThisIteration = locFile.gcount();
			if (locBytesReadThisIteration <= 0) {
				break;
			}
			locOggBuffer.feed((unsigned char *) locBuffer, locBytesReadThisIteration);
		}
	}

	// Get the offset into the file from the requested start time
	unsigned long locRequestedStartTimeOffset =
		locSeekTable->getStartPos(mRequestedStartTime).second;

	// Clear the file's flags, to avoid any fallout from reading the headers
	locFile.clear();
	locFile.seekg(locRequestedStartTimeOffset);

#ifdef DEBUG
	mDebugFile << "mRequestedStartTime: " << mRequestedStartTime << endl;
	mDebugFile << "locRequestedStartTimeOffset: " << locRequestedStartTimeOffset << endl;
	mDebugFile << "Current position: " << locFile.tellg() << endl;
#endif

	// TODO: This is a hack, since sometimes AutoAnxTable returns 0 when it
	// really shouldn't ...
	size_t locCurrentPosition = locFile.tellg();
	if (locCurrentPosition == 0) {
#ifdef DEBUG
		mDebugFile << "Resetting mDemuxState to SEEN_NOTHING bofore LOOK_FOR_BODY" << endl;
#endif
		mDemuxState = SEEN_NOTHING;
	}

#ifdef DEBUG
	for (set<tSerial_HeadCountPair>::iterator i = mWantedStreamSerialNumbers.begin(); i != mWantedStreamSerialNumbers.end(); i++) {
		mDebugFile << "Serialno: " << i->first << endl;
	}

	mDebugFile << "mDemuxState before LOOK_FOR_BODY is " << mDemuxState << endl;
#endif

	mDemuxParserState = LOOK_FOR_BODY;
	{
		OggDataBuffer locOggBuffer;
		locOggBuffer.registerVirtualCallback(this);

		char *locBuffer = new char[BUFF_SIZE];
		for (;;)
		{
			locFile.read(locBuffer, BUFF_SIZE);
			unsigned long locBytesReadThisIteration = locFile.gcount();
			if (locBytesReadThisIteration <= 0) {
				break;
			}
			locOggBuffer.feed((unsigned char *) locBuffer, locBytesReadThisIteration);
		}
	}

	// Output CMML preamble if the user wants it
	if (wantOnlyCMML(mWantedMIMETypes)) {
		const string CMML_POSTAMBLE = "\n</cmml>";
		mBufferWriter((unsigned char *) CMML_POSTAMBLE.c_str(), (unsigned long) CMML_POSTAMBLE.length(), mBufferWriterUserData);
	}

	// Tidy up
	locFile.close();

#ifdef DEBUG
	mDebugFile << "----------------" << endl;
	mDebugFile.close();
#endif

	return true;
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

bool isFisheadPage (OggPage *inOggPage)
{
	return (
			inOggPage->numPackets() == 1
		&&	inOggPage->header()->isBOS()
		&&	strncmp((char*)inOggPage->getPacket(0)->packetData(),
		            "fishead\0", 8) == 0
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

unsigned long secondaryHeaders(OggPacket* inPacket, const unsigned short inAnnodexMajorVersion)
{
	if (inAnnodexMajorVersion == 2) {

		const unsigned short NUM_SEC_HEADERS_OFFSET = 24;

		return iLE_Math::charArrToULong(inPacket->packetData() +
										NUM_SEC_HEADERS_OFFSET);

	} else if (inAnnodexMajorVersion == 3) {

		// We're hardcoding the codec types in here for now: see the comment
		// above the mimeType function for why.

		unsigned long locSecondaryHeaders = 0;

		char* locPacketData = (char*) inPacket->packetData();
		if (memcmp(locPacketData, "\001vorbis", 7) == 0) {
			locSecondaryHeaders = 3;
		} else if (memcmp(locPacketData, "\200theora", 7) == 0) {
			locSecondaryHeaders = 3;
		} else if (memcmp(locPacketData, "CMML\0\0\0\0", 8) == 0) {
			locSecondaryHeaders = 3;
		} else {
			locSecondaryHeaders = 3;
		}

		return locSecondaryHeaders;

	} else {
		// Neither V2 nor V3: ack!
		return 0;
	}
}

void setPresentationTimeOnAnnodexHeaderPage (OggPage *inOggPage, LOOG_UINT64 inPresentationTime)
{
	// Offsets for Annodex v2 (the "timebase" field)
	const unsigned short V2_PRESENTATION_TIME_NUMERATOR_OFFSET = 12;
	const unsigned short V2_PRESENTATION_TIME_DENOMINATOR_OFFSET =
		V2_PRESENTATION_TIME_NUMERATOR_OFFSET + 8;

	// Offsets for Annodex v3 (the "presentation time" field)
	const unsigned short V3_PRESENTATION_TIME_NUMERATOR_OFFSET = 12;
	const unsigned short V3_PRESENTATION_TIME_DENOMINATOR_OFFSET =
		V3_PRESENTATION_TIME_NUMERATOR_OFFSET + 8;

	// Figure out whether we're dealing with V2 or V3 and set the presentation
	// time offsets appropriately
	unsigned short locPresentationTimeNumeratorOffset = 0;
	unsigned short locPresentationTimeDenominatorOffset = 0;
	if (isAnnodexBOSPage(inOggPage)) {
		// Annodex V2
		locPresentationTimeNumeratorOffset = V2_PRESENTATION_TIME_NUMERATOR_OFFSET;
		locPresentationTimeDenominatorOffset = V2_PRESENTATION_TIME_DENOMINATOR_OFFSET;
	} else if (isFisheadPage(inOggPage)) {
		// Annodex V3
		locPresentationTimeNumeratorOffset = V3_PRESENTATION_TIME_NUMERATOR_OFFSET;
		locPresentationTimeDenominatorOffset = V3_PRESENTATION_TIME_DENOMINATOR_OFFSET;
	} else {
		// We don't recognise this page: return early before we do anything
		// harmful
		return;
	}

	unsigned char* locPacketData = inOggPage->getPacket(0)->packetData();

	// Get pointers for the offsets into the packet
	unsigned char* locNumeratorPointer = locPacketData + locPresentationTimeNumeratorOffset;
	unsigned char* locDenominatorPointer = locPacketData + locPresentationTimeDenominatorOffset;
	
	// Set the presentation time on the packet in DirectSeconds (using the
	// denominator to indicate that the units are in DirectSeconds)
	iLE_Math::Int64ToCharArr(inPresentationTime, locNumeratorPointer);
	iLE_Math::Int64ToCharArr(10000000, locDenominatorPointer);

	// Recompute the Ogg page's checksum
	inOggPage->computeAndSetCRCChecksum();
}

#ifdef WIN32
# define strncasecmp _strnicmp
#endif /* will be undef'ed below */
string mimeType(OggPacket* inPacket, const unsigned short inAnnodexMajorVersion)
{
	if (inAnnodexMajorVersion == 2) {
		const unsigned short CONTENT_TYPE_OFFSET = 28;

		if (strncasecmp((char *) inPacket->packetData() + CONTENT_TYPE_OFFSET,
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
	} else if (inAnnodexMajorVersion == 3) {
		// Ahem, we should _really_ peek into the fisbone track to do this
		// properly, but considering that you need to understand the codecs to
		// build the seek table anyway, yet another codec dependency here
		// isn't going to kill us.  Obviously this is a very bad long-term
		// solution since the V3 Annodex skeleton is meant to be completely
		// codec-independent, but too much of this framework is currently
		// littered with codec knowledge.
		string locMimeType;

		char* locPacketData = (char*) inPacket->packetData();
		if (memcmp(locPacketData, "\001vorbis", 7) == 0) {
			locMimeType = "audio/x-vorbis";
		} else if (memcmp(locPacketData, "\200theora", 7) == 0) {
			locMimeType = "video/x-theora";
		} else if (memcmp(locPacketData, "CMML\0\0\0\0", 8) == 0) {
			locMimeType = "text/x-cmml";
		} else if (memcmp(locPacketData, "fisbone\0", 8) == 0) {
			locMimeType = "_skeleton";
		}

		return locMimeType;

	} else {
		// Neither V2 nor V3: exit with extreme prejudice
		return NULL;
	}
}
#ifdef WIN32
# undef strcasecmp
#endif

bool AnnodexRecomposer::acceptOggPage(OggPage* inOggPage)
{
	if (mDemuxParserState == LOOK_FOR_HEADERS) {

		switch (mDemuxState) {

			case SEEN_NOTHING:
				if (isAnnodexBOSPage(inOggPage) || isFisheadPage(inOggPage)) {
					mDemuxState = SEEN_ANNODEX_BOS;

					// Do we have a V2 or V3 Annodex file?
					if (isAnnodexBOSPage(inOggPage)) {
#ifdef DEBUG
						mDebugFile << "Found Annodex v2 file" << endl;
#endif
						mAnnodexMajorVersion = 2;
					} else if (isFisheadPage(inOggPage)) {
#ifdef DEBUG
						mDebugFile << "Found Annodex v3 file" << endl;
#endif
						mAnnodexMajorVersion = 3;
					}

					// Remember the Annodex stream's serial number, so we can output it later
					mAnnodexSerialNumber = inOggPage->header()->StreamSerialNo();
					mWantedStreamSerialNumbers.insert(make_pair<unsigned long, unsigned long>(mAnnodexSerialNumber, 0));

					// Fix up the presentation time of the Annodex BOS page if we're not
					// serving out the data from time 0
					if (mRequestedStartTime != 0) {
						setPresentationTimeOnAnnodexHeaderPage(inOggPage, mRequestedStartTime);
					}

					if (!wantOnlyPacketBody(mWantedMIMETypes)) {
						// Send out the page
						unsigned char *locRawPageData = inOggPage->createRawPageData();
						mBufferWriter(locRawPageData,
							inOggPage->pageSize(), mBufferWriterUserData);
						delete locRawPageData;
					}
				} else {
					// The Annodex/Fishead BOS page should always be the very first page of
					// the stream, so if we don't see it, the stream's invalid
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_BOS:
				if (isAnnodexEOSPage(inOggPage, mAnnodexSerialNumber)) {
#ifdef DEBUG
					mDebugFile << "Seen Annodex skeleton EOS" << endl;
#endif
					mDemuxState = SEEN_ANNODEX_EOS;
					if (!wantOnlyPacketBody(mWantedMIMETypes)) {
						unsigned char *locRawPageData = inOggPage->createRawPageData();
						mBufferWriter(locRawPageData,
							inOggPage->pageSize(), mBufferWriterUserData);
						delete locRawPageData;
					}
				} else if ((	mAnnodexMajorVersion == 2 && isAnxDataPage(inOggPage))
							||	mAnnodexMajorVersion == 3) {
					unsigned long locSerialNumber = inOggPage->header()->StreamSerialNo();
					string locMimeType = mimeType(inOggPage->getPacket(0), mAnnodexMajorVersion);

					for (unsigned int i = 0; i < mWantedMIMETypes->size(); i++) {
						const string locWantedMIMEType = mWantedMIMETypes->at(i);
						if (	locWantedMIMEType == locMimeType
							||	locWantedMIMEType == "*/*" /* accept all */
							||	locWantedMIMEType == "application/x-annodex") {
							// Create an association of serial no and num headers
							tSerial_HeadCountPair locMap;
							locMap.first = locSerialNumber;
							locMap.second = secondaryHeaders(inOggPage->getPacket(0), mAnnodexMajorVersion);

							// Add the association to our stream list
							if ((mWantedStreamSerialNumbers.insert(locMap)).second) {
#ifdef DEBUG
								mDebugFile << "Added " << locSerialNumber << " with " << locMap.second << " to mWantedStreamSerialNumbers" << endl;
#endif
							}

							if (!wantOnlyPacketBody(mWantedMIMETypes)) {
								unsigned char *locRawPageData = inOggPage->createRawPageData();
								mBufferWriter(locRawPageData,
									inOggPage->pageSize(), mBufferWriterUserData);
								delete locRawPageData;
							}
						}
					}
				} else {
					// We didn't spot either an AnxData page or the Annodex
					// EOS (v2), or a fisbone/header page (v3).  WTF?
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_EOS:
				if (mAnnodexMajorVersion == 2) {
					// Only output headers for the streams that the user wants
					// in their request
					for (set<tSerial_HeadCountPair>::iterator i = mWantedStreamSerialNumbers.begin(); i != mWantedStreamSerialNumbers.end(); i++) {
						if (i->first == inOggPage->header()->StreamSerialNo()) {
							if (i->second >= 1) {
								i->second--;
								if (wantOnlyPacketBody(mWantedMIMETypes)) {
									OggPacket* locPacket = inOggPage->getPacket(0);
									mBufferWriter(locPacket->packetData(),
										locPacket->packetSize(), mBufferWriterUserData);
								} else {
									unsigned char *locRawPageData = inOggPage->createRawPageData();
									mBufferWriter(locRawPageData,
										inOggPage->pageSize(), mBufferWriterUserData);
									delete locRawPageData;
								}
							} 
						}
					}

					bool allEmpty = true;
					for (set<tSerial_HeadCountPair>::iterator i = mWantedStreamSerialNumbers.begin(); i != mWantedStreamSerialNumbers.end(); i++) {
						if (i->second != 0) {
							allEmpty = false;
						}
					}

					if (allEmpty) {
						mDemuxState = SEEN_ALL_CODEC_HEADERS;
					}
				} else if (mAnnodexMajorVersion == 3) {
					// Annodex V3: once we hit the Annodex EOS, all the page headers are done
					mDemuxState = SEEN_ALL_CODEC_HEADERS;
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
				if (isAnnodexBOSPage(inOggPage) || isFisheadPage(inOggPage)) {
					mDemuxState = SEEN_ANNODEX_BOS;
				} else {
					// The Annodex BOS page should always be the very first page of
					// the stream, so if we don't see it, the stream's invalid
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_BOS:
				if (isAnnodexEOSPage(inOggPage, mAnnodexSerialNumber)) {
					mDemuxState = SEEN_ANNODEX_EOS;
				} else if (		(mAnnodexMajorVersion == 2 && isAnxDataPage(inOggPage))
							||	 mAnnodexMajorVersion == 3) {
					// Do nothing
				} else {
					// We didn't spot either an AnxData page or the Annodex EOS: WTF?
					mDemuxState = INVALID;
				}
				break;

			case SEEN_ANNODEX_EOS:
				mDemuxState = SEEN_ALL_CODEC_HEADERS;
				// Fallthrough!
			case SEEN_ALL_CODEC_HEADERS:
				{
					// Ignore any header packets which we may encounter
					if ((inOggPage->header()->HeaderFlags() & OggPageHeader::BOS) != 0) {
						break;
					}

					// Only output streams which the user requested
					for (set<tSerial_HeadCountPair>::iterator i = mWantedStreamSerialNumbers.begin(); i != mWantedStreamSerialNumbers.end(); i++) {
						if (i->first ==	inOggPage->header()->StreamSerialNo()) {
#ifdef DEBUG
							mDebugFile << "Outputting page for serialno " << i->first << endl;
#endif
							if (wantOnlyPacketBody(mWantedMIMETypes)) {
								for (unsigned long j = 0; j < inOggPage->numPackets(); j++) {
									OggPacket* locPacket = inOggPage->getPacket(j);
									if (locPacket->packetSize() > 0) {
										mBufferWriter(locPacket->packetData(),
											locPacket->packetSize(), mBufferWriterUserData);
									}
								}
							} else {
								unsigned char *locRawPageData = inOggPage->createRawPageData();								
								mBufferWriter(locRawPageData,
									inOggPage->pageSize(), mBufferWriterUserData);
								delete locRawPageData;
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

	// Tidy up
	delete inOggPage;
	inOggPage = NULL;

	return true;
}
