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
#include "oggsegmenttable.h"

OggSegmentTable::OggSegmentTable()
{	
	mSegmentTable = NULL;
	mNumSegments = 0;
}

OggSegmentTable::~OggSegmentTable(void)
{
	delete mSegmentTable;
}

OggSegmentTable* OggSegmentTable::clone() {
	OggSegmentTable* retVal = new OggSegmentTable;
	if ( (mSegmentTable != NULL) && (mNumSegments > 0)) {
		retVal->mSegmentTable = new unsigned char[mNumSegments];
		memcpy((void*)retVal->mSegmentTable, (const void*) mSegmentTable, mNumSegments);
	} else {
		//Error ??
	}
	return retVal;
}


unsigned long OggSegmentTable::setSegmentTable(unsigned char* inSegTable, unsigned char inNumSegments) 
{
	//Delete any previous table
	delete mSegmentTable;

	mSegmentTable = inSegTable;
	//No longer making copy, assuming the pointer passed in is ours
	//
	////Assign the segment table
	//mSegmentTable = new unsigned char[inNumSegments];
	////Copy the data across
	//memcpy((void*) mSegmentTable, (const void*)inSegTable, inNumSegments);
	mNumSegments = inNumSegments;


	//REVISION:::13/03/04 - Also calculate the number and offsets of packets in this page.
	//Determine the amount of data the segment table represents
	unsigned long retDataSize = 0;
	for (int i = 0; i < inNumSegments; i++) {
		retDataSize += inSegTable[i];
	}

	//Return the size of the data the segment represents
	return retDataSize;

}

void OggSegmentTable::rawData(unsigned char* outData) {
	for( unsigned char i = 0; i < mNumSegments; i++) {
		outData[i] = mSegmentTable[i];
	}
}
unsigned char* OggSegmentTable::segmentTable() {
	return mSegmentTable;
}
unsigned char OggSegmentTable::numSegments() {
	return mNumSegments;
}
