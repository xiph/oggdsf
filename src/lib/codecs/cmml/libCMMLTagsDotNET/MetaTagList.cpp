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
#include ".\metataglist.h"
#using <mscorlib.dll>

namespace illiminable {
namespace libCMMLTagsDotNET {

	MetaTagList::MetaTagList(void)
	{
		mBaseClass = new C_MetaTagList;
	}

	MetaTagList::MetaTagList(C_MetaTagList* inList, bool inDeleteBase)
	{
		mBaseClass = inList;
		mDeleteBase = inDeleteBase;
	}

	MetaTagList::~MetaTagList(void)
	{
		if (mDeleteBase) {
			delete mBaseClass;
		}
		mBaseClass = NULL;
	}

	void MetaTagList::emptyList() {
		getMe()->emptyList();
	}


	void MetaTagList::addTag(MetaTag* inTag) {
		getMe()->addTag(inTag->getMe()->clone());

	}
	void MetaTagList::addTag(String* inName, String* inContent) {
		wchar_t* tc1 = Wrappers::netStrToWStr( inName );
		wchar_t* tc2 = Wrappers::netStrToWStr( inContent );

		wstring locStr1 = tc1;
		wstring locStr2 = tc2;
		
		getMe()->addTag(locStr1, locStr2);

		Wrappers::releaseWStr( tc2 );
		Wrappers::releaseWStr( tc1 );
	}
	//void removeTag ???
	unsigned long MetaTagList::numTags() {
		return getMe()->numTags();

	}

	MetaTag* MetaTagList::getTag(unsigned long inTagNo) {
		return new MetaTag(getMe()->getTag(inTagNo), false);
	}
	MetaTag* MetaTagList::getTag(String* inName) {
		wchar_t* tc = Wrappers::netStrToWStr( inName );
		wstring locStr = tc;
		MetaTag* retVal = new MetaTag(getMe()->getTag(locStr), false);
		Wrappers::releaseWStr( tc );
		return retVal;

	}

	String* MetaTagList::getContent(String* inName) {
		wchar_t* tc = Wrappers::netStrToWStr( inName );
		wstring locStr = tc;
		String* ts = Wrappers::WStrToNetStr(getMe()->getContent(locStr).c_str());
		Wrappers::releaseWStr( tc );
		return ts;
	}

	String* MetaTagList::toString() {
		return Wrappers::WStrToNetStr(getMe()->toString().c_str());
	}


	C_MetaTagList* MetaTagList::getMe() {
		return (C_MetaTagList*)mBaseClass;
	}

}
}
