// testCMMLParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "libCMMLTags.h"

#include "libWinCMMLParse.h"
#include "CMMLParser.h"


bool testHeadParse(wstring inHeadString) {
	CMMLParser locParser;

	wcout << "Original"<<endl<<inHeadString<<endl<<endl;

	C_HeadTag locHead;
	
	bool locWasOK = locParser.parseHeadTag(inHeadString, &locHead);

	if (locWasOK) {
		wcout<<"Parsed OK"<<endl<<endl<<locHead.toString()<<endl<<endl;
	} else {
		wcout<<"*** PARSE FAILED ***"<<endl<<endl;
	}

	return locWasOK;
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{

	wstring head_1 = L"<head><title>Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head>";
	testHeadParse(head_1);

	wstring head_2 = L"asdfasdfasdfasdf";
	testHeadParse(head_2);

	wstring head_3 = L"<blue><red>random stuff</red><green>But still valid XML</green></blue>";
	testHeadParse(head_3);

	return 0;
}

