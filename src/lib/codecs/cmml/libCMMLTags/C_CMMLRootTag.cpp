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
#include ".\c_cmmlroottag.h"

C_CMMLRootTag::C_CMMLRootTag(void)
{
	mStream = NULL;
	mHead = new C_HeadTag;
	mClipList = new C_ClipTagList;
}

C_CMMLRootTag::~C_CMMLRootTag(void)
{
	
	delete mStream;
	delete mHead;
	delete mClipList;
}


C_StreamTag* C_CMMLRootTag::stream() {
	return mStream;
}
C_HeadTag* C_CMMLRootTag::head() {
	return mHead;
}
C_ClipTagList* C_CMMLRootTag::clipList() {
	return mClipList;
}

void C_CMMLRootTag::setClipList(C_ClipTagList* inClipList) {
	delete mClipList;
	mClipList = inClipList;
}

void C_CMMLRootTag::setStream(C_StreamTag* inStreamTag) {
	delete mStream;
	mStream = inStreamTag;

}
void C_CMMLRootTag::setHead(C_HeadTag* inHeadTag) {
	//Delete the existing one
	delete mHead;
	mHead = inHeadTag;
}

void C_CMMLRootTag::privateClone(C_CMMLTag* outTag) {
	C_HumReadCMMLTag::privateClone(outTag);
	C_CMMLRootTag* locTag = reinterpret_cast<C_CMMLRootTag*>(outTag);
	if (mStream != NULL) {
		locTag->setStream(mStream->clone());
	}
	locTag->setHead(mHead->clone());
	locTag->setClipList(mClipList->clone());
	

}
C_CMMLTag* C_CMMLRootTag::genericClone() {
	return clone();
}

C_CMMLRootTag* C_CMMLRootTag::clone() {
	C_CMMLRootTag* retTag = new C_CMMLRootTag;
	privateClone(retTag);
	return retTag;

}

wstring C_CMMLRootTag::toString() {
	wstring retStr;
	retStr = L"<cmml";

	retStr += makeElement(L"id", mId);
	retStr += makeLangElements();
	//TODO::: This shouldn't be hardcoded here !
	retStr += makeElement(L"xmlns", L"http://www.annodex.net/cmml");

	
	retStr += L">\n";
	if (mStream != NULL) {
		retStr += mStream->toString();
	}

	retStr += mHead->toString();
	retStr += mClipList->toString();
	retStr += L"</cmml>\n";
	return retStr;
}
