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
#include ".\cmmlpreamble.h"

namespace illiminable {
namespace libCMMLTagsDotNET {


	CMMLPreamble::CMMLPreamble(void)
	{
	}

	CMMLPreamble::~CMMLPreamble(void)
	{
	}


	//Accessors
	String* CMMLPreamble::xmlVersion() {
		return Wrappers::CStrToNetStr( mBaseClass->xmlVersion().c_str() );
	}
	String* CMMLPreamble::xmlEncoding() {
		return Wrappers::CStrToNetStr( mBaseClass->xmlEncoding().c_str() );
	}
	String* CMMLPreamble::xmlStandAlone() {
		return Wrappers::CStrToNetStr( mBaseClass->xmlStandAlone().c_str() );
	}
		
	//Mutators
	void CMMLPreamble::setXmlVersion(String* inVersion) {
		char* tc = Wrappers::netStrToCStr( inVersion );
		mBaseClass->setXmlVersion( tc );
		Wrappers::releaseCStr( tc );
	}
	void CMMLPreamble::setXmlEncoding(String* inEncoding) {
		char* tc = Wrappers::netStrToCStr( inEncoding );
		mBaseClass->setXmlEncoding( tc );
		Wrappers::releaseCStr( tc );
	}
	void CMMLPreamble::setXmlStandAlone(String* inStandAlone) {
		char* tc = Wrappers::netStrToCStr( inStandAlone );
		mBaseClass->setXmlStandAlone( tc );
		Wrappers::releaseCStr( tc );
	}

	//Others
	String* CMMLPreamble::toString() {
		return Wrappers::CStrToNetStr( mBaseClass->toString().c_str() );
	}

}
}