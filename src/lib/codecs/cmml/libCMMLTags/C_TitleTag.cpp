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
#include <libCMMLTags/C_TitleTag.h>

C_TitleTag::C_TitleTag(void)
{
	mTagType = C_CMMLTag::TITLE;
}

C_TitleTag::~C_TitleTag(void)
{
}

wstring C_TitleTag::toString() {
	//FIX ::: Make this do something
	wstring retStr = L"<title";

	if (mId.size() != 0) {
		retStr += makeAttribute(L"id", mId);
	}

	retStr += makeLangElements();
	retStr += L">";
	retStr += escapeEntities(mText);
	retStr+= L"</title>\n";
	return retStr;
}
void C_TitleTag::privateClone(C_CMMLTag* outTag) {
	C_TextFieldTag::privateClone(outTag);
}
C_TitleTag* C_TitleTag::clone() {
	C_TitleTag* retTag = new C_TitleTag;
	privateClone(retTag);
	return retTag;

}
C_CMMLTag* C_TitleTag::genericClone() {
	return clone();
}
