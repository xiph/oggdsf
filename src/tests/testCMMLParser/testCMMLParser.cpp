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

bool testClipParse(wstring inClipString) {
	CMMLParser locParser;

	wcout << "Original"<<endl<<inClipString<<endl<<endl;

	C_ClipTag locClip;
	
	bool locWasOK = locParser.parseClipTag(inClipString, &locClip);

	if (locWasOK) {
		wcout<<"Parsed OK"<<endl<<endl<<locClip.toString()<<endl<<endl;
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



	wstring clip_1 = L"<clip id=\"dolphin\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img src=\"dolphin.jpg\"/><desc>Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_1);

	wstring clip_2 = L"asdjhaskdljfhksladf";
	testClipParse(clip_2);

	wstring clip_3 = L"<blue><red>random stuff</red><green>But still valid XML</green></blue>";
	testClipParse(clip_3);

	wstring clip_4 = L"<clip id=\"dolphin\" lang=\"en\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img alt=\"Picture of dolphin\" src=\"dolphin.jpg\"/><desc id=\"descID\" lang=\"fr\">Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_4);

	wstring clip_5 = L"<clip id=\"dolphin\" start=\"npt:3.5\" end=\"npt:5:5.9\"><a href\"http:\\linktome.com\" class=\"someClass\">Random anchor text</a><img src=\"dolphin.jpg\"/><desc>Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_5);
	return 0;
}

