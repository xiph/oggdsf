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

#include <iostream>
#include <fstream>



/* Note: This file's pretty ugly at the moment, it's basically a merge of the
   mod_annodex_ap20 source code and AnxCutter.  Don't worry, it will be tidied
   up :)
 */

#define DEBUG

#define ANX_MIME_TYPE "application/x-annodex"
#define CMML_MIME_TYPE "text/x-cmml"

#define CMML_PREAMBLE \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \

#define MOD_ANNODEX_TYPE_UNKNOWN -1

#define MOD_ANNODEX_TYPE_ANX 0
#define MOD_ANNODEX_TYPE_CMML 1

#define MEDIA_BUF_LEN 8192



enum eDemuxState {
	SEEN_NOTHING,
	SEEN_ANNODEX_BOS,
	SEEN_ANNODEX_EOS,
	SEEN_ALL_CODEC_HEADERS,
	INVALID = 100
};

eDemuxState demuxState;

typedef pair<unsigned long, unsigned long> tSerial_HeadCountPair;

unsigned long bytePos;

bool gotAllHeaders;

vector<tSerial_HeadCountPair> theStreams;

unsigned long annodexSerialNo;

using namespace std;

unsigned long headerCount(OggPacket* inPacket)
{
  const unsigned short NUM_SEC_HEADERS_OFFSET = 24;

  return iLE_Math::charArrToULong(inPacket->packetData() +
                                  NUM_SEC_HEADERS_OFFSET);

}

request_rec *theRequest;

bool writePageToOutputFile(OggPage* inOggPage) {
  // outputFile.write((char*)inOggPage->createRawPageData(), inOggPage->pageSize());
  ap_rwrite((char*)inOggPage->createRawPageData(),
            inOggPage->pageSize(),
	    theRequest);
  return true;
}

//This will be called by the callback
bool pageCB(OggPage* inOggPage, void *)
{

	bool allEmpty = true;

	switch (demuxState) {

		case SEEN_NOTHING:
			if (		(inOggPage->numPackets() == 1)
					&&	(inOggPage->header()->isBOS())
					&&	(strncmp((char*)inOggPage->getPacket(0)->packetData(), "Annodex\0", 8) == 0)) {
				
				//Advance the state
				demuxState = SEEN_ANNODEX_BOS;

				//Remember the annodex streams serial no
				annodexSerialNo = inOggPage->header()->StreamSerialNo();

				//Write out the page.
				writePageToOutputFile(inOggPage);
			} else {

				demuxState = INVALID;
			}

			break;
		case SEEN_ANNODEX_BOS:
			if (		(inOggPage->numPackets() == 1)
					&&	(inOggPage->header()->isBOS())
					&&	(strncmp((char*)inOggPage->getPacket(0)->packetData(), "AnxData\0", 8) == 0)) {
				

				//Create an association of serial no and num headers
				tSerial_HeadCountPair locMap;
				locMap.first = inOggPage->header()->StreamSerialNo();
				locMap.second = headerCount(inOggPage->getPacket(0));
				
				//Add the association to the list
				theStreams.push_back(locMap);

				//Write the page out to the output file.
				writePageToOutputFile(inOggPage);
			} else if (			(inOggPage->header()->isEOS())
							&&	(inOggPage->header()->StreamSerialNo() == annodexSerialNo)) {

				//It's the Annodex EOS.
				demuxState = SEEN_ANNODEX_EOS;
				writePageToOutputFile(inOggPage);
			} else {
				demuxState = INVALID;
			}
			break;
		case SEEN_ANNODEX_EOS:
			for (unsigned int i = 0; i < theStreams.size(); i++) {
				if (theStreams[i].first == inOggPage->header()->StreamSerialNo()) {
					if (theStreams[i].second >= 1) {
						theStreams[i].second--;
						writePageToOutputFile(inOggPage);
					} else {
						demuxState = INVALID;
					}
				}
			}

			
			for (unsigned int i = 0; i < theStreams.size(); i++) {
				if (theStreams[i].second != 0) {
					allEmpty = false;
				}
			}

			if (allEmpty) {

				demuxState = SEEN_ALL_CODEC_HEADERS;
			}
			break;
		case SEEN_ALL_CODEC_HEADERS:
			break;
		case INVALID:
			break;
		default:
			break;
	}


	if (demuxState == INVALID) {
	}
	// delete inOggPage;

	return true;
}

static int
ma_anxenc (request_rec * r, char * filename, char * content_type,
	   apr_table_t * cgi_table)
{
  char * val;
  double seek_offset = 0.0;

  theRequest = r;

  /* put the requested time into seek_offset */
  val = (char *)apr_table_get (cgi_table, "t");

  if (!val)
  {
	fstream inputFile;
	inputFile.open(filename, ios_base::in | ios_base::binary);
	for (;;)
	{
	  const unsigned short BUFF_SIZE = 8092;
	  char* locBuff = new char[BUFF_SIZE];

	  inputFile.read(locBuff, BUFF_SIZE);
	  unsigned long locBytesRead = inputFile.gcount();
	  if (locBytesRead == 0) break;
	  ap_rwrite (locBuff, locBytesRead, r);
	}
	inputFile.close();

	return 0;
  }

  seek_offset = anx_parse_time (val);

  /* begin AnxCutter code */
  demuxState = SEEN_NOTHING;
  bytePos = 0;
  gotAllHeaders = false;
  annodexSerialNo = 0;

  OggDataBuffer testOggBuff;
  testOggBuff.registerStaticCallback(&pageCB, NULL);
	
  /* rip out the headers from the file */

  fstream inputFile;
		
  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "ma_anxenc t=%s (%ld)\n", val, seek_offset);

  inputFile.open(filename, ios_base::in | ios_base::binary);

  char* locBuff = new char[MEDIA_BUF_LEN];
  while (demuxState < SEEN_ALL_CODEC_HEADERS) {
    inputFile.read(locBuff, MEDIA_BUF_LEN);
    unsigned long locBytesRead = inputFile.gcount();
	if (locBytesRead > 0)
	{
	  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "feeding %ld bytes to testOggBuff", locBytesRead);
	}
    testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
  }
  inputFile.close();

  // Build a seek table for the file
  AutoAnxSeekTable *locSeekTable = new AutoAnxSeekTable(filename);
  locSeekTable->buildTable();
	
  // Seek to the user's requested start time
  LOOG_UINT64 locStartTime = (LOOG_UINT64) seek_offset * 10000000;
  OggSeekTable::tSeekPair locSeekResult = locSeekTable->getStartPos(locStartTime);
	
  // Stream-copy everything from the requested timepoint onward to the output file
  inputFile.open(filename, ios_base::in | ios_base::binary);
  inputFile.seekg(locSeekResult.second);

  for (;;) {
    inputFile.read(locBuff, MEDIA_BUF_LEN);
    unsigned long locBytesRead = inputFile.gcount();
    if (locBytesRead == 0) break;
    //outputFile.write(locBuff, locBytesRead);
    ap_rwrite (locBuff, locBytesRead, r);
  }
  inputFile.close();

  ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                "Served request");
}


extern "C" {

/* The annodex content handler */
static int AP_MODULE_ENTRY_POINT annodex_handler(request_rec *r)
{
  apr_uri_t * uri;
  char * filename;
  apr_table_t * cgi_table;

	   ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                  "Got here 2");

  uri = &(r->parsed_uri);

  /* usually filename is request filename */
  filename = r->filename;

	   ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r,
                  "filename is %s", filename);

  cgi_table = make_cgi_table (r, uri->query);

  ma_anxenc (r, r->filename, ANX_MIME_TYPE, cgi_table);

  return OK;
}

static void AP_MODULE_ENTRY_POINT annodex_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(AP_HOOK_HANDLER_FUNCTION(annodex_handler),
		            NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA timeslice_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    AP_REGISTER_HOOK_FUNCTION(annodex_register_hooks)  /* register hooks */
};

} /* extern "C" */
