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
#include ".\c_cliptag.h"

//TO DO ::: track element
C_ClipTag::C_ClipTag(void)
{
	mTagType = C_CMMLTag::eTagType::CLIP;

	mMetaList = new C_MetaTagList;
	mAnchor = NULL;
	mImage = NULL;
	mDesc = NULL;
	mStart = L"0";
}

C_ClipTag::~C_ClipTag(void)
{
	delete mAnchor;
	delete mImage;
	delete mDesc;
	delete mMetaList;
}

//Accessors
wstring C_ClipTag::track() {
	return mTrack;
}
C_MetaTagList* C_ClipTag::metaList() {
	return mMetaList;
}
C_AnchorTag* C_ClipTag::anchor() {
	return mAnchor;
}
C_ImageTag* C_ClipTag::image() {
	return mImage;
}
C_DescTag* C_ClipTag::desc() {
	return mDesc;
}

wstring C_ClipTag::start() {
	return mStart;
}
wstring C_ClipTag::end() {
	return mEnd;
}
//Mutators
void C_ClipTag::setTrack(wstring inTrack) {
	mTrack = inTrack;
}
void C_ClipTag::setAnchor(C_AnchorTag* inAnchor) {
	delete mAnchor;
	mAnchor = inAnchor;
}
void C_ClipTag::setImage(C_ImageTag* inImage) {
	delete mImage;
	mImage = inImage;
}
void C_ClipTag::setDesc(C_DescTag* inDesc) {
	delete mDesc;
	mDesc = inDesc;
}

void C_ClipTag::setStart(wstring inStart) {
	if (inStart != L"") {
		mStart = inStart;
	}
}
void C_ClipTag::setEnd(wstring inEnd) {
	mEnd = inEnd;
}

//Others

C_ClipTag* C_ClipTag::clone() {
	C_ClipTag* retTag = new C_ClipTag;
	privateClone(retTag);
	return retTag;

}
C_CMMLTag* C_ClipTag::genericClone() {
	return clone();
}

void C_ClipTag::privateClone(C_CMMLTag* outTag) {
	C_HumReadCMMLTag::privateClone(outTag);
	C_ClipTag* locTag = reinterpret_cast<C_ClipTag*>(outTag);
	locTag->mTrack = mTrack;
	locTag->setAnchor(mAnchor->clone());
	locTag->setImage(mImage->clone());
	locTag->setDesc(mDesc->clone());
	locTag->setStart(mStart);
	locTag->setEnd(mEnd);
}
wstring C_ClipTag::toString() {
	wstring retStr = L"<clip";

	//TO DO::: Language data ???

	//Id element
	if (mId != L"") {
		retStr += makeElement(L"id", mId);
	}

	//track Element
	if (mTrack != L"") {
		retStr += makeElement(L"track", mTrack);
	}

	//TO DO::: Sort out what to do about start and end
	retStr += makeElement(L"start", mStart);

	if (mEnd != L"") {
		retStr += makeElement(L"end", mEnd);
	}

	retStr += L">\n";

	if (mAnchor != NULL) {
		retStr += mAnchor->toString();
	}
	if (mImage != NULL) {
	    retStr += mImage->toString();
	}
	if (mDesc != NULL) {
		retStr += mDesc->toString();
	}

	retStr += mMetaList->toString();
	retStr += L"</clip>\n\n";
	return retStr;
}