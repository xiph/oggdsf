// ===========================================================================
// Copyright (C) 2004-2005  Zentaro Kavanagh
// Copyright (C) 2004-2005  Commonwealth Scientific and Industrial Research
//                          Organisation (CSIRO) Australia
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// - Neither the name of Zentaro Kavanagh, CSIRO Australia nor the names of
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ===========================================================================

#include "query_utils.h"
#include "anx_time.h"
#include "apr_stdcall.h"

#include "httpd.h"
#ifdef WIN32
# undef strtoul /* Otherwise Visual Studio .NET 2003 complains */
#endif
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"
#include "apr_strings.h"

#include <stdio.h>
#include <string.h>

#include <libOOOgg/libOOOgg.h>
#include <libOOOgg/dllstuff.h>
#include <libOOOggSeek/AutoAnxSeekTable.h>
#include <libOOOggSeek/AutoOggSeekTable.h>
#include <libOOOggChef/AnnodexRecomposer.h>
#include <libOOOggChef/IRecomposer.h>


#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <string>

#undef DEBUG


bool isAnnodexFile (string locFilename)
{
	string locExtension = locFilename.substr(locFilename.length() - 4);

	return (locExtension == ".anx" || locExtension == ".axv" || locExtension == ".axa");
}

bool isOggFile (string locFilename)
{
	string locExtension = locFilename.substr(locFilename.length() - 4);

	return (locExtension == ".ogg" || locExtension == ".ogm");
}

typedef pair<float, char *> tQualityPair;

bool qualityPairComparator (const tQualityPair &p1, const tQualityPair &p2)
{
	return (p2.first < p1.first);
}

const vector<const string> *preferredOutputMIMETypes(request_rec *inRequest)
{

	// If the user requested the application/ mime type (i.e. the entire
	// encapsulated file), then just return everything
	string locFilename = inRequest->filename;
	if (	(		isAnnodexFile(locFilename)
				&&	get_accept_quality(inRequest, "application/x-annodex") == 1.0
			)
		||	(		isOggFile(locFilename)
				&& (	get_accept_quality(inRequest, "application/ogg")   == 1.0
					||	get_accept_quality(inRequest, "application/x-ogg") == 1.0)
			)
		)
	{
		vector<const string>* locAcceptAllMimeTypes = new vector<const string>;
		locAcceptAllMimeTypes->push_back("*/*");
		return const_cast<const vector<const string>*> (locAcceptAllMimeTypes);
	}

	vector<tQualityPair> locQualityList;
	
	// TODO: We hardcode the types in here for the moment: we should really
	// iterate over the list of Accept: types.  Note that this is in order
	// of priority!

#define MIME_QUALITY_PAIR(s) ( make_pair<float, char *>(get_accept_quality(inRequest, s), s) )
	locQualityList.push_back( MIME_QUALITY_PAIR("application/x-annodex") );
	locQualityList.push_back( MIME_QUALITY_PAIR("application/ogg") );
	locQualityList.push_back( MIME_QUALITY_PAIR("application/x-ogg") );
	locQualityList.push_back( MIME_QUALITY_PAIR("audio/x-speex") );
	locQualityList.push_back( MIME_QUALITY_PAIR("audio/x-vorbis") );
	locQualityList.push_back( MIME_QUALITY_PAIR("text/x-cmml") );
	locQualityList.push_back( MIME_QUALITY_PAIR("video/x-theora") );
#undef MIME_QUALITY_PAIR

	// TODO: Implement wanted MIME types given the above list and qualities

	// Sort the list in order of Accept: quality (so 1.0 is first, 0.0 is last)
	sort(locQualityList.begin(), locQualityList.end(), qualityPairComparator);

	// Since we have a quality rating now, output only the first (preferred)
	// MIME type that we want
	vector<const string>* locMIMETypes = new vector<const string>;
	tQualityPair locElement = locQualityList[0];
	const string locMIMEType = locElement.second;
	locMIMETypes->push_back(locMIMEType);

	return const_cast<const vector<const string>*> (locMIMETypes);
}

bool httpDataSender (unsigned char *inBuffer, unsigned long inBufferLength, void *inUserData)
{
	request_rec *locRequest = (request_rec *) inUserData;

	ap_rwrite(inBuffer, inBufferLength, locRequest);

	return true;
}

extern "C" {

static int AP_MODULE_ENTRY_POINT oggchef_handler(request_rec *inRequest)
{
	apr_uri_t *locURI = &(inRequest->parsed_uri);

	// Grab the local filename (which is determined by the requested URL)
	string locFilename = inRequest->filename;

	// Make a name=value table of the CGI query parameters
	apr_table_t* locCGITable = make_cgi_table (inRequest, locURI->query);

	// Find out what time we're meant to start serving stuff out at
	const char* locRequestedStartTimeAsCString =
		(const char *) apr_table_get (locCGITable, "t");
	double locRequestedStartTime =
		anx_parse_time(locRequestedStartTimeAsCString);

	// What's the output MIME type requested?
	const vector<const string>* locOutputMIMETypes = preferredOutputMIMETypes(inRequest);

#ifdef DEBUG
	for (unsigned int i = 0; i < locOutputMIMETypes->size(); i++) {
		string locMIMEType = locOutputMIMETypes->at(i);
		ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, inRequest,
			"Wanted MIMEs %d: %s", i, locMIMEType.c_str());
	}
#endif

	// Poor man's factory: create a new class to dynamically generate the Ogg
	// file according to the user's wishes
	IRecomposer *locRecomposer = NULL;
	if (isAnnodexFile(locFilename)) {
		locRecomposer = new AnnodexRecomposer(locFilename, httpDataSender, inRequest);
	} else if (isOggFile(locFilename)) {
		//locRecomposer = new OggRecomposer(locOutputMIMETypes);
	} else {
		// We should never get here
		ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, inRequest,
					  "Couldn't identify filename %s", locFilename.c_str());
	}

	locRecomposer->recomposeStreamFrom(locRequestedStartTime, locOutputMIMETypes);

	// Clean up
	//delete locOutputMIMETypes;
	//delete locRecomposer;

	return OK;
}


static void AP_MODULE_ENTRY_POINT oggchef_register_hooks(apr_pool_t *)
{
    ap_hook_handler(AP_HOOK_HANDLER_FUNCTION(oggchef_handler),
		            NULL,
					NULL,
					APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA oggchef_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    AP_REGISTER_HOOK_FUNCTION(oggchef_register_hooks)  /* register hooks */
};

} /* extern "C" */
