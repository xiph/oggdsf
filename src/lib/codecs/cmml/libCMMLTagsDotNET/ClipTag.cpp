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
#include ".\cliptag.h"

namespace illiminable {
namespace libCMMLTagsDotNET {

	ClipTag::ClipTag(void)
	{
		mBaseClass = new C_ClipTag;
		mDeleteBase = true;
	}

	ClipTag::ClipTag(C_ClipTag* inTag, bool inDeleteBase) {
		mBaseClass = inTag;
		mDeleteBase = inDeleteBase;
	}
	ClipTag::~ClipTag(void)
	{
		delete mBaseClass;
	}
	
	C_ClipTag* ClipTag::getMe() {
		return (C_ClipTag*)mBaseClass;
	}

	//Accessors
	String* ClipTag::track() {
		return Wrappers::WStrToNetStr( getMe()->track().c_str() );
	}
	MetaTagList* ClipTag::metaList() {
		return new MetaTagList(getMe()->metaList(), false);
	}
	AnchorTag* ClipTag::anchor() {
		return new AnchorTag(getMe()->anchor(),false);
	}
	ImageTag* ClipTag::image() {
		return new ImageTag(getMe()->image(), false);
	}
	DescTag* ClipTag::desc() {
		return new DescTag(getMe()->desc(), false);
	}

	String* ClipTag::start() {
		return Wrappers::WStrToNetStr( getMe()->start().c_str() );

	}
	String* ClipTag::end() {
		return Wrappers::WStrToNetStr( getMe()->end().c_str() );

	}


	//Mutators
	void ClipTag::setTrack(String* inTrack) {
		wchar_t* tc = Wrappers::netStrToWStr( inTrack );
		wstring locStr = tc;
		getMe()->setTrack( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ClipTag::setAnchor(AnchorTag* inAnchor) {
		getMe()->setAnchor(inAnchor->getMe()->clone());
	}
	void ClipTag::setImage(ImageTag* inImage) {
		getMe()->setImage(inImage->getMe()->clone());
	}
	void ClipTag::setDesc(DescTag* inDesc) {
		getMe()->setDesc(inDesc->getMe()->clone());
	}

	void ClipTag::setStart(String* inStart) {
		wchar_t* tc = Wrappers::netStrToWStr( inStart );
		wstring locStr = tc;
		getMe()->setStart( locStr );
		Wrappers::releaseWStr( tc );
	}
	void ClipTag::setEnd(String* inEnd) {
		wchar_t* tc = Wrappers::netStrToWStr( inEnd );
		wstring locStr = tc;
		getMe()->setEnd( locStr );
		Wrappers::releaseWStr( tc );
	}


	//Others
	String* ClipTag::toString() {
		return Wrappers::WStrToNetStr( getMe()->toString().c_str() );

	}

}
}