//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
//
//Copyright (C) 2004 Commonwealth Scientific and Industrial Research
// Orgainisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

// libDSPlayDotNET.h

#pragma once
#pragma unmanaged
#include <string>

#pragma managed
#using "libiWrapper.dll"
#using "libCMMLTagsDotNET.dll"
using namespace illiminable::libCMMLTagsDotNET;
using namespace std;
using namespace System;

namespace illiminable {
namespace libDSPlayDotNET {
//	/*class __gc DSPlayer {
//		public:
//			bool loadFile(String inFileName);
//			bool play();
//			bool pause();
//			bool stop();
//			bool seek(Int64 inTime);
//			Int64 queryPosition();
//	}*/

	static const GUID X_IID_ICMMLAppControl = {0x6188ad0c, 0x62cb, 0x4658, {0xa1, 0x4e, 0xcd, 0x23, 0xcf, 0x84, 0xec, 0x31}};
	//DEFINE_GUID(IID_ICMMLAppControl, 0x6188ad0c, 0x62cb, 0x4658, 0xa1, 0x4e, 0xcd, 0x23, 0xcf, 0x84, 0xec, 0x31);
	static wstring toWStr(std::string inString) {

		//This is frmo the String Helper class.

		wstring retVal;

		//LPCWSTR retPtr = new wchar_t[retVal.length() + 1];
		for (std::string::const_iterator i = inString.begin(); i != inString.end(); i++) {
			retVal.append(1, *i);
		}
	

		return retVal;
	}	
}
}