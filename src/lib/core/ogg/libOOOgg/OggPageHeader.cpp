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
#include "oggpageheader.h"


	//unsigned long mPageSize;
	//unsigned long mHeaderSize;
	//unsigned long mDataSize;

	//unsigned char mStructureVersion;
	//unsigned char mHeaderFlags;
	//__int64 mGranulePos;
	//unsigned long mStreamSerialNo;
	//unsigned long mPageSequenceNo;
	//unsigned long mCRCChecksum;
	//unsigned char mNumPageSegments;
	//OggSegmentTable* mSegmentTable;

	//ePageState mPageState;
OggPageHeader::OggPageHeader(void)
	:	mPageSize(0)
	,	mHeaderSize(0)
	,	mDataSize(0)
	,	mStructureVersion(0)
	,	mHeaderFlags(0)
	,	mStreamSerialNo(0)
	,	mPageSequenceNo(0)
	,	mCRCChecksum(0)
	,	mPageState(BLANK)
	,	mGranulePos(0)
	,	mSegmentTable(NULL)
		
{

}

OggPageHeader::~OggPageHeader(void)
{

	delete mSegmentTable;

}

OggPageHeader* OggPageHeader::clone() {
	/*
		unsigned long mPageSize;
	unsigned long mHeaderSize;
	unsigned long mDataSize;

	unsigned char mStructureVersion;
	unsigned char mHeaderFlags;
	OggInt64* mGranulePos;
	unsigned long mStreamSerialNo;
	unsigned long mPageSequenceNo;
	unsigned long mCRCChecksum;
	unsigned char mNumPageSegments;
	OggSegmentTable* mSegmentTable;

	ePageState mPageState;
	*/

	OggPageHeader* retClone = new OggPageHeader();
	retClone->mCRCChecksum = mCRCChecksum;
	retClone->mDataSize = mDataSize;
	retClone->mGranulePos = mGranulePos;
	retClone->mHeaderFlags = mHeaderFlags;
	retClone->mHeaderSize = mHeaderSize;
	//retClone->mNumPageSegments = mNumPageSegments;
	retClone->mPageSequenceNo = mPageSequenceNo;
	retClone->mPageSize = mPageSize;
	retClone->mPageState = mPageState;
	retClone->mSegmentTable = mSegmentTable->clone();
	retClone->mStreamSerialNo = mStreamSerialNo;
	retClone->mStructureVersion = mStructureVersion;

	return retClone;
}

bool OggPageHeader::rawData(unsigned char* outData, unsigned long inBuffSize) {
	
	//0-3			CapPattern						"Oggs"
	//4				Struct Ver
	//5				Head Flags
	//6-13			Granule Pos
	//14-17			Stream Serial No
	//18-21			Page Seq No
	//22-25			CheckSum
	//26			Num Segments
	//27...			SegmentTable
	if (mHeaderSize > inBuffSize) {
		//EXIT POINT
		return false;
	}
	outData[0] = 'O';
	outData[1] = 'g';
	outData[2] = 'g';
	outData[3] = 'S';
	outData[4] = mStructureVersion;
	outData[5] = mHeaderFlags;
	OggMath::Int64ToCharArr(mGranulePos, &outData[6]);
	OggMath::ULongToCharArr(mStreamSerialNo, &outData[14]);
	OggMath::ULongToCharArr(mPageSequenceNo, &outData[18]);
	OggMath::ULongToCharArr(mCRCChecksum, &outData[22]);
	outData[26] = mSegmentTable->numSegments();;
	mSegmentTable->rawData(&outData[27]);

	//EXIT POINT
	return true;
	

}

//ACESSORS
bool OggPageHeader::isBOS() {
	if ( (mHeaderFlags & eHeadFlags::BOS) != 0 ) {
		return true;
	} else {
		return false;
	}
}
bool OggPageHeader::isEOS() {
	if ( (mHeaderFlags & eHeadFlags::EOS) != 0 ) {
		return true;
	} else {
		return false;
	}
}


string OggPageHeader::toString() {

	string retStr =	"Ver No      : " + StringHelper::numToString((unsigned int)mStructureVersion) + "\n";
	retStr +=		"Head Flags  : " + StringHelper::numToString((unsigned int)mHeaderFlags) +"\n";
	retStr +=		"Granule Pos : " + StringHelper::numToString(mGranulePos) + "\n";
	retStr +=		"Serial No   : " + StringHelper::numToString(mStreamSerialNo) + "\n";
	retStr +=		"Seq No      : " + StringHelper::numToString(mPageSequenceNo) + "\n";
	retStr +=		"Checksum    : " + StringHelper::numToString(mCRCChecksum) + "\n";
	retStr +=		"Num Segs    : " + StringHelper::numToString((unsigned int)mSegmentTable->numSegments()) + "\n";
	retStr +=		"------------------------\n";
	retStr +=		"Head Size   : " + StringHelper::numToString(mHeaderSize) + "\n";
	retStr +=		"Data Size   : " + StringHelper::numToString(mDataSize) + "\n";
	retStr +=		"Page Size   : " + StringHelper::numToString(mPageSize) +"\n";
	
	return retStr;

}

void OggPageHeader::setPageState(ePageState inPageState) {
	mPageState = inPageState;
}
unsigned char OggPageHeader::StructureVersion() 
{
	return mStructureVersion;
}

unsigned char OggPageHeader::HeaderFlags() 
{
	return mHeaderFlags;
}

__int64 OggPageHeader::GranulePos()
{
	return mGranulePos;
}

unsigned long OggPageHeader::StreamSerialNo()
{
	return mStreamSerialNo;
}
unsigned long OggPageHeader::PageSequenceNo()
{
	return mPageSequenceNo;
}
unsigned long OggPageHeader::CRCChecksum()
{
	return mCRCChecksum;
}

unsigned char OggPageHeader::NumPageSegments()
{
	if (mSegmentTable == NULL) {
		return mNumPageSegments;
	} else {
		return mSegmentTable->numSegments();
	}
}
OggSegmentTable* OggPageHeader::SegmentTable()
{
	return mSegmentTable;
}



bool OggPageHeader::setBaseHeader(const unsigned char* inBaseHeader) {
	//This now does not delete the buffer
	bool locIsValidPage = true;
	unsigned long locOffset = 0;

	//Check if the page has the correct capture pattern
	if (strncmp((const char*)inBaseHeader, "OggS", OGG_CAPTURE_PATTERN_SIZE) == 0) {
		locIsValidPage = true;

		if (mPageState == BLANK) {
			locOffset += OGG_CAPTURE_PATTERN_SIZE;

			//Assign the structure version
			setStructureVersion(inBaseHeader[locOffset]);
			locOffset++;

			//Assign the header flags
			setHeaderFlags(inBaseHeader[locOffset]);
			locOffset++;

			//Assign the granule pos	
			setGranulePos((const unsigned char*)(inBaseHeader + locOffset));
			locOffset += 8;

			//Assign Serial No
			setStreamSerialNo((const unsigned char*)(inBaseHeader + locOffset));
			locOffset += 4;

			//Assign Page Seq No	
			setPageSequenceNo(inBaseHeader + locOffset);
			locOffset += 4;

			//Assign CheckSum
			setCRCChecksum(inBaseHeader + locOffset);
			locOffset += 4;

			//Assign Num Page Segments
			setNumPageSegments(inBaseHeader[locOffset]);
			locOffset++;

			//Set the size of the header
			setHeaderSize(OGG_BASE_HEADER_SIZE + mNumPageSegments);

			mPageState = BASE_HEAD_SET;

			//We are passed our own copy of this so we can delete it now.
			//delete inBaseHeader;
			
			//EXIT POINT
			return true;
			
		} else {
			//The page wasn't blank... so you can't set it's header or the data and header won't
			// match

			//EXIT POINT
			return false;

		}
		
	} else {
		locIsValidPage = false;
		return false;
	}



}

bool OggPageHeader::setSegmentTable(const unsigned char* inSegTable, unsigned char inNumSegs) {

	//This assumes that mNumPageSegments is set.
	//ISSUE ::: What happens when numPageSegments is zero ?
	if ( /*(mPageState == BASE_HEAD_SET) && */ (mSegmentTable == NULL)) {
		//Make a new segtable object
		OggSegmentTable* locSegTable = new OggSegmentTable;

		//Put the data in it and set the pagedata size
		unsigned long locDataSize = locSegTable->setSegmentTable(inSegTable, inNumSegs);
		setDataSize( locDataSize );
		
		//Assign the segtable into the page, the page header will look after deleing the memory.
		setSegmentTable(locSegTable);
		mPageState = FULL_HEAD_SET;
		return true;
		
		
	} else {
		//The page isn't being built right.	
		return false;
	}
}



void OggPageHeader::setStructureVersion(unsigned char inVal) 
{
	mStructureVersion = inVal;
}

void OggPageHeader::setHeaderFlags(unsigned char inVal) 
{
	mHeaderFlags = inVal;
}

void OggPageHeader::setGranulePos(__int64 inGranulePos)
{
	mGranulePos = inGranulePos;
}
void OggPageHeader::setGranulePos(const unsigned char* inPtr)
{
	mGranulePos = OggMath::CharArrToInt64(inPtr);
}

void OggPageHeader::setStreamSerialNo(unsigned long inVal)
{
	mStreamSerialNo = inVal;
}
void OggPageHeader::setStreamSerialNo(const unsigned char* inPtr)
{
	mStreamSerialNo = OggMath::charArrToULong(inPtr);
}
void OggPageHeader::setPageSequenceNo(unsigned long inVal)
{
	mPageSequenceNo = inVal;
}
void OggPageHeader::setPageSequenceNo(const unsigned char* inPtr)
{
	mPageSequenceNo = OggMath::charArrToULong(inPtr);;
}
void OggPageHeader::setCRCChecksum(unsigned long inVal)
{
	mCRCChecksum = inVal;
}
void OggPageHeader::setCRCChecksum(const unsigned char* inPtr)
{
	
	mCRCChecksum = OggMath::charArrToULong(inPtr);;
}

void OggPageHeader::setNumPageSegments(unsigned char inVal)
{
	if (mSegmentTable == NULL) {
		mNumPageSegments = inVal;
	}
}
void OggPageHeader::setSegmentTable(OggSegmentTable* inPtr)
{
	//Keeps your pointer !
	delete mSegmentTable;
	mSegmentTable = inPtr;
	
}

OggPageHeader::ePageState OggPageHeader::pageState() {
	return mPageState;
}
unsigned long OggPageHeader::pageSize()
{
	return mPageSize;
}
unsigned long OggPageHeader::headerSize()
{
	return mHeaderSize;
}
unsigned long OggPageHeader::dataSize()
{
	return mDataSize;
}

//unsigned long PageSize()
//{
//	return mPageSize;
//}
void OggPageHeader::setHeaderSize(unsigned long inVal)
{
	mHeaderSize = inVal;
	mPageSize = mHeaderSize + mDataSize;
}
void OggPageHeader::setDataSize(unsigned long inVal)
{
	mDataSize = inVal;
	mPageSize = mHeaderSize + mDataSize;
}

