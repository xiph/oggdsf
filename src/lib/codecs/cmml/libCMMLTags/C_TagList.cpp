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
#include "StdAfx.h"
#include ".\c_taglist.h"

C_TagList::C_TagList(void)
{
}

C_TagList::~C_TagList(void)
{
	//Empty the tag list
	for (unsigned long i = 0; i < mTagList.size(); i++) {
		delete mTagList[i];
	}
	
	mTagList.clear();
}

void C_TagList::addTag(C_CMMLTag* inTag) {
	//Fix for sorted list
	mTagList.push_back(inTag);
}
unsigned long C_TagList::numTags() {
	return (unsigned long)mTagList.size();
}

C_CMMLTag* C_TagList::getTag(unsigned long inTagNo) {
	//Error check index
	if ( (inTagNo < mTagList.size()) && (mTagList.size() > 0)) {
		return mTagList[inTagNo];
	} else {
		//ISSUE :: Or throw exception ??
		return NULL;
	}
}

wstring C_TagList::toString() {
	//Return the CMML of the list of tags appended together
	wstring retStr = L"";
	
	for (unsigned long i = 0; i < mTagList.size(); i++) {
		retStr.append(mTagList[i]->toString());
	}
	return retStr;
}

void C_TagList::privateClone(C_TagList* outTagList) {
	for (int i = 0; i < mTagList.size(); i++) {
		outTagList->addTag(mTagList[i]->genericClone());
	}

}