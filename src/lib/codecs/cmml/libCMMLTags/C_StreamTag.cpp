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
#include ".\c_streamtag.h"

C_StreamTag::C_StreamTag(void)
{
	mTagType = C_CMMLTag::eTagType::STREAM;
	mImportList = new C_ImportTagList;
	mTimebase = L"0";
}

C_StreamTag::~C_StreamTag(void)
{
	delete mImportList;
}

//Accessors
wstring C_StreamTag::timebase() {
	return mTimebase;
}
wstring C_StreamTag::utc() {
	return mUtc;
}
C_ImportTagList* C_StreamTag::importList() {
	return mImportList;
}

//Mutators
void C_StreamTag::setTimebase(wstring inTimebase) {
	mTimebase = inTimebase;
}
void C_StreamTag::setUtc(wstring inUtc) {
	mUtc = inUtc;
}

void C_StreamTag::setImportList(C_ImportTagList* inTagList) {
	delete mImportList;
	mImportList = inTagList;
}

//Other

void C_StreamTag::privateClone(C_CMMLTag* outTag) {
	C_CMMLTag::privateClone(outTag);
	C_StreamTag* locTag = reinterpret_cast<C_StreamTag*>(outTag);
	locTag->setUtc(mUtc);
	locTag->setTimebase(mTimebase);
	locTag->setImportList(mImportList->clone());

}

C_StreamTag* C_StreamTag::clone() {
	C_StreamTag* retTag = new C_StreamTag;
	privateClone(retTag);
	return retTag;

}
C_CMMLTag* C_StreamTag::genericClone() {
	return clone();
}
wstring C_StreamTag::toString() {
	//FIX ::: Make this do something
	wstring retStr;

	retStr = L"<stream";
	if (mId.size() != 0) {
		retStr += makeElement(L"id", mId);
	}
	retStr += makeElement(L"timebase", mTimebase);
	if (mUtc.size() != 0) {
		retStr += makeElement(L"utc", mUtc);
	}
	retStr += L">\n";
	retStr += mImportList->toString();
	retStr += L"</stream>\n\n";

	return retStr;
}
