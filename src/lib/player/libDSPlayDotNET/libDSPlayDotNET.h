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