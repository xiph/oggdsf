// testCMMLParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "libCMMLTags.h"

#include "libWinCMMLParse.h"
#include "CMMLParser.h"


bool testHeadParse(wstring inHeadString, C_HeadTag* outHeadTag) {
	CMMLParser locParser;
	return locParser.parseHeadTag(inHeadString, outHeadTag);

}

int _tmain(int argc, _TCHAR* argv[])
{

	wstring head_1 = L"<head><title>Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head>";

	wcout << "Original"<<endl<<head_1<<endl<<endl;

	C_HeadTag locHead;
	testHeadParse(head_1, &locHead);

	cout<<"Parsed"<<endl<<locHead.toString()<<endl<<endl;

	return 0;
}

