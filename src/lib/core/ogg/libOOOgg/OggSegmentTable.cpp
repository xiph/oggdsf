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
	:	mSegmentTable(NULL)
	,	mNumSegments(0)
{	

}

OggSegmentTable::~OggSegmentTable(void)
{
	delete mSegmentTable;
}

OggSegmentTable* OggSegmentTable::clone() {

	//Creates a new Segment table which is a deep copy of this one.
	OggSegmentTable* retVal = new OggSegmentTable;
	if ( (mSegmentTable != NULL) && (mNumSegments > 0)) {
		//Make a new buffer in the returning class
		retVal->mSegmentTable = new unsigned char[mNumSegments];

		//Copy this segment table into the ne one.
		memcpy((void*)retVal->mSegmentTable, (const void*) mSegmentTable, mNumSegments);
		retVal->mNumSegments = mNumSegments;

		return retVal;
	} else {
		//Error ??
		delete retVal;
		return NULL;
	}
}

unsigned long OggSegmentTable::calculateDataSize() {

		//Sums the bytes in the segment table to calculate the size of data.
		//FIX::: ??? No checks on pointers.

		unsigned long retDataSize = 0;
		for (int i = 0; i < mNumSegments; i++) {
			retDataSize += mSegmentTable[i];
		}

		return retDataSize;
}
unsigned long OggSegmentTable::setSegmentTable(const unsigned char* inSegTable, unsigned char inNumSegments) 
{
	//Copies the buffer.
	if (inNumSegments != 0) {
		if (inSegTable == NULL) {
			//do nothing if pointer was null and numsegments is not 0.
			return 0;
		}

		//Delete any previous table
		delete mSegmentTable;
		mSegmentTable = NULL;

		//Make a new buffer.
		unsigned char* locSegTablePtr = new unsigned char[inNumSegments];
		
		//SLOW:::
		//Copy the incoming data into the new segemnt buffer
		memcpy((void*)locSegTablePtr, (const void*)inSegTable, inNumSegments);
		mSegmentTable = locSegTablePtr;
		
		mNumSegments = inNumSegments;
        
		//Return the size of the data the segment represents
		return calculateDataSize();
	} else {
		//Numsegments is zero.

		if (inSegTable == NULL) {
			//If num segments is null and the segtable is null we set delete the classes segment table and null it.
			delete mSegmentTable;
			mSegmentTable = NULL;
			return 0;
		} else {
			//Do nothing if inNumSegments is 0
			return 0;
		}
		
	}

}

void OggSegmentTable::rawData(unsigned char* outData) {
	//Must be a preprepared buffer at least mNumSegments. Does no error chceking.
	for( unsigned char i = 0; i < mNumSegments; i++) {
		outData[i] = mSegmentTable[i];
	}
}
unsigned char* OggSegmentTable::segmentTable() {
	//Returns a pointer to the internal segment table... 
	return mSegmentTable;
}
unsigned char OggSegmentTable::numSegments() {
	//Number off segments in the table.
	return mNumSegments;
}
