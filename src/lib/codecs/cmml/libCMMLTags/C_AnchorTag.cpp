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
#include ".\c_anchortag.h"

//Constructors
C_AnchorTag::C_AnchorTag(void)
{
	//Set the tag type
	mTagType = C_CMMLTag::eTagType::ANCHOR;
}

C_AnchorTag::~C_AnchorTag(void)
{
}
C_AnchorTag* C_AnchorTag::clone() {
	C_AnchorTag* retTag = new C_AnchorTag;
	privateClone(retTag);
	return retTag;

}
C_CMMLTag* C_AnchorTag::genericClone() {
	return clone();
}

//Accessors
wstring C_AnchorTag::cls() {
	return mCls;
}
wstring C_AnchorTag::href() {
	return mHref;
}

//Mutators
void C_AnchorTag::setCls(wstring inCls)  {
	mCls = inCls;
}
void C_AnchorTag::setHref(wstring inHref)  {
	mHref = inHref;
}

//Other
wstring C_AnchorTag::toString()  {
	 

	wstring retStr =	L"<a";
	//Put in the id element if there is one
	if (mId.size() != 0) {
		retStr += makeElement(L"id", mId);
	}

	retStr += makeLangElements();

	if (mCls.size() != 0) {
		retStr += makeElement(L"class", mCls);
	}

	retStr += makeElement(L"href", mHref);

	retStr += L">";
	retStr += mText;
	retStr += L"</a>\n";
	
	return retStr;
}

void C_AnchorTag::privateClone(C_CMMLTag* outTag) {
	C_TextFieldTag::privateClone(outTag);
	C_AnchorTag* locTag = reinterpret_cast<C_AnchorTag*>(outTag);
	locTag->mCls = mCls;
	locTag->mHref = mHref;
}
