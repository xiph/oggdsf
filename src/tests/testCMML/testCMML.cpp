// testCMML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "libWinCMMLParse.h"
#include "CMMLParser.h"

#include "libCMMLTags.h"
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

	CMMLParser testParse;
	C_ClipTag locClip;
	wstring clipTest = L"<clip id=\"intro\" start=\"npt:0\"><a href=\"http://www.csiro.au/promos/ozadvances/\">http://www.csiro.au/promos/ozadvances/</a><img src=\"/media_images/cleaner1.jpg\"/><desc>Introduction to 'Cleaner Power Stations - Australia Advances video'.</desc></clip>";
	testParse.parseClipTag(clipTest, &locClip);

	cout<<"PARSED"<<locClip.toString()<<endl<<endl<<endl;

	C_HeadTag locHead;
	wstring headTest = L"<head><title>Australia Advances Series - Cleaner Power Stations</title><meta name=\"DESCRIPTION\" content=\"A power station in New South Wales is using a new system of filter bags that remove the coal dust from the emitted fumes\"/><meta name=\"KEYWORDS\" content=\"Polution, Power Station\"/><meta name=\"TYPE\" content=\"video\"/></head>";
	testParse.parseHeadTag(headTest, &locHead);

	cout<<"PARSED"<<locHead.toString()<<endl<<endl;


	int x;
	cin>>x;
	return 0;
}

