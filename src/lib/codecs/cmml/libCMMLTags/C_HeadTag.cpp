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
#include ".\c_headtag.h"

C_HeadTag::C_HeadTag(void)
{
	mTagType = C_CMMLTag::eTagType::HEAD;
	mMetaList  = new C_MetaTagList;
	mTitle = new C_TitleTag;
	mBase = NULL;
}

C_HeadTag::~C_HeadTag(void)
{
	delete mTitle;
	delete mBase;
	delete mMetaList;

}

//Accessors
wstring C_HeadTag::profile() {
	return mProfile;
}
C_TitleTag* C_HeadTag::title() {
	return mTitle;
}
C_BaseTag* C_HeadTag::base() {
	return mBase;
}
C_MetaTagList* C_HeadTag::metaList() {
	return mMetaList;
}
//Mutators
void C_HeadTag::setProfile(wstring inProfile) {
	mProfile = inProfile;
}
void C_HeadTag::setTitle(C_TitleTag* inTitle) {
	delete mTitle;
	mTitle = inTitle;
}
void C_HeadTag::setBase(C_BaseTag* inBase) {
	delete mBase;
	mBase = inBase;
}
//Other

void C_HeadTag::privateClone(C_CMMLTag* outTag) {
	C_HumReadCMMLTag::privateClone(outTag);
	C_HeadTag* locTag = reinterpret_cast<C_HeadTag*>(outTag);
	locTag->setProfile(mProfile);
	//locTag->mTitle = mTitle;
	if (mBase != NULL) {
		locTag->setBase(mBase->clone());
	}
	locTag->setTitle(mTitle->clone());
}
C_CMMLTag* C_HeadTag::genericClone() {
	return clone();
}
C_HeadTag* C_HeadTag::clone() {
	C_HeadTag* retTag = new C_HeadTag;
	privateClone(retTag);
	return retTag;

}


wstring C_HeadTag::toString() {
	//TO DO::: Optional tags
	wstring retStr = L"<head";

	if (mId.size() != 0) {
		retStr += L" id=\"";
		retStr += mId;
		retStr += L"\"";
	}

	retStr += makeLangElements();

	if (mProfile.size() != 0) {
		retStr += L" profile=\"";
		retStr += mProfile;
		retStr += L"\"";
	}
	retStr+= L">\n";
	
	if (mBase != NULL)  {
		retStr += mBase->toString();
	}

	retStr += mTitle->toString();
	retStr += mMetaList->toString();
	retStr += L"</head>\n\n";
	return retStr;
}
