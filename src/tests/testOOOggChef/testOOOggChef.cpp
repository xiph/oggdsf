// testOOOggChef.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <libCMMLTags/libCMMLTags.h>
#include <libilliCore/illicoreconfig.h>
#include <libilliCore/StringHelper.h>
#include <libTemporalURI/C_TimeStamp.h>
#include <libCMMLParse/CMMLParser.h>
#include <libCMMLParse/CMMLTagUtils.h>

#ifdef WIN32
int __cdecl _tmain(int argc, _TCHAR* argv[])
#else /* assume POSIX */
int main(int argc, char* argv[])
#endif
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
	if (!locTimeStamp.parseTimeStamp(locTimeString)) {
		cerr << "Couldn't convert time stamp to DirectSeconds" << endl;
		return 2;
	}
	LOOG_INT64 locTime = locTimeStamp.toHunNanos();
	cout << "Time in DirectSeconds: " << locTime << endl;

	// Reconstruct the CMML document from the given time offset

	// n.b. No error checking done to see whether the <cmml> and <clip> tags exist, so
	// make sure they exist in the given CMML file!
	C_ClipTagList *locClipTagList =
		CMMLTagUtils::getClipsFrom(locCMMLDoc->root()->clipList(), locTime);
	
	locCMMLDoc->root()->setClipList(locClipTagList);
	string locCMMLDocString = StringHelper::toNarrowStr(locCMMLDoc->toString());
	cout << locCMMLDocString << endl;

	delete locCMMLDoc;

	return 0;
}

