/*
   Copyright (C) 2002, 2003, 2004 Zentaro Kavanagh
   
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

   - Neither the name of Zentaro Kavanagh nor the names of its
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

#include "wrappers.h"



namespace illiminable {
	namespace libiWrapper {
	Wrappers::Wrappers(void)
	{
	}

	Wrappers::~Wrappers(void)
	{
	}

	
	char* Wrappers::netStrToCStr(String* inNetString) {
		char* locCStr = (char*)Marshal::StringToHGlobalAnsi(inNetString).ToPointer();
		return locCStr;
	}

	void Wrappers::releaseCStr(char* inCStr) {
		Marshal::FreeHGlobal((int)inCStr);
	}

	wchar_t* Wrappers::netStrToWStr(String* inNetString) {
		wchar_t* locWStr = (wchar_t*)Marshal::StringToHGlobalUni(inNetString).ToPointer();
		return locWStr;
	}

	void Wrappers::releaseWStr(wchar_t* inWStr) {
		Marshal::FreeHGlobal((int)inWStr);
	}
	String* Wrappers::CStrToNetStr(const char* inCStr) {
		String* retStr;
		retStr = Marshal::PtrToStringAnsi((char*)inCStr);
		return retStr;
	}

	String* Wrappers::WStrToNetStr(const wchar_t* inWStr) {
		String* retStr;
		//retStr = Marshal::PtrToStringUni((wchar_t*)inWStr);
		retStr = new String(inWStr);
		return retStr;
	}


}
}