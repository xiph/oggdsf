/*
   Copyright (C) 2003 Zentaro Kavanagh
   
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
#include ".\c_basetag.h"

C_BaseTag::C_BaseTag(void)
{
	mTagType = C_CMMLTag::eTagType::BASE;
}

C_BaseTag::~C_BaseTag(void)
{
}

//Accessors
wstring C_BaseTag::href() {
	return mHref;
}

//Mutators
void C_BaseTag::setHref(wstring inHref) {
	mHref = inHref;
}

//Other
C_BaseTag* C_BaseTag::clone() {
	C_BaseTag* retTag = new C_BaseTag;
	privateClone(retTag);
	return retTag;
}

C_CMMLTag* C_BaseTag::genericClone() {
	return clone();
}
wstring C_BaseTag::toString() {
	//FIX ::: Make this do something
	wstring retStr = L"<base";

	if (mId.size() != 0) {
		retStr += makeElement(L"id", mId);
	}

	retStr += makeElement(L"href", mHref);
	retStr += L"/>\n";
	return retStr;
}

void C_BaseTag::privateClone(C_CMMLTag* outTag) {
	C_CMMLTag::privateClone(outTag);
	((C_BaseTag*)outTag)->mHref = mHref;
}