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

#include "StdAfx.h"
#include "oggpage.h"

OggPage::OggPage(void)
{
	mHeader = new OggPageHeader;
	
	
}
OggPage::~OggPage(void)
{
	delete mHeader;
	for (unsigned long i = 0; i < mPacketList.size(); i++) {
		delete mPacketList[i];
	}
}
OggPageHeader* OggPage::header() {
	return mHeader;
}

OggPacket* OggPage::getPacket(unsigned long inPacketNo) {
	if (inPacketNo < numPackets()) {
		return mPacketList[inPacketNo];
	} else {
		return NULL;
	}
}

StampedOggPacket* OggPage::getStampedPacket(unsigned long inPacketNo) {
	if (inPacketNo < numPackets()) {
		return mPacketList[inPacketNo];
	} else {
		return NULL;
	}
}
OggPage* OggPage::clone() {
	OggPage* retClone = new OggPage;
	retClone->mHeader = mHeader->clone();
	for (size_t i = 0; i < mPacketList.size(); i++) {
		retClone->mPacketList.push_back(mPacketList[i]);
	}
	
	return retClone;
}

unsigned long OggPage::numPackets() {
	return (unsigned long)mPacketList.size();
}

unsigned char* OggPage::createRawPageData() {
	unsigned char* locPage = new unsigned char[mHeader->pageSize()];
	mHeader->rawData(locPage, mHeader->pageSize());

	unsigned long locOffset = mHeader->headerSize();
	for (size_t i = 0; i < mPacketList.size(); i++) {
		OggPacket* locPack = mPacketList[i];
		memcpy((void*)(locPage + locOffset), (const void*)(locPack->packetData()), locPack->packetSize());
		locOffset += locPack->packetSize();
	}
	return locPage;
}

bool OggPage::addPacket(StampedOggPacket* inPacket) {
	if (inPacket != NULL) {
		mPacketList.push_back(inPacket);
		return true;
	} else {
		return false;
	}
}



unsigned long OggPage::pageSize()
{
	return mHeader->pageSize();
}
unsigned long OggPage::headerSize()
{
	return mHeader->headerSize();
}
unsigned long OggPage::dataSize()
{
	return mHeader->dataSize();
}




//string OggPage::toString()
//{
//	
//	return NULL;
//}

//void OggPage::screenDump()
//{
//	//Dump the header
//	cout<< mHeader->toString();
//	//Dump the data
//	dataDumpAsHex();
//}
//
//void OggPage::dumpNChars(unsigned char* inStartPoint, unsigned long inNumChars) {
//
//	//NOTE::: Also needs reworking
//	const unsigned char BELL = 7;
//	//Set the fill character back to space ' '
//	cout << setfill(' ');
//
//	//Put some space after the hex section
//	unsigned long locPadding = 3 * (HEX_DUMP_LINE_LENGTH - inNumChars) + 4;
//	cout << setw(locPadding) << "    ";
//
//	//Loop through the characters
//	for (unsigned long i = 0; i < inNumChars; i++) {
//
//		//If they are *not* going to mess up the layout (\r, \n or \t or bell(7))
//		if ( (inStartPoint[i] != '\n') && (inStartPoint[i] != '\r') && (inStartPoint[i] != '\t') && (inStartPoint[i] != BELL )) {
//			//Write them out
//			cout << inStartPoint[i];						
//		} else {
//			//Otherwise just write a null char
//			cout << (char) 0;
//		}
//	}
//	//End the line and put the fill character back to a number 0
//	cout << endl << setfill('0');
//	
//}
//
//
//
//void OggPage::dataDumpAsHex() {
//	///NOTE::: ShOuld be reworked.
//	//Needs dataSize and data pointer
//
//	//Put the stream in hex mode with a fill character of 0
//	hex(cout);
//	cout << setfill('0');
//
//	//Loop through every character of data
//	for (unsigned long i = 0; i < mHeader->dataSize(); i++) {
//		//If it is the end of the previous hex dump line or first line)
//		if ( (i % HEX_DUMP_LINE_LENGTH == 0) ) {
//			//And this is not the first line
//			if ( i != 0 ) {
//				//Write the actual characters out at the end of the line
//				dumpNChars( &mPageData[i - HEX_DUMP_LINE_LENGTH],  HEX_DUMP_LINE_LENGTH);
//			}
//
//			//At the start of the line write out the base address in an 8 hex-digit field 
//			cout << setw(8) << i << ": ";		
//		}
//
//		//Write out the value of the character in a 2 hex-digit field
//		cout << setw(2) << (int)mPageData[i] << " ";
//	}
//
//	//Find out how many leftover charcters didn't get written out.
//	unsigned long locLeftovers = (mHeader->dataSize() % HEX_DUMP_LINE_LENGTH);
//	locLeftovers = (locLeftovers > 0)	? (locLeftovers)	
//										: (HEX_DUMP_LINE_LENGTH);
//
//
//	//If there was any data in this dump
//	if ( mHeader->dataSize() > 0 ) {
//		//Dump the last part out
//		dumpNChars( &mPageData[mHeader->dataSize() - locLeftovers], locLeftovers );
//	}
//
//	cout << "==============================================================================" << endl;
//	//Put the stream back to decimal mode
//	dec(cout);
//}

