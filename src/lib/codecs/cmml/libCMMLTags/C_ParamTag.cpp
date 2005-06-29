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
#include <libCMMLTags/C_ParamTag.h>

C_ParamTag::C_ParamTag(void)
{
}

C_ParamTag::~C_ParamTag(void)
{
}

wstring C_ParamTag::toString() {
	wstring retStr;
	retStr = L"<param";
	if (mId.size() != 0) {
		retStr += makeAttribute(L"id", mId);
	}

	retStr += makeLangElements();
	retStr += makeAttribute(L"name", mName);
	retStr += makeAttribute(L"value", mContent);
	retStr += L"/>\r\n";
	return retStr;

}

void C_ParamTag::privateClone(C_CMMLTag* outTag) {
	C_MappedTag::privateClone(outTag);	
}
C_ParamTag* C_ParamTag::clone() {
	C_ParamTag* retTag = new C_ParamTag;
	privateClone(retTag);
	return retTag;

}
C_MappedTag* C_ParamTag::mappedClone() {
	return clone();
}
C_CMMLTag* C_ParamTag::genericClone() {
	return clone();
}
