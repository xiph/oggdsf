//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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

#include "StdAfx.h"
#include ".\stringhelper.h"

StringHelper::StringHelper(void)
{
}

StringHelper::~StringHelper(void)
{
}

wstring StringHelper::toWStr(string inString) {
	wstring retVal;

	//LPCWSTR retPtr = new wchar_t[retVal.length() + 1];
	for (std::string::const_iterator i = inString.begin(); i != inString.end(); i++) {
		retVal.append(1, *i);
	}
	

	return retVal;
}


string StringHelper::toNarrowStr(wstring inString) {
	string retVal;

	//LPCWSTR retPtr = new wchar_t[retVal.length() + 1];
	for (std::wstring::const_iterator i = inString.begin(); i != inString.end(); i++) {
		retVal.append(1, *i);
	}
	

	return retVal;
}

string StringHelper::numToString(unsigned __int64 inNum) {
	char locDigit = 0;
	string retStr = "";
	string temp = "";

	if (inNum == 0) return "0";

	while (inNum > 0) {
		locDigit = ((char)(inNum % 10)) + '0';
		inNum /= 10;
		temp = locDigit;
		temp.append(retStr);
		retStr = temp;
		//retStr.append(1, locDigit);
	}
	return retStr;
}

unsigned __int64 StringHelper::stringToNum(string inString) {
	int locDigit = 0;
	unsigned __int64 retVal = 0;
	size_t locStrLen = inString.length();

	for (unsigned long i = 0; i < locStrLen; i++) {
		locDigit = inString[i] - '0';
		//If it's not in the range 0-9 we bail out
		if ( !((locDigit >= 0) && (locDigit <=9)) ) {
			//FIX::: throw exception
			throw 0;
		}
		retVal *= 10;
		retVal += locDigit;

	}
	return retVal;

}

//These should go out to the string helper.
unsigned char StringHelper::digitToHex(unsigned char inDigit) {
	
	unsigned char locDigit = (inDigit > 9)		?	(inDigit  - 10) + A_BASE
												:	(inDigit) + ZERO_BASE;
	return locDigit;

}

string StringHelper::charToHexString(unsigned char inChar) {
	
	string retStr ="";
	retStr +=digitToHex(inChar / 16);

	retStr+= digitToHex(inChar % 16);
	return retStr;


}
