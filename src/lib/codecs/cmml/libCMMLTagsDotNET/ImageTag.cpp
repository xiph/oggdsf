/*
   Copyright (C) 2003 Commonwealth Scientific and Industrial Research
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
#include ".\imagetag.h"


namespace CSIRO {
namespace libCMMLTagsDotNET {

	ImageTag::ImageTag(void)
	{
		mBaseClass = new C_ImageTag;
	}

	ImageTag::~ImageTag(void)
	{
		delete mBaseClass;
	}

	ImageTag::ImageTag(C_ImageTag* inTag) {
		mBaseClass = inTag;
	}

	//Accessors
	String* ImageTag::src() {
		return Wrappers::CStrToNetStr( getMe()->src().c_str() );
	}
	String* ImageTag::alt() {
		return Wrappers::CStrToNetStr( getMe()->alt().c_str() );
	}

	//Mutators
	void ImageTag::setSrc(String* inSrc) {
		char* tc = Wrappers::netStrToCStr( inSrc );
		getMe()->setSrc( tc );
		Wrappers::releaseCStr( tc );		
	}
	void ImageTag::setAlt(String* inAlt) {
		char* tc = Wrappers::netStrToCStr( inAlt );
		getMe()->setAlt( tc );
		Wrappers::releaseCStr( tc );
	}

	//Other
	String* ImageTag::toString() {
		return Wrappers::CStrToNetStr( getMe()->toString().c_str() );
	}


	C_ImageTag* ImageTag::getMe() {
		return (C_ImageTag*)mBaseClass;
	}

}
}
