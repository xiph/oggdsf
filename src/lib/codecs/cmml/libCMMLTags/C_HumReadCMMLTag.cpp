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
#include <libCMMLTags/C_HumReadCMMLTag.h>

C_HumReadCMMLTag::C_HumReadCMMLTag(void)
{
}

C_HumReadCMMLTag::~C_HumReadCMMLTag(void)
{
}

//Accessors
wstring C_HumReadCMMLTag::lang() {
	return mLang;
}
wstring C_HumReadCMMLTag::dirn() {
	return mDirn;
}

//Mutators
void C_HumReadCMMLTag::setLang(wstring inLang) {
	mLang = inLang;
}
void C_HumReadCMMLTag::setDirn(wstring inDirn) {
	mDirn = inDirn;
}

//Protected Helper Methods
void C_HumReadCMMLTag::privateClone(C_CMMLTag* outTag) {
	C_CMMLTag::privateClone(outTag);
	C_HumReadCMMLTag* locTag = reinterpret_cast<C_HumReadCMMLTag*>(outTag);
	locTag->setLang(mLang);
	locTag->setDirn(mDirn);
}

wstring C_HumReadCMMLTag::makeLangElements() {
	wstring retStr = L"";

	if (mLang.size() != 0) {
		retStr += makeElement(L"lang", mLang);
	}

	if (mDirn.size() != 0) {
		retStr += makeElement(L"dir", mDirn);
	}

	return retStr;
}
