/*
   Copyright (C) 2003 Zentaro Kavanagh
   
   Copyright (C) 2003 Commonwealth Scientific and Industrial Research
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
#include ".\c_importtag.h"

C_ImportTag::C_ImportTag(void)
{
	mTagType = C_CMMLTag::eTagType::IMPORT;
	mStart = "0";
	mParamList = new C_ParamTagList;
}

C_ImportTag::~C_ImportTag(void)
{
	delete mParamList;
}


//Accessors
string C_ImportTag::granuleRate() {
	return mGranuleRate;
}
string C_ImportTag::contentType() {
	return mContentType;
}
string C_ImportTag::src() {
	return mSrc;
}
string C_ImportTag::start() {
	return mStart;
}
string C_ImportTag::end() {
	return mEnd;
}
string C_ImportTag::title() {
	return mTitle;
}
C_ParamTagList* C_ImportTag::paramList() {
	return mParamList;
}

//Mutators
void C_ImportTag::setGranuleRate(string inGranuleRate) {
	mGranuleRate = inGranuleRate;
}
void C_ImportTag::setContentType(string inContentType) {
	mContentType = inContentType;
}
void C_ImportTag::setSrc(string inSrc) {
	mSrc = inSrc;
}
void C_ImportTag::setStart(string inStart) {
	mStart = inStart;
}
void C_ImportTag::setEnd(string inEnd) {
	mEnd = inEnd;
}
void C_ImportTag::setTitle(string inTitle) {
	mTitle = inTitle;
}
void C_ImportTag::setParamList(C_ParamTagList* inParamList) {
	delete mParamList;
	mParamList = inParamList;
}

//Other

void C_ImportTag::privateClone(C_CMMLTag* outTag) {

	C_HumReadCMMLTag::privateClone(outTag);
	C_ImportTag* locTag = reinterpret_cast<C_ImportTag*>(outTag);
	locTag->mGranuleRate = mGranuleRate;
	locTag->mContentType = mContentType;
	locTag->mSrc = mSrc;
	locTag->mStart = mStart;
	locTag->mEnd = mEnd;
	locTag->mTitle = mTitle;
	locTag->setParamList(mParamList->clone());
}
C_ImportTag* C_ImportTag::clone() {
	C_ImportTag* retTag = new C_ImportTag;
	privateClone(retTag);
	return retTag;

}

C_CMMLTag* C_ImportTag::genericClone() {
	return clone();
}
string C_ImportTag::toString() {
	
	string retStr;
	retStr = "<import";
	
	if (mId.size() != 0) {
		retStr += makeElement("id", mId);
	}

	if (mGranuleRate.size() != 0) {
		retStr += makeElement("granulerate", mGranuleRate);
	}

	if (mContentType.size() != 0) {
		retStr += makeElement("contenttype", mContentType);
	}

	retStr += makeElement("src", mSrc);
	retStr += makeElement("start", mStart);
	if (mEnd.size() != 0) {
		retStr += makeElement("end", mEnd);
	}

	if (mTitle.size() != 0) {
		retStr += makeElement("title", mTitle);
	}

	retStr += ">\n";

	retStr += mParamList->toString();

	retStr += "</import>\n";
	return retStr;

}

	