#include "StdAfx.h"
#include ".\dsstringer.h"
#include "StringHelper.h"
string DSStringer::GUID2String(const GUID* inGUID) {
	string retStr = "{" + StringHelper::numToString(inGUID->Data1) + 
					"-" + StringHelper::numToString(inGUID->Data2) + 
					"-" + StringHelper::numToString(inGUID->Data3) +
					"-";
	
	for (int i = 0; i < 8; i++) {
		retStr += inGUID->Data4[i];
	}
	retStr += "}";
	return retStr;
	
}