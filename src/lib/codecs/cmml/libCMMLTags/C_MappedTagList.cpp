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
#include ".\c_mappedtaglist.h"

C_MappedTagList::C_MappedTagList(void)
{
}

C_MappedTagList::~C_MappedTagList(void)
{
	emptyList();
}

void C_MappedTagList::emptyList() {
	for (unsigned long i = 0; i < mTagList.size(); i++) {
		delete mTagList[i];
	}
	mTagList.clear();
}

void C_MappedTagList::addTag(C_MappedTag* inTag) {
	//Fix for sorted list
	mTagList.push_back(inTag);
}

//void C_MappedTagList::addTag(wstring inName, wstring inContent) {
//	C_MappedTag* locMappedTag = new C_MappedTag(inName, inContent);
//	addTag(locMappedTag);
//}

unsigned long C_MappedTagList::numTags() {
	return (unsigned long)mTagList.size();
}	

C_MappedTag* C_MappedTagList::getTag(unsigned long inTagNo) {
	//Error check index
	if ( (inTagNo < mTagList.size()) && (mTagList.size() > 0)) {
		return mTagList[inTagNo];
	} else {
		//ISSUE :: Or throw exception ??
		return NULL;
	}
}
C_MappedTag* C_MappedTagList::getTag(wstring inName) {
	unsigned long i = 0;
	unsigned long locSize = (unsigned long)mTagList.size();

	//while there is more items in the list and we haven't found what we are after
	while ( i < locSize ) {
		if (inName == mTagList[i]->name()) {
			//EXIT POINT
			return mTagList[i];
		}
		i++;
	}
	return NULL;
}

wstring C_MappedTagList::getContent(wstring inName) {
	return getTag(inName)->name();
}

wstring C_MappedTagList::toString() {
	//FIX ::: Make this do something
	wstring retStr = L"";
	return retStr;
}

void C_MappedTagList::privateClone(C_MappedTagList* outTagList) {
	for(int i = 0; i < mTagList.size(); i++) {
		outTagList->addTag(mTagList[i]->mappedClone());

	}
}