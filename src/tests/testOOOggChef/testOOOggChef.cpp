// testOOOggChef.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libCMMLTags/libCMMLTags.h>
#include <libilliCore/illicoreconfig.h>
#include <libilliCore/StringHelper.h>
#include <libTemporalURI/C_TimeStamp.h>
#include <libWinCMMLParse/CMMLParser.h>

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	C_CMMLDoc *locCMMLDoc = new C_CMMLDoc;

	// Parse the document first

	CMMLParser locCMMLParser;
	string locFilename = argv[1];
	if (!locCMMLParser.parseDocFromFile(StringHelper::toWStr(locFilename), locCMMLDoc)) {
		cerr << "parseDocFromFile failed" << endl;
		return 1;
	}

	// Convert the user's time specification to an int64

	C_TimeStamp locTimeStamp;
	string locTimeString = argv[2];
	LOOG_INT64 locTime = locTimeStamp.parseTimeStamp(locTimeString);
	cout << "Time in DirectSeconds: " << locTime << endl;

	locTime = 150000000;

	// Reconstruct the CMML document from the given time offset

	// n.b. No error checking done to see whether the <cmml> and <clip> tags exist, so
	// make sure they exist in the given CMML file!
	C_ClipTagList *locClipTagList = locCMMLDoc->root()->clipList()->getClipsFrom(locTime);
	
	locCMMLDoc->root()->setClipList(locClipTagList);
	string locCMMLDocString = StringHelper::toNarrowStr(locCMMLDoc->toString());
	cout << locCMMLDocString << endl;

	delete locCMMLDoc;

	return 0;
}

