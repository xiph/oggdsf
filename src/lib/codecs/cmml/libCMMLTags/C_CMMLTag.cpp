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
#include <libCMMLTags/C_CMMLTag.h>

C_CMMLTag::C_CMMLTag(void)
{
}

C_CMMLTag::~C_CMMLTag(void)
{
}

//Accessors
wstring C_CMMLTag::id() {
	return mId;
}

//Mutators
void C_CMMLTag::setId(wstring inId) {
	mId = inId;
}

//Protected Helper Methods
wstring C_CMMLTag::makeAttribute(wstring inElemName, wstring inElemContent) {
	if (inElemContent != L"") {
		wstring retStr;
		retStr = L" " + inElemName + L"=\"" + inElemContent + L"\"";
		return retStr;
	} else {
		return L"";
	}
}

wstring C_CMMLTag::makeRequiredAttribute(wstring inElemName, wstring inElemContent) {
	wstring retStr;
	retStr = L" " + inElemName + L"=\"" + inElemContent + L"\"";
	return retStr;
}


wstring C_CMMLTag::replaceAll(wstring inOriginal, wchar_t inReplaceThis, wstring inWithThis)
{
    size_t locPos = 0;
	
	while ((locPos = inOriginal.find(inReplaceThis,locPos)) != wstring::npos)
    {
        inOriginal.replace( locPos++, 1, inWithThis );
    }
	return inOriginal;

}

//Character Name 				Entity Reference 	
//Ampersand (&)		 			&amp; 				
//Left angle bracket (<)	 	&lt; 				
//Right angle bracket (>)	 	&gt; 	
//Straight quotation mark (")	&quot;
//Apostrophe (')				&apos;

wstring C_CMMLTag::escapeEntities(wstring inString)
{
	wstring retStr = inString;
	//Do the ampersand first !!
	retStr = replaceAll(retStr, L'&', L"&amp;");
	retStr = replaceAll(retStr, L'<', L"&lt;");
	retStr = replaceAll(retStr, L'>', L"&gt;");
	retStr = replaceAll(retStr, L'"', L"&quot;");
	retStr = replaceAll(retStr, L'\'', L"&apos;");

	return retStr;

}


void C_CMMLTag::privateClone(C_CMMLTag* outTag) {
	outTag->setId(mId);	
}

C_CMMLTag::eTagType C_CMMLTag::tagType() {
	return mTagType;
}

