/*
   Copyright (C) 2003, 2004 Zentaro Kavanagh
   
   Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
   Organisation (CSIRO) Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of CSIRO Australia nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stdafx.h"

#include <libCMMLTags/C_ClipTagList.h>
#include <libilliCore/StringHelper.h>
#include <libTemporalURI/C_TimeStamp.h>

#include <fstream>

#include <assert.h>

C_ClipTagList::C_ClipTagList(void)
{
}

C_ClipTagList::~C_ClipTagList(void)
{
	//Everything is deleted by the base class
}


wstring C_ClipTagList::toString() {
	wstring retStr = L"";
	for (unsigned long i = 0; i < mTagList.size(); i++) {
		retStr += mTagList[i]->toString();
	}
	return retStr;
}

void C_ClipTagList::addTag(C_ClipTag* inTag) {
	C_TagList::addTag(inTag);

}
C_ClipTag* C_ClipTagList::getTag(unsigned long inTagNo) {
	return (C_ClipTag*) C_TagList::getTag(inTagNo);
}

void C_ClipTagList::privateClone(C_TagList* outTagList) {
	C_TagList::privateClone(outTagList);	


}

C_ClipTagList* C_ClipTagList::clone() {
	C_ClipTagList* retList = new C_ClipTagList;
	privateClone(retList);
	return retList;
}

/** Note that the returned C_ClipTagList must be deleted by you.
  */
C_ClipTagList *C_ClipTagList::getClipsFrom(LOOG_INT64 inTimeInDirectShowUnits)
{
	C_ClipTagList *locClipTagList = new C_ClipTagList;

	bool locAlreadyEncounteredClipInTimeRange = false;

	for (unsigned long i = 0; i < numTags(); i++) {
		C_ClipTag *locTag = getTag(i);
		
		// Convert the time stamp from a string to time in DirectSeconds(TM)
		wstring locStart = locTag->start();
		C_TimeStamp locTimeStamp;
		if (!locTimeStamp.parseTimeStamp(StringHelper::toNarrowStr(locStart))) {
			// Mmm, couldn't parse the time stamp for this clip ... so, err,
			// let's just skip it.  Yeah, that's a grreeeeat idea ...
			continue;
		}
		LOOG_INT64 locStartTime = locTimeStamp.toHunNanos();

		if (locStartTime >= inTimeInDirectShowUnits) {
			if (!locAlreadyEncounteredClipInTimeRange) {
				// Only add the previous clip to the clip list if its end time
				// is beyond the requested time
				{
					wstring locEnd = locTag->end();
					C_TimeStamp locEndTimeStamp;
					if (locEndTimeStamp.parseTimeStamp(StringHelper::toNarrowStr(locEnd))) {
						// Clip has an end time: check if it's before the requested time
						LOOG_INT64 locEndTime = locEndTimeStamp.toHunNanos();
						if (locEndTime < inTimeInDirectShowUnits) {
							continue;
						}
					}
				}

				// If we're not the very first clip ...
				if (i > 0) {
					C_ClipTag *locClipInTimeRange = getTag(i - 1);
					locClipTagList->addTag(locClipInTimeRange->clone());
				}
				locAlreadyEncounteredClipInTimeRange = true;
			}
			locClipTagList->addTag(locTag->clone());
		}
	}

	return locClipTagList;
}
