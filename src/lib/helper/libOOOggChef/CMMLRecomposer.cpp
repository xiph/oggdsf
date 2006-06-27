//===========================================================================
//Copyright (C) 2005 Zentaro Kavanagh
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================


#include "stdafx.h"

#include <libOOOggChef/AnnodexRecomposer.h>
#include <libOOOggChef/CMMLRecomposer.h>
#include <libOOOggChef/utils.h>

#include <libilliCore/StringHelper.h>
#include <libOOOgg/libOOOgg.h>
#include <libCMMLTags/libCMMLTags.h>
#include <libTemporalURI/C_TimeStamp.h>
#include <libCMMLParse/CMMLParser.h>
#include <libCMMLParse/CMMLTagUtils.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#undef DEBUG

#ifdef UNICODE
CMMLRecomposer::CMMLRecomposer(wstring inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData)
#else 
CMMLRecomposer::CMMLRecomposer(string inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData)
#endif
	:	mCMMLFilename(inFilename)
	,	mBufferWriter(inBufferWriter)
	,	mBufferWriterUserData(inBufferWriterUserData)
{
}


CMMLRecomposer::~CMMLRecomposer(void)
{
}

	
bool CMMLRecomposer::recomposeStreamFrom(double inStartingTimeOffset, const vector<string>* inWantedMIMETypes)
{
	// If the only wants just zee basic CMML, well, just serve out the CMML
	if (wantOnlyCMML(inWantedMIMETypes) && inStartingTimeOffset == 0) {
		sendFile(mCMMLFilename, mBufferWriter, mBufferWriterUserData);
		return true;
	}

#ifdef DEBUG
	mDebugFile.open("G:\\Logs\\CMMLRecomposer.log", ios_base::out);
	mDebugFile << "CMMLRecomposer 1 " << endl;
#endif

	// Parse in the CMML into a C_CMMLDoc class
	C_CMMLDoc *locCMML = new C_CMMLDoc;
	CMMLParser locCMMLParser;
	locCMMLParser.parseDocFromFile(mCMMLFilename, locCMML);

	// We assume that CMML recomposition fails unless explicitly set otherwise
	bool locReturnValue = false;

	// We need to declare and initialise all our variables here, because
	// in the presence of errors, we may be deleting variables which haven't
	// been initialised yet ...
	C_CMMLRootTag *locCMMLRoot = NULL;
	C_StreamTag *locStream = NULL;

	if (locCMML == NULL) {
		goto cleanup;
	}

	// No matter what the output type is, we always have to read & parse the stream
	// and head tags anyway, so do that now

	// If we don't have a root tag at all, we're pretty screwed: fail with prejudice
	locCMMLRoot = locCMML->root();
	if (locCMMLRoot == NULL) {
		goto cleanup;
	}

	// It's not catastrophic if we don't have <stream>, <head>, or even <clip> tags
	locStream = locCMMLRoot->stream();
#if 0
	C_HeadTag *locHead = locCMMLRoot->head();
	C_ClipTagList *locClipList = locCMMLRoot->clipList();
#endif

	// If our final output type is CMML, we deal with it (otherwise we hand off the
	// recomposition to AnnodexRecomposer -- see below)

	// XXX: Checking for * / * is a pretty dodgy hack ...
	if (wantOnlyCMML(inWantedMIMETypes) || inWantedMIMETypes->at(0) == "*/*") {
		C_CMMLDoc* locCMMLDoc = locCMML->clone();

		// If the clip list exists ...
		if (locCMMLDoc && locCMMLDoc->root() && locCMMLDoc->root()->clipList()) {
			// ... we need to replace it with only the clips during or after
			// the user's requested time

#ifdef DEBUG
			mDebugFile << "Got here 1" << endl;
#endif

			// Convert the requested time from a double to an int64
			C_TimeStamp locTimeStamp;
			locTimeStamp.parseTimeStamp(inStartingTimeOffset);
			LOOG_INT64 locTime = locTimeStamp.toHunNanos();

#ifdef DEBUG
			mDebugFile << "locTime: " << locTime << endl;
#endif

			// Get the clip tags during or after the wanted time
			C_ClipTagList *locRequestedClips =
				CMMLTagUtils::getClipsFrom(locCMMLDoc->root()->clipList(), locTime);

			// Replace the clip list in our new CMML document.  We don't need to
			// delete the old clip list before doing this, since the setClipList()
			// method takes care of that for us.
			locCMMLDoc->root()->setClipList(locRequestedClips);
		}

		// Pump out the newly created CMML document
		string locCMMLDocString =
			StringHelper::toNarrowStr(locCMMLDoc->toString());
		mBufferWriter((unsigned char *) locCMMLDocString.c_str(),
			(unsigned long) locCMMLDocString.size(), mBufferWriterUserData);

		// Indicate success to our callee
		locReturnValue = true;
	} else {
		// The user didn't want CMML, i.e. they'll be wanting media.  We scan the
		// CMML file for the <stream> tag, to see if it imports any media -- if it
		// does, we'll have to figure out what MIME types the media files are (and
		// perhaps mux them) so that the the user gets it in the form they want.
		//
		// e.g. if the user requests an application/x-annodex file, the CMML
		// <stream> tag has an import expression pointing to a Vorbis file, and
		// another import expression pointing to a Theora file, we need to mux
		// those two files together, add an empty CMML track (so it's a valid
		// Annodex file), and then send that to the user -- possibly only from
		// a particular time offset.  Fun!


	}

#if 0
	C_ImportTagList *locImportTagList = locStream->importList();
	if (locImportTagList == NULL) {
		goto cleanup;
	}
#endif

cleanup:
	if (locCMML) {
		delete locCMML;
		locCMML = NULL;
	}

#ifdef DEBUG
	mDebugFile.close();
#endif

	return locReturnValue;
}


bool CMMLRecomposer::acceptOggPage(OggPage*)
{
	return true;
}

