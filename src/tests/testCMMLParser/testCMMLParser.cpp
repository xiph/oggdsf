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

bool testCMMLRootParse(wstring inCMMLRootString) {
	CMMLParser locParser;

	wcout << "Original"<<endl<<inCMMLRootString<<endl<<endl;

	C_CMMLRootTag locCMMLRoot;
	
	bool locWasOK = locParser.parseCMMLRootTag(inCMMLRootString, &locCMMLRoot);

	if (locWasOK) {
		wcout<<"Parsed OK"<<endl<<endl<<locCMMLRoot.toString()<<endl<<endl;
	} else {
		wcout<<"*** PARSE FAILED ***"<<endl<<endl;
	}

	return locWasOK;

}

bool testCMMLFileParse(wstring inFilename) {
	CMMLParser locParser;

	C_CMMLDoc locDoc;
	bool locWasOK = locParser.parseDocFromFile(inFilename, &locDoc);

	wcout<<"Trying to parse "<<inFilename<<endl;

	if (locWasOK) {
		wcout<<locDoc.toString()<<endl;
	} else {
		wcout<<"*** PARSE FAILED ***"<<endl;
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


	//Valid minimal
	wstring clip_1 = L"<clip id=\"dolphin\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img src=\"dolphin.jpg\"/><desc>Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_1);

	//INVALID: Random data
	wstring clip_2 = L"asdjhaskdljfhksladf";
	testClipParse(clip_2);

	//INVALID: valid xml but invalid cmml
	wstring clip_3 = L"<blue><red>random stuff</red><green>But still valid XML</green></blue>";
	testClipParse(clip_3);

	//Valid
	wstring clip_4 = L"<clip id=\"dolphin\" lang=\"en\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img alt=\"Picture of dolphin\" src=\"dolphin.jpg\"/><desc id=\"descID\" lang=\"fr\">Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_4);

	//INVALID: Missing equals on href=
	wstring clip_5 = L"<clip id=\"dolphin\" start=\"npt:3.5\" end=\"npt:5:5.9\"><a href\"http:\\linktome.com\" class=\"someClass\">Random anchor text</a><img src=\"dolphin.jpg\"/><desc>Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_5);

	//Valid
	wstring clip_6 = L"<clip id=\"dolphin\" start=\"npt:3.5\" end=\"npt:5:5.9\"><a href=\"http:\\linktome.com\" class=\"someClass\">Random anchor text</a><img src=\"dolphin.jpg\"/><desc>Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip>";
	testClipParse(clip_6);


	//Valid
	wstring cmml_1 = L"<cmml lang=\"en\"><head id=\"headID\" lang=\"en\" dir=\"ltr\" profile=\"some profile\"><base href=\"http://baseurl.com\"/><title lang=\"fr\">Types of fish</title><meta name=\"Producer\" content=\"Joe Ordinary\" scheme=\"some scheme\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head><clip id=\"dolphin\" lang=\"en\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img alt=\"Picture of dolphin\" src=\"dolphin.jpg\"/><desc id=\"descID\" lang=\"fr\">Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip></cmml>";
	testCMMLRootParse(cmml_1);

	//INVALID: No title
	wstring cmml_2 = L"<cmml lang=\"en\"><head id=\"headID\" lang=\"en\" dir=\"ltr\" profile=\"some profile\"><base href=\"http://baseurl.com\"/><meta name=\"Producer\" content=\"Joe Ordinary\" scheme=\"some scheme\"/><meta name=\"DC.Author\" content=\"Joe's friend\"/></head><clip id=\"dolphin\" lang=\"en\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img alt=\"Picture of dolphin\" src=\"dolphin.jpg\"/><desc id=\"descID\" lang=\"fr\">Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip></cmml>";
	testCMMLRootParse(cmml_2);

	//INVALID: No head
	wstring cmml_3 = L"<cmml lang=\"en\"><clip id=\"dolphin\" lang=\"en\" start=\"npt:3.5\" end=\"npt:5:5.9\"><img alt=\"Picture of dolphin\" src=\"dolphin.jpg\"/><desc id=\"descID\" lang=\"fr\">Here, Joe caught sight of a dolphin in the ocean.</desc><meta name=\"Subject\" content=\"dolphin\"/></clip></cmml>";
	testCMMLRootParse(cmml_3);

	//INVALID: Random data
	wstring cmml_4 = L"asdfasd fasd fasdf ds ";
	testCMMLRootParse(cmml_4);

	//INVALID: valid xml but invalid cmml
	wstring cmml_5 = L"<blue><red>random stuff</red><green>But still valid XML</green></blue>";
	testCMMLRootParse(cmml_5);

	int x;
	cin>>x;

	wstring file_1 = L"testcmml.xml";
	testCMMLFileParse(file_1);




	return 0;
}

