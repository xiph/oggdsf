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
#include ".\importtag.h"



namespace illiminable {
namespace libCMMLTagsDotNET {

	ImportTag::ImportTag(void)
	{
		mBaseClass = new C_ImportTag;
	}

	ImportTag::ImportTag(C_ImportTag* inTag)
	{
		mBaseClass = inTag;
	}

	ImportTag::~ImportTag(void)
	{
		delete mBaseClass;
	}

	//Accessors
	String* ImportTag::granuleRate() {
		return Wrappers::WStrToNetStr( getMe()->granuleRate().c_str() );
	}
	String* ImportTag::contentType() {
		return Wrappers::WStrToNetStr( getMe()->contentType().c_str() );
	}
	String* ImportTag::src() {
		return Wrappers::WStrToNetStr( getMe()->src().c_str() );
	}
	String* ImportTag::start() {
		return Wrappers::WStrToNetStr( getMe()->start().c_str() );
	}
	String* ImportTag::end() {
		return Wrappers::WStrToNetStr( getMe()->end().c_str() );
	}
	String* ImportTag::title() {
		return Wrappers::WStrToNetStr( getMe()->title().c_str() );
	}
	ParamTagList* ImportTag::paramList() {
		return new ParamTagList(getMe()->paramList()->clone());

	}

	//Mutators
	void ImportTag::setGranuleRate(String* inGranuleRate) {
		wchar_t* tc = Wrappers::netStrToWStr( inGranuleRate );
		wstring locStr = tc;
		getMe()->setGranuleRate( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ImportTag::setContentType(String* inContentType) {
		wchar_t* tc = Wrappers::netStrToWStr( inContentType );
		wstring locStr = tc;
		getMe()->setContentType( locStr );
		Wrappers::releaseWStr( tc );		
	}
	void ImportTag::setSrc(String* inSrc) {
		wchar_t* tc = Wrappers::netStrToWStr( inSrc );
		wstring locStr = tc;
		getMe()->setSrc( locStr );
		Wrappers::releaseWStr( tc );		
	}
	void ImportTag::setStart(String* inStart) {
		wchar_t* tc = Wrappers::netStrToWStr( inStart );
		wstring locStr = tc;
		getMe()->setStart( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ImportTag::setEnd(String* inEnd) {
		wchar_t* tc = Wrappers::netStrToWStr( inEnd );
		wstring locStr = tc;
		getMe()->setEnd( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ImportTag::setTitle(String* inTitle) {
		wchar_t* tc = Wrappers::netStrToWStr( inTitle );
		wstring locStr = tc;
		getMe()->setTitle( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ImportTag::setParamList(ParamTagList* inParamList) {
		getMe()->setParamList(inParamList->getMe()->clone());
	}

	//Other
	String* ImportTag::toString() {
		return Wrappers::WStrToNetStr( getMe()->toString().c_str() );
		
	}

	C_ImportTag* ImportTag::getMe() {
		return (C_ImportTag*)mBaseClass;
	}
}
}
