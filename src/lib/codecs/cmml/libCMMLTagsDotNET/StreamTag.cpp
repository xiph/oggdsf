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
#include ".\streamtag.h"

namespace illiminable {
namespace libCMMLTagsDotNET {


	StreamTag::StreamTag(void)
	{
		mBaseClass = new C_StreamTag;
	}

	StreamTag::StreamTag(C_StreamTag* inTag)
	{
	
		mBaseClass = inTag;
	}

	StreamTag::~StreamTag(void)
	{
		delete mBaseClass;
	}

			//Accessors
	String* StreamTag::timebase() {
		return Wrappers::WStrToNetStr( getMe()->timebase().c_str() );
	}
	String* StreamTag::utc() {
		return Wrappers::WStrToNetStr( getMe()->utc().c_str() );
	}
	ImportTagList* StreamTag::importList() {
		return new ImportTagList( getMe()->importList()->clone());

	}

			//Mutators
	void StreamTag::setTimebase(String* inTimebase) {
		wchar_t* tc = Wrappers::netStrToWStr( inTimebase );
		getMe()->setTimebase( tc );
		Wrappers::releaseWStr( tc );		

	}
	void StreamTag::setUtc(String* inUtc) {
		wchar_t* tc = Wrappers::netStrToWStr( inUtc );
		getMe()->setUtc( tc );
		Wrappers::releaseWStr( tc );
	}
	void StreamTag::setImportList(ImportTagList* inTagList) {
		getMe()->setImportList(inTagList->getMe()->clone());
	}

			//Other
	String* StreamTag::toString() {
		return Wrappers::WStrToNetStr( getMe()->toString().c_str());

	}
	C_StreamTag* StreamTag::getMe() {
		return (C_StreamTag*) mBaseClass;

	}

}
}
