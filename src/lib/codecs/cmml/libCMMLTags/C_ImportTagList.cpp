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
#include ".\c_importtaglist.h"

C_ImportTagList::C_ImportTagList(void)
{
}

C_ImportTagList::~C_ImportTagList(void)
{
	//The tags are deleted from the base class
}


wstring C_ImportTagList::toString() {
	wstring retStr;

	for (unsigned long i = 0; i < mTagList.size(); i++) {
		retStr += mTagList[i]->toString();
	}

	return retStr;
}

void C_ImportTagList::addTag(C_ImportTag* inTag) {
	C_TagList::addTag(inTag);
}
C_ImportTag* C_ImportTagList::getTag(unsigned long inTagNo) {
	//TO DO::: Should be dynamic casts
	return (C_ImportTag*)C_TagList::getTag(inTagNo);
}

void C_ImportTagList::privateClone(C_TagList* outTagList) {
	
	C_TagList::privateClone(outTagList);
}

C_ImportTagList* C_ImportTagList::clone() {
	C_ImportTagList* retList = new C_ImportTagList;
	privateClone(retList);
	return retList;

}