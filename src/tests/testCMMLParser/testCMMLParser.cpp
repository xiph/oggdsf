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
	//Valid minimal
	wstring head_1 = L"<head><title>Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head>";
	testHeadParse(head_1);

	//INVALID: Random data
	wstring head_2 = L"asdfasdfasdfasdf";
	testHeadParse(head_2);

	//INVALID: Valid xml, invalid cmml
	wstring head_3 = L"<blue><red>random stuff</red><green>But still valid XML</green></blue>";
	testHeadParse(head_3);

	//INVALID: No title tag
	wstring head_4 = L"<head><meta name=\"Producer\" content=\"Joe Ordinary\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head>";
	testHeadParse(head_4);

	//Valid use most
	wstring head_5 = L"<head id=\"headID\" lang=\"en\" dir=\"ltr\" profile=\"some profile\"><base href=\"http://baseurl.com\"/><title lang=\"fr\">Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\" scheme=\"some scheme\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head>";
	testHeadParse(head_5);

	//INVALID: Missing closing xml tag on option element.
	wstring head_6 = L"<head id=\"headID\" lang=\"en\" dir=\"ltr\" profile=\"some profile\"><base href=\"http://baseurl.com\"/><title lang=\"fr\">Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\" scheme=\"some scheme\"/><meta name=\"DC.Author\" content=\"Joe's friend\"></head>";
	testHeadParse(head_6);
	return 0;
}

