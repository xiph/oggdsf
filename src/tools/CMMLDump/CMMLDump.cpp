//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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

#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
#include <libCMMLTags/libCMMLTags.h>
#include <libCMMLParse/libCMMLParse.h>
#include <libCMMLParse/CMMLParser.h>

#include <iostream>
#include <fstream>

#include <string.h>

using namespace std;


class OggDataBufferProcessor
	: public IOggCallback
{
public:
	OggDataBufferProcessor(void);

	enum eDemuxState {
		SEEN_NOTHING,
		SEEN_CMML_BOS,
		INVALID = 100
	};

	eDemuxState mDemuxState;

	unsigned long mCMMLSerialNumber;

	virtual bool acceptOggPage(OggPage* inOggPage);
};


OggDataBufferProcessor::OggDataBufferProcessor (void)
	:	mDemuxState(SEEN_NOTHING)
	,	mCMMLSerialNumber(0)
{
}

bool OggDataBufferProcessor::acceptOggPage (OggPage* inOggPage)
{
	switch (mDemuxState)
	{
		case SEEN_NOTHING:
		{
			// Haven't seen anything yet: look for the CMML BOS page

			if (		(inOggPage->numPackets() == 1)
					&&	(inOggPage->header()->isBOS())
					&&	(strncmp((char*)inOggPage->getPacket(0)->packetData(), "AnxData", 7) == 0)
#ifdef WIN32
					&&  (  _strnicmp((char*) &(inOggPage->getPacket(0)->packetData()[28]), "Content-type: text/x-cmml", 25) == 0)	) {
#else  /* assume POSIX */
					&&  (strncasecmp((char*) &(inOggPage->getPacket(0)->packetData()[28]), "Content-type: text/x-cmml", 25) == 0)	) {
#endif
				// We've hit a BOS page: advance the state and remember the serial number
				
				mDemuxState = SEEN_CMML_BOS;

				mCMMLSerialNumber = inOggPage->header()->StreamSerialNo();
			}

			break;
		}
		case SEEN_CMML_BOS:
		{
			// We've seen the CMML BOS now, so every CMML page after this one (including the secondary
			// header) will be CMML data.

			if (inOggPage->header()->StreamSerialNo() == mCMMLSerialNumber) {
				unsigned long locPackets = inOggPage->numPackets();

				for (unsigned long i = 0; i < locPackets; i++) {
					OggPacket *locPacket = inOggPage->getPacket(i);

					unsigned long locPacketSize = locPacket->packetSize();
					char *locPacketData = (char *) inOggPage->getPacket(i)->packetData();

					// Add a terminating '\0'
					char *locMyPacketData = new char[locPacketSize + 1];
					(void) memcpy(locMyPacketData, locPacketData, locPacketSize);
					locMyPacketData[locPacketSize] = '\0';

					// Get the packet's time in seconds
					LOOG_INT64 locPacketGranulePos = inOggPage->header()->GranulePos();
					float locPacketStartTimeInSeconds = (float) locPacketGranulePos/ (float) 1000;

					// Let's make the CMML parsing library decide whether it's a <head>
					// or <clip> tag -- after all, that's what it's there for :)
					
					wstring locPacketDataWString = StringHelper::toWStr(locMyPacketData);

					CMMLParser locCMMLParser;
					C_CMMLTag *locCMMLTag = NULL;

					// Try parsing it as a <head> tag
					C_HeadTag *locHeadTag = new C_HeadTag;
					bool locDidParseHead = 
						locCMMLParser.parseHeadTag(locPacketDataWString, locHeadTag);
					if (locDidParseHead) {
						locCMMLTag = locHeadTag;
					}

					// Try parsing it as a <clip> tag
					C_ClipTag *locClipTag = new C_ClipTag;
					bool locDidParseClip = 
						locCMMLParser.parseClipTag(locPacketDataWString, locClipTag);
					if (locDidParseClip) {
						// Only succeed if parsing it as a <head> tag didn't work
						if (!locDidParseHead) {
							locCMMLTag = locClipTag;
						} else {
							cout << "Parsed successfully as both a <clip> and <head>:" << endl;
							wcout << locPacketDataWString << endl;
						}
					}

					if (locCMMLTag == NULL) {
						cout << "Couldn't parse as either <clip> or <head>:" << endl;
						wcout << locPacketDataWString << endl;
					}

					if (locDidParseClip) {
						cout << locPacketStartTimeInSeconds << " seconds:" << endl;
					}
					wcout << locCMMLTag->toString() << endl;

					delete locCMMLTag;
					delete locMyPacketData;
				}
			}

			break;
		}
	}

	delete inOggPage;

	return true;
}


#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " <input_filename>" << endl;
		return 1;
	}

	// Somebody set us up the bomb

	OggDataBuffer locDataBuffer;

	OggDataBufferProcessor *locDataBufferProcessor;
	locDataBufferProcessor = new OggDataBufferProcessor;
	locDataBuffer.registerVirtualCallback(locDataBufferProcessor);

	fstream locInputFile;

	// Let's go!

	const unsigned short BUFF_SIZE = 8092;
	char* locBuff = new char[BUFF_SIZE];
	
	locInputFile.open(argv[1], ios_base::in | ios_base::binary);
	for (;;) {
		locInputFile.read(locBuff, BUFF_SIZE);
		unsigned long locBytesRead = locInputFile.gcount();
		if (locBytesRead <= 0) {
			break;
		}
    	locDataBuffer.feed((const unsigned char*)locBuff, locBytesRead);
	}

	// Tidy up

	locInputFile.close();

	delete locDataBufferProcessor;

	return 0;
}

